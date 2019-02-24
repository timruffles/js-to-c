import {Node} from "estree";
import {CompileTimeState} from "./CompileTimeState";

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


    /*
    Let lref be the result of evaluating AdditiveExpression.
    Let lval be GetValue(lref).
    Let rref be the result of evaluating MultiplicativeExpression.
    Let rval be GetValue(rref).
    Let lprim be ToPrimitive(lval).
    Let rprim be ToPrimitive(rval).
    If Type(lprim) is String or Type(rprim) is String, then
    Return the String that is the result of concatenating ToString(lprim) followed by ToString(rprim)
    Return the result of applying the addition operation to ToNumber(lprim) and ToNumber(rprim). See the Note below 11.6.3.
     */
function addOperation(left: Node, right: Node, state: CompileTimeState) {

    const lref = compileTimeEvaluate(left, state);
    const lval =

}

function toPrimitive(node: Node, hint: HintType, state: CompileTimeState) {
    switch(node.type) {
        case 'Literal':
            switch(typeof node.value) {
                case 'number':

            }
    }
}
