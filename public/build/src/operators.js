"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var js_to_c_1 = require("./js-to-c");
var binaryOpToFunction = {
    ">": "GTOperator",
    ">=": "GTEOperator",
    "<": "LTOperator",
    "<=": "LTEOperator",
    "+": "addOperator",
    "-": "subtractOperator",
    "*": "multiplyOperator",
    "===": "strictEqualOperator",
    "instanceof": "objectInstanceof",
};
var isNumericLiteral = function (n) { return n.type === 'Literal' && (typeof n.value === 'number') && !isNaN(n.value); };
function compileBinaryExpression(node, state) {
    var left = node.left, right = node.right, operator = node.operator;
    var leftIsLiteral = isNumericLiteral(left);
    if ((leftIsLiteral ? !isNumericLiteral(right) : isNumericLiteral(right)) && (operator === '*' || operator === "/")) {
        var _a = leftIsLiteral ? [left, right] : [right, left], literal = _a[0], dynamic = _a[1];
        return optimisedMultiplyOrDivide(literal, dynamic, operator, leftIsLiteral, state);
    }
    var targetLeft = new js_to_c_1.IntermediateVariableTarget(state.getNextSymbol('left'));
    var targetRight = new js_to_c_1.IntermediateVariableTarget(state.getNextSymbol('right'));
    var leftSrc = js_to_c_1.compile(node.left, state.childState({
        target: targetLeft,
    }));
    var rightSrc = js_to_c_1.compile(node.right, state.childState({
        target: targetRight,
    }));
    var operatorFn = getBinaryOperatorFunction(node.operator);
    var linkSrc = js_to_c_1.assignToTarget(operatorFn + "(" + targetLeft.id + ", " + targetRight.id + ")", state.target);
    return leftSrc + "\n            " + rightSrc + "\n            " + linkSrc;
}
exports.compileBinaryExpression = compileBinaryExpression;
function getBinaryOperatorFunction(operator) {
    var operatorFn = binaryOpToFunction[operator];
    if (!operatorFn) {
        throw Error("unimplemented operator '" + operator + "'");
    }
    return operatorFn;
}
function optimisedMultiplyOrDivide(literal, dynamic, op, leftIsLiteral, state) {
    var compileOp = function (literalSrc, dynamicSrc) {
        var _a = leftIsLiteral ? [literalSrc, dynamicSrc]
            : [dynamicSrc, literalSrc], left = _a[0], right = _a[1];
        return "jsValueCreateNumber(" + left + " " + op + " " + right + ")";
    };
    var rSym = state.getNextSymbol('r');
    var interned = state.internedNumber(literal.value);
    var operationSrc = compileOp(interned.value, rSym + ".number");
    var dynamicResult = new js_to_c_1.PredefinedVariableTarget(state.getNextSymbol('dynamic'));
    var resultTarget = new js_to_c_1.PredefinedVariableTarget(state.getNextSymbol('result'));
    var multiplySrc = "\n    } else if(" + rSym + ".isOne) {\n        " + js_to_c_1.assignToTarget(interned.reference, resultTarget) + "\n    } else if(" + rSym + ".isZero) {\n        " + js_to_c_1.assignToTarget('getZero()', resultTarget) + "\n    ";
    var dynamicIsDivisor = leftIsLiteral;
    var divideSrc = dynamicIsDivisor
        ? "\n            } else if(" + rSym + ".isOne) {\n                " + js_to_c_1.assignToTarget(interned.reference, resultTarget) + "\n            } else if(" + rSym + ".isZero) {\n                " + js_to_c_1.assignToTarget('getInfinity()', resultTarget) + "\n            "
        : "\n            } else if(" + rSym + ".isZero) {\n                " + js_to_c_1.assignToTarget(literal.value === 0 ? 'getNaN()' : 'getZero()', resultTarget) + "\n            ";
    var specialCases = op === '*' ? multiplySrc : divideSrc;
    return "\n    " + resultTarget.definition + "\n    " + dynamicResult.definition + " \n    \n    " + js_to_c_1.compile(dynamic, state.childStateWithTarget(dynamicResult)) + "\n    ToNumberResult " + rSym + " = jsValueToNumber(" + dynamicResult.id + ");\n    if(" + rSym + ".isNaN) {\n        " + js_to_c_1.assignToTarget("getNaN()", resultTarget) + "\n    " + specialCases + "\n    } else {\n        " + js_to_c_1.assignToTarget(operationSrc, resultTarget) + "\n    }\n   \n    " + js_to_c_1.assignToTarget(resultTarget.id, state.target) + "\n    ";
}
