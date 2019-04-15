__dirname=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

JsToC() {
    node js-build/js-to-c.js $1
}

CToExec() {
    ${__dirname}/runtime/scripts/c-to-exec $1 $2
}

if [[ uname == "Darwin" ]]; then
    _DLLEXT=dylib
else
    _DLLEXT=so
fi

DllExt() {
    echo $_DLLEXT
}

