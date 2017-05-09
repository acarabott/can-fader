class Touch {
  constructor() {
    this.canvas = document.createElement('canvas');
    this.ctx = this.canvas.getContext('2d');
    this.canvas.width = 800;
    this.canvas.height = 450;
    this.data = { min: [], touch: [], thresh: [], max: [] };
    this.histSize = 100;
  }

  render() {
    const width = this.ctx.canvas.width;
    const height = this.ctx.canvas.height;
    this.ctx.clearRect(0, 0, width, height);

    this.ctx.save();
    const colors = [
      'rgb(0, 0, 0)',
      'rgb(43, 156, 212)',
      'rgb(249, 182, 118)',
      'rgb(43, 212, 156)'
    ];

    Object.keys(this.data).forEach((key, i) => {
      this.ctx.strokeStyle = colors[i];
      this.ctx.beginPath();
      this.data[key].forEach((v, j) => {
        const x = (j / this.histSize) * width;
        const y = height - ((v / 1023) * height);
        this.ctx.lineTo(x, y);
      });
      this.ctx.stroke();
    });

    this.ctx.restore();
  }

  update(min, touch, thresh, max) {
    const formatted = { min, touch, thresh, max };
    Object.keys(this.data).forEach(key => {
      this.data[key].push(formatted[key]);
      this.data[key].splice(0, this.data[key].length - this.histSize);
    });
    this.render();
  }
}
