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

    const initialCode = `// write JS here\nfor(var i = 0; i < 5; i+=1) { console.log('Hi C!') }`

    const compile = () => {
        const compiled = compileString(editor.getValue(), {})
        output.setValue(`// compiled C output\n${compiled}`)
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
