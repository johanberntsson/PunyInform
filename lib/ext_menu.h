Global menu_item;
Global item_width;
Global item_name;
Global menu_nesting;

#IfV3;
[ DoMenu menu_choices EntryR ChoiceR lines main_title i j;
	menu_nesting++;
.LKRD;
	menu_item = 0;
	lines = indirect(EntryR);
	main_title = item_name;

	print "--- "; print (string) main_title; print " ---^^";

	if (menu_choices ofclass Routine) menu_choices.call();
	else                              print (string) menu_choices;

	for (::) {
		print "^Type a number from 1 to ", lines, ", ENTER to redisplay or 0 to return to the game.^";
		print "> ";

       _ReadPlayerInput(true);
		j = parse_array->1; ! number of words
		i = parse_array-->1;
		if (j == 0) jump LKRD;
		i = TryNumber(1);
		if(i == 0) {
			menu_nesting--; if (menu_nesting > 0) rfalse;
			if (deadflag == 0) <<Look>>;
			rfalse;
		}
		if (i < 1 || i > lines) continue;
		menu_item = i;
		j = indirect(EntryR);
		print "^--- "; print (string) item_name; print " ---^^";
		j = indirect(ChoiceR);
		if (j == 2) jump LKRD;
		if (j == 3) rfalse;
	}
];
#IfNot;
#EndIf;

