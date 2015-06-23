require('segfault-handler').registerHandler();
var prettyJson = require('prettyjson');
var SynthBridge = require('../build/Debug/SynthBridge');
var MediaDevices = new SynthBridge.MediaDevices();
var peer = new SynthBridge.Peer();

function renderObject(obj) {
  console.log(prettyJson.render(obj), '\n');
}

MediaDevices.enumerateDevices({video: true, audio: true}, function(err, devices) {
  console.log('Enumerate Devices');
  if (err) {
    console.error(err);
    return;
  }

  devices.forEach(function(device) {
    renderObject(device);
  });

  var constraints = {
    video: '0x1a11000005ac8510',
    audio: '60'
  };

  MediaDevices.getMedia(constraints, function(err, mediaDevice) {
    if (err) {
      console.error('MediaDevices.getMedia ::', err);
      return;
    }

    console.info('Got Media:');
    renderObject(mediaDevice);
  });
});

// MediaDevices.on('devicesChanged', function() {
//   // you would enumerate devices here
// });

peer.onSignal(function(event) {
  renderObject(event);
});


setTimeout(function() {
  peer.close();
  peer = null;
  MediaDevices = null;
}, 20000);
