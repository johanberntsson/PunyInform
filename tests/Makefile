all: test1

test1:
	rm test1.scr
	inform +.  +../lib -v5 -w -s -D test1.inf
	frotz test1.z5 < test1.cmd
	diff test1.txt test1.scr

