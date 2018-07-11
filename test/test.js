'use strict';

const fs = require('fs');
const path = require('path');
const toml = require('toml');

function main() {
    updateFixtures();
    runTests();
}

function updateFixtures() {
    const tomls = fs.readdirSync(__dirname).filter(f => path.extname(f) === '.toml');

    tomls.forEach(tf => {
        const tests = toml.parse(fs.readFileSync(__dirname + '/' + tf, { encoding: 'utf8' }));

        for(const testName in tests) {
            const testBody = tests[testName];

            fs.writeFileSync(`${__dirname}/fixtures/${testNameToFileName(testName)}.js`, testBody);
        }
    });

    function testNameToFileName(tn) {
        return tn.replace(/\s+/g, '-').replace(/[^\w-]/g, '');
    }
}
