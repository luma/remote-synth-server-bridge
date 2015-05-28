'use strict';

var crossSpawn = require('cross-spawn'),
    Promise = require("bluebird");;

module.exports = function(cmd, args, cwd) {
  return new Promise(function(resolve, reject) {
    var stdout = '',
        stderr = '';

    var fullCmd = [cmd];
    if (args) fullCmd.push.apply(fullCmd, args.slice());
    fullCmd = fullCmd.join(' ');
    console.log('\n', fullCmd);

    var process = crossSpawn(cmd, args, {cwd: cwd});
    process.stdout.on('data', function (data) {
      stdout += data;
      console.log(data.toString());
    });

    process.stderr.on('data', function (data) {
      stderr += data;
      console.error(data.toString());
    });

    var rejectWithError = function(fullCmd, errorMessage) {
      var error = new Error('Failed to execute command "' + fullCmd +
                                '" in "' + cwd +
                                '", failed with error ' + errorMessage);
      error.code = errorMessage;
      error.details = stdout + '\n\n' + stderr;
      return reject(error);
    };

    // If there is an error spawning the command, reject the promise
    process.on('error', function (errorMessage) {
      var fullCmd = [cmd];
      if (args) fullCmd.push.apply(fullCmd, args.slice());
      fullCmd = fullCmd.join(' ');

      return rejectWithError(fullCmd, errorMessage);
    });

    process.on('close', function (code) {
      if (code) {
        var fullCmd = [cmd];
        if (args) fullCmd.push.apply(fullCmd, args.slice());
        fullCmd = fullCmd.join(' ');

        return rejectWithError(fullCmd, code);
      }

      resolve(stdout);
    });
  });
};
