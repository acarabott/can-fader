class Gestures {
  constructor() {
    this.canvas = document.createElement('canvas');
    this.ctx = this.canvas.getContext('2d');
    this.canvas.width = 800;
    this.canvas.height = 450;
    this.liveData = [];
    this.gestures = [];

    document.body.addEventListener('keydown', event => {
      const actions = {
        s: () => this.gestures.push(this.liveData.slice()),
        r: () => this.gestures.length = 0
      };
      if (actions.hasOwnProperty(event.key)) { actions[event.key](); }
    });
  }

  getPeak(gesture) {
    const val = Math.max(...gesture);
    const idx = gesture.indexOf(val);
    return { val, idx };
  }

  getGestureDistance(src, target, method='squared') {
    if (src.length !== target.length) { return Infinity; }

    if (method === 'squared') {
      return src.map((v, i) => Math.pow(v - target[i], 2)).reduce((p, c) => p + c, 0);
    }

    if (method === 'abs') {
      return src.map((v, i) => Math.abs(v - target[i])).reduce((p, c) => p + c, 0);
    }

    if (method === 'peakSquared') {
      const srcPeak = this.getPeak(src);
      const targetPeak = this.getPeak(target);

      return Math.pow((srcPeak.val - targetPeak.val) / 1023, 2) +
             Math.pow((srcPeak.idx - targetPeak.idx) / src.length, 2);
    }

    if (method === 'peakAbs') {
      const srcPeak = this.getPeak(src);
      const targetPeak = this.getPeak(target);

      return Math.abs((srcPeak.val - targetPeak.val) / 1023) +
             Math.abs((srcPeak.idx - targetPeak.idx) / src.length);
    }
  }

  closestGesture(input, gestures) {
    const errors = gestures.map(g => this.getGestureDistance(input, g, 'peakSquared'));
    const closest = gestures[errors.indexOf(Math.min(...errors))];
    return closest;
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

  render() {
    const width = this.ctx.canvas.width;
    const height = this.ctx.canvas.height;
    this.ctx.clearRect(0, 0, width, height);

    this.renderData(this.liveData, '#000000');
    const closest = this.closestGesture(this.liveData, this.gestures);
    if (closest !== undefined) {
      this.renderData(closest, 'rgb(43, 156, 212)', 5);
    }
    this.gestures.forEach(g => this.renderData(g, 'rgba(0, 0, 0, 0.2)'));
  }

  update(data) {
    this.liveData = data;
    this.render();
  }
}
