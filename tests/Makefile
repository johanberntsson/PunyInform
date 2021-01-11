all: runtests

runtests:
	ruby runtests.rb

test-allbut:
	inform +../lib -v3 test-allbut.inf
	frotz test-allbut.z3

test-beforeparsing:
	inform +../lib -v3 test-beforeparsing.inf
	frotz test-beforeparsing.z3

containers:
	inform +../lib -v3 containers.inf
	frotz containers.z3

inform:
	inform -v8 test-allbut.inf
	frotz test-allbut.z8

clean:
	rm -f *z3 *z5 *z8 *scr *output
