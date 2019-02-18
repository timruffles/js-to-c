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
    execSync(`${__dirname}/../link-c ${cFile} ${binPath}`);
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
    expectedStatus = 0,
}) {
    const errors = compositeErrors(
        () => assert.isUndefined(error, 'unexpected execution error'),
        () => assert.isNull(signal, 'unexpected signal'),
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

    assert.deepEqual(errors.map(formatError), []);
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
