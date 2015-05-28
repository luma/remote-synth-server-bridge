'use strict';

var fs = require('fs-extra'),
    Promise = require("bluebird"),
    tmpFile = require('tmp').file;


// exports.extractTar = function(tarPath, destPath) {
//   return new Promise(function(resolve, reject) {
//     console.log('\n', tarPath, destPath);
//     function onError(err) {
//       console.error(err);
//       reject(err);
//     }

//     function onEnd() {
//       resolve();
//     }

//     var extractor = tar.Extract({path: destPath})
//       .on('error', onError)
//       .on('end', onEnd);

//     fs.createReadStream(tarPath)
//       .on('error', onError)
//       .pipe(extractor);
//   });
// };

exports.createTmpFile = function() {
  return new Promise(function(resolve, reject) {
    tmpFile(function(err, path, fd, cleanupCallback) {
      if (err) {
        reject(err);
        return;
      }

      resolve({
        tmpPath:path,
        fd:fd,
        cleanupCallback:cleanupCallback
      });
    });
  });
};
