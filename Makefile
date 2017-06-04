all: html/sudoku

html/sudoku: generator/sudoku
	cp $< $@

generator/sudoku:
	cd generator && $(MAKE)

.PHONY: generator/sudoku
