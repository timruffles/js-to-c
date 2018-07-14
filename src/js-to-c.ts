import fs from 'fs';
import {parseScript, Syntax} from "esprima";
import {
    BinaryExpression,
    BlockStatement,
    CallExpression,
    ConditionalExpression,
    FunctionDeclaration,
    Identifier,
    Literal,
    MemberExpression,
    Node,
    Pattern,
    Program,
    ReturnStatement,
    VariableDeclaration,
    VariableDeclarator
} from 'estree';
import {CompileTimeState} from "./CompileTimeState";


type NodeCompiler = (n: any, s: CompileTimeState) => string;

type NodeCompilerLookup = {
    [k in keyof typeof Syntax]: NodeCompiler
}


export class InternedString {
   constructor(public readonly id: string, public readonly value: string) {}
}

export type JsIdentifier = string;

export class IntermediateVariableTarget {
    readonly type: 'IntermediateVariableTarget' = 'IntermediateVariableTarget';
    constructor(readonly id: JsIdentifier) {}
}

export class PredefinedVariableTarget {
    readonly type: 'PredefinedVariableTarget' = 'PredefinedVariableTarget';
    constructor(readonly id: JsIdentifier) {}
}

export const ReturnTarget = {
    type: 'ReturnTarget' as 'ReturnTarget',
};

// when an expression is being evaluated for side effects only
export const SideEffectTarget = {
    type: 'SideEffectTarget' as 'SideEffectTarget',
};


/**
 * Where are we targetting this compilation?
 */
export type CompileTarget =
    typeof SideEffectTarget |
    IntermediateVariableTarget |
    PredefinedVariableTarget |
    typeof ReturnTarget;


const binaryOpToFunction: {[k: string]: string} = {
    ">": "GTOperator",
    ">=": "GTEOperator",
    "<": "LTOperator",
    "<=": "LTEOperator",
    "+": "addOperator",
    "-": "subtractOperator",
    "*": "multiplyOperator",
};

const compileExpressionStatement: NodeCompiler = (node, state) => compile(node.expression, state);
const lookup = getCompilers();

if(require.main === module) {
    main();
}

function main() {
    const input = fs.readFileSync(process.argv[2], { encoding: 'utf8'});

    const ast = parseScript(input);

    console.log(compile(ast, new CompileTimeState));
}

function compile(ast: Node, state: CompileTimeState): string {
    return lookup[ast.type](ast, state);
}


