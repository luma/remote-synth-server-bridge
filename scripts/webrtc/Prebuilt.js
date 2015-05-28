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
//
//

var https = require('https'),
    Promise = require('bluebird'),
    querystring = require('querystring'),
    installWebrtc = require('./install');

function request(options) {
  return new Promise(function(resolve, reject) {
    https.get(constructLumberghUrl(options))
        .on('error', reject)
        .on('finish', resolve);
  });
}

var BASE_URL = 'https://lumbergh.tokbox.com/v2/prebuilts/webrtc';

// Of the form:
//   https://lumbergh.tokbox.com/v2/prebuilts/webrtc?platform=iossim&revision=&version=43&configuration=Release
//
function constructLumberghUrl(options) {
  return BASE_URL + '?' + querystring.stringify(options);
}




function Prebuilt(props) {
  for (var key in props) {
    if (props.hasOwnProperty(key)) {
      this[key] = props;
    }
  }
}

Prebuilt.prototype.install = function(outputPath) {
  return installWebrtc(this);
};


Prebuilt.install = function(outputPath, options) {
  return Prebuilt.get(options).then(function(prebuilt) {
    return prebuilt.install(outputPath);
  })
};

Prebuilt.get = function(options) {
  return request(options).then(function(result) {
    console.log(result);
    return new Prebuilt(result);
  });
};

Prebuilt.list = function(options) {

};



module.exports = Prebuilt;