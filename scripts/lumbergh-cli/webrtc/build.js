'use strict';

//// DO NOT USE: TOTALLY OUT OF DATE

var fs = require('fs-extra'),
    spawnCommand = require('../helpers/spawnCommand'),
    path = require('path');

var DEFAULT_CONFIG = 'Debug';

var projectDir = path.dirname(path.dirname(path.dirname(fs.realpathSync(__filename)))),
    srcDir = projectDir + '/ext/webrtc/src',
    repo = 'https://github.com/luma/webrtc.git';

path.dirname(fs.realpathSync(__filename))

function onError(err) {
  console.error('ERROR:', err.message, '(' + err.code + ')', '\n');
  console.error(err.details);

  process.exit(-1);
}

module.exports = function(config, options) {
  if(!fs.existsSync(srcDir)) {
    throw new Error('You must fetch WebRTC before building it');
  }

  if (!config) config = DEFAULT_CONFIG;

  console.log('Building WebRTC in ' + config + ' config');

  return spawnCommand('ninja', ['-C', 'out/' + config], srcDir).then(function() {
    console.log('Successfully build WebRTC!');
  }, onError);
};
