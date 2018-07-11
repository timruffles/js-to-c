import fs from 'fs';
import {parseScript, Syntax} from "esprima";
import {
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


class CompileTimeState {
    functions: JsFunctionBody[] = [];
    // our intern string pool
    interned: {[k: string]: InternedString } = {};
    private id = 0;

    internString(str: string) {
        if(!(str in this.interned)) {
            this.interned[str] = new InternedString(this.nextId('interned'), str);
        }
        return this.interned[str];
    }

    nextId(prefix: string) {
        this.id += 1;
        return `${prefix}_${this.id}`;
    }
}
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
        CallExpression: unimplemented('CallExpression'),
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
        ExpressionStatement: unimplemented('ExpressionStatement'),
        ForInStatement: unimplemented('ForInStatement'),
        ForOfStatement: unimplemented('ForOfStatement'),
        ForStatement: unimplemented('ForStatement'),
        FunctionDeclaration: unimplemented('FunctionDeclaration'),
        FunctionExpression: unimplemented('FunctionExpression'),
        Identifier: unimplemented('Identifier'),
        IfStatement: unimplemented('IfStatement'),
        Import: unimplemented('Import'),
        ImportDeclaration: unimplemented('ImportDeclaration'),
        ImportDefaultSpecifier: unimplemented('ImportDefaultSpecifier'),
        ImportNamespaceSpecifier: unimplemented('ImportNamespaceSpecifier'),
        ImportSpecifier: unimplemented('ImportSpecifier'),
        LabeledStatement: unimplemented('LabeledStatement'),
        Literal: unimplemented('Literal'),
        LogicalExpression: unimplemented('LogicalExpression'),
        MemberExpression: unimplemented('MemberExpression'),
        MetaProperty: unimplemented('MetaProperty'),
        MethodDefinition: unimplemented('MethodDefinition'),
        NewExpression: unimplemented('NewExpression'),
        ObjectExpression: unimplemented('ObjectExpression'),
        ObjectPattern: unimplemented('ObjectPattern'),
        Program,
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
        VariableDeclaration,
        VariableDeclarator,
        WhileStatement: unimplemented('WhileStatement'),
        WithStatement: unimplemented('WithStatement'),
        YieldExpression: unimplemented('YieldExpression')
    }
}

function joinNodeOutput(srcList: string[]) {
    return srcList.join('\n');
}

function Program(node: Program, state: CompileTimeState) {

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
        `char* const ${id} = "${value}";`
    )));
}

function VariableDeclaration(node: VariableDeclaration, state: CompileTimeState) {
    if(node.kind !== 'var') {
        return unimplemented(node.kind)();
    }

    return mapCompile(node.declarations, state);
}

function mapCompile(nodes: Node[], state: CompileTimeState) {
    return joinNodeOutput(nodes.map(n => compile(n, state)));
}

function VariableDeclarator(node: VariableDeclarator, state: CompileTimeState) {
    const target = ensureSupportedTarget(node.id);
    if(target.type === 'Identifier') {
        return `envDeclare(env, ${state.internString(target.name).id});`
    } else {
       return unimplemented('MemberExpression')();
    }
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

