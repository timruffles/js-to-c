js-build/js-to-c.js: src/*.ts
	tsc

out/prelude.dylib: js-build/js-to-c.js runtime/prelude.js
	./compile-js-lib prelude runtime/prelude.js out/prelude.dylib

build: js-build/js-to-c.js out/prelude.dylib

out/ci-docker-container: .
	cd actions/ci && docker build -t jsc-ci . && cd - && touch $@

.PHONY: ci-run
ci-run: out/ci-docker-container
	docker run -e GITHUB_WORKSPACE=/github/workspace -v `pwd`:/github/workspace jsc-ci

.PHONY: ci
ci: install build test
	echo

.PHONY: clean
clean:
	rm -rf out/* js-build/*

.PHONY: install
install:
	npm install && cd runtime && make libs

.PHONY: test
test:
	cd runtime && make test && cd .. && npm test
