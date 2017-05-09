importScripts('convnet.js', 'Gestures.js');

let gestures;
function create(numInputs) {
  gestures = new Gestures(numInputs);
  gestures.addTrainingCallback(() => postMessage({key: 'trained'}));
  gestures.addStopRecordingCallback(() => postMessage({ key: 'stoppedRecording' }));
}


const msgActions = {
  toggleRecording: () => gestures.toggleRecording(),
  reset: () => gestures.reset(),
  train: () => gestures.train(),
  data: (e) => {
    if (gestures === undefined) { create(e.data.value.length); }

    gestures.update(e.data.value);
    postMessage({ key: 'closest', value: gestures.currentGesture });
  },
  getGestures: () => {
    postMessage({ key: 'gestures', value: gestures.gestures });
  },
  recording: () => postMessage({
    key: 'recording',
    value: gestures === undefined ? false : gestures.recording
  })
};

onmessage = e => {
  const key = e.data.key;
  // console.log(`worker: ${key}`);
  if (msgActions.hasOwnProperty(key)) { msgActions[key](e); }
};
