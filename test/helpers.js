'use strict';
const { writeFileSync, unlinkSync } = require('fs');
const { execSync, spawnSync } = require('child_process');
const assert = require('chai').assert;
const { compileFile } = require('..');
const path = require('path');

exports.compile = function(file) {
    const output = pathInFixtures(`${path.basename(file, '.js')}.c`);
    try {
        // ensure later steps don't run a stale exe
        unlinkSync(output);
    } catch(e) {
        if(e.code !== 'ENOENT') {
            throw e;
        }
    }

    const cSrc = compileFile(file);
    writeFileSync(output, cSrc);
    return output;
}

exports.link = function(cFile) {
    const binPath = pathInFixtures(path.basename(cFile, '.c'));
    execSync(`${__dirname}/../runtime/scripts/c-to-exec ${cFile} ${binPath}`);
    return binPath;
}

exports.runExecutable = function(path, opts) {
    try {
        return spawnSync(path, opts);
    } catch(e) {
        console.log(e);
        return e;
    }
}
exports.debugExecutable = function(path, opts) {
    return exports.runExecutable("/usr/bin/lldb", ["-S", "./test/lldb.conf", "-o", "run", path], opts);
}

const MISSING_ARG = {}

exports.assertOutput = function({
    stderr,
    stdout,
    status,
    error,
    signal,
}, {
    // ''/Match pairs are mutually exclusive
    output,
    outputMatch,
    errorOut,
    errorOutMatch,
    expectedStatus = MISSING_ARG,
    expectedSignal = MISSING_ARG,
    expectJsException,
}, {
    cFile,
}) {

    if(expectJsException) {
        // exits with 128 + numeric val of signal (SIGABT), which gives exit code 134 (ignored by node and treated as null)
        expectedSignal = 'SIGABRT'
        errorOutMatch = expectJsException;
        assert(expectedStatus == MISSING_ARG)
        expectedStatus = null
    } else if(expectedStatus === MISSING_ARG) {
        expectedStatus = 0
    }

    const errors = compositeErrors(
        () => assert.isUndefined(error, 'unexpected execution error'),
        () => expectedSignal === MISSING_ARG 
          ? assert.isNull(signal, 'unexpected signal')
          : assert.equal(signal, expectedSignal, 'wrong signal'),
        () => {
            if(expectedStatus != null) {
                assert.equal(status, expectedStatus, 'unexpected status');
            }
        },
        () => {
            if(output != null) {
                assert.equal(stdout.toString().trim(), output);
            }
        },
        () => {
            if(outputMatch != null) {
                assert.match(stdout.toString(), new RegExp(outputMatch, 'm'));
            }
        },
        () => {
            if(errorOut != null) {
                assert.equal(stderr.toString().trim(), errorOut);
            }
        },
        () => {
            if(errorOutMatch != null) {
                
                assert.match(stderr.toString(), new RegExp(errorOutMatch, 'm'));
            }
        },
    )

    if(process.env.TEST_DEBUG) {
        process.stdout.write(stdout);
        process.stdout.write(stderr);
    }

    const debugCmd = `./scripts/debug-system-test ${cFile}`
    assert.deepEqual(errors.map(formatError), [], `failed, to debug: \n${debugCmd}`);
}

function formatError({stack, message, name}) {
    return {
        name,
        message,
        stack,
    }
}

function compositeErrors(...tests) {
    return tests.reduce((es, t) => {
        try {
            t()
        } catch(e) {
            es.push(e)
        }
        return es
    }, [])
  
}

function pathInFixtures(fn) {
    return `${__dirname}/fixtures/${fn}`;
}
