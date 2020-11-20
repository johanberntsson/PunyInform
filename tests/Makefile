all: runtests

runtests:
	ruby runtests.rb

grabifnotheld:
	inform +../lib -v3 grabifnotheld.inf
	frotz grabifnotheld.z3

test-score:
	inform +../lib -v3 test-score.inf
	frotz test-score.z3

inform:
	inform -v8 test-fullscore.inf
	frotz test-fullscore.z8

clean:
	rm -f *z3 *z5 *z8 *scr *output
