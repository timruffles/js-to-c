'use strict';
const { writeFileSync } = require('fs');
const { execSync, execFileSync } = require('child_process');
const assert = require('assert');
const { compileFile } = require('..');
const path = require('path');

exports.compile = function(file) {
    const cSrc = compileFile(file);
    const output = pathInFixtures(`${path.basename(file)}.c`);
    writeFileSync(output, cSrc);
    return output;
}

exports.link = function(cFile) {
    const binPath = pathInFixtures(path.basename(cFile, '.c'));
    execSync(`clang -Weverything ${cFile} -o ${binPath}`);
    return binPath;
}

exports.runExecutable = function(path) {
    const stdout = execFileSync(path); 
    return { stdout };
}

exports.assertOutputEqual = function(actual, expected) {
    assert.equal(actual.trim(), expected.trim());
}

function pathInFixtures(fn) {
    return `${__dirname}/fixtures/${fn}`;
}
