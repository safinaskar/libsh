#!/usr/bin/make -f

SHELL = /bin/bash # Because of pipefail

GEN_HEADER = grep '//@' $^ | sed 's~ *//@\( \|\)~~' > $@ || { rm -f $@; exit 1; }

all: ex.h.in etc.h funcs.h.in funcs.c funcs.cmake

check: funcs.in expand-table.sh
	set -o pipefail; ./expand-table.sh 18 16 89 6 5 < funcs.in | diff -w - funcs.in

dev-clean:
	rm -f ex.h.in etc.h funcs.h.in funcs.c funcs.cmake

ex.h.in: ex.c
	$(GEN_HEADER)

etc.h: etc.c
	$(GEN_HEADER)

funcs.h.in funcs.c funcs.cmake: gen-funcs.sh funcs.in
	./gen-funcs.sh funcs.h.in funcs.c funcs.cmake < funcs.in || { rm -f funcs.h.in funcs.c funcs.cmake; exit 1; }
