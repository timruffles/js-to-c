import {CompileTarget, InternedString, SideEffectTarget} from "./js-to-c";

/**
 * The body of a JS function, ready to be linked with correct env etc
 * for execution
 **/
type JsFunctionBody = string;

export interface CompileOptions {
    outputLibraryName?: string,
}

export class CompileTimeState {
    // shared between all state objects for a given compilation
    functions: JsFunctionBody[] = [];
    // our intern string pool
    interned: {[k: string]: InternedString } = {};
    private counter = { id: 0 };

    target: CompileTarget = SideEffectTarget;

    readonly outputLibraryName?: string

    constructor({ outputLibraryName }: CompileOptions = {}) {
        this.outputLibraryName = outputLibraryName;
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
}
