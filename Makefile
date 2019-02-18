js-build/js-to-c.js: src/*.ts
	tsc

out/runtime.dylib: runtime/*.c
	./runtime/scripts/compile-dylib

out/prelude.dylib: js-build/js-to-c.js runtime/prelude.js out/runtime.dylib
	./compile-js-lib prelude runtime/prelude.js out/prelude.dylib

build: js-build/js-to-c.js out/runtime.dylib out/prelude.dylib

.PHONY: clean
clean:
	rm -rf out/* js-build/*



.PHONY: install
install:
	npm install
