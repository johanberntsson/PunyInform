all: test1

test1:
	@rm -f test1.scr
	@inform +.  +../lib -v3 -D test1.inf > /dev/null
	@frotz test1.z3 < test1.cmd > /dev/null
	@echo -n "test1 done: "
	diff test1.txt test1.scr

