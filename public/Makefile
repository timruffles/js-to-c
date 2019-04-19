
build/demo.built.js: *.ts ../src/*.ts
	tsc && browserify build/public/demo.js --debug -o build/demo.built.js


.PHONY: watch
watch:
	ls ../src/*.ts *.ts | entr make

.PHONY: deploy
deploy:
	git checkout --orphan gh-pages && git add --force build && git add . node_modules/codemirror && git commit -m "deploy" && git push -f origin head:gh-pages
