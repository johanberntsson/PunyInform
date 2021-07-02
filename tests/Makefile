all: runtests

runtests:
	ruby runtests.rb

testbench:
	inform +../lib -v3 testbench.inf
	frotz testbench.z3

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
	rm -f *z3 *z5 *z8 *scr *output *messages
