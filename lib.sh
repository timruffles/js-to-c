JsToC() {
    node js-build/js-to-c.js $1
}

CToExec() {
    local cTarget=$1
    local exe=$2

    # string format warnings: ignored as we're compiling to C
    # float-equal: fine, JS float semantics too
    # padding: something to consider much later
    # gnu stuff: research and consider later
    clang -g -Weverything \
             -Wno-format-security \
             -Wno-format-pedantic \
             -Wno-float-equal \
             -Wno-padded \
             -Wno-gnu-folding-constant \
             -Wno-gnu-folding-constant \
             -Wno-missing-noreturn \
             -Wno-newline-eof \
             $cTarget $(GetRuntimeLibs) -o $exe
}

GetRuntimeLibs() {
    find runtime -depth 1 -name '*.c' -not -name '*.test.c'
}
