'use strict';

var fs = require('fs-extra'),
    spawnCommand = require('../helpers/spawnCommand'),
    fileHelpers = require('../helpers/fileHelpers'),
    downloadFile = require('../helpers/downloadFile'),
    createTmpFile = fileHelpers.createTmpFile,
    http = require('http'),
    https = require('https'),
    Promise = require('bluebird'),
    tar = require('tar'),
    zlib = require('zlib');


// module.exports = function(url, dest) {
//   return new Promise(function(resolve, reject) {
//     var displayProgress;
//     var protocol = (url.indexOf('https') === -1 ? http : https);
//     var request = protocol.get(url)
//                       .on('error', onErrorCurry(reject, 'REQUEST'));

//     var gunzip = zlib.createGunzip()
//                      .on('error', onErrorCurry(reject, 'ZLIB'));

//     var extractTar = tar.Extract({path: dest})
//             .on('error', onErrorCurry(reject, 'EXTRACTOR'))
//             .on('finish', resolve);

//     request.on('response', function(response) {
//       displayProgress = generateProgressIndicator(parseInt(response.headers['content-length'], 10));

//       response
//         .on('error', onErrorCurry(reject, 'RESPONSE'))
//         .on('data', displayProgress)
//         .pipe(zlib.Unzip())
//         .pipe(extractTar);
//     });
//   });
// };

module.exports = function(url, dest) {
  return createTmpFile().then(function(args) {
    return downloadFile(url, args.tmpPath).then(function() {
      // @fixme This will not work on Windows
      return spawnCommand('tar', ['-xjf' , args.tmpPath], dest).then(function() {
        args.cleanupCallback();
      });
    });
  });
}
