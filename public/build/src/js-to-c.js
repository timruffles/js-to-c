"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var tslib_1 = require("tslib");
var fs_1 = tslib_1.__importDefault(require("fs"));
var esprima_1 = require("esprima");
var CompileTimeState_1 = require("./CompileTimeState");
var operators_1 = require("./operators");
var CompileTimeError = (function (_super) {
    tslib_1.__extends(CompileTimeError, _super);
    function CompileTimeError() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return CompileTimeError;
}(Error));
exports.CompileTimeError = CompileTimeError;
var InternedString = (function () {
    function InternedString(id, value) {
        this.id = id;
        this.value = value;
    }
    Object.defineProperty(InternedString.prototype, "reference", {
        get: function () {
            return this.id + " /* " + this.value + " */";
        },
        enumerable: true,
        configurable: true
    });
    return InternedString;
}());
exports.InternedString = InternedString;
var InternedNumber = (function () {
    function InternedNumber(id, value) {
        this.id = id;
        this.value = value;
    }
    Object.defineProperty(InternedNumber.prototype, "reference", {
        get: function () {
            return this.id + " /* " + this.value + " */";
        },
        enumerable: true,
        configurable: true
    });
    return InternedNumber;
}());
exports.InternedNumber = InternedNumber;
var IntermediateVariableTarget = (function () {
    function IntermediateVariableTarget(id) {
        this.id = id;
        this.type = IntermediateVariableTarget.type;
    }
    Object.defineProperty(IntermediateVariableTarget.prototype, "definition", {
        get: function () {
            return "JsValue* " + this.id + ";";
        },
        enumerable: true,
        configurable: true
    });
    IntermediateVariableTarget.type = 'IntermediateVariableTarget';
    return IntermediateVariableTarget;
}());
exports.IntermediateVariableTarget = IntermediateVariableTarget;
var PredefinedVariableTarget = (function () {
    function PredefinedVariableTarget(id) {
        this.id = id;
        this.type = PredefinedVariableTarget.type;
    }
    Object.defineProperty(PredefinedVariableTarget.prototype, "definition", {
        get: function () {
            return "JsValue* " + this.id + ";";
        },
        enumerable: true,
        configurable: true
    });
    PredefinedVariableTarget.type = 'PredefinedVariableTarget';
    return PredefinedVariableTarget;
}());
exports.PredefinedVariableTarget = PredefinedVariableTarget;
exports.ReturnTarget = {
    type: 'ReturnTarget',
};
exports.SideEffectTarget = {
    type: 'SideEffectTarget',
};
var unaryOpToFunction = {
    "!": "notOperator",
    "typeof": "typeofOperator",
};
var assignmentOpToFunction = {
    "+=": "addOperator",
    "-=": "subtractOperator",
};
var compileExpressionStatement = function (node, state) { return compile(node.expression, state); };
var lookup = getCompilers();
if (require.main === module) {
    main();
}
function main() {
    console.log(compileFile(process.argv[2], {
        outputLibraryName: process.env.JSC_OUTPUT_LIBRARY,
        outputLibraryHeader: process.env.JSC_LIBRARY_HEADER,
    }));
}
function compileFile(fn, options) {
    var input = fs_1.default.readFileSync(fn, { encoding: 'utf8' });
    return compileString(input, options);
}
exports.compileFile = compileFile;
function compileString(src, compileOptions) {
    var ast = esprima_1.parseScript(src);
    return compile(ast, new CompileTimeState_1.CompileTimeState(compileOptions));
}
exports.compileString = compileString;
function compile(ast, state) {
    return lookup[ast.type](ast, state);
}
exports.compile = compile;
function always(output) {
    return function () { return output; };
}
function compileLogicalExpression(node, state) {
    var returnSetup = state.withManualReturn(state.getNextSymbol('evaluation'));
    var leftTarget = new IntermediateVariableTarget(state.getNextSymbol('lhs'));
    var leftSrc = compile(node.left, returnSetup.state.childStateWithTarget(leftTarget));
    var rightSrc = compile(node.right, returnSetup.state);
    var shortCircuit = node.operator === '&&' ? 'false' : 'true';
    return "/* " + node.operator + " */\n            " + leftSrc + "\n            if(isTruthy(" + leftTarget.id + ") == " + shortCircuit + ") {\n                " + assignToTarget(leftTarget.id, returnSetup.state.target) + "\n            } else {\n                " + rightSrc + ";\n            }\n            " + returnSetup.returnSrc;
}
function compileUpdateExpression(node, state) {
    var value = node.operator === '++' ? 1 : -1;
    var evaluatedTo = new IntermediateVariableTarget(state.getNextSymbol('eval'));
    var updateSrc = compileAssignmentExpression({
        type: 'AssignmentExpression',
        operator: '+=',
        left: node.argument,
        right: {
            type: 'Literal',
            value: value,
        }
    }, state.childStateWithTarget(evaluatedTo));
    var finalSrc = node.prefix
        ? evaluatedTo.id
        : "addOperator(" + evaluatedTo.id + ", jsValueCreateNumber(" + -value + "))";
    return updateSrc + "\n            " + assignToTarget(finalSrc, state.target) + "\n    ";
}
function getCompilers() {
    return {
        ArrayExpression: compileArrayExpression,
        AssignmentExpression: compileAssignmentExpression,
        BinaryExpression: operators_1.compileBinaryExpression,
        BlockStatement: compileBlockStatement,
        BreakStatement: always('break;\n'),
        CallExpression: compileCallExpression,
        ConditionalExpression: compileConditionalExpression,
        ContinueStatement: always('continue;\n'),
        DebuggerStatement: unimplemented('DebuggerStatement'),
        DoWhileStatement: unimplemented('DoWhileStatement'),
        EmptyStatement: always('/* empty statement */'),
        ExpressionStatement: compileExpressionStatement,
        ForInStatement: compileForInStatement,
        ForStatement: compileForStatement,
        FunctionDeclaration: compileFunctionDeclaration,
        FunctionExpression: compileFunctionExpression,
        Identifier: compileIdentifier,
        IfStatement: compileIfStatement,
        LabeledStatement: unimplemented('LabeledStatement'),
        Literal: compileLiteral,
        LogicalExpression: compileLogicalExpression,
        MemberExpression: compileMemberExpression,
        MethodDefinition: unimplemented('MethodDefinition'),
        NewExpression: compileNewExpression,
        ObjectExpression: compileObjectExpression,
        Program: compileProgram,
        ReturnStatement: compileReturnStatement,
        SequenceExpression: unimplemented('SequenceExpression'),
        SwitchStatement: compileSwitchStatement,
        ThisExpression: compileThisExpression,
        ThrowStatement: compileThrowStatement,
        TryStatement: compileTryStatement,
        UnaryExpression: compileUnaryExpression,
        UpdateExpression: compileUpdateExpression,
        VariableDeclaration: compileVariableDeclaration,
        VariableDeclarator: compileVariableDeclarator,
        WhileStatement: compileWhileStatement,
        CatchClause: unimplemented('CatchClause'),
        SwitchCase: unimplemented('SwitchCase'),
        Property: unimplemented('Property'),
        WithStatement: notInStrictMode('WithStatement'),
        ArrayPattern: unimplemented('ArrayPattern'),
        ArrowFunctionExpression: unimplemented('ArrowFunctionExpression'),
        AssignmentPattern: unimplemented('AssignmentPattern'),
        AwaitExpression: unimplemented('AwaitExpression'),
        ClassBody: unimplemented('ClassBody'),
        ClassDeclaration: unimplemented('ClassDeclaration'),
        ClassExpression: unimplemented('ClassExpression'),
        ExportAllDeclaration: unimplemented('ExportAllDeclaration'),
        ExportDefaultDeclaration: unimplemented('ExportDefaultDeclaration'),
        ExportNamedDeclaration: unimplemented('ExportNamedDeclaration'),
        ExportSpecifier: unimplemented('ExportSpecifier'),
        ForOfStatement: unimplemented('ForOfStatement'),
        Import: unimplemented('Import'),
        ImportDeclaration: unimplemented('ImportDeclaration'),
        ImportDefaultSpecifier: unimplemented('ImportDefaultSpecifier'),
        ImportNamespaceSpecifier: unimplemented('ImportNamespaceSpecifier'),
        ImportSpecifier: unimplemented('ImportSpecifier'),
        MetaProperty: unimplemented('MetaProperty'),
        ObjectPattern: unimplemented('ObjectPattern'),
        RestElement: unimplemented('RestElement'),
        SpreadElement: unimplemented('SpreadElement'),
        Super: unimplemented('Super'),
        TaggedTemplateExpression: unimplemented('TaggedTemplateExpression'),
        TemplateElement: unimplemented('TemplateElement'),
        TemplateLiteral: unimplemented('TemplateLiteral'),
        YieldExpression: unimplemented('YieldExpression'),
    };
}
function joinNodeOutput(srcList) {
    return srcList.join('\n');
}
function compileProgram(node, state) {
    var body = joinNodeOutput(node.body.map(function (n) { return compile(n, state); }));
    var interned = compileInternedInitializers(state);
    return "\n        #include <stdio.h>\n        #include \"../../runtime/environments.h\"\n        #include \"../../runtime/strings.h\"\n        #include \"../../runtime/objects.h\"\n        #include \"../../runtime/_memory.h\"\n        #include \"../../runtime/array.h\"\n        #include \"../../runtime/language.h\"\n        #include \"../../runtime/operators.h\"\n        #include \"../../runtime/global.h\"\n        #include \"../../runtime/objects.h\"\n        #include \"../../runtime/functions.h\"\n        #include \"../../runtime/runtime.h\"\n        #include \"../../runtime/exceptions.h\"\n        #include \"../../runtime/lib/debug.h\"\n        #include \"../../runtime/event.h\"\n        #include \"../../runtime/operations.h\"\n\n        \n        " + interned + "\n        \n        " + joinNodeOutput(state.functions) + "\n        \n        static void userProgram(Env* env) {\n            " + body + ";\n        }\n        \n        " + compileInternInitialisation(state) + "\n        \n        " + (state.outputTarget.type === 'Library' ? bodyForLibrary(state.outputTarget) : bodyForMain()) + "\n";
}
function bodyForLibrary(_a) {
    var name = _a.name, header = _a.header;
    return "\n    #include \"" + header + "\"\n    \n    extern void " + name + "LibraryInit() {\n        initialiseInternedValues();\n        RuntimeEnvironment* runtime = runtimeGet();\n        \n        log_info(\"Initializing JS library '" + name + "'\");\n        userProgram(runtime->globalEnv);\n    }";
}
function bodyForMain() {
    return "\n    #include \"../../runtime/prelude.h\"\n\n    int main() {\n        RuntimeEnvironment* runtime = runtimeInit(NULL);\n        initialiseInternedValues();\n\n        preludeLibraryInit();\n        \n        log_info(\"Running user program\");\n        userProgram(runtime->globalEnv);\n        eventLoop();\n        return 0;\n    }\n    ";
}
function compileInternedInitializers(state) {
    var init = state.getInternedNumbers().concat(state.getInternedStrings());
    return joinNodeOutput(init.map(function (_a) {
        var id = _a.id;
        return ("static JsValue* " + id + ";");
    }));
}
function compileInternInitialisation(state) {
    var stringInitializers = joinNodeOutput(state.getInternedStrings().map(function (_a) {
        var id = _a.id, value = _a.value;
        return (id + " = stringFromLiteral(\"" + value.replace(/"/g, '"') + "\");");
    }));
    var numberInitializers = joinNodeOutput(state.getInternedNumbers().map(function (_a) {
        var id = _a.id, value = _a.value;
        return (id + " = jsValueCreateNumber(" + value + ");");
    }));
    return "static void initialiseInternedValues() {\n        gcStartProtectAllocations();\n        " + stringInitializers + "\n        " + numberInitializers + "\n        gcStopProtectAllocations();\n    }";
}
function compileVariableDeclaration(node, state) {
    if (node.kind !== 'var') {
        return unimplemented(node.kind)();
    }
    return mapCompile(node.declarations, state);
}
function mapCompile(nodes, state) {
    return joinNodeOutput(nodes.map(function (n) { return compile(n, state); }));
}
function compileVariableDeclarator(node, state) {
    var target = ensureSupportedTarget(node.id);
    if (target.type === 'Identifier') {
        var targetInternedString = state.internString(target.name);
        var initTarget = new IntermediateVariableTarget(state.getNextSymbol('init'));
        var initSrc = node.init
            ? compile(node.init, state.childState({ target: initTarget }))
            : '';
        var setSrc = node.init
            ? "envSet(env, " + targetInternedString.id + ", " + initTarget.id + ");"
            : '';
        return "envDeclare(env, " + targetInternedString.id + ");\n                " + initSrc + "\n                " + setSrc;
    }
    else {
        return unimplemented('MemberExpression')();
    }
}
function exhaustive(fn) {
    return fn();
}
function assignToTarget(cExpression, target) {
    return exhaustive(function () {
        switch (target.type) {
            case 'SideEffectTarget':
                return cExpression + ";";
            case IntermediateVariableTarget.type:
                return "JsValue* " + target.id + " = (" + cExpression + ");";
            case PredefinedVariableTarget.type:
                return target.id + " = (" + cExpression + ");";
            case 'ReturnTarget':
                return "return (" + cExpression + ");";
        }
    });
}
exports.assignToTarget = assignToTarget;
function compileCallExpression(node, state) {
    var id = state.nextId();
    var calleeTarget = new IntermediateVariableTarget(state.getSymbolForId('callee', id));
    var argsArrayVar = state.getSymbolForId('args', id);
    var _a = exhaustive(function () {
        switch (node.callee.type) {
            case "ThisExpression":
                return {
                    calleeSrc: compile(node.callee, state.childStateWithTarget(calleeTarget)),
                    thisSrc: "envGet(env, stringFromLiteral(\"this\"))",
                };
            case "MemberExpression":
                var expr = prepareMemberExpression(node.callee, state.childStateWithTarget(calleeTarget));
                return {
                    calleeSrc: expr.source,
                    thisSrc: expr.objectTarget.id,
                };
            default:
                return {
                    calleeSrc: compile(node.callee, state.childStateWithTarget(calleeTarget)),
                    thisSrc: calleeTarget.id,
                };
        }
    }), calleeSrc = _a.calleeSrc, thisSrc = _a.thisSrc;
    var argsWithTargets = node.arguments.map(function (argNode, i) {
        var callVar = state.getSymbolForId("call" + id + "Arg", i);
        var target = new PredefinedVariableTarget(callVar);
        var expression = compile(argNode, state.childState({
            target: target,
        }));
        return {
            target: target,
            expression: expression,
        };
    });
    var argsValuesSrc = argsWithTargets
        .map(function (_a) {
        var target = _a.target;
        return target.id;
    })
        .join(', ');
    var argsArrayInit = argsWithTargets.length > 0
        ? "JsValue* " + argsArrayVar + "[] = {" + argsValuesSrc + "};"
        : "JsValue** " + argsArrayVar + " = NULL;";
    var argsDefinitionsSrc = argsWithTargets.map(function (_a) {
        var target = _a.target;
        return "JsValue* " + target.id + ";";
    }).join("\n");
    var isNew = node.type === 'NewExpression';
    var runtimeOperation = isNew ? 'objectNewOperation' : 'functionRunWithArguments';
    var thisArgumentSrc = isNew ? '' : "," + thisSrc;
    return argsDefinitionsSrc + "\n            " + calleeSrc + "\n            " + joinNodeOutput(argsWithTargets.map(function (_a) {
        var expression = _a.expression;
        return expression;
    })) + "\n            " + argsArrayInit + "\n            " + assignToTarget(runtimeOperation + "(\n                " + calleeTarget.id + ", \n                " + argsArrayVar + ",\n                " + argsWithTargets.length + "\n                " + thisArgumentSrc + "\n            )", state.target) + "\n            ";
}
function compileMemberExpression(node, state) {
    return prepareMemberExpression(node, state).source;
}
function prepareMemberExpression(node, state) {
    var objectTarget = new IntermediateVariableTarget(state.getNextSymbol('object'));
    var propertyTarget = new IntermediateVariableTarget(state.getNextSymbol('property'));
    var objectSrc = compile(node.object, state.childState({
        target: objectTarget,
    }));
    var propertySrc = compileProperty(node.property, node.computed, state.childState({ target: propertyTarget }));
    var resultSrc = assignToTarget("objectGet(" + objectTarget.id + ", " + propertyTarget.id + ")", state.target);
    return {
        source: objectSrc + "\n            " + propertySrc + "\n            " + resultSrc,
        objectTarget: objectTarget,
        propertyTarget: propertyTarget,
    };
}
function compileIdentifier(node, state) {
    if (node.name === 'undefined') {
        return assignToTarget("getUndefined()", state.target);
    }
    var interned = state.internString(node.name);
    return assignToTarget("envGet(env, " + interned.reference + ")", state.target);
}
function ensureSupportedTarget(node) {
    if (node.type === 'Identifier' || node.type === 'MemberExpression') {
        return node;
    }
    else {
        throw Error("No support for " + node.type);
    }
}
function unimplemented(node) {
    return function () {
        throw Error("unimplemented " + node);
    };
}
function notInStrictMode(node) {
    return function () {
        throw Error(node + " in invalid in strict mode");
    };
}
function getIdentifierName(node) {
    if (node.type === 'Identifier') {
        return node.name;
    }
    else {
        throw Error("No support for " + node.type);
    }
}
function createCFunction(name, bodySrc) {
    return "static JsValue* " + name + "(Env* env) {\n        " + bodySrc + "\n    }";
}
function compileUnaryExpression(node, state) {
    var operandTarget = new IntermediateVariableTarget(state.getNextSymbol('operand'));
    var operandSrc = compile(node.argument, state.childState({ target: operandTarget }));
    var operatorFn = getUnaryOperatorFunction(node.operator);
    return operandSrc + "\n            " + assignToTarget(operatorFn + "(" + operandTarget.id + ")", state.target);
}
function compileConditionalExpression(node, state) {
    var resultTarget = new PredefinedVariableTarget(state.getNextSymbol('conditionalValue'));
    var testResultTarget = new IntermediateVariableTarget(state.getNextSymbol('conditionalPredicate'));
    var testSrc = compile(node.test, state.childState({
        target: testResultTarget,
    }));
    var consequentSrc = compile(node.consequent, state);
    var alternateSrc = compile(node.alternate, state);
    return "/* ternary */\n            " + testSrc + ";\n            JsValue* " + resultTarget.id + ";\n            if(isTruthy(" + testResultTarget.id + ")) {\n                " + consequentSrc + "\n            } else {\n                " + alternateSrc + "\n            }";
}
function internString(str, state) {
    return state.internString(str).reference;
}
function compileFunctionDeclaration(node, state) {
    var name = node.id && node.id.name;
    if (!name) {
        throw Error("ES6 module syntax not supported");
    }
    var functionId = state.getNextSymbol(name);
    var argumentNames = node.params.map(getIdentifierName);
    var argumentNamesSrc = argumentNames
        .map(function (s) { return internString(s, state); })
        .join(", ");
    var argsArrayName = functionId + "_args";
    var ensureUndefined = "\nreturn getUndefined();";
    var bodySrc = "log_info(\"User function " + name + "\");" + compile(node.body, state) + ensureUndefined;
    state.functions.push(createCFunction(functionId, bodySrc));
    var fnName = state.getNextSymbol('functionName');
    var argCount = node.params.length;
    var argsArraySrc = argumentNames.length > 0
        ? "JsValue* " + argsArrayName + "[] = {" + argumentNamesSrc + "};"
        : "JsValue** " + argsArrayName + " = NULL;";
    return argsArraySrc + "\n            JsValue* " + fnName + " = " + internString(name, state) + ";\n            envDeclare(env, " + fnName + ");\n            envSet(env, " + fnName + ", functionCreate(" + functionId + ", " + argsArrayName + ", " + argCount + ", env));";
}
function compileFunctionExpression(node, state) {
    var functionId = state.getNextSymbol(node.id ? node.id.name : 'anonFunction');
    var argumentNames = node.params.map(getIdentifierName);
    var argumentNamesSrc = argumentNames
        .map(function (s) { return internString(s, state); })
        .join(", ");
    var argsArrayName = functionId + "_args";
    var argCount = node.params.length;
    var ensureUndefined = "\nreturn getUndefined();";
    var bodySrc = "log_info(\"User function " + functionId + "\");" + compile(node.body, state) + ensureUndefined;
    state.functions.push(createCFunction(functionId, bodySrc));
    var argsArraySrc = argumentNames.length > 0
        ? "JsValue* " + argsArrayName + "[] = {" + argumentNamesSrc + "};"
        : "JsValue** " + argsArrayName + " = NULL;";
    return argsArraySrc + "\n            " + assignToTarget("functionCreate(" + functionId + ", " + argsArrayName + ", " + argCount + ", env)", state.target);
}
function compileBlockStatement(node, state) {
    return mapCompile(node.body, state);
}
function compileReturnStatement(node, state) {
    if (!node.argument) {
        return "return getUndefined();";
    }
    var returnTarget = new PredefinedVariableTarget(state.getNextSymbol('return'));
    var argumentSrc = compile(node.argument, state.childState({
        target: returnTarget,
    }));
    return "JsValue* " + returnTarget.id + ";\n            " + argumentSrc + "\n            return " + returnTarget.id + ";";
}
function compileWhileStatement(node, state) {
    var testVariable = new IntermediateVariableTarget(state.getNextSymbol('testResult'));
    return "while(1) {\n        " + compile(node.test, state.childState({
        target: testVariable
    })) + "\n        if (!isTruthy(" + testVariable.id + ")) {\n            break;\n        }\n        " + compile(node.body, state) + "\n    }";
}
function compileForStatement(node, state) {
    var testVariable = new IntermediateVariableTarget(state.getNextSymbol('testResult'));
    var initSrc = node.init ? compile(node.init, state) : '/* no init */';
    var testSrc = node.test
        ? compile(node.test, state.childState({
            target: testVariable
        }))
        : "// no for test";
    var testBranchSrc = node.test
        ? "if (!isTruthy(" + testVariable.id + ")) {\n            break;\n        }"
        : '';
    var updateSrc = node.update ? compile(node.update, state) : "/* no update */";
    var bodySrc = compile(node.body, state);
    return "/* for loop */\n            " + initSrc + "\n            while(1) {\n                " + testSrc + "\n                " + testBranchSrc + "\n                " + bodySrc + "\n                " + updateSrc + "\n            }";
}
function compileForInStatement(node, state) {
    var objectTarget = new IntermediateVariableTarget(state.getNextSymbol('forInObject'));
    var leftVariableName = internString(leftVariable(), state);
    var rightSrc = compile(node.right, state.childState({ target: objectTarget }));
    var bodySrc = compile(node.body, state);
    return "/* for in loop */\n            {\n                " + leftSrc() + "\n                " + rightSrc + "\n                for(ForOwnIterator iterator = objectForOwnPropertiesIterator(" + objectTarget.id + ");\n                    iterator.property != NULL;\n                    iterator = objectForOwnPropertiesNext(iterator)\n                ) {\n                    envSet(env, " + leftVariableName + ", iterator.property);\n                    " + bodySrc + "\n                }\n            }";
    function leftVariable() {
        switch (node.left.type) {
            case "Identifier":
                return node.left.name;
            case "VariableDeclaration":
                return specifyType(node.left.declarations[0].id).name;
            default:
                return unimplemented(node.type)();
        }
    }
    function leftSrc() {
        switch (node.left.type) {
            case "Identifier":
                return '';
            case "VariableDeclaration":
                return compile(node.left, state);
            default:
                return unimplemented(node.type)();
        }
    }
}
function compileLiteral(node, state) {
    if ("regex" in node)
        return unimplemented("Literal<regex>")();
    var getValue = function () {
        if (typeof node.value === 'string') {
            return internString(node.value, state);
        }
        else if (typeof node.value === 'number') {
            return state.internedNumber(node.value).reference;
        }
        else if (node.value === null) {
            return "getNull()";
        }
        else {
            return node.value === true
                ? "getTrue()"
                : "getFalse()";
        }
    };
    return assignToTarget(getValue(), state.target);
}
function collapseExpressionToSupportedType(t, predicate) {
    if (predicate(t)) {
        return t;
    }
    else {
        throw Error("Unsupported type: " + t.type);
    }
}
function isEs5ObjectKey(t) {
    return t.type === 'Identifier' || (t.type === 'Literal' && !('regex' in t));
}
function objectKeyToString(key) {
    var es5Key = collapseExpressionToSupportedType(key, isEs5ObjectKey);
    switch (es5Key.type) {
        case 'Literal':
            return "" + es5Key.value;
        case 'Identifier':
            return es5Key.name;
    }
}
function compileObjectExpression(node, state) {
    var objectVar = state.getNextSymbol('objectLiteral');
    var objectCreateSrc = "JsValue* " + objectVar + " = objectCreatePlain();";
    var objectSrc = assignToTarget(objectVar, state.target);
    var hasProperties = node.properties.length > 0;
    var propertiesSrc = node.properties.map(function (property) {
        var valueTarget = new IntermediateVariableTarget(state.getNextSymbol('value'));
        var key = state.internString(objectKeyToString(property.key));
        var protect = hasProperties ? "gcProtectValue(" + valueTarget.id + ");" : '';
        return compile(property.value, state.childState({ target: valueTarget })) + "\n            " + protect + "\n            objectSet(" + objectVar + ", " + key.id + ", " + valueTarget.id + ");";
    }).join('\n');
    var objectProtect = hasProperties ? "gcProtectValue(" + objectVar + ");" : '';
    var propertiesProtect = hasProperties ? "gcUnprotectValues(" + (node.properties.length + 1) + ");" : '';
    return objectCreateSrc + "\n            " + objectProtect + "\n            " + propertiesSrc + "\n            " + propertiesProtect + " \n            " + objectSrc;
}
function compileProperty(property, computed, state) {
    return property.type === 'Identifier' && !computed
        ? assignToTarget(internString(property.name, state), state.target)
        : compile(property, state.childState({
            target: state.target,
        }));
}
function compileArrayExpression(node, state) {
    if (node.elements.length === 0) {
        return assignToTarget("arrayCreate(0)", state.target);
    }
    var arrayTarget = new IntermediateVariableTarget(state.getNextSymbol('array'));
    var arrayCreateSrc = assignToTarget("arrayCreate(" + node.elements.length + ")", arrayTarget);
    var protect = state.protectStack();
    var elementsSrc = node.elements.map(function (element, index) {
        var ii = state.internString(index.toString()).reference;
        if (element === null) {
            return "arrayInitialiseIndex(" + arrayTarget.id + ", " + ii + ", getNull());";
        }
        var valueTarget = new IntermediateVariableTarget(state.getNextSymbol('value'));
        return compile(element, state.childState({ target: valueTarget })) + "\n            " + protect.idSrc(valueTarget) + "\n            arrayInitialiseIndex(" + arrayTarget.id + ", " + ii + ", " + valueTarget.id + ");";
    }).join('\n');
    for (var i = 0; i < node.elements.length; i++) {
        state.internedNumber(i);
    }
    return arrayCreateSrc + "\n            " + protect.idSrc(arrayTarget) + "\n            " + elementsSrc + "\n            " + protect.endSrc() + "\n            " + assignToTarget(arrayTarget.id, state.target) + "\n            ";
}
function compileAssignmentExpression(node, state) {
    var target = node.left;
    var result = new PredefinedVariableTarget(state.getNextSymbol('result'));
    switch (target.type) {
        case 'Identifier': {
            if (target.name === 'this') {
                throw new CompileTimeError('Invalid left-hand side in assignment (this)');
            }
            var variable = state.internString(target.name);
            var update = node.operator === '='
                ? ''
                : result.id + " = " + getAssignmentOperatorFunction(node.operator) + "(envGet(env, " + variable.id + "), " + result.id + ");";
            return "JsValue* " + result.id + ";\n                    " + compile(node.right, state.childState({ target: result })) + "\n                    " + update + "\n                    envSet(env, " + variable.id + ", " + result.id + ");\n                    " + assignToTarget(result.id, state.target);
        }
        case 'MemberExpression': {
            var propertyTarget = new IntermediateVariableTarget(state.getNextSymbol('property'));
            var propertySrc = compileProperty(target.property, target.computed, state.childState({ target: propertyTarget }));
            var assignmentTarget = new IntermediateVariableTarget(state.getNextSymbol('object'));
            var assignmentTargetSrc = compile(target.object, state.childState({ target: assignmentTarget }));
            var update = node.operator === '='
                ? ''
                : result.id + " = " + getAssignmentOperatorFunction(node.operator) + "(objectGet(" + assignmentTarget.id + ", " + propertyTarget.id + "), " + result.id + ");";
            return "JsValue* " + result.id + ";\n                    " + assignmentTargetSrc + " // assignment target src\n                    " + propertySrc + " // property src\n                    " + compile(node.right, state.childState({ target: result })) + " // RHS\n                    " + update + " // any update\n                    objectSet(" + assignmentTarget.id + ", " + propertyTarget.id + ", " + result.id + "); // obj set\n                    " + assignToTarget(result.id, state.target);
        }
        default:
            return unimplemented(target.type)();
    }
}
function compileThrowStatement(node, state) {
    var errorTarget = new IntermediateVariableTarget(state.getNextSymbol('error'));
    return compile(node.argument, state.childState({ target: errorTarget })) + "\n            exceptionsThrow(" + errorTarget.id + ");";
}
function specifyType(t) {
    return t;
}
function compileTryStatement(node, state) {
    var trySrc = compile(node.block, state);
    var catchClause = specifyType(node.handler);
    var errorName = state.internString(getIdentifierName(catchClause.param));
    var catchSrc = "\n    {\n        JsValue* errorVars[1] = { runtimeGet()->thrownError };\n        JsValue* errorNames[1] = { " + errorName.id + " };\n        Env* parentEnv = env;\n        Env* env = envCreateForCall(parentEnv, errorNames, 1, errorVars, 1);\n        " + compile(catchClause.body, state) + "\n    }";
    return "if(exceptionsTry(env)) {\n        " + trySrc + "\n    } else {\n        " + catchSrc + "\n    }";
}
function compileIfStatement(node, state) {
    var testResultTarget = new IntermediateVariableTarget(state.getNextSymbol('conditionalPredicate'));
    var testSrc = compile(node.test, state.childState({
        target: testResultTarget,
    }));
    var consequentSrc = compile(node.consequent, state);
    var alternateSrc = node.alternate
        ? " else {\n          " + compile(node.alternate, state) + "\n        }"
        : '';
    return "/* if */\n            " + testSrc + "\n            if(isTruthy(" + testResultTarget.id + ")) {\n                " + consequentSrc + "\n            } " + alternateSrc;
}
function getUnaryOperatorFunction(operator) {
    var operatorFn = unaryOpToFunction[operator];
    if (!operatorFn) {
        throw Error("unimplemented operator '" + operator + "'");
    }
    return operatorFn;
}
function getAssignmentOperatorFunction(operator) {
    var operatorFn = assignmentOpToFunction[operator];
    if (!operatorFn) {
        throw Error("unimplemented operator '" + operator + "'");
    }
    return operatorFn;
}
function compileNewExpression(node, state) {
    return compileCallExpression(node, state);
}
function compileThisExpression(_node, state) {
    return assignToTarget("envGet(env, stringFromLiteral(\"this\"))", state.target);
}
function isDefaultClause(e) {
    return !e.test;
}
function compileSwitchStatement(node, state) {
    var discriminantTarget = new IntermediateVariableTarget(state.getNextSymbol('discriminant'));
    var discriminantSrc = compile(node.discriminant, state.childStateWithTarget(discriminantTarget));
    var runDefault = new PredefinedVariableTarget(state.getNextSymbol('runDefault'));
    var protect = state.protectStack();
    var nonDefaults = [];
    var defaults = [];
    for (var _i = 0, _a = node.cases; _i < _a.length; _i++) {
        var cs = _a[_i];
        if (isDefaultClause(cs)) {
            defaults.push(cs);
        }
        else {
            nonDefaults.push(cs);
        }
    }
    var casesSrc = [];
    var currentConsequent = state.getNextSymbol('consequent');
    for (var _b = 0, nonDefaults_1 = nonDefaults; _b < nonDefaults_1.length; _b++) {
        var cs = nonDefaults_1[_b];
        var consequentSrc = cs.consequent
            ? "goto after_" + currentConsequent + ";\n               " + currentConsequent + ":;\n               " + mapCompile(cs.consequent, state.childStateWithTarget(exports.SideEffectTarget)) + "\n               after_" + currentConsequent + ":;"
            : '';
        var testValueTarget = new IntermediateVariableTarget(state.getNextSymbol('caseValue'));
        var valueSrc = compile(cs.test, state.childStateWithTarget(testValueTarget));
        var caseSrc = "\n            if(getTrue() == strictEqualOperator(" + discriminantTarget.id + ", " + testValueTarget.id + ")) {\n                " + runDefault.id + " = false;\n                goto " + currentConsequent + ";\n            }\n        ";
        casesSrc.push(valueSrc + "\n                " + caseSrc + "\n                " + consequentSrc);
        if (cs.consequent) {
            currentConsequent = state.getNextSymbol('consequent');
        }
    }
    var defaultSrc = defaults.map(function (d) {
        return mapCompile(d.consequent, state.childStateWithTarget(exports.SideEffectTarget));
    }).join('');
    return "\n        do {\n            bool " + runDefault.id + " = true;\n            " + discriminantSrc + "\n            " + protect.idSrc(discriminantTarget) + "\n            " + casesSrc.join("\n") + "\n            \n            if(!" + runDefault.id + ") {\n                break;\n            }\n            " + defaultSrc + "\n            \n            break;\n        } while(1);\n        " + protect.endSrc() + "\n    ";
}
