'use strict';

const fs = require('fs');
const path = require('path');
const toml = require('toml');

main();

function main() {
    const tests = getTests();
    tests.forEach(updateFixture);
}

function getTests() {
    const tomls = fs.readdirSync(__dirname).filter(f => path.extname(f) === '.toml');
    return tomls.map(tf => {
        const name = path.basename(tf, '.toml');
        const config = toml.parse(fs.readFileSync(__dirname + '/' + tf, { encoding: 'utf8' }))

        const tests = [...objectEntries(config)].map(([testName,test]) => (
            {
                name: testName,
                fixtureFile: `${__dirname}/fixtures/${testNameToFileName(testName)}.js`,
                ...test,
            }
        ));

        return {
            name,
            testFile: `${__dirname}/fixtures/${name}Test.js`,
            tests,
        };
    });
}

function updateFixture({ name, tests, testFile }) {
    for(const [testName,{fixtureFile,example}] of objectEntries(tests)) {
        fs.writeFileSync(fixtureFile, example);
    }

    const testsSrc = tests.map((testSetup) => {

        const {name,fixtureFile, output, outputMatch, errorOut, errorOutMatch, expectedStatus = 0, env = {}} = testSetup;

        if(output && outputMatch) {
            throw Error(`${fixtureFile}/${name} specifies both output + output match, only one can be used`);
        }
        if(errorOut && errorOutMatch) {
            throw Error(`${fixtureFile}/${name} specifies both errorOut + errorOut match, only one can be used`);
        }

        return `it("${name}", () => {
            const cFile = compile('${fixtureFile}');
            const executable = link(cFile);
            const spawnResult = runExecutable(executable, { env: ${JSON.stringify(env)} });
            assertOutput(
              spawnResult,
              ${JSON.stringify(testSetup, null, '\t')}
            );
          });`
    }).join('\n\n');

    const testFileSrc = `'use strict';
    const { compile, runExecutable, link, assertOutput } = require("../helpers.js");

    describe("${name}", () => {
      ${testsSrc}
    });`

    fs.writeFileSync(testFile, testFileSrc);
}

function testNameToFileName(tn) {
    return tn.replace(/\s+/g, '-').replace(/[^\w-]/g, '');
}

function *objectEntries(obj) {
    for(const p in obj) yield [p,obj[p]]; 
}
