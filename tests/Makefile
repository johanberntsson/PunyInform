all: runtests

runtests:
	ruby runtests.rb

testbench:
	inform +../lib -v3 testbench.inf
	frotz testbench.z3

large3:
	inform +../lib -v3 largegame.inf
	frotz largegame.z3

large5:
	inform +../lib -v5 largegame.inf
	frotz largegame.z5

test-grammarproperty:
	inform +../lib -v3 -s test-grammarproperty.inf
	frotz test-grammarproperty.z3

inform:
	inform -v8 test-grammarproperty.inf
	frotz test-grammarproperty.z8

clean:
	rm -f *z3 *z5 *z8 *scr *output *messages
