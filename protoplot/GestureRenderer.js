class GestureRenderer {
  constructor(canvas) {
    this.canvas = document.createElement('canvas');
    this.ctx = this.canvas.getContext('2d');
    this.canvas.width = 800;
    this.canvas.height = 450;
    this.gestures = [];
  }

  renderData(data, style='#000000', lineWidth=1) {
    const width = this.ctx.canvas.width;
    const height = this.ctx.canvas.height;
    const stepX = width / data.length;

    this.ctx.save();
    this.ctx.strokeStyle = style;
    this.ctx.lineWidth = lineWidth;
    this.ctx.beginPath();
    data.forEach((p, i) => {
      const x = stepX * i;
      const y = height - (p / 1023) * height;
      this.ctx.lineTo(x, y);
    });
    this.ctx.stroke();
    this.ctx.restore();
  }

  averageData(data) {
    return data.reduce((acc, cur) => cur.map((v, i) => (v + cur[i]) / 2));
  }

  render(liveData, recording, closestIdx) {
    const width = this.ctx.canvas.width;
    const height = this.ctx.canvas.height;
    this.ctx.clearRect(0, 0, width, height);

    this.renderData(liveData, recording ? 'rgb(249, 182, 118)' : '#000000');

    const haveGestures = this.gestures.length > 0;
    if (haveGestures) {
      if (!recording && closestIdx !== undefined) {
        const closest = this.gestures[closestIdx];
        this.renderData(this.averageData(closest), 'rgb(43, 156, 212)', 5);
      }
      this.gestures.forEach(g => this.renderData(this.averageData(g), 'rgba(0, 0, 0, 0.2)'));
    }
  }

  setGestures(gestures) { this.gestures = gestures; }
}
