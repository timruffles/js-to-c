import {Node} from "estree";
import {CompileTimeState} from "./CompileTimeState";
import {CompilerIdentifier, PredefinedVariableTarget} from "./js-to-c";

enum HintType {
    Number = 'Number',
    String = 'String',
    None = 'None'
}

/**
 * An abstract variable that may be compiled in different ways,
 * used to implement algorithms in the spec.
 */
class CompileTimeValue {
   toSource() {}

    static Literal = class CompileTimeValueLiteral {
        constructor(private readonly node: Node) {}

        toSource() {
        }
    }

    static MustEvaluate = class CompileTimeValueMustEvaluate {
        constructor(private readonly node: Node) {}

        toSource() {
        }
    }
}


function compileTimeEvaluate(node: Node, state: CompileTimeState) {
    switch(node.type) {
        case 'Literal':
            return new CompileTimeValue.Literal(node)
        default:
            return new CompileTimeValue.MustEvaluate(node)
    }
}

interface CompileTypeNode {
    toSource(state: CompileTimeState): string
}

class CFunctionCall implements CompileTypeNode {
    readonly args: CompileTypeNode[]

    constructor(
        readonly target: CompilerIdentifier,
        ...args: CompileTypeNode[]
    ) {
        this.args = args
    }

    toSource(state: CompileTimeState) {
        const ids = this.args.map(() => new PredefinedVariableTarget('arg'))
        return `
            ${ids.map(id => `JsValue* ${id.id};`).join('\n')}
            ${this.args.map(n => n.toSource(state)).join('\n')}
            ${this.target.id}(${ids.map(id => id.id).join(', ')}
        `
    }
}

/*
Let lval be GetValue(lref).
Let rval be GetValue(rref).
Let lprim be ToPrimitive(lval).
Let rprim be ToPrimitive(rval).
 */
function addOperation(left: Node, right: Node): CompileTypeNode {
    // if we are adding two literal number, do `jsValueNumber() + literal value`
    // if we are adding anything to a literal string, do `strcat(stringsGetCString(), "literal val")`

    //return new CFunctionCall(new PredefinedVariableTarget('addOperation'), toPrimitive(left, HintType.None), toPrimitive(right, HintType.None));
}

function toPrimitive(node: Node, hint: HintType): CompileTypeNode {
    switch(node.type) {
        case 'Literal':
            switch(typeof node.value) {
                case 'number':


            }
    }
}
