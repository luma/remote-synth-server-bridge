'use strict';

var crossSpawn = require('../../node_modules/cross-spawn'),
    q = require('../../node_modules/q');

exports.spawnCommand = function(cmd, args, cwd) {
  var deferred = q.defer(),
      stdout = '',
      stderr = '';

  var process = crossSpawn(cmd, args, {cwd: cwd});
  process.stdout.on('data', function (data) {
    stdout += data;
  });

  process.stderr.on('data', function (data) {
    stderr += data;
  });

  var reject = function(fullCmd, errorMessage) {
    var error = new Error('Failed to execute command "' + fullCmd +
                              '" in "' + cwd +
                              '", failed with error ' + errorMessage);
    error.code = errorMessage;
    error.details = stdout + '\n\n' + stderr;
    return deferred.reject(error);
  };

  // If there is an error spawning the command, reject the promise
  process.on('error', function (errorMessage) {
    var fullCmd = [cmd];
    if (args) fullCmd.push(args.slice());
    fullCmd = fullCmd.join(' ');

    return reject(fullCmd, errorMessage);
  });

  process.on('close', function (code) {
    if (code) {
      var fullCmd = [cmd];
      if (args) fullCmd.push(args.slice());
      fullCmd = fullCmd.join(' ');

      return reject(fullCmd, code);
    }

    deferred.resolve(stdout);
  });

  return deferred.promise;
};
