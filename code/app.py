from machine import Pin, PWM
from server import Server


PWM_FREQ = 500


class App(Server):
    def __init__(self):
        self.left = PWM(Pin(12), freq=PWM_FREQ, duty=0)
        self.right = PWM(Pin(15), freq=PWM_FREQ, duty=0)
        self.back = PWM(Pin(13), freq=PWM_FREQ, duty=0)
        self.back_reverse = Pin(14, Pin.OUT, value=0)

    def message(self, message):
        if len(message) != 3:
            return

        left, right, back = message

        self.left.duty(left << 2)
        self.right.duty(right << 2)

        if back < 128:
            back = 127 - back
            self.back_reverse.on()
        else:
            back -= 128
            self.back_reverse.off()
        self.back.duty(back << 3)
