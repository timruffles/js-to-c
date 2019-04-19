
build/demo.built.js: *.ts ../src/*.ts
	tsc && browserify build/public/demo.js --debug -o build/demo.built.js


.PHONY: watch
watch:
	ls ../src/*.ts *.ts | entr make

.PHONY: deploy
deploy:
	git branch -D gh-pages || : && git checkout --orphan gh-pages && npm i && make && git add --force build node_modules/codemirror && git add . && git commit -m "deploy" && git push -f origin head:gh-pages
