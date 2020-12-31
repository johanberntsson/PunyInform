all: runtests

runtests:
	ruby runtests.rb

thirdnoun:
	inform +../lib -v3 thirdnoun.inf
	frotz thirdnoun.z3

test-beforeparsing:
	inform +../lib -v3 test-beforeparsing.inf
	frotz test-beforeparsing.z3

containers:
	inform +../lib -v3 containers.inf
	frotz containers.z3

inform:
	inform -v8 thirdnoun.inf
	frotz thirdnoun.z8

clean:
	rm -f *z3 *z5 *z8 *scr *output
