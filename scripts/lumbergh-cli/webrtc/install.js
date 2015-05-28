'use strict';

var fs = require('fs-extra'),
    downloadAndUntar = require('../helpers/downloadAndUntar'),
    path = require('path');

var projectDir = path.dirname(path.dirname(path.dirname(fs.realpathSync(__filename))));

function onError(err) {
  console.error('ERROR:', err.message, '(' + err.code + ')', '\n');
  console.error(err.details);

  process.exit(-1);
}

module.exports = function(installDir, prebuilt) {
  installDir = installDir || projectDir + '/ext/webrtc/' + prebuilt.platform + '/' + prebuilt.arch;

  if(!fs.existsSync(installDir)) {
    fs.mkdirsSync(installDir);
  } else {
    fs.emptyDirSync(installDir);
  }

  console.log('Fetching WebRTC for', JSON.stringify(prebuilt));
  console.log('In directory', installDir);

  return downloadAndUntar(prebuilt.url, installDir).then(function() {
    console.log('Successfully fetched WebRTC. Pat yourself on the back!');
  }, onError);
};
