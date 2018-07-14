all_source_including_tests=$(wildcard *.h *.c)
source_files=$(filter-out %test.c %.h, $(all_source_including_tests))
source_with_headers=$(filter-out %test.c, $(all_source_including_tests))

CC=clang

out/environments_test: $(source_with_headers)
	$(CC) $(source_files) environments.test.c -o $@

out/objects_test: objects.test.c $(source_with_headers)
	$(CC) $(source_files) objects.test.c -o $@

out/strings_test: $(source_with_headers)
	$(CC) $(source_files) strings.test.c -o $@

out/language_test: $(source_with_headers)
	$(CC) $(source_files) language.test.c -o $@

out/functions_test: functions.test.c $(source_with_headers)
	$(CC) $(source_files) functions.test.c -o $@

out/global_test: global.test.c $(source_with_headers)
	$(CC) $(source_files) global.test.c -o $@

out/operators_test: global.test.c $(source_with_headers)
	$(CC) $(source_files) operators.test.c -o $@

test: out/environments_test out/language_test out/objects_test out/strings_test
	for t in out/*_test; do echo TESTING $$t; ./$$t; done


