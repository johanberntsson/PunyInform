all: runtests

runtests:
	ruby runtests.rb

inform:
	inform -v8 -D test-issue107.inf
	frotz test-issue107.z8

testbench:
	inform +../lib -v3 testbench.inf
	frotz testbench.z3

ruletest:
	inform +../lib -v3 ruletest.inf
	frotz ruletest.z3

large3:
	inform +../lib -v3 largegame.inf
	frotz largegame.z3

large5:
	inform +../lib -v5 largegame.inf
	frotz largegame.z5

test-parser:
	inform +../lib -v5 -s -S -D test-parser.inf
	frotz test-parser.z5

test-issue107:
	inform +../lib -v3 -s test-issue107.inf
	frotz test-issue107.z3

clean:
	rm -f *z3 *z5 *z8 *scr *output *messages
