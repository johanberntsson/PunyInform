all: test1

test1:
	@rm -f test1.scr test1.output
	@inform +.  +../lib -v3 -D test1.inf > /dev/null
	@frotz test1.z3 < test1.cmd > /dev/null
	@echo -n "test1 done: "
	@tail +6 test1.scr > test1.output
	diff test1.txt test1.output

