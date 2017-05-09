class Gestures {
  constructor(numInputs) {
    this.liveData = [];
    this.recording = false;
    this.numInputs = numInputs;
    this.maxGestures = 4;
    this.gestures = [];
    this.trainingLabel = 0;
    this.trainingCount = 0;
    this.iterations = 1000;
    this.createClassifier(this.numInputs, this.maxGestures);
    this.trainingCallbacks = [];
    this.stopRecordingCallbacks = [];
  }

  reset() {
    this.gestures.length = 0;
    delete this.net;
    this.createClassifier(this.numInputs, this.maxGestures);
  }

  startRecording() {
    this.trainingCount = 0;
    this.recording = true;
    this.trainingLabel = this.gestures.length;
  }

  stopRecording() {
    this.recording = false;
    this.stopRecordingCallbacks.forEach(cb => cb());
  }

  toggleRecording() {
    this.recording ? this.stopRecording() : this.startRecording();
    if (!this.recording) { this.train(); }
  }

  addTrainingCallback(func) { this.trainingCallbacks.push(func); }

  addStopRecordingCallback(func) { this.stopRecordingCallbacks.push(func); }

  train() {
    console.log('started training');
    for (let i = 0; i < this.iterations; i++) {
      this.gestures.forEach((gesture, label) => {
        gesture.forEach(data => this.trainClassifier(data, label));
      });
    }
    console.log('finished training');
    this.trainingCallbacks.forEach(cb => cb());
  }

  storeData(data, label) {
    if (this.gestures[label] === undefined) { this.gestures[label] = []; }
    this.gestures[label].push(data);
  }

  createClassifier(dataSize, numClasses) {
    const layer_defs = [];
    layer_defs.push({type:'input', out_sx:1, out_sy:1, out_depth: dataSize});
    layer_defs.push({type:'fc', num_neurons:6, activation: 'tanh'});
    layer_defs.push({type:'fc', num_neurons:2, activation: 'tanh'});
    layer_defs.push({type:'softmax', num_classes: numClasses});

    this.net = new convnetjs.Net();
    this.net.makeLayers(layer_defs);

    this.trainer = new convnetjs.SGDTrainer(this.net, {
      learning_rate:0.01,
      momentum:0.1,
      batch_size:10,
      l2_decay:0.001
    });
  }

  getInputVol(data) {
    const norm = data.map(v => v / 1023);
    return new convnetjs.Vol(norm);
  }

  trainClassifier(data, label) {
    const vol = this.getInputVol(data);
    this.trainer.train(vol, label);
  }

  getPeak(gesture) {
    const val = Math.max(...gesture);
    const idx = gesture.indexOf(val);
    return { idx, val };
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

  closestGesture(input) {
    if (this.gestures.length === 0) { return; }
    const vol = this.getInputVol(input);
    const scores = this.net.forward(vol);
    const max = scores.w.indexOf(Math.max(...scores.w));
    return max;
  }

  get currentGesture() { return this.closestGesture(this.liveData); }

  update(data) {
    this.liveData = data;
    if (this.recording) {
      this.storeData(data, this.trainingLabel);
      this.trainingCount++;
      if (this.trainingCount === 50) {
        this.stopRecording();
        this.train();
      }
    }
  }
}
