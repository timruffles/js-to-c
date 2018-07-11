

PSEDUO: test

*.test.c:
	clang -Weverything $?

tests := $(wildcard *.test.c)
test: $(tests)
	for t in *.test; do ./$$t; done

