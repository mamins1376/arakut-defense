import uasyncio
import hashlib
import ubinascii


class Server:
    def start(self):
        uasyncio.run(self.listen())

    async def listen(self):
        host = '192.168.4.1'
        print('listening on http://%s:80' % host)
        server = await uasyncio.start_server(self.accept, host, 80)
        await server.wait_closed()

    async def accept(self, reader, writer):
        print('connection accepted')
        await Request(reader, writer, self).talk()
        print('connection closed')

    def message(self, message):
        raise NotImplementeddError()


class Request:
    def __init__(self, reader, writer, server):
        self.reader = reader
        self.writer = writer
        self.server = server
        self.mime = b'application/octet-stream'

    async def talk(self):
        line = await self.readline()
        print('request:', line)

        try:
            method, path, version = line.split(b' ')
        except ValueError:
            return
        self.version = version

        await self.assume(version == b'HTTP/1.1',
                          b'505 HTTP Version Not Supported')
        await self.assume(method == b'GET', b'405 Method Not Allowed')
        await self.assume(path.startswith(b'/'), b'400 Bad Request')

        if path.endswith(b'/'):
            path += b'index.html'

        if path == b'/ws':
            return await self.websocket()

        await self.send_file(path)

    async def websocket(self):
        key = await self.websocket_key()
        if not key:
            return

        await self.writeline(b'HTTP/1.1 101 Switching Protocols')
        await self.writeline(b'Upgrade: websocket')
        await self.writeline(b'Connection: upgrade')
        await self.writeline(b'Sec-WebSocket-Accept: %s' % key)
        await self.writeline()

        while True:
            x, pl = await self.reader.readexactly(2)
            if x != 0x82:
                break
            masking = pl & 0x80

            pl &= 0x7F
            if pl >= 126:
                break

            if masking:
                mask = await self.reader.readexactly(4)
                mesg = bytearray(await self.reader.readexactly(pl))
                for i in range(pl):
                    mesg[i] ^= mask[i & 3]
            else:
                mesg = await self.reader.readexactly(pl)

            self.server.message(mesg)

        await self.writer.wait_closed()

    async def websocket_key(self):
        key = None
        while True:
            line = str(await self.readline(), 'ascii').strip()
            if not line:
                return key
            try:
                idx = line.index(':')
            except ValueError:
                return
            header = line[:idx].strip().lower()
            if header != 'sec-websocket-key':
                continue
            key = line[idx+1:].strip()
            key = key + '258EAFA5-E914-47DA-95CA-C5AB0DC85B11'
            key = hashlib.sha1(key).digest()
            key = ubinascii.b2a_base64(key).strip()

    async def send_file(self, path, status=None):
        try:
            file = open(b'/public' + path, 'rb')
        except OSError:
            path = '/404.html'
            status = b'404 Not Found'
            file = open(path, 'rb')

        if path.endswith(b'.html'):
            self.mime = b'text/html'

        if not status:
            status = b'200 Ok'
        await self.start_response(status)

        while True:
            chunk = file.read(128)
            if not chunk:
                break
            await self.send_chunk(chunk)
        await self.end_response()
        file.close()

    async def readline(self):
        return (await self.reader.readline())[:-2]

    async def assume(self, cond, status):
        if not cond:
            await self.start_response(status)
            await self.end_response()

    async def start_response(self, status):
        print('response:', status)
        await self.writeline(b'HTTP/1.1 %s' % status)
        await self.writeline(b'Content-Type: %s' % self.mime)
        await self.writeline(b'Transfer-Encoding: chunked')
        await self.writeline(b'Connection: close')
        await self.writeline()

    async def end_response(self):
        await self.send_chunk(b'')

    async def send_chunk(self, chunk):
        await self.writeline(bytes(hex(len(chunk))[2:], 'ascii'))
        await self.write(chunk)
        await self.writeline()

    async def writeline(self, line=None):
        if line:
            await self.write(line)
        await self.write(b'\r\n')

    async def write(self, data):
        self.writer.write(data)
        await self.writer.drain()
