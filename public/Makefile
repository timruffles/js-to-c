
build/demo.built.js: *.ts ../src/*.ts
	tsc && browserify build/public/demo.js --debug -o build/demo.built.js


.PHONY: watch
watch:
	ls ../src/*.ts *.ts | entr make
