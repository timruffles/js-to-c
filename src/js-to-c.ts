import fs from 'fs';
import {parseScript, Syntax} from "esprima";
import {
    CallExpression,
    Identifier,
    MemberExpression,
    Node,
    Pattern,
    Program, VariableDeclaration,
    VariableDeclarator
} from 'estree';


type NodeCompiler = (n: any, s: CompileTimeState) => string;

type NodeCompilerLookup = {
    [k in keyof typeof Syntax]: NodeCompiler
}

/**
 * The body of a JS function, ready to be linked with correct env etc
 * for execution
 **/
type JsFunctionBody = string;

class InternedString {
   constructor(public readonly id: string, public readonly value: string) {}
}

type JsIdentifier = string;

class IntermediateVariableTarget {
    readonly type: 'IntermediateVariableTarget' = 'IntermediateVariableTarget';
    constructor(readonly id: JsIdentifier) {}
}

const ReturnTarget = {
    type: 'ReturnTarget' as 'ReturnTarget',
};

// when an expression is being evaluated for side effects only
const SideEffectTarget = {
    type: 'SideEffectTarget' as 'SideEffectTarget',
};


/**
 * Where are we targetting this compilation?
 */
type CompileTarget =
    typeof SideEffectTarget |
    IntermediateVariableTarget |
    typeof ReturnTarget;


class CompileTimeState {
    // shared between all state objects for a given compilation
    functions: JsFunctionBody[] = [];
    // our intern string pool
    interned: {[k: string]: InternedString } = {};
    private counter = { id: 0 };

    target: CompileTarget = SideEffectTarget;

    /**
     * When evaluating sub-expressions etc, aspects of the state
     * changes such as target.
     *
     * The child state uses prototypes to access compile-wide state.
     */
    childState({
        target,
    }: {
        target: CompileTarget,
    }) {
        const child = Object.create(this);
        Object.assign(child, {
            target,
        });
        return child;
    }

    internString(str: string) {
        if(!(str in this.interned)) {
            this.interned[str] = new InternedString(this.getNextSymbol('interned'), str);
        }
        return this.interned[str];
    }

    getSymbolForId(prefix: string, id: number) {
        return `${prefix}_${id}`;
    }

    getNextSymbol(prefix: string) {
        return this.getSymbolForId(prefix, this.nextId());
    }

    nextId() {
        this.counter.id += 1;
        return this.counter.id;
    }
}

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
        BinaryExpression: unimplemented('BinaryExpression'),
        BlockStatement: unimplemented('BlockStatement'),
        BreakStatement: unimplemented('BreakStatement'),
        CallExpression: compileCallExpression,
        CatchClause: unimplemented('CatchClause'),
        ClassBody: unimplemented('ClassBody'),
        ClassDeclaration: unimplemented('ClassDeclaration'),
        ClassExpression: unimplemented('ClassExpression'),
        ConditionalExpression: unimplemented('ConditionalExpression'),
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
        FunctionDeclaration: unimplemented('FunctionDeclaration'),
        FunctionExpression: unimplemented('FunctionExpression'),
        Identifier: compileIdentifier,
        IfStatement: unimplemented('IfStatement'),
        Import: unimplemented('Import'),
        ImportDeclaration: unimplemented('ImportDeclaration'),
        ImportDefaultSpecifier: unimplemented('ImportDefaultSpecifier'),
        ImportNamespaceSpecifier: unimplemented('ImportNamespaceSpecifier'),
        ImportSpecifier: unimplemented('ImportSpecifier'),
        LabeledStatement: unimplemented('LabeledStatement'),
        Literal: unimplemented('Literal'),
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
        ReturnStatement: unimplemented('ReturnStatement'),
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

    const interned = compileInternedStrings(Object.values(state.interned));

    return `
        #include <stdio.h>
        #include "../runtime/environments.h"
        
        ${interned}
        
        ${joinNodeOutput(state.functions.reverse())}
        
        void userProgram(Env* env) {
            ${body};
        }
        
        int main(int argc, char**argv) {
            Env* global = envCreateRoot();
            userProgram(global);
            return 0;
        }
    `
}


function compileInternedStrings(interned: InternedString[]): string {
    return joinNodeOutput(interned.map(({id, value}) => (
        `char ${id}[] = "${value}";`
    )));
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
    const envVar = state.getSymbolForId('env', id);
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
            JsValue ${argsArrayVar}[] = {${argsValuesSrc}};
            Env* ${envVar} = envCreateChild(
               env, 
               ${calleeTarget.id}->argumentNames,
               &${argsArrayVar}
            );
            ${assignToTarget(`${calleeTarget.id}->fn(${envVar})`, state.target)}
            `;
}

function compileMemberExpression(node: MemberExpression, state: CompileTimeState) {
    const objectTarget = new IntermediateVariableTarget(state.getNextSymbol('object'));
    const propertyTarget = new IntermediateVariableTarget(state.getNextSymbol('property'));

    const objectSrc = compile(node.object, state.childState({
        target: objectTarget,
    }));
    const propertySrc = compile(node.property, state.childState({
        target: propertyTarget,
    }));

    const resultSrc = assignToTarget(`objectGet(${objectTarget.id}, ${propertyTarget.id})`, state.target);
    return `${objectSrc}
            ${propertySrc}
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


