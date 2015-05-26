require('segfault-handler').registerHandler();

var SynthBridge = require('../build/Debug/SynthBridge'),
    // MediaDevices = SynthBridge.MediaDevices,
    peer = new SynthBridge.Peer();

// MediaDevices.enumerateDevices(function(devices) {
// });

// MediaDevices.on('devicesChanged', function() {
//   // you would enumerate devices here
// });

peer.onSignal(function(event) {
  console.log(event);
});

setTimeout(function() {
  peer.close();
  peer = null;
}, 10000);
