all: runtests

runtests:
	ruby runtests.rb

testbench:
	inform +../lib -v3 testbench.inf
	frotz testbench.z3

test-beforeparsing:
	inform +../lib -v3 test-beforeparsing.inf
	frotz test-beforeparsing.z3

test-issue85:
	inform +../lib -v3 test-issue85.inf
	frotz test-issue85.z3

inform:
	inform -v8 test-grammarproperty.inf
	frotz test-grammarproperty.z8

clean:
	rm -f *z3 *z5 *z8 *scr *output *messages
