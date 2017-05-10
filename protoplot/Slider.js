class Slider {
  constructor(rgb) {
    this.canvas = document.createElement('canvas');
    this.canvas.width = 80;
    this.canvas.height = 450;
    this.ctx = this.canvas.getContext('2d');
    this._value = 0.0;
    this._active = false;
    this.rgb = rgb;
    this.render();
  }

  set value(value) {
    this._value = Math.max(0, Math.min(1.0, value));
    this.render();
  }

  get value() { return this._value; }

  set active(active) {
    this._active = active;
    this.render();
  }

  get active() { return this._active; }

  render() {
    const ctx = this.ctx;
    const width = this.canvas.width;
    const height = this.canvas.height;

    const handleWidth = width;
    const handleHeight = width;
    const sliderHeight = height - handleHeight;

    ctx.clearRect(0, 0, width, height);

    ctx.save();

    this.ctx.fillStyle = `rgba(0, 0, 0, ${this.active ? 1.0 : 0.5})`;
    const lineHeight = height - (handleHeight);
    const lineWidth = 2;
    const lineX = width / 2 - lineWidth / 2;
    const lineY = (height - sliderHeight) / 2;
    this.ctx.fillRect(lineX, lineY, lineWidth, lineHeight);

    // handle
    this.ctx.fillStyle = `rgba(${this.rgb.join(',')}, ${this.active ? 1.0 : 0.5})`;
    const handleX = (width / 2) - (handleWidth / 2);
    const handleY = (1 - this.value) * lineHeight;

    // const handleY = (minY + lineHeight * (1 - this.value)) - (handleHeight / 2);
    this.ctx.fillRect(handleX, handleY, handleWidth, handleHeight);

    ctx.restore();
  }
}
