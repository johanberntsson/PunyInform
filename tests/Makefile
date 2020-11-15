all: grabifnotheld

runtests:
	ruby runtests.rb

grabifnotheld:
	inform +../lib -v3 grabifnotheld.inf
	frotz grabifnotheld.z3

test6:
	inform +../lib -v3 test6.inf
	frotz test6.z3

inform:
	inform -v8 test6.inf
	frotz test6.z8

clean:
	rm -f *z3 *z5 *z8 *scr *output
