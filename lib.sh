__dirname=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )

JsToC() {
    node js-build/js-to-c.js $1
}

CToExec() {
    ${__dirname}/runtime/scripts/c-to-exec $1 $2
}

CToLib() {
    ${__dirname}/runtime/scripts/c-to-lib $1 $2
}