function getCompilers(): NodeCompilerLookup {
    return {
        ArrayExpression: unimplemented('ArrayExpression'),
        ArrayPattern: unimplemented('ArrayPattern'),
        ArrowFunctionExpression: unimplemented('ArrowFunctionExpression'),
        AssignmentExpression: unimplemented('AssignmentExpression'),
        AssignmentPattern: unimplemented('AssignmentPattern'),
        AwaitExpression: unimplemented('AwaitExpression'),
        BinaryExpression: compileBinaryExpression,
        BlockStatement: compileBlockStatement,
        BreakStatement: unimplemented('BreakStatement'),
        CallExpression: compileCallExpression,
        CatchClause: unimplemented('CatchClause'),
        ClassBody: unimplemented('ClassBody'),
        ClassDeclaration: unimplemented('ClassDeclaration'),
        ClassExpression: unimplemented('ClassExpression'),
        ConditionalExpression: compileConditionalExpression,
        ContinueStatement: unimplemented('ContinueStatement'),
        DebuggerStatement: unimplemented('DebuggerStatement'),
        DoWhileStatement: unimplemented('DoWhileStatement'),
        EmptyStatement: unimplemented('EmptyStatement'),
        ExportAllDeclaration: unimplemented('ExportAllDeclaration'),
        ExportDefaultDeclaration: unimplemented('ExportDefaultDeclaration'),
        ExportNamedDeclaration: unimplemented('ExportNamedDeclaration'),
        ExportSpecifier: unimplemented('ExportSpecifier'),
        ExpressionStatement: compileExpressionStatement,
        ForInStatement: unimplemented('ForInStatement'),
        ForOfStatement: unimplemented('ForOfStatement'),
        ForStatement: unimplemented('ForStatement'),
        FunctionDeclaration: compileFunctionDeclaration,
        FunctionExpression: unimplemented('FunctionExpression'),
        Identifier: compileIdentifier,
        IfStatement: unimplemented('IfStatement'),
        Import: unimplemented('Import'),
        ImportDeclaration: unimplemented('ImportDeclaration'),
        ImportDefaultSpecifier: unimplemented('ImportDefaultSpecifier'),
        ImportNamespaceSpecifier: unimplemented('ImportNamespaceSpecifier'),
        ImportSpecifier: unimplemented('ImportSpecifier'),
        LabeledStatement: unimplemented('LabeledStatement'),
        Literal: compileLiteral,
        LogicalExpression: unimplemented('LogicalExpression'),
        MemberExpression: compileMemberExpression,
        MetaProperty: unimplemented('MetaProperty'),
        MethodDefinition: unimplemented('MethodDefinition'),
        NewExpression: unimplemented('NewExpression'),
        ObjectExpression: unimplemented('ObjectExpression'),
        ObjectPattern: unimplemented('ObjectPattern'),
        Program: compileProgram,
        Property: unimplemented('Property'),
        RestElement: unimplemented('RestElement'),
        ReturnStatement: compileReturnStatement,
        SequenceExpression: unimplemented('SequenceExpression'),
        SpreadElement: unimplemented('SpreadElement'),
        Super: unimplemented('Super'),
        SwitchCase: unimplemented('SwitchCase'),
        SwitchStatement: unimplemented('SwitchStatement'),
        TaggedTemplateExpression: unimplemented('TaggedTemplateExpression'),
        TemplateElement: unimplemented('TemplateElement'),
        TemplateLiteral: unimplemented('TemplateLiteral'),
        ThisExpression: unimplemented('ThisExpression'),
        ThrowStatement: unimplemented('ThrowStatement'),
        TryStatement: unimplemented('TryStatement'),
        UnaryExpression: unimplemented('UnaryExpression'),
        UpdateExpression: unimplemented('UpdateExpression'),
        VariableDeclaration: compileVariableDeclaration,
        VariableDeclarator: compileVariableDeclarator,
        WhileStatement: unimplemented('WhileStatement'),
        WithStatement: unimplemented('WithStatement'),
        YieldExpression: unimplemented('YieldExpression')
    }
}

function joinNodeOutput(srcList: string[]) {
    return srcList.join('\n');
}

function compileProgram(node: Program, state: CompileTimeState) {

    const body = joinNodeOutput(node.body.map(n => compile(n, state)));

    const internedStrings = Object.values(state.interned);
    const interned = compileInternedStrings(internedStrings);

    return `
        #include <stdio.h>
        #include "../runtime/environments.h"
        #include "../runtime/strings.h"
        #include "../runtime/objects.h"
        #include "../runtime/language.h"
        #include "../runtime/operators.h"
        #include "../runtime/global.h"
        
        ${interned}
        
        ${joinNodeOutput(state.functions)}
        
        void userProgram(Env* env) {
            ${body};
        }
        
        ${compileInternInitialisation(internedStrings)}
        
        int main() {
            initialiseInternedStrings();
            JsValue* global = createGlobalObject();
            Env* globalEnv = envFromGlobal(global);
            userProgram(globalEnv);
            return 0;
        }
    `
}


function compileInternedStrings(interned: InternedString[]): string {
    return joinNodeOutput(interned.map(({id, value}) => (
        `char ${id}_cstring[] = "${value}";
         JsValue* ${id};`
    )));
}

function compileInternInitialisation(interned: InternedString[]): string {
    const initialisers = joinNodeOutput(interned.map(({id}) => (
        `${id} = stringCreateFromCString(${id}_cstring);`
    )));

    return `void initialiseInternedStrings() {
        ${initialisers}
    }`;
}

function compileVariableDeclaration(node: VariableDeclaration, state: CompileTimeState) {
    if(node.kind !== 'var') {
        return unimplemented(node.kind)();
    }

    return mapCompile(node.declarations, state);
}

