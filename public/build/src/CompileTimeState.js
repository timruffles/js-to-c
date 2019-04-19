"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var js_to_c_1 = require("./js-to-c");
var ExecutableTarget = (function () {
    function ExecutableTarget() {
        this.type = 'Executable';
    }
    return ExecutableTarget;
}());
var LibraryTarget = (function () {
    function LibraryTarget(header, name) {
        this.header = header;
        this.name = name;
        this.type = 'Library';
    }
    return LibraryTarget;
}());
exports.LibraryTarget = LibraryTarget;
function getTarget(_a) {
    var header = _a.outputLibraryHeader, name = _a.outputLibraryName;
    if (!header && !name) {
        return new ExecutableTarget;
    }
    else {
        if (!header || !name) {
            throw Error("Must supply both library header and name");
        }
        return new LibraryTarget(header, name);
    }
}
var CompileTimeState = (function () {
    function CompileTimeState(options) {
        if (options === void 0) { options = {}; }
        this.functions = [];
        this.internedStrings = {};
        this.internedNumbers = new Map();
        this.counter = { id: 0 };
        this.target = js_to_c_1.SideEffectTarget;
        this.outputTarget = getTarget(options);
    }
    CompileTimeState.prototype.childState = function (_a) {
        var target = _a.target;
        var child = Object.create(this);
        Object.assign(child, {
            target: target,
        });
        return child;
    };
    CompileTimeState.prototype.childStateWithTarget = function (target) {
        return this.childState({ target: target });
    };
    CompileTimeState.prototype.internString = function (str) {
        if (!(str in this.internedStrings)) {
            this.internedStrings[str] = new js_to_c_1.InternedString(this.getNextSymbol('interned'), str);
        }
        return this.internedStrings[str];
    };
    CompileTimeState.prototype.getSymbolForId = function (prefix, id) {
        return prefix + "_" + id;
    };
    CompileTimeState.prototype.getNextSymbol = function (prefix) {
        return this.getSymbolForId(prefix, this.nextId());
    };
    CompileTimeState.prototype.nextId = function () {
        this.counter.id += 1;
        return this.counter.id;
    };
    CompileTimeState.prototype.withManualReturn = function (symbol) {
        if (this.target.type !== "ReturnTarget") {
            return { state: this, returnSrc: '' };
        }
        var target = new js_to_c_1.PredefinedVariableTarget(symbol);
        return {
            returnSrc: "return " + target.id + ";",
            state: this.childStateWithTarget(target),
        };
    };
    CompileTimeState.prototype.protectStack = function () {
        return new ProtectionStack();
    };
    CompileTimeState.prototype.internedNumber = function (value) {
        if (!this.internedNumbers.has(value)) {
            this.internedNumbers.set(value, new js_to_c_1.InternedNumber(this.getNextSymbol('internedNumber'), value));
        }
        return this.internedNumbers.get(value);
    };
    CompileTimeState.prototype.getInternedStrings = function () {
        return Object.values(this.internedStrings);
    };
    CompileTimeState.prototype.getInternedNumbers = function () {
        return Array.from(this.internedNumbers.values());
    };
    return CompileTimeState;
}());
exports.CompileTimeState = CompileTimeState;
var ProtectionStack = (function () {
    function ProtectionStack() {
        this.stack = [];
        this.ended = false;
    }
    ProtectionStack.prototype.idSrc = function (target) {
        this.stack.push(target);
        return "gcProtectValue(" + target.id + ");";
    };
    ProtectionStack.prototype.endSrc = function () {
        if (this.ended) {
            throw Error('Already ended');
        }
        this.ended = true;
        return "gcUnprotectValues(" + this.stack.length + ");";
    };
    return ProtectionStack;
}());
exports.ProtectionStack = ProtectionStack;
