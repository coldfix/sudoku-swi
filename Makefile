all: bin/sudoku

bin/sudoku: generator/sudoku
	mkdir -p bin
	cp $< $@

generator/sudoku:
	$(MAKE) -C generator

.PHONY: generator/sudoku
