const SerialPort = require('serialport');
const WebSocket = require('ws');

const port = new SerialPort('/dev/cu.usbserial-A105AC2O', {
  baudRate: 115200,
  parser: SerialPort.parsers.readline('\n')
});

port.on('open', err => {
  if (err) {
    return console.log(`Error opening port: ${err.message}`);
  }
  console.log('serial port open');
});

const wsPort = 9999;
const server = new WebSocket.Server({
  port: wsPort,
  perMessageDeflate: false
});

console.log(`started WebSocket server at ws://localhost:${wsPort}`);

server.on('connection', ws => {
  ws.on('message', message => {
    console.log(`received message: ${message}`);
  });
  port.on('data', data => {
    const values = new Float32Array(data.split(',').map(s => parseFloat(s, 10)));
    if (ws.readyState === WebSocket.OPEN) {
      ws.send(values);
    }
  });
});
