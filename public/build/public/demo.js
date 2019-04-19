"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var tslib_1 = require("tslib");
var js_to_c_1 = require("../src/js-to-c");
var codemirror_1 = tslib_1.__importDefault(require("codemirror"));
require("codemirror/mode/javascript/javascript");
require("codemirror/mode/clike/clike");
var onIdle = window.requestAnimationFrame
    || window.requestAnimationFrame;
main();
function main() {
    var editorEl = getEl('.editor');
    var outputEl = getEl('.output');
    var initialCode = "// write strict mode ES5 JavaScript here \nfor(var i = 0; i < 5; i++) {\n  console.log('Hi C!')\n}\n\n// wonder how closures work? \uD83D\uDC40\nfunction countDown(n) {\n   return function() {\n     return n-- <= 0 ? \"done\" : n\n   }\n}\n\nvar counter = countDown(3)\nconsole.log(counter())\n\n// spot the compiler optimisation in this multiply \uD83D\uDC40\nvar theAnswer = Math.ceil(Math.random() * 42)\n\n// control flow is a blast\nswitch(theAnswer) {\n    case \"we're all just, like, vibrations man\":\n        console.error(\"unlikely\")\n        break;\n    case 42:\n        console.log(\"okay, that's progress\");\n        break;\n    default:\n        console.log(\"look, life just isn't like that okay?\");\n        break;\n}\n";
    var compile = function () {
        try {
            var compiled = js_to_c_1.compileString(editor.getValue(), {});
            output.setValue("// compiled C output\n" + compiled);
        }
        catch (e) {
            output.setValue("// You hit an error!\n// N.B JSC only supports ES5 strict mode code, so let, fat arrows, classes etc are not supported\n" + e.stack);
        }
    };
    var output = codemirror_1.default(outputEl, {
        readOnly: true,
        value: "// C output here",
        mode: "clike",
    });
    var editor = codemirror_1.default(editorEl, {
        mode: "javascript",
        value: initialCode,
    });
    editor.on("change", compile);
    onIdle(compile);
}
function getEl(css) {
    var el = document.body.querySelector(css);
    if (!el) {
        throw Error("failed to find node matching '" + css);
    }
    return el;
}
