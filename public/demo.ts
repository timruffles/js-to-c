import { compileString } from "../src/js-to-c";
import CodeMirror from "codemirror"
import "codemirror/mode/javascript/javascript"
import "codemirror/mode/clike/clike"

const onIdle = (window.requestAnimationFrame as any)
    || window.requestAnimationFrame

main();

function main() {
    const editorEl = getEl('.editor')
    const outputEl = getEl('.output')

    const initialCode = `// write strict mode ES5 JavaScript here 
for(var i = 0; i < 5; i++) {
  console.log('Hi C!')
}

// wonder how closures work? 👀
function countDown(n) {
   return function() {
     return n-- <= 0 ? "done" : n
   }
}

var counter = countDown(3)
console.log(counter())

// spot the compiler optimisation in this multiply 👀
var theAnswer = Math.ceil(Math.random() * 42)

// control flow is a blast
switch(theAnswer) {
    case "we're all just, like, vibrations man":
        console.error("unlikely")
        break;
    case 42:
        console.log("okay, that's progress");
        break;
    default:
        console.log("look, life just isn't like that okay?");
        break;
}
`

    const compile = () => {
        try {
            const compiled = compileString(editor.getValue(), {})
            output.setValue(`// compiled C output\n${compiled}`)
        } catch (e) {
            output.setValue(`// You hit an error!\n// N.B JSC only supports ES5 strict mode code, so let, fat arrows, classes etc are not supported\n${e.stack}`)
        }
    }

    const output = CodeMirror(outputEl, {
        readOnly: true,
        value: "// C output here",
        mode: "clike",
    })

    const editor = CodeMirror(editorEl, {
        mode: "javascript",
        value: initialCode,
    })

    editor.on("change", compile)
    onIdle(compile)
}


function getEl(css: string): HTMLElement {
    const el = document.body.querySelector<HTMLElement>(css)
    if(!el) {
        throw Error(`failed to find node matching '${css}`)
    }
    return el
}
