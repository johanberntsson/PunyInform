all: runtests

runtests:
	ruby runtests.rb

test-issue64:
	inform +../lib -v3 test-issue64.inf
	frotz test-issue64.z3

test-beforeparsing:
	inform +../lib -v3 test-beforeparsing.inf
	frotz test-beforeparsing.z3

containers:
	inform +../lib -v3 containers.inf
	frotz containers.z3

inform:
	inform -v8 test-issue64.inf
	frotz test-issue64.z8

clean:
	rm -f *z3 *z5 *z8 *scr *output
