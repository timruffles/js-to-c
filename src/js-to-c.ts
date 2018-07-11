import fs from 'fs';
import {parseScript, Syntax} from "esprima";
import {
    Node, Program
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

class CompileTimeState {
    functions: JsFunctionBody[] = [];
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
        VariableDeclaration: unimplemented('VariableDeclaration'),
        VariableDeclarator: unimplemented('VariableDeclarator'),
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

    return `
        #include <stdio.h>
        #include <../runtime/environments.h>
        
        int main(int argc, char**argv) {
            Env* global = EnvCreate();
            userProgram(global);
            return 0;
        }
        
        void userProgram(Env* global) {
            ${body};
        }
        
        ${joinNodeOutput(state.functions)}
    `
}

function unimplemented(node: string) {
    return function () {
        return `/* unimplemented ${node} */`;
    }
}
