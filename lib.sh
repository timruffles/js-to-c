JsToC() {
    node js-build/js-to-c.js $1
}

CToExec() {
    local cTarget=$1
    local exe=$2

    clang -g -Weverything $cTarget $(GetRuntimeLibs) -o $exe
}

GetRuntimeLibs() {
    find runtime -depth 1 -name '*.c' -not -name '*.test.c'
}
