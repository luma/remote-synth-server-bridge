'use strict';

// https://lumbergh.tokbox.com/v2/prebuilts/webrtc?platform=iossim&revision=&version=43&configuration=Release
//
// Prebuilt.list().then(function(prebuilts) {
//   prebuilts.forEach(function(prebuilt) {
//    console.log(prebuilt);
//   });
// });
//
// var prebuilt = Prebuilt.get({
//   platform: ,
//   arch: ,
//   revision: ,
//   version: ,
//   configuration:
// });
//
// prebuilt.install(outputPath)

var Prebuilt = require('./Prebuilt');

var options = {
  arch: process.arch,
  platform: 'osx',
  configuration: 'Debug'
};

Prebuilt.get(options).then(function(prebuilt) {
  console.log('GOT IT', prebuilt, '\nInstalling...');
  prebuilt.install('../../ext/webrtc/');
}).catch(function(err) {
  console.error(err);
});