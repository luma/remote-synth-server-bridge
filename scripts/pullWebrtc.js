#!/usr/bin/env node
(function() {
  'use strict';

  var spawnCommand = require('./helpers/spawnCommand').spawnCommand,
      fs = require('../node_modules/fs-extra');

  var sourceDir = process.cwd(),
      webrtcDir = sourceDir + '/ext/webrtc',
      webrtcSrcDir = sourceDir + '/ext/webrtc/src',
      tasks;

  function onError() {
    console.error('ERROR:', arguments, '\n');
    process.exit(-1);
  }

  if(!fs.existsSync(webrtcDir)) {
    fs.mkdirSync(webrtcDir);
    console.log('Fetching webrtc...');
    tasks = spawnCommand('fetch', ['webrtc'], webrtcDir);
  }
  else {
    console.log('Updating webrtc...');
    tasks = spawnCommand('git', ['pull', 'origin', 'master'], webrtcSrcDir).then(function() {
      console.log('Syncing webrtc...');
      return spawnCommand('gclient', ['sync'], webrtcSrcDir)
    });
  }

  tasks.then(function() {
    console.log('Successfully pulled down WebRTC. Pat yourself on the back!');
  }, onError);
})();
