class Controller {
  /** @type {Map<string, number>} */
  sliders;
  /** @type {Map<string, number>} */
  sliders_last;
  /** @type {boolean} */
  needs_update;
  /** @type {?WebSocket} */
  socket;

  constructor() {
    this.sliders = new Map()
    this.sliders_last = new Map();
    this.needs_update = false;
  }

  attach() {
    for (const slider of document.querySelectorAll('input[type=range]')) {
      this.read(slider);
      slider.addEventListener('input', event => this.read(event.currentTarget));
    }

    const pitch = document.getElementById('slider:pitch');
    document.getElementById('label:pitch')
      .addEventListener('click', () => pitch.value = 0);

    /*
    const yaw = document.getElementById('slider:yaw');
    yaw.addEventListener('mouseup', () => yaw.value = 0.5);
    */

    setInterval(() => this.send(), 100);

    const host = window.location.host;
    if (!host)
      return;

    const socket = new WebSocket(`ws://${host}/ws`);
    socket.addEventListener('open', event => this.opened(event));
  }

  /**
    @param {HTMLInputElement} slider
   */
  read(slider) {
    const key = slider.getAttribute("id").replace("slider:", "");
    const value = Number(slider.value);
    console.debug(`read ${key}: ${value}`);

    this.sliders.set(key, value);

    if (this.sliders_last.get(key) !== value) {
      this.sliders_last.set(key, value);
      this.needs_update = true;
    }
  }

  opened(event) {
    this.socket = event.currentTarget;
    this.socket.addEventListener('close', () => {
      delete this.socket;
    });
  }

  send() {
    if (!this.needs_update)
      return;
    this.needs_update = false;

    const power = this.sliders.get('power');
    const yaw = this.sliders.get('yaw');
    const pitch = this.sliders.get('pitch');

    const left = (Math.sqrt(1 - yaw) * power) * 127;
    const right = (Math.sqrt(yaw) * power) * 127;
    const back = pitch * 127;

    const motors = new Int8Array([left, right, back]);
    console.debug("motors [left, right, back]", motors);

    if (this.socket)
      this.socket.send(motors);
  }
}

(new Controller()).attach();
