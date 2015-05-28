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
// {
//   "id":"webrtc",
//   "url":"https://s3.amazonaws.com/artifact.tokbox.com/ext/webrtc/43/iossim/webrtc-iossim-43-011-0a8a5e5bd1cec4782d1aa30fec5e85dddfc0d2b4-Release/fattycakes-0a8a5e5bd1cec4782d1aa30fec5e85dddfc0d2b4.tar.bz2?AWSAccessKeyId=AKIAJSFKVALMI4KCEVRQ\u0026Expires=1432682110\u0026Signature=8knLD%2B%2BKccZOEjXedkgiHgzFz0k%3D",
//   "webrtc_revision":"0a8a5e5bd1cec4782d1aa30fec5e85dddfc0d2b4",
//   "webrtc_version":"43",
//   "webrtc_buildscript_revision":"b33b62483ce0962b883d40f94659f064607db605",
//   "webrtc_commit_position":"011",
//   "configuration":"Release"
// }


var https = require('https');
var Promise = require('bluebird');
var querystring = require('querystring');
var installWebrtc = require('./install');
var prettyjson = require('prettyjson');

function request(options) {
  return new Promise(function(resolve, reject) {
    var responseBody = [];

    https.get(constructLumberghUrl(options), function(response) {
      response
        .on('data', function(chunk) {
          responseBody.push(chunk.toString());
        })
        .on('end', function() {
          resolve(JSON.parse(responseBody.join('')));
        });
    }).on('error', reject)
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
      this[key] = props[key];
    }
  }
}

Prebuilt.prototype.install = function(outputPath) {
  return installWebrtc(outputPath, this);
};

Prebuilt.prototype.toString = function prebuiltToString() {
  return prettyjson.render(this);
};

Prebuilt.install = function(outputPath, options) {
  return Prebuilt.get(options).then(function(prebuilt) {
    return prebuilt.install(outputPath);
  })
};

Prebuilt.get = function(options) {
  return request(options).then(function(result) {
    if (result && Object.keys(result).length > 0) {
      return new Prebuilt(result);
    } else {
      return void 0;
    }
  });
};

Prebuilt.list = function(options) {

};



module.exports = Prebuilt;