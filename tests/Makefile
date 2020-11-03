all:
	ruby runtests.rb

test5:
	inform +../lib -v3 test5.inf
	frotz test5.z3

inform:
	inform -v8 test5.inf
	frotz test5.z8

clean:
	rm -f *z3 *z5 *z8 *scr *output
