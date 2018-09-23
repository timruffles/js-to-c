

js-build/js-to-c.js: src/*.ts
	tsc


out/runtime.dylib: runtime/*.c
	./runtime/scripts/compile-dylib

out/prelude.dylib: out/runtime.dylib
	./compile-js-lib prelude runtime/prelude.js out

default: prelude
