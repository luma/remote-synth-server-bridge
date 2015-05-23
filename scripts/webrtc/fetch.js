'use strict';

var fs = require('fs-extra'),
    spawnCommand = require('../helpers/spawnCommand').spawnCommand,
    path = require('path');

var DEFAULT_BRANCH = 'develop';

var projectDir = path.dirname(path.dirname(path.dirname(fs.realpathSync(__filename)))),
    webrtcDir = projectDir + '/ext/webrtc',
    srcDir = projectDir + '/src',
    repo = 'https://github.com/luma/webrtc.git';

function onError(err) {
  console.error('ERROR:', err.message, '(' + err.code + ')', '\n');
  console.error(err.details);

  process.exit(-1);
}

module.exports = function(branch, options) {
  if (!branch) branch = DEFAULT_BRANCH;
  var tasks;

  if(!fs.existsSync(webrtcDir)) {
    console.log('Fetching branch ' + branch + ' of WebRTC');
    console.log('In directory', webrtcDir);

    fs.mkdirsSync(webrtcDir);
    tasks = spawnCommand('git', ['clone', '--branch=' + branch, '--progress',  repo, '.'], webrtcDir);

  } else {
    console.log('Updating branch ' + branch + ' of WebRTC');
    console.log('In directory', webrtcDir);
    tasks = spawnCommand('git', ['pull', 'origin', branch], webrtcDir);
  }

  tasks = tasks.then(function() {
    console.log('Syncing webrtc...');
    return spawnCommand('python', ['src/webrtc/build/gyp_webrtc'], webrtcDir);
  });

  return tasks.then(function() {
    console.log('Successfully pulled down WebRTC. Pat yourself on the back!');
  }, onError);
};
