all: runtests

runtests:
	ruby runtests.rb

test-issue73:
	inform +../lib -v3 test-issue73.inf
	frotz test-issue73.z3

test-beforeparsing:
	inform +../lib -v3 test-beforeparsing.inf
	frotz test-beforeparsing.z3

containers:
	inform +../lib -v3 containers.inf
	frotz containers.z3

inform:
	inform -v8 testbench.inf
	frotz testbench.z8

clean:
	rm -f *z3 *z5 *z8 *scr *output