function mapCompile(nodes: Node[], state: CompileTimeState) {
    return joinNodeOutput(nodes.map(n => compile(n, state)));
}

function compileVariableDeclarator(node: VariableDeclarator, state: CompileTimeState) {
    const target = ensureSupportedTarget(node.id);
    if(target.type === 'Identifier') {
        return `envDeclare(env, ${state.internString(target.name).id});`
    } else {
       return unimplemented('MemberExpression')();
    }
}

function assignToTarget(cExpression: string, target: CompileTarget) {
    switch(target.type) {
        case 'SideEffectTarget':
            return cExpression;
        case 'IntermediateVariableTarget':
            return `JsValue* ${target.id} = (${cExpression});`;
        case 'PredefinedVariableTarget':
            return `${target.id} = (${cExpression});`;
        case 'ReturnTarget':
            return `return (${cExpression});`;
    }
}

/**
 * Examples:
 *
 *   f()
 *   foo.bar()
 *   f()()
 *   f['x']()
 *
 * All come down to
 *
 *
 *   callee_n = <process to evaluate callee>
 *   env_n = <generate new env>
 *   arg_1..n = <generate args left to right>
 *   <target> = returned
 */
function compileCallExpression(node: CallExpression, state: CompileTimeState) {
    const id = state.nextId();
    const calleeTarget = new IntermediateVariableTarget(state.getSymbolForId('callee', id));
    const argsArrayVar = state.getSymbolForId('args', id);

    const calleeSrc = compile(node.callee, state.childState({
        target: calleeTarget
    }));

    const argsWithTargets = node.arguments.map((argNode, i) => {
        const callVar = state.getSymbolForId(`call${id}Arg`, i);
        const target = new IntermediateVariableTarget(callVar);
        const expression = compile(argNode, state.childState({
            target,
        }));

        return {
            target,
            expression,
        };
    });

    const argsValuesSrc = argsWithTargets
        .map(({target}) => target.id)
        .join(', ');

    return `${calleeSrc}
            ${joinNodeOutput(argsWithTargets.map(({expression}) => expression))}
            JsValue* ${argsArrayVar}[] = {${argsValuesSrc}};
            ${assignToTarget(`functionRunWithArguments(
                ${calleeTarget.id}, 
                env,
                ${argsArrayVar},
                ${argsWithTargets.length}
            )`, state.target)}
            `;
}

function compileMemberExpression(node: MemberExpression, state: CompileTimeState) {
    const objectTarget = new IntermediateVariableTarget(state.getNextSymbol('object'));
    const propertyTarget = new IntermediateVariableTarget(state.getNextSymbol('property'));

    const objectSrc = compile(node.object, state.childState({
        target: objectTarget,
    }));

    // TODO only supporting literal property lookup at mo
    //const propertySrc = compile(node.property, state.childState({
    //    target: propertyTarget,
    //}));

    const resultSrc = assignToTarget(`objectGet(${objectTarget.id}, ${propertyTarget.id})`, state.target);
    return `${objectSrc}
            JsValue* ${propertyTarget.id} = ${wrapStringAsJsValueString(getIdentifierName(node.property as any))};
            ${resultSrc}
    `
}

function compileIdentifier(node: Identifier, state: CompileTimeState) {
    const interned = state.internString(node.name);
   return assignToTarget(
       `envGet(env, ${interned.id}) /* ${interned.value} */`,
       state.target,
   );
}

type SupportedAssignmentTarget = MemberExpression | Identifier;
function ensureSupportedTarget(node: Pattern): SupportedAssignmentTarget {
    if(node.type === 'Identifier' || node.type === 'MemberExpression') {
        return node
    } else {
        throw Error(`No support for ${node.type}`);
    }
}

function unimplemented(node: string) {
    return function () {
        return `/* unimplemented ${node} */`;
    }
}

function getIdentifierName(node: Pattern): string {
    if(node.type === 'Identifier') {
        return node.name;
    } else {
        throw Error(`No support for ${node.type}`);
    }
}

