'use strict';

var fs = require('fs-extra'),
    http = require('http'),
    https = require('https'),
    Promise = require('bluebird');

var generateProgressIndicator = function(fileSize) {
  //1048576 - bytes in 1Megabyte
  var total = fileSize / 1048576;
  var ps = ' of ' + total.toFixed(2) + 'mb';
  var downloadedBytes = 0;

  return function(chunk) {
    downloadedBytes += chunk.length;
    process.stdout.cursorTo(0);
    process.stdout.write('Downloaded ' +
                          (100.0 * downloadedBytes / fileSize).toFixed(2) + '%\t' +
                          (downloadedBytes / 1048576).toFixed(2) + ps );
  };
};

module.exports = function(url, dest) {
  var onError = function(err) {
    fs.unlink(dest);
    reject(err);
  };

  return new Promise(function(resolve, reject) {
    var displayProgress;
    var file = fs.createWriteStream(dest);
    var protocol = (url.indexOf('https') === -1 ? http : https);
    var request = protocol.get(url).on('error', onError);

    request.on('response', function(response) {
      var fileSize = parseInt(response.headers['content-length'], 10);

      response
        .on('error', onError)
        .on('data', generateProgressIndicator(fileSize))
        .pipe(file);

      file.on('error', onError);
      file.on('finish', function() {
        file.close(function(err) {
          if (err) {
            reject(err);
            return;
          }

          resolve();
        });
      });
    });
  });
};




// exports.downloadFile = function(url, dest) {
//   return new Promise(function(resolve, reject) {
//     var file = fs.createWriteStream(dest);
//     var protocol = (url.indexOf('https') === -1 ? http : https);

//     var onError = function(err) {
//       fs.unlink(dest);
//       reject(err);
//     };

//     protocol.get(url, function(response) {
//       var fileSize = parseInt(response.headers['content-length'], 10);
//       var total = fileSize / 1048576; //1048576 - bytes in  1Megabyte
//       var downloadedBytes = 0;
//       var ps = ' of ' + total.toFixed(2) + 'mb';

//       response.pipe(file);

//       response.on('data', function(chunk) {
//         downloadedBytes += chunk.length;
//         process.stdout.cursorTo(0);
//         process.stdout.write('Downloaded ' + (100.0 * downloadedBytes / fileSize).toFixed(2) + '%\t' +
//                                           (downloadedBytes / 1048576).toFixed(2) + ps );
//       });

//       file.on('error', onError);
//       file.on('finish', function() {
//         file.close(function(err) {
//           if (err) {
//             reject(err);
//             return;
//           }

//           resolve();
//         });
//       });
//     }).on('error', onError);
//   });
// };

