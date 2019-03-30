# TODO would be good to move this duplication out into lib.sh
all_source_including_tests=$(wildcard *.h *.c)
source_files=$(filter-out %.test.c %.h, $(all_source_including_tests))
all_deps=$(source_files)  vendor/libuv-1.26.0/out/cmake/libuv.dylib
source_with_headers=$(filter-out %.test.c, $(all_source_including_tests))
all_tests=$(filter %.test.c, $(all_source_including_tests))
libuv="vendor/libuv-1.26.0/out/cmake/libuv.dylib"

CC=clang
CLANG_OPTS=-std=c11 -O0 -g -Weverything -Wno-format-security -Wno-format-pedantic -Wno-float-equal -Wno-padded -Wno-gnu-folding-constant -Wno-gnu-folding-constant -Wno-missing-noreturn


$(libuv): vendor/libuv-1.26.0
	cd vendor/libuv-1.26.0 && mkdir -p out/cmake ; cd out/cmake ; cmake -DBUILD_TESTING=ON ../.. && make all

.PHONY: libs
libs: $(libuv)

out/environments_test: environments.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(all_deps) environments.test.c -o $@

out/objects_test: objects.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(all_deps) objects.test.c -o $@

out/strings_test: strings.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(all_deps) strings.test.c -o $@

out/language_test: language.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(all_deps) language.test.c -o $@

out/functions_test: functions.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(all_deps) functions.test.c -o $@

out/global_test: global.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(all_deps) global.test.c -o $@

out/operators_test: operators.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(all_deps) operators.test.c -o $@

out/gc_test: gc.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(all_deps) gc.test.c -o $@

out/config_test: config.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(all_deps) config.test.c -o $@

out/exceptions_test: exceptions.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(all_deps) exceptions.test.c -o $@

out/operations_test: operations.c operations.h operations.test.c
	$(CC) $(CLANG_OPTS) $(all_deps) operations.test.c -o $@

out/array_test: array.c array.h array.test.c
	$(CC) $(CLANG_OPTS) $(all_deps) array.test.c -o $@


# special case tests with fewer dependencies

out/_freelist_test: _freelist.c _freelist.h _freelist.test.c
	$(CC) $(CLANG_OPTS) _freelist.c _freelist.test.c -o $@


# manually ordered for dependencies - i.e objects aren't going to work if we can't allocate
# or have reliable strings
.PHONY: test
test: out/environments_test out/objects_test out/strings_test out/language_test out/operators_test out/functions_test out/global_test out/gc_test out/config_test out/exceptions_test out/operations_test out/_freelist_test
	for t in out/*_test; do $$t; done

.PHONY: test_debug
test_debug: out/environments_test out/objects_test out/strings_test out/language_test out/operators_test out/functions_test out/global_test out/gc_test out/config_test out/exceptions_test out/operations_test out/_freelist_test
	set -e; for t in out/*_test; do echo TESTING $$t; lldb -o run ./$$t || break; done


