! This PunyInform extension offers a way to present menus that, from the
! programmer's perspective, work the same on any Z-machine version. The
! presentation on z5+ is like the DoMenu extension described in DM3 and
! which is currently part of the Inform 6 standard library. On z3 the
! presentation is simpler, but still fully functional.
!
! This extension deviates from the standard DoMenu implementation in one
! respect - when assigning a value to the variable item_width, this value
! should be the full width of the item, e.g. if the item title is "Help",
! item_width should be set to 4, whereas in the standard library it should
! be set to half the width of the item, e.g. 2 in this case.

! If you're writing a game which should be compilable with both PunyInform
! and the standard library, it may be more convenient to have ext_menu
! use the same logic for item_width as DoMenu in the standard library does.
! To get that behaviour, define the constant EXT_MENU_STDLIB_MODE before
! including the extension.

System_file;

Constant EXT_MENU = 1;

#Ifdef EXT_MENU_STDLIB_MODE; #Endif;

Global menu_item;
Global item_width;
Global item_name;
Global menu_nesting;

#IfV3;
[ DoMenu menu_choices EntryR ChoiceR lines main_title i j;
	menu_choices = 0; ! Avoid warning
	menu_nesting++;
._v3_redisplay_menu;
	menu_item = 0;
	lines = indirect(EntryR);
	main_title = item_name;

	print "--- "; print (string) main_title; print " ---^^";

	!if (menu_choices ofclass Routine) menu_choices.call();
	!else                              print (string) menu_choices;

	print "There is information provided on the following:^^";
	for(i = 1: i <= lines: i++) {
		menu_item = i;
		indirect(EntryR);
		print i, ": ", (string) item_name, "^";
	}
	if(menu_nesting == 1) {
		print "q: Resume the game^";
	} else {
		print "q: Previous menu^";
	}

	for (::) {
		print "^Select 1 to ", lines, " or ENTER to show the options again.^";
		print "> ";

       _ReadPlayerInput(true);
		j = parse->1; ! number of words
		if (j == 0) jump _v3_redisplay_menu;
		i = parse-->1;
		if(i == 'q//') {
			menu_nesting--; if (menu_nesting > 0) rfalse;
			if (deadflag == 0) <<Look>>;
			rfalse;
		}
		i = TryNumber(1);
		if (i < 1 || i > lines) continue;
		menu_item = i;
		j = indirect(EntryR);
		print "^--- "; print (string) item_name; print " ---^^";
		j = indirect(ChoiceR);
		if (j == 2) jump _v3_redisplay_menu;
		if (j == 3) rfalse;
	}
];
#IfNot;
! v5+

Constant NKEY__TX       = "N = next subject";
Constant PKEY__TX       = "P = previous";
Constant QKEY1__TX      = "  Q = resume game";
Constant QKEY2__TX      = "Q = previous menu";
Constant RKEY__TX       = "RETURN = read subject";

Constant NKEY1__KY      = 'N';
Constant NKEY2__KY      = 'n';
Constant PKEY1__KY      = 'P';
Constant PKEY2__KY      = 'p';
Constant QKEY1__KY      = 'Q';
Constant QKEY2__KY      = 'q';

[ DoMenu menu_choices EntryR ChoiceR
         lines main_title main_wid cl i j oldcl pkey ch y x;
	menu_nesting++;
	menu_item = 0;
	lines = indirect(EntryR);
	main_title = item_name; main_wid = item_width;
	cl = 7;

._redisplay_menu;

	oldcl = 0;
	@erase_window $ffff;
	ch = 1;
	i = ch * (lines+7);
	@split_window i;
	i = HDR_SCREENWCHARS->0;
	if (i == 0) i = 80;
	@set_window 1;
	@set_cursor 1 1;

	style reverse;
	FastSpaces(i);
#Ifdef EXT_MENU_STDLIB_MODE;
	j=1+(i/2-main_wid);
#Ifnot;
	j=1+(i-main_wid)/2;
#Endif;
	@set_cursor 1 j;
	print (string) main_title;
	y=1+ch; @set_cursor y 1; FastSpaces(i);
	x=1+1; @set_cursor y x; print (string) NKEY__TX;
	j=1+i-13; @set_cursor y j; print (string) PKEY__TX;
	y=y+ch; @set_cursor y 1; FastSpaces(i);
	@set_cursor y x; print (string) RKEY__TX;
	j=1+i-18; @set_cursor y j;

	if (menu_nesting == 1) print (string) QKEY1__TX;
	else                   print (string) QKEY2__TX;
	style roman;
	y = y+2*ch;
	@set_cursor y x; font off;

	if (menu_choices ofclass String) print (string) menu_choices;
	else                             menu_choices.call();

	x = 1+3;


	for (::) {
		if (cl ~= oldcl) {
			if (oldcl>0) {
				y=1+(oldcl-1)*ch; @set_cursor y x; print " ";
			}
			y=1+(cl-1)*ch; @set_cursor y x; print ">";
		}

		oldcl = cl;
		@read_char 1 -> pkey;
		if (pkey == NKEY1__KY or NKEY2__KY or 130) {
			cl++; if (cl == 7+lines) cl = 7; continue;
		}
		if (pkey == PKEY1__KY or PKEY2__KY or 129) {
			cl--; if (cl == 6) cl = 6+lines; continue;
		}
		if (pkey == QKEY1__KY or QKEY2__KY or 27 or 131) break;
		if (pkey == 10 or 13 or 132) {
			@set_window 0; font on;
			new_line; new_line; new_line;

			menu_item = cl-6;
			EntryR.call();

			@erase_window $ffff;
			@split_window ch;
			@set_window 1; @set_cursor 1 1; style reverse; FastSpaces(i);
#Ifdef EXT_MENU_STDLIB_MODE;
			j=1+(i/2-item_width);
#Ifnot;
			j=1+(i-item_width)/2;
#Endif;
			@set_cursor 1 j;
			print (string) item_name;
			style roman; @set_window 0; new_line;

			i = ChoiceR.call();
			if (i == 2) jump _redisplay_menu;
			if (i == 3) break;

			print "^[Please press SPACE.]";
			@read_char 1 -> pkey; jump _redisplay_menu;
		}
	}
	menu_nesting--; if (menu_nesting > 0) rfalse;
	font on; @set_cursor 1 1;
	@erase_window -1; @set_window 0;
#IfDef PUNYINFORM_MAJOR_VERSION;
	statusline_current_height = 0;
#IfNot;
	gg_statuswin_cursize = 0;
#EndIf;
	new_line; new_line; new_line;
	if (deadflag == 0) <<Look>>;
];

#EndIf;
