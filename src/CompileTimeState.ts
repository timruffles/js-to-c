import {CompileTarget, InternedString, PredefinedVariableTarget, SideEffectTarget} from "./js-to-c";

/**
 * The body of a JS function, ready to be linked with correct env etc
 * for execution
 **/
type JsFunctionBody = string;

export interface CompileOptions {
    outputLibraryName?: string,
    outputLibraryHeader?: string,
}

class ExecutableTarget {
   readonly type: 'Executable' = 'Executable';
}

export class LibraryTarget {
    readonly type: 'Library' = 'Library';
    constructor(
        readonly header: string,
        readonly name: string,
    ) {}
}

function getTarget({ outputLibraryHeader: header, outputLibraryName: name}: CompileOptions)  {
    if(!header && !name) {
        return new ExecutableTarget;
    } else {
        if(!header || !name) {
            throw Error("Must supply both library header and name");
        }
        return new LibraryTarget(header, name);
    }
}

export type OutputTarget = ExecutableTarget | LibraryTarget;

export class CompileTimeState {
    // shared between all state objects for a given compilation
    functions: JsFunctionBody[] = [];
    // our intern string pool
    interned: {[k: string]: InternedString } = {};
    private counter = { id: 0 };

    target: CompileTarget = SideEffectTarget;

    readonly outputTarget: OutputTarget;

    constructor(options: CompileOptions = {}) {
        this.outputTarget = getTarget(options);
    }

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

    /**
     * In cases where we need to compile multiple branches for target's value (e.g &&, ?)
     * we need to control where to place the return.
     *
     * This returns a new child state that either points at the original, or a predefined
     */
    withManualReturn(symbol: string): { state: CompileTimeState } | { returnSrc: string, state: CompileTimeState } {
        if(this.target.type !== "ReturnTarget") {
            return { state: this }
        }

        const target = new PredefinedVariableTarget(symbol)

        return {
            returnSrc: `return ${target.id};`,
            state: this.childState({
                target,
            }),
        }
    }
}

