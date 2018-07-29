all_source_including_tests=$(wildcard *.h *.c)
source_files=$(filter-out %test.c %.h, $(all_source_including_tests))
source_with_headers=$(filter-out %test.c, $(all_source_including_tests))

CC=clang
CLANG_OPTS=-O0 -g -Wno-format-security -Wno-format-pedantic -Wno-float-equal -Wno-padded -Wno-gnu-folding-constant -Wno-gnu-folding-constant -Wno-missing-noreturn

out/environments_test: $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(source_files) environments.test.c -o $@

out/objects_test: objects.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(source_files) objects.test.c -o $@

out/strings_test: $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(source_files) strings.test.c -o $@

out/language_test: $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(source_files) language.test.c -o $@

out/functions_test: functions.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(source_files) functions.test.c -o $@

out/global_test: global.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(source_files) global.test.c -o $@

out/operators_test: operators.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(source_files) operators.test.c -o $@

out/heap_test: heap.test.c $(source_with_headers)
	$(CC) $(CLANG_OPTS) $(source_files) heap.test.c -o $@

.PHONY: test
test: out/environments_test out/language_test out/objects_test out/strings_test out/operators_test out/heap_test
	set -e; for t in out/*_test; do echo TESTING $$t; lldb -o run ./$$t || break; done


