JsToC() {
    node js-build/js-to-c.js $1
}

ClangOptions() {
    # string format warnings: ignored as we're compiling to C
    # float-equal: fine, JS float semantics too
    # padding: something to consider much later
    # gnu stuff: research and consider later
    echo -g  \
         -std=c11 \
         -O0 \
         -Weverything \
         -Wno-format-security \
         -Wno-format-pedantic \
         -Wno-float-equal \
         -Wno-padded \
         -Wno-gnu-folding-constant \
         -Wno-gnu-folding-constant \
         -Wno-missing-noreturn \
         -Wno-newline-eof
}

CToExec() {
    local cTarget=$1
    local exe=$2

    clang $(ClangOptions) $cTarget out/runtime.dylib out/prelude.dylib -o $exe
}

CToLib() {
    local cTarget=$1
    local out=$2

    clang -dynamiclib $(ClangOptions) out/runtime.dylib $cTarget -o $out
}

GetRuntimeLibs() {
    find runtime -depth 1 -name '*.c' -not -name '*.test.c'
}
