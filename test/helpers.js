'use strict';
const { writeFileSync } = require('fs');
const { execSync, execFileSync } = require('child_process');
const assert = require('assert');
const { compileFile } = require('..');
const path = require('path');

exports.compile = function(file) {
    const cSrc = compileFile(file);
    const output = pathInFixtures(`${path.basename(file, '.js')}.c`);
    writeFileSync(output, cSrc);
    return output;
}

exports.link = function(cFile) {
    const binPath = pathInFixtures(path.basename(cFile, '.c'));
    execSync(`${__dirname}/../link-c ${cFile} ${binPath}`);
    return binPath;
}

exports.runExecutable = function(path) {
    const stdout = execFileSync(path).toString(); 
    return { stdout };
}

exports.assertOutputEqual = function(actual, expected) {
    assert.equal(actual.trim(), expected.trim());
}

function pathInFixtures(fn) {
    return `${__dirname}/fixtures/${fn}`;
}
