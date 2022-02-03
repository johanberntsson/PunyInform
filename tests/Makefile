all: runtests

runtests:
	ruby runtests.rb

inform:
	inform -v8 test-grammarproperty.inf
	frotz test-grammarproperty.z8

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

test-parsetoken:
	inform +../lib -v3 -s test-parsetoken.inf
	frotz test-parsetoken.z3

test-parser:
	inform +../lib -v3 -s test-parser.inf
	frotz test-parser.z3

clean:
	rm -f *z3 *z5 *z8 *scr *output *messages
