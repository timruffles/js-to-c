import {BinaryExpression, BinaryOperator, Literal, Node} from "estree";
import {CompileTimeState} from "./CompileTimeState";
import {assignToTarget, compile, IntermediateVariableTarget, PredefinedVariableTarget} from "./js-to-c";


const binaryOpToFunction: { [k: string]: string | undefined } = {
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

const isNumericLiteral = (n: Node): n is NumericLiteral => n.type === 'Literal' && (typeof n.value === 'number') && !isNaN(n.value)

export function compileBinaryExpression(node: BinaryExpression, state: CompileTimeState) {
    const {left, right, operator} = node;

    // optimises cases with a single numeric literal value
    const leftIsLiteral = isNumericLiteral(left)
    if ((leftIsLiteral ? !isNumericLiteral(right) : isNumericLiteral(right)) && (operator === '*' || operator === "/")) {
        const [literal, dynamic] = leftIsLiteral ? [left, right] : [right, left]
        return optimisedMultiplyOrDivide(literal as NumericLiteral, dynamic, operator, leftIsLiteral, state)
    }

    const targetLeft = new IntermediateVariableTarget(state.getNextSymbol('left'))
    const targetRight = new IntermediateVariableTarget(state.getNextSymbol('right'))

    const leftSrc = compile(node.left, state.childState({
        target: targetLeft,
    }));
    const rightSrc = compile(node.right, state.childState({
        target: targetRight,
    }));

    const operatorFn = getBinaryOperatorFunction(node.operator);

    const linkSrc = assignToTarget(`${operatorFn}(${targetLeft.id}, ${targetRight.id})`, state.target);

    return `${leftSrc}
            ${rightSrc}
            ${linkSrc}`;
}

function getBinaryOperatorFunction(operator: BinaryOperator): string {
    const operatorFn = binaryOpToFunction[operator];
    if (!operatorFn) {
        throw Error(`unimplemented operator '${operator}'`)
    }
    return operatorFn;
}


type NumericLiteral = Literal & {
    value: number
}

function optimisedMultiplyOrDivide(literal: NumericLiteral, dynamic: Node, op: '*' | '/', leftIsLiteral: boolean, state: CompileTimeState) {
    const compileOp = (literalSrc: number, dynamicSrc: string) => {
        const [left, right] = leftIsLiteral ? [literalSrc, dynamicSrc]
            : [dynamicSrc, literalSrc]
        return `jsValueCreateNumber(${left} ${op} ${right})`
    }

    const rSym = state.getNextSymbol('r');

    const interned = state.internedNumber(literal.value)

    const operationSrc = compileOp(interned.value, `${rSym}.number`)

    const dynamicResult = new PredefinedVariableTarget(state.getNextSymbol('dynamic'))
    const resultTarget = new PredefinedVariableTarget(state.getNextSymbol('result'))

    // multiply is commutative
    const multiplySrc = `
    } else if(${rSym}.isOne) {
        ${assignToTarget(interned.reference, resultTarget)}
    } else if(${rSym}.isZero) {
        ${assignToTarget('getZero()', resultTarget)}
    `

    // ops involving 1 and 0 as divisor are simple, and 0 as denominator
    const dynamicIsDivisor = leftIsLiteral
    const divideSrc = dynamicIsDivisor
        ? `
            } else if(${rSym}.isOne) {
                ${assignToTarget(interned.reference, resultTarget)}
            } else if(${rSym}.isZero) {
                ${assignToTarget('getInfinity()', resultTarget)}
            `
        : `
            } else if(${rSym}.isZero) {
                ${assignToTarget(literal.value === 0 ? 'getNaN()' : 'getZero()', resultTarget)}
            `
    const specialCases = op === '*' ? multiplySrc : divideSrc


    // shouldn't need protection for dynamic result as the
    // various cases end up with jsValueCreateNumber( ... ), so
    // we can GC the value safely after we enter jsValueCreateNumber
    // as we've already extracted the value
    return `
    ${resultTarget.definition}
    ${dynamicResult.definition} 
    
    ${compile(dynamic, state.childStateWithTarget(dynamicResult))}
    ToNumberResult ${rSym} = jsValueToNumber(${dynamicResult.id});
    if(${rSym}.isNaN) {
        ${assignToTarget(`getNaN()`, resultTarget)}
    ${specialCases}
    } else {
        ${assignToTarget(operationSrc, resultTarget)}
    }
   
    ${assignToTarget(resultTarget.id, state.target)}
    `
}