function createCFunction(name: string, bodySrc: string) {
    return `JsValue* ${name}(Env* env) {
        ${bodySrc}
    }`
}
function wrapAsCStringLiteral(string: string) {
    return `"${string}"`
}

function compileBinaryExpression(node: BinaryExpression, state: CompileTimeState) {
    const targetLeft = new IntermediateVariableTarget(state.getNextSymbol('left'))
    const targetRight = new IntermediateVariableTarget(state.getNextSymbol('right'))

    const leftSrc = compile(node.left, state.childState({
        target: targetLeft,
    }));
    const rightSrc = compile(node.right, state.childState({
        target: targetRight,
    }));

    const operatorFn = binaryOpToFunction[node.operator] || 'notDefined';

    const linkSrc = assignToTarget(`${operatorFn}(${targetLeft.id}, ${targetRight.id})`, state.target);

    return `${leftSrc}
            ${rightSrc}
            ${linkSrc}`;
}

function compileConditionalExpression(node: ConditionalExpression, state: CompileTimeState) {
    const resultTarget = new PredefinedVariableTarget(state.getNextSymbol('conditionalValue'))
    const testResultTarget = new IntermediateVariableTarget(state.getNextSymbol('conditionalPredicate'));
    const testSrc = compile(node.test, state.childState({
        target: testResultTarget,
    }));

    const consequentSrc = compile(node.consequent, state);
    const alternateSrc = compile(node.alternate, state);

    return `/* ternary */
            ${testSrc};
            JsValue* ${resultTarget.id};
            if(isTruthy(${testResultTarget.id})) {
                ${consequentSrc}
            } else {
                ${alternateSrc}
            }`
}

function wrapStringAsJsValueString(str: string) {
    return `stringCreateFromCString(${wrapAsCStringLiteral(str)})`
}

function compileFunctionDeclaration(node: FunctionDeclaration, state: CompileTimeState) {
    const name = node.id && node.id.name;
    if(!name) {
        // this would only be default in ES6 'export default fun...'
        throw Error("ES6 module syntax not supported");
    }
    const functionId = state.getNextSymbol(name);
    const argumentNames = node.params.map(getIdentifierName);
    const argumentNamesSrc = argumentNames
        .map(wrapStringAsJsValueString)
        .join(", ");
    const argsArrayName = `${functionId}_args`;

    const bodySrc = compile(node.body, state);

    // TODO this will need topographic sorting
    state.functions.push(createCFunction(functionId, bodySrc));

    const fnName = state.getNextSymbol('functionName');
    const argCount = node.params.length;

    // TODO proper hoisting
    return `JsValue* ${argsArrayName}[] = {${argumentNamesSrc}};
            JsValue* ${fnName} = stringCreateFromCString("${name}");
            envDeclare(env, ${fnName});
            envSet(env, ${fnName}, functionCreate(${functionId}, ${argsArrayName}, ${argCount}));`
}

function compileBlockStatement(node: BlockStatement, state: CompileTimeState) {
    return mapCompile(node.body, state);
}

function compileReturnStatement(node: ReturnStatement, state: CompileTimeState) {
    if(!node.argument) {
        return `return getUndefined()`;
    }
    const returnTarget = new PredefinedVariableTarget(state.getNextSymbol('return'));
    const argumentSrc = compile(node.argument, state.childState({
        target: returnTarget,
    }))
    return `JsValue* ${returnTarget.id};
            ${argumentSrc}
            return ${returnTarget.id};`;
}

function compileLiteral(node: Literal, state: CompileTimeState) {
    if("regex" in node) return unimplemented("Literal<regex>")();
    const getValue = () => {
        if(typeof node.value === 'string') {
            return `stringCreateFromCString(${wrapAsCStringLiteral(node.value)})`;
        } else if(typeof node.value === 'number') {
            return `jsValueCreateNumber(${node.value})`
        } else if(node.value === null) {
            return `getNull()`
        } else {
            return node.value === true
                ? `getTrue()`
                : `getFalse()`;
        }
    }

    return assignToTarget(getValue(), state.target);
}
