! PunyInform: A small stdlib and parser for interactive fiction
! suitable for old-school computers such as the Commodore 64.
! Designed to be similar, but not identical, to the Inform 6 library.

! Most public routines, as described in DM4, are available for a game
! developer. See the manual in the documentation folder for a full list
! of these routines, as well as additional useful routines that are
! PunyInform only.

! Reference documentation:
! DM4: http://www.inform-fiction.org/manual/html/dm4index.html
! Tech: https://www.inform-fiction.org/source/tm/TechMan.txt
! Z-machine: https://www.inform-fiction.org/zmachine/standards/z1point1

! Comment/uncomment to restrict default debug messages behaviour
! (all can be overridden by adding them in the game source)
!Constant DEBUG_PARSEANDPERFORM;
!Constant DEBUG_PARSETOKEN;
!Constant DEBUG_PARSEPATTERN;
!Constant DEBUG_PARSENOUNPHRASE;
!Constant DEBUG_GETNEXTNOUN;
!Constant DEBUG_SCOPE;

System_file;

Include "messages.h";


! ######################### Include utility files

Include "grammar.h";

! ######################### Helper routines
[ UnsignedCompare p_x p_y _u _v;
	if (p_x == p_y) return 0;
	if (p_x < 0 && p_y >= 0) return 1;
	if (p_x >= 0 && p_y < 0) return -1;
	_u = p_x&~WORD_HIGHBIT; _v = p_y&~WORD_HIGHBIT;
	if (_u > _v) return 1;
	return -1;
];

#Ifdef OPTIONAL_MANUAL_SCOPE;
[ RunEntryPointRoutine p_routine;
	return p_routine();
];
#Ifnot;
[ RunEntryPointRoutine p_routine _ret;
	if(p_routine) {
		_ret = p_routine();
		scope_modified = true;
		return _ret;
	}
];
#Endif;

[ IndirectlyContains p_o1 p_o2;
	! Does o1 indirectly contain o2?  (Same as testing if o1 is one of the ancestors of o2.)
	@jz p_o2 ?rfalse;
.recheck;
	if (p_o1 == p_o2) rtrue;
	p_o2 = parent(p_o2);
	@jz p_o2 ?~recheck;
	rfalse;
];

[ CommonAncestor p_o1 p_o2 _i _j;
    ! Find the nearest object indirectly containing o1 and o2,
    ! or return 0 if there is no common ancestor.
    _i = p_o1;
    while (_i) {
        _j = p_o2;
        while (_j) {
            if (_j == _i) return _i;
            _j = parent(_j);
        }
        _i = parent(_i);
    }
    return 0;
];

#Ifv3;
[ ChangeFgColour p_colour;
	p_colour = 1; ! Avoid warning
];
#Endif;

#IfV5;

[ ClearScreen window;
	if (clr_on) {
		@set_colour clr_fg clr_bg;
	}
    switch (window) {
      WIN_ALL:    @erase_window -1;
      WIN_STATUS: @erase_window 1;
      WIN_MAIN:   @erase_window 0;
    }
];

[ ChangeFgColour p_colour;
	clr_fg = p_colour;
	if (clr_on) {
		@set_colour clr_fg CLR_CURRENT;
	}
];

Array cursor_pos --> 2;

[ _StatusLineHeight p_height;
	if (statusline_current_height ~= p_height) {
		@split_window p_height;
		statusline_current_height = p_height;
	}
];


[ _MoveCursor line column;  ! 1-based postion on text grid
	if (~~statuswin_current) {
		@set_window 1;
		if (clr_on && clr_fgstatus > 1) {
			@set_colour clr_fgstatus clr_bg;
		}
		style reverse;
	}
	if (line == 0) {
		line = 1;
		column = 1;
	}
	@set_cursor line column;
	statuswin_current = true;
];

[ _MainWindow;
	if (statuswin_current) {
		if (clr_on && clr_fgstatus > 1) {
			@set_colour clr_fg clr_bg;
		}
		style roman;
		@set_window 0;
	}
	statuswin_current = false;
];


Array TenSpaces -> "          ";

[ _PrintSpacesOrMoveBack p_col p_string _current_col;
	p_col = screen_width - p_col;

	@get_cursor cursor_pos;
	_current_col = cursor_pos --> 1;

	if(_current_col > p_col || cursor_pos --> 0 > 1)
		_MoveCursor(1, p_col);
	else
		FastSpaces(p_col - _current_col);

	if(p_string)
		print (string) p_string;
];

Constant ONE_SPACE_STRING = " ";

[ _PrintStatusLineTime _h _pm;
	if (screen_width > 29) {
		if (screen_width > 39) {
			if (screen_width > 66) {
				! Width is 67-, print "Time: 12:34 pm" with some space to the right
				_PrintSpacesOrMoveBack(20, TIME__TX);
			} else {
				! Width is 40-66, print "Time: 12:34 pm" at right edge
				_PrintSpacesOrMoveBack(15, TIME__TX);
			}
		} else {
			! Width is 30-, print "12:34 pm" at right edge
			_PrintSpacesOrMoveBack(9, ONE_SPACE_STRING);
		}
		_h = status_field_1;
		if (_h > 11) {
			_pm = true;
		}
		if (_h > 12) {
			_h = _h - 12;
		}
		if(_h < 1) print 12; else print _h;
		@print_char ':';
		if (status_field_2<10)
			@print_char '0';
		print status_field_2;
		if (_pm)
			print " pm";
		else
			print " am";
	}
];

[ _PrintStatusLineScore;
#Ifdef OPTIONAL_SL_NO_SCORE;
#Ifndef OPTIONAL_SL_NO_MOVES;
	! Show moves only
	if (screen_width > 25) {
		if (screen_width < 30) {
			! Width is 25-29, only print moves as "0"
			_PrintSpacesOrMoveBack(4, ONE_SPACE_STRING);
		} else {
			! Width is 30-, print "Moves: 0"
			if (screen_width > 52) {
				! Width is 53+, leave some space to the right
				_PrintSpacesOrMoveBack(15, MOVES__TX);
			} else {
				_PrintSpacesOrMoveBack(11, MOVES__TX);
			}
		}
		print status_field_2;
	}
#Endif; ! Ifndef NO_MOVES
#Ifnot;
	! Show score and maybe moves
	if (screen_width > 24) {
		if (screen_width < 30) {
			! Width is 25-29, only print score as "0", no moves
			_PrintSpacesOrMoveBack(3, ONE_SPACE_STRING);
			print status_field_1;
		} else {
#Ifdef OPTIONAL_SL_NO_MOVES;
	! Show score only
			! Width is 30-, print "Score: 0"
			if(screen_width < 55)
				_PrintSpacesOrMoveBack(10, SCORE__TX);
			else
				_PrintSpacesOrMoveBack(13, SCORE__TX);
			print status_field_1;
#Ifnot;
	! Show score + moves
			if (screen_width > 66) {
				! Width is 67-, print "Score: 0 Moves: 0"
				_PrintSpacesOrMoveBack(28, SCORE__TX);
				print status_field_1;
				_PrintSpacesOrMoveBack(14, MOVES__TX);
			} else {
				if (screen_width > 36) {
					! Width is 37-66, print "Score: 0/0"
					_PrintSpacesOrMoveBack(15, SCORE__TX);
				} else {
					! Width is 29-35, print "0/0"
					_PrintSpacesOrMoveBack(8, ONE_SPACE_STRING);
				}
				print status_field_1;
				@print_char '/';
			}
			print status_field_2;
#Endif;
		}
	}
#Endif;
];

[ DrawStatusLine _visibility_ceiling;
	! For wide screens (67+ columns):
	! * print a space before room name, and "Score: xxx  Moves: xxxx" to the right.
	! * Room names up to 39 characters are never truncated.
	! For narrow screens:
	! * No space before room name
	! * Print "Score: xxx/yyyy", "xxx/yyyy", "xxx" or nothing, depending on screen width
	! * Room names up to 21 characters are never truncated. On a 40 column screen, room names up to 24 characters are never truncated.

	! If there is no player location, we shouldn't try to draw status window
	if (location == nothing || parent(player) == nothing)
		return;

	_StatusLineHeight(statusline_height);
	_MoveCursor(1, 1); ! This also sets the upper window as active.
	if(screen_width > 66) @print_char ' ';

	_visibility_ceiling = ScopeCeiling(player);
! print (object) _visibility_ceiling;
#Ifdef OPTIONAL_NO_DARKNESS;
	if (_visibility_ceiling == location)
		PrintShortName(location); ! If it's light, location == real_location
	else
		print (The) _visibility_ceiling;
#Ifnot;
	if (location == thedark || _visibility_ceiling == location)
		PrintShortName(location); ! If it's light, location == real_location
	else
		print (The) _visibility_ceiling;
#Endif;

#Ifdef STATUSLINE_TIME;
	_PrintStatusLineTime();
#Ifnot;
	#Ifdef STATUSLINE_SCORE;
		_PrintStatusLineScore();
	#Ifnot;
		if (sys_statusline_flag) {
			_PrintStatusLineTime();
		} else {
			_PrintStatusLineScore();
		}
	#Endif;
#Endif;

	! Regardless of what kind of status line we have printed, print spaces to the end.
	_PrintSpacesOrMoveBack(-1);
	_MainWindow(); ! set_window
];
#EndIf;

[ ObjectCapacity p_obj;
	if(p_obj provides capacity)
		return RunRoutines(p_obj, capacity);
	return DEFAULT_CAPACITY;
];

[ _AtFullCapacity p_s _obj _k;
    if (p_s == player) {
        objectloop (_obj in p_s)
            if (_obj hasnt worn) _k++;
    } else
        _k = children(p_s);
#IfDef SACK_OBJECT;
	if (_k < ObjectCapacity(p_s) || (p_s == player && _RoomInSack())) rfalse;
#IfNot;
	if (_k < ObjectCapacity(p_s)) rfalse;
#EndIf;
];

#IfDef SACK_OBJECT;
[ _RoomInSack _obj _ks;
    if (SACK_OBJECT in player) {
        for (_obj=youngest(player) : _obj : _obj=elder(_obj)) {
#Ifdef OPTIONAL_NO_DARKNESS;
            if (_obj ~= SACK_OBJECT && _obj hasnt worn) {
#Ifnot;
            if (_obj ~= SACK_OBJECT && _obj hasnt worn or light) {
#Endif;
                _ks = keep_silent;
                keep_silent = 1;
                <Insert _obj SACK_OBJECT>;
                keep_silent = _ks;
                if (_obj in SACK_OBJECT) {
                    if (keep_silent == 0) PrintMsg(MSG_SACK_PUTTING, _obj);
                    rtrue;
                }
            }
		}
    }
    rfalse;
];
#EndIf;

#IfDef DEBUG;
#IfnDef DebugParseNameObject;
[ DebugParseNameObject p_obj;
	@inc p_obj; ! Just to get rid of warning that p_obj isn't used
	rfalse;
];
#Endif;
#Endif;

#Ifdef OPTIONAL_LANGUAGE_NUMBER;

[ LanguageNumber n f;
    if (n == 0)    { print "zero"; rfalse; }
    if (n < 0)     { print "minus "; n = -n; }
    if (n >= 1000) { print (LanguageNumber) n/1000, " thousand"; n = n%1000; f = 1; }
    if (n >= 100)  {
        if (f == 1) print ", ";
        print (LanguageNumber) n/100, " hundred"; n = n%100; f = 1;
    }
    if (n == 0) rfalse;
    #Ifdef DIALECT_US;
    if (f == 1) print " ";
    #Ifnot;
    if (f == 1) print " and ";
    #Endif;
#Ifdef OPTIONAL_ALLOW_WRITTEN_NUMBERS;
#IfV3;
	if(n < 13 || n == 20)
		print (address) LanguageNumbers-->(2 * n - 1);
	else if(n < 20)
		print (string) LanguageNumberStrings-->(n - 13);
	else {
		print (string) LanguageNumberTensStrings-->(n / 10 - 2);
        if (n%10 ~= 0) print "-", (LanguageNumber) n%10;
	}
#Ifnot;
	if(n < 21)
		print (address) LanguageNumbers-->(2 * n - 1);
	else {
		print (string) LanguageNumberTensStrings-->(n / 10 - 2);
        if (n%10 ~= 0) print "-", (LanguageNumber) n%10;
	}
#Endif;
#Ifnot;
	if(n < 20)
		print (string) LanguageNumberStrings-->(n - 1);
	else {
		print (string) LanguageNumberTensStrings-->(n / 10 - 2);
        if (n%10 ~= 0) print "-", (LanguageNumber) n%10;
	}
#Endif;
];

#Endif;


[ PrintShortName o;
    if (o == 0) { print "nothing"; rtrue; }
    switch (metaclass(o)) {
      Routine:  print "<routine ", o, ">"; rtrue;
      String:   print "<string ~", (string) o, "~>"; rtrue;
      nothing:  print "<illegal object number ", o, ">"; rtrue;
    }
    if (o.short_name ~= 0 && PrintOrRun(o, short_name, true) ~= 0) rtrue;
    print (object) o;
];

[ _PrintObjName p_obj p_form;
	if(p_obj hasnt proper) {
		if(p_form == FORM_CDEF) {
			print "The ";
		} else if(p_form == FORM_DEF) {
			print "the ";
		} else if(p_form == FORM_INDEF) {
			if(p_obj.&article) {
				PrintOrRun(p_obj, article, true);
				print " ";
			}
			else if(p_obj has pluralname)
				print "some ";
			else
				print "a ";
		}
	}
	PrintShortName(p_obj);
];

[ _IsAreString p_plural;
	if(p_plural == 2) return ARE_STR;
	return IS_STR;
];

[ _PrintAfterEntry p_obj;
#Ifndef OPTIONAL_NO_DARKNESS;
	if(p_obj has light && p_obj hasnt animate) print " (providing light)";
#Endif;
	if(p_obj has worn && action == ##Inv) print " (worn)";
	if(p_obj has container && p_obj hasnt open) print " (which is closed)";
	if(p_obj has container && (p_obj has open || p_obj has transparent)) {
		if(PrintContentsFromR(1, child(p_obj)) == 0) {
			print " (which is empty)";
			if(c_style & NEWLINE_BIT)
				new_line;
		} else {
			if(c_style & NEWLINE_BIT == 0)
				print " (which contains ";
			else {
				if(p_obj has open)
					print " (which is open)";
				new_line;
			}
			c_style = c_style & ~ISARE_BIT;
			PrintContentsFromR(0, child(p_obj));
			if(c_style & NEWLINE_BIT == 0) {
				print (char) ')';
			}
		}

	} else if(p_obj has supporter) {
		if(c_style & NEWLINE_BIT) {
			new_line;
			PrintContentsFromR(0, child(p_obj));
		} else
			if(PrintContents(" (on which ", p_obj, ISARE_BIT)) print (char) ')';
	} else if(c_style & NEWLINE_BIT)
		new_line;
];

#Ifdef OPTIONAL_LIST_TOGETHER;

[_SortForListTogether p_obj
	_obj _val _val2 _temp _next _first_obj _last_obj _ret;
	_obj = child(p_obj);
	while(_obj) {
#Iftrue LIST_TOGETHER_PROP_ID < INDIV_PROP_START;
		_val = _val = _obj.list_together;
#Ifnot;
		_val = _GetIndividualLTValue(_obj);
#Endif;
		if(_val == 0 || _PrintContentsShowObj(_obj) == false) {
			_temp = _obj;
			_obj = sibling(_obj);
			move _temp to _PunyObj;
			continue;
		}
		! This object is the first that has this list_together value
		_next = 0;
		_first_obj = _obj;
		while(_obj) {
#Iftrue LIST_TOGETHER_PROP_ID < INDIV_PROP_START;
			_val2 = _obj.list_together;
#Ifnot;
			_val2 = _GetIndividualLTValue(_obj);
#Endif;
			if(_val2 ~= _val || _PrintContentsShowObj(_obj) == false) {
				if(_next == 0)
					_next = _obj;
				_obj = sibling(_obj);
				continue;
			}
			! This object has the list_together value we're looking for
			_last_obj = _obj;
			_temp = sibling(_obj);
			move _obj to Directions;
			_obj = _temp;
		}

		! If group of objects holds a single object, consider it a normal object
		if(_first_obj == _last_obj)
			move _first_obj to _PunyObj;

		_obj = _next;
	}
	_next = child(Directions);
	while(_next) {
		move _next to p_obj;
		_next = child(Directions);
	}
	_ret = child(p_obj);
	_next = child(_PunyObj);
	while(_next) {
		move _next to p_obj;
		_next = child(_PunyObj);
	}
	return _ret;
];

[ _GetIndividualLTValue p_obj;
	if(p_obj provides list_together)
		return p_obj.list_together;
	rfalse;
];

[_PrintContentsPrintLTGroup p_obj _count _obj
		_bak_lt _bak_lt_val _bak_style;

	_bak_lt_val = lt_value;
#Iftrue LIST_TOGETHER_PROP_ID < INDIV_PROP_START;
	lt_value = p_obj.list_together;
#Ifnot;
	lt_value = _GetIndividualLTValue(p_obj);
#Endif;

	_count = 0;
	for(_obj = p_obj: _obj ~= 0: _obj = NextEntry(_obj, pc_depth)) _count++;

	if(lt_value ofclass String) {
		if(c_style & NEWLINE_BIT) {
			FastSpaces(pc_indent);
		}
#Ifdef OPTIONAL_LANGUAGE_NUMBER;
		LanguageNumber(_count);
#Ifnot;
		print _count;
#Endif;
		print " ", (string) lt_value;
		if(c_style & NEWLINE_BIT)
			print ":^";
		else
			print " (";

		PrintContentsFromR(0, p_obj, _count);
		if(c_style & NEWLINE_BIT == 0)
			print ")";
	} else {
		_bak_lt = listing_together;
		_bak_style = c_style;
		! list_together is a routine
		inventory_stage = 1;
		parser_one = p_obj;
		parser_two = pc_depth;
		listing_together = p_obj;
		if(c_style & NEWLINE_BIT) {
			FastSpaces(pc_indent);
		}
		if(p_obj.list_together() == false) {
			if(c_style & NEWLINE_BIT) {
				pc_skip_next_indent = true;
			}
			PrintContentsFromR(0, p_obj, _count);

			inventory_stage = 2;
			parser_one = p_obj;
			parser_two = pc_depth;
			p_obj.list_together();
		}

		inventory_stage = 0;
		listing_together = _bak_lt;
		c_style = _bak_style;
	}
	lt_value = _bak_lt_val;
];

[ NextEntry p_obj
	p_depth
	_LT_value_2 _obj;
	_obj = sibling(p_obj);
	if(_obj == 0 || _obj has concealed or scenery ||
			(_obj hasnt workflag && c_style & WORKFLAG_BIT ~= 0 && p_depth == 0))
		rfalse;
#Iftrue LIST_TOGETHER_PROP_ID < INDIV_PROP_START;
	_LT_value_2 = _obj.list_together;
#Ifnot;
	_LT_value_2 = _GetIndividualLTValue(_obj);
#Endif;
	if(_LT_value_2 == lt_value)
		return _obj;
	rfalse;
];

#Endif;

[ PrintContents p_first_text p_obj p_style
		_bak_style _ret _bak_depth _bak_indent
		_bak_inv_stage;
! Print the contents of p_obj. Return true if anything was printed.
! If any objects are printed, prefix with p_first_text.
! If p_style has WORKFLAG_BIT set, only print objects which have workflag set.
! If p_style has NEWLINE_BIT set, print each object on a new line, indented.
! If p_style has ISARE_BIT set, print is/are before list.
!   Special parameters:
!   - If p_first_text is a routine, it will be called with p_obj as argument
!   - If p_first_text is 0, no prefix string will be printed
!   - If p_first_text is 1, don't print anything, but return:
!       0 if there are no printable objects in/on p_obj
!       1 if there's exactly one printable object and it doesn't have pluralname
!       2 if there are 2+ printable objects or one object with pluralname
	_bak_style = c_style; _bak_depth = pc_depth; _bak_indent = pc_indent;
	_bak_inv_stage = inventory_stage;
	c_style = p_style; pc_depth = pc_initial_depth - 1; pc_indent = 2 + 2 * pc_depth;

	_ret = PrintContentsFromR(p_first_text, child(p_obj));

	c_style = _bak_style; pc_depth = _bak_depth; pc_indent = _bak_indent;
	inventory_stage = _bak_inv_stage;
	return _ret;
];

#Ifdef OPTIONAL_LIST_TOGETHER;
[ PrintContentsFromR p_first_text p_obj p_max_count ! p_style
		_obj _printed_any_objects _last_obj _show_obj _plural
		_first_LT_obj _LT_mode
		_LT_value _LT_special _last_obj_was_LT_special;
#Ifnot;
[ PrintContentsFromR p_first_text p_obj p_max_count ! p_style
		_obj _printed_any_objects _last_obj _show_obj _plural;
#Endif;

! Print p_obj and its siblings, but no more than p_max_count objects.
! Return true if anything was printed.
! If any objects are printed, prefix with p_first_text.

	if(p_obj == 0) rfalse;

	if(p_obj == child(parent(p_obj)))
		pc_depth++;

	if(p_first_text ~= 1) {
		! This is a call to print something
		pc_indent = pc_indent + 2;
#Ifdef OPTIONAL_LIST_TOGETHER;
		if(p_obj == child(parent(p_obj)) && p_max_count == 0) {
			_first_LT_obj = _SortForListTogether(parent(p_obj));
			p_obj = child(parent(p_obj));
		}
#Endif;
	}

	if(p_max_count == 0)
		p_max_count = 10000;
	for(_obj = p_obj: _obj ~= 0 && p_max_count-- ~= 0: _obj = sibling(_obj)) {
#Ifdef OPTIONAL_LIST_TOGETHER;
		if(_obj == _first_LT_obj)
			_LT_mode = true;
		if(_LT_mode) {
			_LT_special = false;
			_show_obj = true; ! Has already been checked
! Trust objects to provide list_together in this context?
#Iftrue LIST_TOGETHER_PROP_ID < INDIV_PROP_START;
			_LT_value = _obj.list_together;
#Ifnot;
			_LT_value = _GetIndividualLTValue(_obj);
#Endif;
			if(metaclass(_LT_value) == String or Routine)
				_LT_special = true;
		} else
			_show_obj = _PrintContentsShowObj(_obj);
#Ifnot;
		_show_obj = _PrintContentsShowObj(_obj);
#Endif;

		if(p_first_text == 1) {
			if(_show_obj) {
#Ifdef OPTIONAL_LIST_TOGETHER;
				if(_LT_mode || _plural || _obj has pluralname) {
					pc_depth--;
					return 2;
				}
#Ifnot;
				if(_plural || _obj has pluralname) {
					pc_depth--;
					return 2;
				}
#Endif;
				_plural = 1;
			}
			continue;
		}

		if(_show_obj) {
			if(_last_obj == 0) {
				if(p_first_text ofclass String)
					print (string) p_first_text;
				else if(p_first_text ~= 0)
					p_first_text(parent(p_obj));
				if(c_style & ISARE_BIT)
					print (string) _IsAreString(PrintContentsFromR(1, p_obj));
				if(p_first_text && c_style & NEWLINE_BIT ~= 0) new_line;
			}
			! Push obj onto queue, printing the object that is shifted out, if any
			if(_last_obj) {
				if(_printed_any_objects ~= 0 && c_style & NEWLINE_BIT == 0) print ", ";
#Ifdef OPTIONAL_LIST_TOGETHER;
				if(_last_obj_was_LT_special)
					_PrintContentsPrintLTGroup(_last_obj);
				else
					_PrintContentsPrintAnObj(_last_obj);
#Ifnot;
				_PrintContentsPrintAnObj(_last_obj);
#Endif;
				_printed_any_objects = 1;
			}
			_last_obj = _obj;
#Ifdef OPTIONAL_LIST_TOGETHER;
			_last_obj_was_LT_special = _LT_special;
			if(_last_obj_was_LT_special) {
				_show_obj = lt_value;
				lt_value = _LT_value;
				for(_plural = sibling(_last_obj): _plural ~= 0: _plural = NextEntry(_plural, pc_depth)) _obj = _plural;
				lt_value = _show_obj;
			}
#Endif;
		}
	}
	if(p_first_text == 1) {
		pc_depth--;
		return _plural;
	}

	if(_last_obj) {
		if(_printed_any_objects ~= 0 && c_style & NEWLINE_BIT == 0) print " and ";
#Ifdef OPTIONAL_LIST_TOGETHER;
		if(_last_obj_was_LT_special)
			_PrintContentsPrintLTGroup(_last_obj);
		else
			_PrintContentsPrintAnObj(_last_obj);
#Ifnot;
		_PrintContentsPrintAnObj(_last_obj);
#Endif;
		_printed_any_objects = 1;
	}
	pc_depth--;
	pc_indent = pc_indent - 2;
	return _printed_any_objects;
];

[ _PrintContentsShowObj p_obj;
	! Return true if object should be shown in list, false if not
	if(p_obj ~= parent(player) && ! don't print container when player in it
			(pc_depth > 0 || c_style & WORKFLAG_BIT == 0 || p_obj has workflag) &&
	! Hide concealed and scenery unless taking inventory
			(action == ##Inv || (p_obj hasnt concealed && p_obj hasnt scenery)))
		rtrue;
	rfalse;
];

[ _PrintContentsPrintAnObj p_obj _inv _skip;
	if(pc_skip_next_indent)
		pc_skip_next_indent = false;
	else if(c_style & NEWLINE_BIT)
		FastSpaces(pc_indent);
	if(p_obj.invent ~= 0) {
		_inv = true;
		inventory_stage = 1;
		_skip = PrintOrRun(p_obj, invent, true);
	}
	if(_skip == false) {
		_PrintObjName(p_obj, FORM_INDEF);
		if(_inv) {
			inventory_stage = 2;
			if(PrintOrRun(p_obj, invent, true)) rtrue;
		}
		_PrintAfterEntry(p_obj);
	} else if(c_style & NEWLINE_BIT) 
		new_line;

];

[ FastSpaces p_spaces;
#Ifv3;
	while(p_spaces >= 6) {
		print "      ";
		p_spaces = p_spaces - 6;
	}
	for( : p_spaces > 0 : p_spaces--) @print_char ' ';
#Ifnot;
	while(p_spaces > 10) {
		@print_table TenSpaces 10 1;
		p_spaces = p_spaces - 10;
	}
	@print_table TenSpaces p_spaces 1;
#Endif;
];

[ RunRoutines p_obj p_prop p_switch;
#Ifndef OPTIONAL_NO_DARKNESS;
	if(p_obj == thedark && p_prop ~= initial or short_name or description) p_obj = real_location;
#Endif;
	if(p_switch == 0) sw__var = action; else sw__var = p_switch;
	if (p_prop >= INDIV_PROP_START && p_obj.&p_prop == 0) rfalse;
#Ifdef OPTIONAL_MANUAL_SCOPE;
	return p_obj.p_prop();
#Ifnot;
	p_switch = p_obj.p_prop(); ! Repurposing p_switch
	scope_modified = true;
	return p_switch;
#Endif;
];

[ PrintOrRun p_obj p_prop p_no_string_newline _val;
	_val = p_obj.p_prop;
	if (p_obj.#p_prop > WORDSIZE || _val ofclass Routine) return RunRoutines(p_obj, p_prop);
	if(_val ofclass String) {
		print (string) p_obj.p_prop;
		if(p_no_string_newline == 0) new_line;
	}
];

[ MoveFloatingObjects _i _j _o _len _obj;	
	while((_obj = floating_objects-->_i) ~= 0) {
		if(IndirectlyContains(player, _obj))
			jump _continue_loop;
		_len = _obj.#found_in;
		if(_obj has absent)
			jump _isnt_present;
		else if(_len == 2 && UnsignedCompare(_obj.found_in, top_object) > 0) {
			if(RunRoutines(_obj, found_in))
				jump _is_present;
			jump _isnt_present;
		} else {
			_j = _obj.&found_in;
			_len = _len / 2;
			_len = _len - 1;
._check_next_value;
				_o = _j-->_len;
				if(_o in Class) {
					if(location ofclass _o)
						jump _is_present;
				} else if(_o == location || _o in location)
					jump _is_present;
			@dec_chk _len 0 ?~_check_next_value;
._isnt_present;
			remove _obj;
			jump _continue_loop;
._is_present;
			if(_obj notin location)
				move _obj to location;
		}
._continue_loop;
		_i++;
	}
	! It's not certain that scope has been modified, but PlayerTo relies on it
	! being set.
	scope_modified = true;
!	print "MFO done!^";
];

[ CalculateVisibilityCeiling;
#Ifdef OPTIONAL_NO_DARKNESS;
	return ScopeCeiling(player);
#Ifnot;
	if(location == thedark)
		return thedark;
	else {
		return ScopeCeiling(player);
	}
#Endif;
];

[ PlayerTo p_loc p_flag _old_loc _old_real_loc _old_lookmode _old_parent _vc _old_vc;
!	print "PlayerTo, moving player to ", (the) p_loc, ".^";
	_old_loc = location;
	_old_real_loc = real_location;
	_old_vc = CalculateVisibilityCeiling();
	move Player to p_loc;
	real_location = superparent(p_loc);
	location = real_location;
	MoveFloatingObjects(); ! Also sets scope_modified = true;
#Ifndef OPTIONAL_NO_DARKNESS;
	_UpdateDarkness();
#Endif;
._recheck_visibility_ceil;
	_vc = CalculateVisibilityCeiling();

	if(_vc == location && _vc ~= _old_vc) {
		if(location provides initial) {
			_old_parent = parent(player);
			location.initial();
			if(parent(player) ~= _old_parent)
				jump _recheck_visibility_ceil;
		}
#Ifdef OPTIONAL_NO_DARKNESS;
		RunEntryPointRoutine(NewRoom);
#Ifnot;
		if(location ~= thedark)
			RunEntryPointRoutine(NewRoom);
#Endif;
	}

#Ifndef OPTIONAL_NO_DARKNESS;
	if(_old_real_loc ~= real_location && location == thedark && _old_loc == thedark) {
		! we have moved between dark rooms
		! give entry point a chance to react
		RunEntryPointRoutine(DarkToDark);
	}
#Endif;
	_old_lookmode = lookmode;
	if(p_flag==false)
		lookmode = 2;
	if(p_flag==false or 2 && deadflag == GS_PLAYING)
		<Look>;
	lookmode = _old_lookmode;
];

[ Superparent p_obj _parent;
	while(true) {
		_parent = parent(p_obj);
		if(_parent == 0)
			return p_obj;
		p_obj = _parent;
	}
];

#Ifndef OPTIONAL_NO_DARKNESS;
[ _UpdateDarkness p_look _ceil _old_darkness _darkness;
	if(location == thedark) _old_darkness = true;
	_ceil = ScopeCeiling(player);
	if(_LookForLightInObj(_ceil, _ceil) == false) _darkness = true;
	if(_darkness ~= _old_darkness) scope_modified = true;
	if(_darkness) {
		location = thedark;
	} else {
		location = real_location;
		if(_old_darkness == true && p_look == true)
			<Look>;
	}
];

[ _LookForLightInObj p_obj p_ceiling _o;
	if(p_obj has light) rtrue;
	if(p_obj == p_ceiling || p_obj has transparent || p_obj has supporter || (p_obj has container && p_obj has open))
		objectloop(_o in p_obj)
			if(_LookForLightInObj(_o))
				rtrue;
	rfalse;
];
#Endif;


Include "scope.h";
Include "parser.h";

[ ActionPrimitive; indirect(#actions_table-->action); ];

[ PerformPreparedAction _ret_val;
#IfDef DEBUG;
	if(debug_flag & 2) TraceAction();
#EndIf;
	if ((meta || (BeforeRoutines() == false)) && action < 4096) {
		@push run_after_routines_msg; @push run_after_routines_arg_1;
		run_after_routines_msg = 0;
		ActionPrimitive();
		! If the action has set run_after_routines_msg = true, after routines
		! should be run. If it has set it to another value, this message should
		! be printed unless after routines returns true.
		if(run_after_routines_msg && AfterRoutines() == false &&
				run_after_routines_msg ~= 0 or 1 &&
				keep_silent == false)
			PrintMsg(run_after_routines_msg, run_after_routines_arg_1);
		@pull run_after_routines_arg_1; @pull run_after_routines_msg;
		_ret_val = true; ! could run the command
	}
	return _ret_val;
];

[ _NoteObjectAcquisitions _i;
	objectloop(_i in player) {
		if(_i hasnt moved) {
			give _i moved;
#IfDef OPTIONAL_SCORED;
			if(_i has scored) {
				score = score + OBJECT_SCORE;
#IfDef OPTIONAL_FULL_SCORE;
				things_score = things_score + OBJECT_SCORE;
#EndIf;
			}
#EndIf;
		}
	}
];


[ _RunReact p_property p_break _i _obj _max _any;

	! Loop over the scope to find possible react_before/after routines
	! to run in each object. Return:
	! * false if no routines were found
	! * true if routines were found, p_break was true, and a routine didn't return false
	! * Otherwise:
	!    * if OPTIONAL_MANUAL_SCOPE_BOOST is defined, return 2
	!    * if not, return false

	if(scope_copy_objects == 0) rfalse;
	_max = scope_copy_objects - 1;
._RunReactNext;
	_obj = scope_copy-->_i;
	if (_obj has reactive && _obj.&p_property ~= 0 && ( _obj.#p_property > 2 || _obj.p_property ~= NULL or 0)) {
#Ifdef OPTIONAL_MANUAL_SCOPE_BOOST;
		_any = 2;
#EndIf;
#IfDef DEBUG;
#IfV3;
		if(debug_flag & 1) print "(", (name) _obj, ").",(property) p_property,"()^";
#EndIf;
#EndIf;
		if(RunRoutines(_obj, p_property) && p_break) {
			rtrue;
		}
	}
	@inc_chk _i _max ?~_RunReactNext;
	return _any;
];

[ RunEachTurn;
	! Run all each_turn routines for location and all objects in scope.

	GetScopeCopy(); ! later used by _RunReact

	if(location has reactive && location.&each_turn ~= 0) {
#IfDef DEBUG;
#IfV3;
		if(debug_flag & 1) print "(", (name) location, ").each_turn()^";
#EndIf;
#EndIf;
		RunRoutines(location, each_turn);
	}

#Ifdef OPTIONAL_MANUAL_SCOPE_BOOST;

#Ifdef DEBUG_MANUAL_SCOPE_BOOST;
	if(each_turn_in_scope == false)
		print "SKIPPING EACH_TURN^";
	else
		print "PERFORMING EACH_TURN^";
#EndIf;

	if(each_turn_in_scope == false) rtrue;
	each_turn_in_scope = _RunReact(each_turn);
#Ifnot;
	_RunReact(each_turn);
#Endif;
];

[ BeforeRoutines;

	GetScopeCopy(); ! later used by _RunReact

#IfDef GamePreRoutine;
#IfDef DEBUG;
#IfV3;
	if(debug_flag & 1) print "GamePreRoutine()^";
#EndIf;
#EndIf;
	if(RunEntryPointRoutine(GamePreRoutine)) rtrue;
#EndIf;

#IfDef DEBUG;
#IfV3;
	if(debug_flag & 1) print "player.orders()^";
#EndIf;
#EndIf;
	if(RunRoutines(player, orders)) rtrue;

#Ifdef OPTIONAL_MANUAL_SCOPE_BOOST;

#Ifdef DEBUG_MANUAL_SCOPE_BOOST;
	if(react_before_in_scope == false)
		print "SKIPPING REACT_BEFORE^";
	else
		print "PERFORMING REACT_BEFORE^";
#EndIf;

	if(react_before_in_scope) {
		react_before_in_scope = _RunReact(react_before, true);
		if(react_before_in_scope == true) rtrue;
	}
#Ifnot;
	if(_RunReact(react_before, true) == true) rtrue;
#Endif;

#IfDef DEBUG;
#IfV3;
	if(debug_flag & 1) print "(", (name) real_location, ").before()^";
#EndIf;
#EndIf;
	if(real_location.&before) {
		if(RunRoutines(real_location, before)) rtrue;
	}
	if(inp1 > 1) {
#IfDef DEBUG;
#IfV3;
		if(debug_flag & 1) print "(", (name) inp1, ").before()^";
#EndIf;
#EndIf;
		if(inp1.&before) {
			if(RunRoutines(inp1, before)) rtrue;
		}
	}
	rfalse;
];

[ AfterRoutines;
	! react_after - Loops over the scope to find possible react_before routines
	! to run in each object, if it's found stop the action by returning true

	GetScopeCopy(); ! later used by _RunReact

#Ifdef OPTIONAL_MANUAL_SCOPE_BOOST;

#Ifdef DEBUG_MANUAL_SCOPE_BOOST;
	if(react_after_in_scope == false)
		print "SKIPPING REACT_AFTER^";
	else
		print "PERFORMING REACT_AFTER^";
#EndIf;

	if(react_after_in_scope) {
		react_after_in_scope = _RunReact(react_after, true);
		if(react_after_in_scope == true) rtrue;
	}
#Ifnot;
	if(_RunReact(react_after, true) == true) rtrue;
#Endif;

#IfDef DEBUG;
#IfV3;
	if(debug_flag & 1) print "(", (name) real_location, ").after()^";
#EndIf;
#EndIf;
	if(real_location.&after) {
		if(RunRoutines(real_location, after)) rtrue;
	}
	if(inp1 > 1) {
#IfDef DEBUG;
#IfV3;
		if(debug_flag & 1) print "(", (name) inp1, ").after()^";
#EndIf;
#EndIf;
		if(inp1.&after) {
			if(RunRoutines(inp1, after)) rtrue;
		}
	}
#IfDef GamePostRoutine;
#IfDef DEBUG;
#IfV3;
	if(debug_flag & 1) print "GamePostRoutine()^";
#EndIf;
#EndIf;
	if(RunEntryPointRoutine(GamePostRoutine)) rtrue;
#EndIf;
	rfalse;
];

[ RunLife p_actor p_reason;
#IfDef DEBUG;
#IfV3;
	if(debug_flag & 1 && p_actor provides life) print "(", (name) p_actor, ").life()^";
#EndIf;
#EndIf;
    return RunRoutines(p_actor, life, p_reason);
];

[ _SetDirectionIfIsFakeDir p_obj p_noun_no _idx;
	if(p_obj >= FAKE_N_OBJ && p_obj <= FAKE_OUT_OBJ) {
		_idx = p_obj - FAKE_N_OBJ;
		selected_direction_index = _idx + 1;
		selected_direction = _idx + N_TO_CONST;
		if(p_noun_no == 1) {
			inp1 = Directions;
			noun = inp1;
		}
		else {
			inp2 = Directions;
			second = inp2;
		}
	}
];

#IfDef DEBUG;
[DebugAttribute p_attr;
	print "(attribute ", p_attr, ")";
];

[ DebugParameter p_w;
    print p_w;
    if (p_w >= 1 && p_w <= top_object) print " (", (name) p_w, ")";
    if (UnsignedCompare(p_w, dict_start) >= 0 &&
            UnsignedCompare(p_w, dict_end) < 0 &&
            (p_w - dict_start) % dict_entry_size == 0)
        print " ('", (address) p_w, "')";
];

[ DebugAction p_a _anames;
    if (p_a >= 4096) { print "<fake action ", p_a-4096, ">"; return; }
    _anames = #identifiers_table;
    _anames = _anames + 2*(_anames-->0) + 2*48;
    print (string) _anames-->p_a;
];

[ TraceAction;
	print "[ Action ", (DebugAction) action;
    if (noun ~= 0) {
		print " with noun ";
		if(inp1 == 1) print noun;
		else print (DebugParameter) noun;
		if (second ~= 0) {
			print " and second ";
			if(inp2 == 1) print second;
			else print (DebugParameter) second;
		}
	}
    print "]^";
];

#EndIf;

[ PerformAction p_action p_noun p_second _sa _sn _ss _sdi _sd _sinp1 _sinp2 _result;
	_sa = action; _sn = noun; _ss = second; _sinp1 = inp1; _sinp2 = inp2; _sdi = selected_direction_index; _sd = selected_direction;
	action = p_action; noun = p_noun; second = p_second; inp1 = p_noun; inp2 = p_second;
	selected_direction_index = 0; selected_direction = 0;
	_SetDirectionIfIsFakeDir(noun, 1);
	_SetDirectionIfIsFakeDir(second, 2);
	_result = PerformPreparedAction();
	action = _sa; noun = _sn; second = _ss; selected_direction_index = _sdi; selected_direction = _sd; inp1 = _sinp1; inp2 = _sinp2;
	return _result;
];

[ R_Process p_action p_noun p_second _s1 _s2;
	_s1 = inp1; _s2 = inp2;
	inp1 = p_noun; inp2 = p_second;
	PerformAction(p_action, p_noun, p_second);
	inp1 = _s1; inp2 = _s2;
];

[ CDefArt p_obj _result;
	_result = _PrintObjName(p_obj, FORM_CDEF);
];

[ DefArt p_obj _result;
	_result = _PrintObjName(p_obj, FORM_DEF);
];

[ IndefArt p_obj _result;
	_result = _PrintObjName(p_obj, FORM_INDEF);
];

[ StartTimer p_obj p_timer;
	StartDaemon(p_obj, p_obj, p_timer);
];

#IfDef OPTIONAL_ORDERED_TIMERS;
[ StartDaemon p_obj p_array_val p_timer _i _order _order2 _obj _pos;
#IfNot;
[ StartDaemon p_obj p_array_val p_timer _i;
#EndIf;
	if(p_array_val == 0)
		p_array_val = WORD_HIGHBIT + p_obj;
	for (_i=0 : _i<active_timers : _i++)
		if (the_timers-->_i == p_array_val) rfalse;
	if (active_timers >= MAX_TIMERS) {
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
		_RunTimeError(ERR_TOO_MANY_TIMERS_DAEMONS);
#EndIf;
		return;
	}
	if (p_array_val > 0) {
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
		if (p_obj.&time_left == 0) {
			_RunTimeError(ERR_OBJECT_HASNT_PROPERTY, p_obj); 
			return;
		}
#EndIf;
		p_obj.time_left = p_timer;
	}
	_i = active_timers++;
#IfDef DEBUG;
	if(debug_flag & 4) {
		print "[ Starting ";
		if(p_array_val > 0)
			print "timer ", (DebugParameter) p_obj, " with time_left = ", p_timer;
		else
			print "daemon ", (DebugParameter) p_obj;
		print "]^";
	}
#EndIf;
#IfDef OPTIONAL_ORDERED_TIMERS;
	_order = 100;
	if(p_obj provides timer_order)
		_order = p_obj.timer_order;
	for(_i=_i-1 : _i>=0 : _i--) {
		_obj = the_timers-->_i;
		the_timers-->(_i+1) = _obj;
		_obj = _obj & ~WORD_HIGHBIT;
		_order2 = 100;
		if(_obj provides timer_order)
			_order2 = _obj.timer_order;
		if(_order >= _order2) {
			_pos = _i + 1;
			break;
		}
	}
	the_timers-->_pos = p_array_val;
	if(current_timer >= _pos)
		current_timer++;
#IfNot;
	the_timers-->_i = p_array_val;
#EndIf;
];

[ StopTimer p_obj;
	StopDaemon(p_obj, p_obj);
];

[ StopDaemon p_obj p_array_val _i;
	if(p_array_val == 0)
		p_array_val = WORD_HIGHBIT + p_obj;
	for (_i=0 : _i<active_timers : _i++)
		if (the_timers-->_i == p_array_val) jump _FoundTSlot4;
	rfalse;
._FoundTSlot4;
#IfDef DEBUG;
	if(debug_flag & 4) {
		print "[ Stopping ";
		if(p_array_val > 0)
			print "timer ";
		else
			print "daemon ";
		print (DebugParameter) p_obj, "]^";
	}
#EndIf;
	if (p_array_val > 0) { ! This is a timer, not a daemon
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
		if (p_obj.&time_left == 0) {
			_RunTimeError(ERR_OBJECT_HASNT_PROPERTY, p_obj); 
			return;
		}
#EndIf;
		p_obj.time_left = 0;
	}
	if(_i <= current_timer)
		current_timer--;
	for (_i=_i + 1: _i < active_timers : _i++)
		the_timers-->(_i - 1) = the_timers-->_i;
	active_timers--;
];

[ RunTimersAndDaemons _j _t;
	for (current_timer=0 : current_timer<active_timers : current_timer++) {
		if (deadflag >= GS_DEAD) return;
		_j = the_timers-->current_timer;
#IfDef DEBUG;
		if(debug_flag & 4) {
			print "[ Running ";
			if(_j > 0) {
				print "timer ", (DebugParameter) _j;
				if(_j.time_left > 0)
					print " with time_left = ", _j.time_left;
				else
					print ": time_out";
			} else
				print "daemon ", (DebugParameter) (_j & ~WORD_HIGHBIT);
			print "]^";
		}
#EndIf;

		if (_j < 0) RunRoutines(_j & ~WORD_HIGHBIT, daemon);
		else {
			_t = _j.time_left;
			if (_t == 0) {
				StopTimer(_j);
				RunRoutines(_j, time_out);
			} else {
				_t--;
				_j.time_left = _t;
			}
		}
	}
];


#Ifndef DEBUG;
#Ifndef STRICT_MODE;
#IfTrue RUNTIME_ERRORS == RTE_MINIMUM;

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! Replace some veneer routines with smaller versions
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


!   Print__PName: Print the name of a property.

[ Print__PName; ];

!   WV__Pr:  write a value to the property for the given
!			 object having the given identifier

[ WV__Pr obj identifier value x;
 x = obj..&identifier;
 if (x==0) { 
	!RT__Err(\"write to\", obj, identifier); 
	return; 
 }
 !#ifdef INFIX;
 !if (obj has infix__watching || (debug_flag & 15)) RT__TrPS(obj,identifier,value);
 !#ifnot; 
 !#ifdef DEBUG;
 !if (debug_flag & 15) RT__TrPS(obj,identifier,value);
 !#endif; #endif;
 x-->0 = value;
];

!   RV__Pr:  read a value from the property for the given
!	 		 object having the given identifier

[ RV__Pr obj identifier x;
 x = obj..&identifier;
 if (x==0) {   
	!if (identifier >= 1 && identifier < 64 && obj.#identifier <= 2)
		 @get_prop obj identifier -> sp;
		 @ret_popped;
		 !return obj.identifier;
	 !RT__Err(\"read\", obj, identifier); return; 
 }
 #IFV3;
 !if (obj..#identifier > 2) RT__Err(\"read\", obj, identifier);
 #IFNOT;
 !if (obj..#identifier > 2) RT__Err(\"read\", obj, identifier, 2);
 #ENDIF;
 return x-->0;
];

#IfV5;

!   CA__Pr:  call, that is, print-or-run-or-read, a property:
!			 this exactly implements obj..prop(...).  Note that
!			 classes (members of Class) have 5 built-in properties
!			 inherited from Class: create, recreate, destroy,
!			 remaining and copy.  Implementing these here prevents
!			 the need for a full metaclass inheritance scheme.

[ CA__Pr obj id a b c d e f x y z s s2 n m;
 if (obj < 1 || obj > #largest_object-255)
 {   switch(Z__Region(obj))
	 { 2: if (id == call) { 
			s = sender; 
		    sender = self; 
			self = obj;
			#ifdef action;
			sw__var=action;
			#endif;
			x = indirect(obj, a, b, c, d, e, f);
			self = sender; 
			sender = s; 
			return x; 
		  }
		  jump Call__Error;
	   3: if (id == print) { 
			@print_paddr obj; 
			rtrue; 
		  }
		  if (id == print_to_array) { 
			@output_stream 3 a; 
			@print_paddr obj; 
			@output_stream -3;
			return a-->0; 
		  }
		  jump Call__Error;
	 }
	 jump Call__Error;
 }
 @check_arg_count 3 ?~A__x;y++;@check_arg_count 4 ?~A__x;y++;
 @check_arg_count 5 ?~A__x;y++;@check_arg_count 6 ?~A__x;y++;
 @check_arg_count 7 ?~A__x;y++;@check_arg_count 8 ?~A__x;y++;.A__x;
 !#ifdef INFIX;if (obj has infix__watching) n=1;#endif;
 !#ifdef DEBUG;if (debug_flag & 1 ~= 0) n=1;#endif;
 !if (n==1) {
	!#ifdef DEBUG;n=debug_flag & 1; debug_flag=debug_flag-n;#endif;
	!print \"[ ~\", (name) obj, \"~.\", (property) id, \"(\";
	!switch(y) 
	!{ 1: print a; 
	  !2: print a,\",\",b; 
	  !3: print a,\",\",b,\",\",c;
	  !4: print a,\",\",b,\",\",c,\",\",d;
	  !5: print a,\",\",b,\",\",c,\",\",d,\",\",e;
	  !6: print a,\",\",b,\",\",c,\",\",d,\",\",e,\",\",f; 
	!}
	!print \") ]^\";
   !#ifdef DEBUG;debug_flag = debug_flag + n;#endif;
 !}
 if (id > 0 && id < 64) { 
	!x = obj.&id; 
	@get_prop_addr obj id -> x;
	if (x==0) { 
		x=$000a-->0 + 2*(id-1); 
		n=2; 
	} else 
		!n = obj.#id;
		@get_prop_len x -> n;
 } else { 
	if (id>=64 && id<69 && obj in Class)
		return Cl__Ms(obj,id,y,a,b,c,d);
	x = obj..&id;
	if (x == 0) { 
		.Call__Error;
		!RT__Err(\"send message\", obj, id); 
		return; 
	}
	n = 0->(x-1);
	if (id&$C000==$4000)
		switch (n&$C0) 
			{   0: n=1; 
			  $40: n=2; 
			  $80: n=n&$3F; 
			}
 }
 for (:2*m<n:m++)
 {  if (x-->m==$ffff) rfalse;
	switch(Z__Region(x-->m))
	{ 2: s = sender; 
		 sender = self; 
		 self = obj; 
		 s2 = sw__var;
	     #ifdef LibSerial;
	     if (id==life) sw__var=reason_code; else sw__var=action;
	     #endif;
		 switch(y) 
			{ 0: z = indirect(x-->m); 
			  1: z = indirect(x-->m, a);
			  2: z = indirect(x-->m, a, b); 
			  3: z = indirect(x-->m, a, b, c);
			  4: z = indirect(x-->m, a, b, c, d); 
			  5: z = indirect(x-->m, a, b, c, d, e);
			  6: z = indirect(x-->m, a, b, c, d, e, f); 
			}
		 self = sender; 
		 sender = s; 
		 sw__var = s2;
		 if (z ~= 0) return z;
	  3: print_ret (string) x-->m;
	  default: return x-->m;
	}
 }
 rfalse;
];

[ Cl__Ms obj id y a b c d x;
 switch(id)
 {   create:
		 if (children(obj)<=1) rfalse; 
		 x=child(obj);
		 remove x; 
		 if (x provides create) { 
			if (y==0) x.create();
			if (y==1) x.create(a); 
			if (y==2) x.create(a,b);
			!if (y>3) RT__Err(1,obj); 
			if (y>=3) x.create(a,b,c);
		 }
		 return x;
	 recreate:
		 if (~~(a ofclass obj)) { 
			!RT__Err(\"recreate\", a, -obj); 
			rfalse;
 		 }
		 Copy__Primitive(a, child(obj));
		 if (a provides create) { 
			if (y==1) a.create();
			if (y==2) a.create(b); 
			if (y==3) a.create(b,c);
			!if (y>4) RT__Err(1,obj); 
			if (y>=4) a.create(b,c,d);
		 } 
		 rfalse;
	 destroy:
		 if (~~(a ofclass obj)) { 
			!RT__Err(\"destroy\", a, -obj); 
			rfalse; 
		 }
		 if (a provides destroy) a.destroy();
		 Copy__Primitive(a, child(obj));
		 move a to obj; 
		 rfalse;
	 remaining:
		 return children(obj)-1;
	 copy:
		 if (~~(a ofclass obj)) { 
			!RT__Err(\"copy\", a, -obj); 
			rfalse;
		 }
		 if (~~(b ofclass obj)) { 
			!RT__Err(\"copy\", b, -obj); 
			rfalse; 
		 }
		 Copy__Primitive(a, b); 
		 rfalse;
 }
];

#Endif; ! IfV5

!   IB__Pr:  ++(individual property)

[ IB__Pr obj identifier x;
 x = obj..&identifier;
 if (x==0) { 
	!RT__Err(\"increment\", obj, identifier); 
	return; 
 }
 !#ifdef INFIX;
 !if (obj has infix__watching || (debug_flag & 15)) RT__TrPS(obj,identifier,(x-->0)+1);
 !#ifnot; #ifdef DEBUG;
 !if (debug_flag & 15) RT__TrPS(obj,identifier,(x-->0)+1);
 !#endif; #endif;
 return ++(x-->0);
];

!   IA__Pr:  (individual property)++

[ IA__Pr obj identifier x;
 x = obj..&identifier;
 if (x==0) { 
	!RT__Err(\"increment\", obj, identifier); 
	return; 
 }
 !#ifdef INFIX;
 !if (obj has infix__watching || (debug_flag & 15))
 !RT__TrPS(obj,identifier,(x-->0)+1);
 !#ifnot; #ifdef DEBUG;
 !if (debug_flag & 15) RT__TrPS(obj,identifier,(x-->0)+1);
 !#endif; #endif;
 return (x-->0)++;
];

!   DB__Pr:  --(individual property)

[ DB__Pr obj identifier x;
 x = obj..&identifier;
 if (x==0) { 
	!RT__Err(\"decrement\", obj, identifier); 
	return; 
 }
 !#ifdef INFIX;
 !if (obj has infix__watching || (debug_flag & 15)) RT__TrPS(obj,identifier,(x-->0)-1);
 !#ifnot; #ifdef DEBUG;
 !if (debug_flag & 15) RT__TrPS(obj,identifier,(x-->0)-1);
 !#endif; #endif;
 return --(x-->0);
];

!   DA__Pr:  (individual property)--

[ DA__Pr obj identifier x;
 x = obj..&identifier;
 if (x==0) { 
	!RT__Err(\"decrement\", obj, identifier); 
	return; 
 }
 !#ifdef INFIX;
 !if (obj has infix__watching || (debug_flag & 15)) RT__TrPS(obj,identifier,(x-->0)-1);
 !#ifnot; #ifdef DEBUG;
 !if (debug_flag & 15) RT__TrPS(obj,identifier,(x-->0)-1);
 !#endif; #endif;
 return (x-->0)--;
];

!   RA__Pr:  read the address of a property value for a given object,
!			 returning 0 if it doesn't provide this individual
!			 property

[ RA__Pr obj identifier i otherid cla;
 if (obj==0) rfalse;
 if (identifier<64 && identifier>0) {
	 !return obj.&identifier;
	 @get_prop_addr obj identifier -> sp;
	 @ret_popped;
 }
 if (identifier & $8000 ~= 0)
 {   cla = #classes_table-->(identifier & $ff);
	 if (cla.&3 == 0) rfalse;
	 if (~~(obj ofclass cla)) rfalse;
	 identifier = (identifier & $7f00) / $100;
	 i = cla.3;
	 while (identifier>0)
	 {   identifier--;
		 i = i + i->2 + 3;
	 }
	 return i+3;
 }
 if (identifier & $4000 ~= 0)
 {   cla = #classes_table-->(identifier & $ff);
	 identifier = (identifier & $3f00) / $100;
	 if (~~(obj ofclass cla)) rfalse; i=0-->5;
	 if (cla == 2) return i+2*identifier-2;
	 i = 0-->((i+124+cla*14)/2);
	 i = CP__Tab(i + 2*(0->i) + 1, -1)+6;
	 return CP__Tab(i, identifier);
 }
 if (obj.&3 == 0) rfalse;
 if (obj in 1)
 {  	 if (identifier<64 || identifier>=72) rfalse;
 }
 if (self == obj)
	 otherid = identifier | $8000;
 i = obj.3;
 while (i-->0 ~= 0)
 {    if (i-->0 == identifier or otherid)
		return i+3;
	 i = i + i->2 + 3;
 }
 rfalse;
];

!   RL__Pr:  read the property length of an individual property value,
!		 	 returning 0 if it isn't provided by the given object

[ RL__Pr obj identifier x;
 if (identifier<64 && identifier>0) {
	!return obj.#identifier;
	@get_prop_addr obj identifier -> x;
	@get_prop_len x -> sp;
	@ret_popped;
 }
 x = obj..&identifier;
 if (x==0) rfalse;
 if (identifier&$C000==$4000)
	 switch (((x-1)->0)&$C0)
	 {    0: return 1;  
		$40: return 2;  
		$80: return ((x-1)->0)&$3F; 
	 }
 return (x-1)->0;
];

!   RA__Sc:  implement the "superclass" (::) operator,
!			 returning an identifier

[ RA__Sc cla identifier otherid i j k;
 if (cla notin 1 && cla > 4)
 {   
	!RT__Err(\"be a '::' superclass\", cla, -1); 
	rfalse; 
 }
 if (self ofclass cla) otherid = identifier | $8000;
 for (j=0: #classes_table-->j ~= 0: j++)
 {   if (cla==#classes_table-->j)
	 {   if (identifier < 64) return $4000 + identifier*$100 + j;
		 if (cla.&3 == 0) break;
		 i = cla.3;
		 while (i-->0 ~= 0)
		 {   if (i-->0 == identifier or otherid)
				 return $8000 + k*$100 + j;
			 i = i + i->2 + 3;
			 k++;
		 }
		 break;
	 }
 }
 !RT__Err(\"make use of\", cla, identifier);
 rfalse;
 ];

!   OP__Pr:  test whether or not given object provides individual
!			 property with the given identifier code

[ OP__Pr obj identifier x;
 if (obj<1 || obj > (#largest_object-255))
 {   if (identifier ~= print or print_to_array or call) rfalse;
	 switch(Z__Region(obj))
	 {   2: if (identifier == call) rtrue;
		 3: if (identifier == print or print_to_array) rtrue;
	 }
	 rfalse;
 }
 if (identifier<64)
 {   @get_prop_addr obj identifier -> x;
	 if (x ~= 0) rtrue;
	 !if (obj.&identifier ~= 0) rtrue;
	 rfalse;
 }
 if (obj..&identifier ~= 0) rtrue;
 if (identifier<72 && obj in 1) rtrue;
 rfalse;
];

!   OC__Cl:  test whether or not given object is of the given class

[ OC__Cl obj cla j a n;
 if (obj<1 || obj > (#largest_object-255))
 {   if (cla ~= 3 or 4) rfalse;
	 if (Z__Region(obj) == cla-1) rtrue;
	 rfalse;
 }
 if (cla == 1) {
	 if (obj<=4) rtrue;
	 if (obj in 1) rtrue;
	 rfalse;
 } else if (cla == 2) {
	 if (obj<=4) rfalse;
	 if (obj in 1) rfalse;
	 rtrue;
 } else if (cla == 3 or 4) {
	 rfalse;
 }
 if (cla notin 1) { 
	!RT__Err(\"apply 'ofclass' for\", cla, -1);
	rfalse;
 }
 @get_prop_addr obj 2 -> a;
 if (a==0) rfalse;
 @get_prop_len a -> n;
 for (j=0: j<n/2: j++) {   
	if (a-->j == cla) rtrue;
 }
 rfalse;
];


#Endif;
#Endif;
#Endif;


#IfV3;
! These routines are implemented by Veneer, but the default implementations give compile errors for z3

! [ Print__PName prop p size cla i;
	 ! if (prop & $c000)
	 ! {   cla = #classes_table-->(prop & $ff);
		 ! print (name) cla, "::";
		 ! if ((prop & $8000) == 0) prop = (prop & $3f00)/$100;
		 ! else
		 ! {   prop = (prop & $7f00)/$100;
			 ! i = cla.3;
			 ! while ((i-->0 ~= 0) && (prop>0))
			 ! {   i = i + i->2 + 3;
				 ! prop--;
			 ! }
			 ! prop = (i-->0) & $7fff;
		 ! }
	 ! }
	 ! p = #identifiers_table;
	 ! size = p-->0;
	 ! if (prop<=0 || prop>=size || p-->prop==0)
		 ! print "<number ", prop, ">";
	 ! else print (string) p-->prop;
 ! ];

! [ FindIndivPropValue p_obj p_property _x _prop_id;
	! _x = p_obj.3;
	! if (_x == 0) rfalse;
	! !  print "Table for ", (object) obj, " is at ", (hex) x, "^";
	! while (true) {
		! _prop_id = _x-->0;
		! if(_prop_id == 0) break;
		! if(_prop_id & 32767 == p_property) break;
		! ! print (hex) x-->0, " (", (property) x-->0, ")  length ", x->2, ": ";
		! !       for (n = 0: n< (x->2)/2: n++)
		! !           print (hex) (x+3)-->n, " ";
		! !       @new_line;
		! _x = _x + _x->2 + 3;
	! }
	! return _x;
! ];

! [ RV__Pr p_object p_property _value _address;
	! if(p_object == 0) {
		! print "*Error: Tried to read property ";
		! Print__PName(p_property);
		! " of nothing.*";
	! }
	! if(p_object provides p_property) {
		! if(p_property >= INDIV_PROP_START) {
			! _address = FindIndivPropValue(p_object, p_property);
			! return (_address + 3)-->0;
		! }
		! @get_prop p_object p_property -> _value;
		! return _value;
	! }
	! print "[Error: Tried to read undefined property ";
	! Print__PName(p_property);
	! print " of ";
	! PrintShortName(p_object);
	! print "]";
	! rfalse;
! ];

!      CA__Pr:  call, that is, print-or-run-or-read, a property:
!                      this exactly implements obj..prop(...).  Note that
!                      classes (members of Class) have 5 built-in properties
!                      inherited from Class: create, recreate, destroy,
!                      remaining and copy.  Implementing these here prevents
!                      the need for a full metaclass inheritance scheme.      */

[ CA__Pr obj id a    x z s s2 n m;
! print "CA_Pr obj = ", obj,", id = ", id,", a = ", a, "^";
	if (obj < 1 || obj > #largest_object-255) {
		switch(Z__Region(obj)) {
			2:
			if (id == call) {
				s = sender; sender = self; self = obj;
				#ifdef action;sw__var=action;#endif;
				x = indirect(obj, a);
				self = sender; sender = s; return x;
			}
			jump Call__Error;
			3:
			if (id == print) { @print_paddr obj; rtrue; }
			if (id == print_to_array) {
				@output_stream 3 a; @print_paddr obj; @output_stream -3;
				return a-->0;
			}
			jump Call__Error;
		}
		jump Call__Error;
	}
! print "CA_Pr(2) obj = ", obj,", id = ", id,", a = ", a, "^";
!   @check_arg_count 3 ?~A__x;y++;@check_arg_count 4 ?~A__x;y++;
!   @check_arg_count 5 ?~A__x;y++;@check_arg_count 6 ?~A__x;y++;
!   @check_arg_count 7 ?~A__x;y++;@check_arg_count 8 ?~A__x;y++;.A__x;
!   #ifdef INFIX;if (obj has infix__watching) n=1;#endif;
	#ifdef DEBUG;if (debug_flag & 1 ~= 0) n=1;#endif;
!   if (n==1) {
!     n=debug_flag & 1; debug_flag=debug_flag-n;
!     print "[ ~", (name) obj, "~.", (property) id, "(";
!     switch(y) {
!     1:
!       print a; 2: print a,",",b; 3: print a,",",b,",",c;
!     4:
!       print a,",",b,",",c,",",d;
!     5:
!       print a,",",b,",",c,",",d,",",e;
!     6:
!       print a,",",b,",",c,",",d,",",e,",",f;
!     }
!     print ") ]^"; debug_flag = debug_flag + n;
!   }
	if (id > 0 && id < INDIV_PROP_START) {
!   print "CA_Pr OK obj = ", obj,", id = ", id,", a = ", a, "^";
		x = obj.&id;
		if (x==0) {
			x=$000a-->0 + 2*(id-1); n=2;
		} else n = obj.#id;
	} else {
		if (id>=64 && id<69 && obj in Class) {
!     print "CA_Pr ERROR0 obj = ", obj,", id = ", id,", a = ", a, "^";
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
			RT__Err("Class create etc", obj, id); 
#Endif;
			return;
!     return Cl__Ms(obj,id,y,a,b,c);
		}
!   print "CA_Pr(2.1) obj = ", obj,", id = ", id,", n = ", n, "^";
		x = obj..&id;
!   print "CA_Pr(2.2) obj = ", obj,", id = ", id,", x = ", x, "^";
		if (x == 0) {
!     print "CA_Pr ERROR1 obj = ", obj,", id = ", id,", a = ", a, "^";
			.Call__Error;
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
			RT__Err("send message", obj, id); 
#Endif;
			return;
		}
!   print "Reading n at ", x-1,": ", 0->(x-1), "^";
		n = 0->(x-1);
		if (id&$C000==$4000)
		switch (n&$C0) { 0: n=1; $40: n=2; $80: n=n&$3F; }
	}
! print "CA_Pr(3) obj = ", obj,", id = ", id,", a = ", a, "^";
	for (:2*m<n:m++) {
!   print "Considering routine at ", x+2*m,": ", x-->m, "^";
		if (x-->m==$ffff) rfalse;
		switch(Z__Region(x-->m)) {
		2:
			s = sender; sender = self; self = obj; s2 = sw__var;
!       switch(y) {
!       0:
!         z = indirect(x-->m);
!       1:
			z = indirect(x-->m, a);
!       2:
!         z = indirect(x-->m, a, b);
!       3:
!       z = indirect(x-->m, a, b, c);
!       4:
!         z = indirect(x-->m, a, b, c, d);
!       5:
!         z = indirect(x-->m, a, b, c, d, e);
!       6:
!         z = indirect(x-->m, a, b, c, d, e, f);
!       }
			self = sender; sender = s; sw__var = s2;
			if (z ~= 0) return z;
		3:
			print_ret (string) x-->m;
		default:
			return x-->m;
		}
	}
	rfalse;
];

[ Cl__Ms;
	rfalse;
];
#EndIf;

#Ifndef CUSTOM_PLAYER_OBJECT;
Object selfobj "you"
	with
		name 'me' 'myself' 'self',
		short_name  "yourself",
		description "As good-looking as ever.",
		before NULL,
		after NULL,
		life NULL,
		each_turn NULL,
		time_out NULL,
		describe NULL,
		add_to_scope 0,
		capacity MAX_CARRIED,
		parse_name 0,
		orders 0,
		number 0,
	has concealed animate proper transparent;
#Endif;

#Ifndef OPTIONAL_NO_DARKNESS;
Object thedark "Darkness"
	with
		initial 0,
		description "It is pitch dark here!",
 		short_name 0;
#Endif;

[ _UpdateScoreOrTime;
#Ifdef STATUSLINE_TIME;
	status_field_1 = the_time/60;
	status_field_2 = the_time%60;
#Ifnot;
	#Ifdef STATUSLINE_SCORE;
	#Ifndef NO_SCORE;
		status_field_1 = score;
	#Endif;
		status_field_2 = turns;
	#Ifnot;
		if (sys_statusline_flag == 0) {
			#Ifndef NO_SCORE;
				status_field_1 = score;
			#Endif;
			status_field_2 = turns;
		} else {
			status_field_1 = the_time/60;
			status_field_2 = the_time%60;
		}
	#Endif;
#Endif;
];

[ EndTurnSequence;
	turns++;
#Ifndef STATUSLINE_SCORE;
#Ifndef STATUSLINE_TIME;
	if (sys_statusline_flag) {
#Endif;
		if (time_rate >= 0) the_time=the_time+time_rate;
		else {
			time_step--;
			if (time_step == 0) {
				the_time++;
				time_step = -time_rate;
			}
		}
		the_time = the_time % 1440;
#Ifndef STATUSLINE_TIME;
	}
#Endif;
#Endif;
	RunTimersAndDaemons(); if(deadflag >= GS_DEAD) rtrue;
	RunEachTurn(); if(deadflag >= GS_DEAD) rtrue;
	RunEntryPointRoutine(TimePasses);
#Ifndef OPTIONAL_NO_DARKNESS;
	_UpdateDarkness(true);
#Endif;

	if(update_moved || child(player) ~= last_player_child or 0) {
		_NoteObjectAcquisitions();
		update_moved = false;
		last_player_child = child(player);
	}

];

#Ifdef OPTIONAL_PROVIDE_UNDO_FINAL;
[ PerformUndo _i;
	if (turns == 0) {
		PrintMsg(MSG_UNDO_NOTHING_DONE);
		return 0;
	}
	if (undo_flag == 0) {
		PrintMsg(MSG_UNDO_NOT_PROVIDED);
		return 0;
	}
	if (undo_flag == 1) {
		PrintMsg(MSG_UNDO_FAILED);
		return 0;
	}
	@restore_undo _i;
	if (_i == 0) {
		PrintMsg(MSG_UNDO_FAILED);
		return 0;
	}
	return 1;
];
#Endif;

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! Rarely used routines
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#IfTrue RUNTIME_ERRORS < RTE_VERBOSE;
[RT__Err err_no par1 par2;
	print "Inform error: ";
	if(err_no ofclass String)
		print (string) err_no, " - ";
	else
		print_ret err_no;
	print_ret " (", par1, ", ", par2, ")";
];
#EndIf;

[ DirPropToFakeObj p_dir_prop;
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(p_dir_prop < N_TO_CONST || p_dir_prop > OUT_TO_CONST)
		_RunTimeError(ERR_NOT_DIR_PROP);
#EndIf;
	return p_dir_prop - N_TO_CONST + FAKE_N_OBJ;
];

[ FakeObjToDirProp p_fake_obj;
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(p_fake_obj < FAKE_N_OBJ || p_fake_obj > FAKE_OUT_OBJ)
		_RunTimeError(ERR_NOT_FAKE_OBJ);
#EndIf;
	return p_fake_obj - FAKE_N_OBJ + N_TO_CONST;
];

[ _InitObjects _i _k _stop;
	_stop = top_object + 1;
	for(_i = Directions : _i < _stop : _i++) {
#Ifndef OPTIONAL_MANUAL_REACTIVE;
#Ifdef OPTIONAL_REACTIVE_PARSE_NAME;
		if(_i.&react_before ~= 0 || _i.&react_after ~= 0 || _i.&each_turn ~= 0 ||
				_i.&add_to_scope ~= 0 || _i.&parse_name ~= 0)
			give _i reactive;
#Ifnot;
		if(_i.&react_before ~= 0 || _i.&react_after ~= 0 || _i.&each_turn ~= 0 ||
				_i.&add_to_scope ~= 0)
			give _i reactive;
#Endif;
#Endif;
		if(_i.&found_in) {
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
			if(_k >= MAX_FLOATING_OBJECTS) {
				_RunTimeError(ERR_TOO_MANY_FLOATING);
				rtrue;
			}
#EndIf;
			floating_objects-->(_k++) = _i;
		}
	}
];

[ SetTime p_time p_step;
	the_time = p_time;
	time_rate = p_step;
	time_step = 0;
	if(p_step < 0) time_step = -p_step;
];

#Ifdef NO_SCORE;
[ main _i _j _copylength _sentencelength _parsearraylength _again_saved _parser_oops _disallow_complex_again;
#Ifnot;
[ main _i _j _copylength _sentencelength _parsearraylength _score _again_saved _parser_oops _disallow_complex_again;
#Endif;
#IfV5;
	screen_width = HDR_SCREENWCHARS->0;
#EndIf;
	dict_start = HDR_DICTIONARY-->0;
	dict_entry_size = dict_start->(dict_start->0 + 1);
	dict_start = dict_start + dict_start->0 + 4;
	dict_end = dict_start + (dict_start - 2)-->0 * dict_entry_size;

	parse->0 = MAX_INPUT_WORDS;
#IfV5;
    buffer->0 = MAX_INPUT_CHARS;
#IfNot;
    buffer->0 = MAX_INPUT_CHARS - 1;
#EndIf;

	top_object = #largest_object-255;
	sys_statusline_flag = ( ($1->0) & 2 ) / 2;

#Ifdef CUSTOM_PLAYER_OBJECT;
	player = CUSTOM_PLAYER_OBJECT;
#Ifnot;
	player = selfobj;
#Endif;
	deadflag = GS_PLAYING;
	turns = -1;
#Ifndef NO_SCORE;
	score = 0;
#Endif;
#IfDef OPTIONAL_FULL_SCORE;
#IfDef OPTIONAL_SCORED;
	places_score = 0;
	things_score = 0;
#EndIf;
#IfDef TASKS_PROVIDED;
	for(_i = 0 : _i < NUMBER_TASKS : _i++) task_done->_i = 0;
#EndIf;
#EndIf;

#IfV5;
	new_line; ! So the first line of text isn't covered by the statusline
#Endif;
	_j = Initialise();

	objectloop (_i in player) give _i moved ~concealed;
	update_moved = false;

	if(_j ~= 2) {
		Banner();
		@new_line;
	}

	_InitObjects(); ! after initialise since location set there
	if(parent(player) == 0) { _i = location; location = 0; PlayerTo(_i); }

	@new_line;
	turns++; ! Change turns from -1 to 0, signaling that game has now started
	while(deadflag == GS_PLAYING) {
#Ifdef DEBUG_TIMER;
	timer1 = 0-->2;
#Endif;
#IfV5;
		screen_width = HDR_SCREENWCHARS->0;
#EndIf;

		_UpdateScoreOrTime();
		if(_sentencelength > 0) @new_line;

		_UpdateScope(player);
#Ifndef NO_SCORE;
		_score = score;
#Endif;
#Ifdef DEBUG_TIMER;
	timer1 = 0-->2 - timer1;
	print "[Before ReadPlayerInput: ",timer1," jf]^";
#Endif;
		if(parse->1 == 0) {
			_ReadPlayerInput();
			_disallow_complex_again = false;
#Ifdef OPTIONAL_PROVIDE_UNDO_FINAL;
			if(parse-->1 == 'undo') {
				PerformUndo();
				@new_line;
				jump _abort_current_input;
			}
			@save_undo _i;
			undo_flag = 2;
			if(_i == -1) undo_flag = 0;
			if(_i == 0) undo_flag = 1;
			if(_i == 2) {
				! undo has just been issued
				PrintMsg(MSG_UNDO_DONE);
				@new_line;
				jump _abort_current_input;
			}
#Endif;
		}
		_parser_oops = parser_unknown_noun_found;
._try_same_command_again;
#Ifdef DEBUG_TIMER;
		timer1 = 0-->2;
#Endif;
		_sentencelength = _ParseAndPerformAction();
#Ifdef DEBUG_TIMER;
		timer1 = 0-->2 - timer1;
		print "[ParseAndPerformAction: ",timer1," jf]^";
		timer1 = 0-->2;
#Endif;
		if(action == ##OopsCorrection) {
			if(_again_saved && _parser_oops > 0) {
				_CopyInputArray(buffer2, buffer);
				_CopyParseArray(parse2, parse);
				num_words = parse -> 1;
				_parser_oops-->0 = special_word;
				_again_saved = false;
				jump _try_same_command_again;
			} else {
				PrintMsg(MSG_PARSER_CANT_OOPS);
			}
		}
		if(action == ##Again) {
			! restore from the 'again' buffers and reparse
			if(_disallow_complex_again) {
				PrintMsg(MSG_PARSER_COMPLEX_AGAIN);
		        parse->1 = 0;
		        continue;
			} else if(_again_saved) {
				_CopyInputArray(buffer2, buffer);
				_CopyParseArray(parse2, parse);
				num_words = parse -> 1;
				jump _try_same_command_again;
			} else {
				PrintMsg(MSG_PARSER_NOTHING_TO_AGAIN);
			}
		} else if(parse->1 == 0) {
			_again_saved = 0;
		} else {
			! store the current buffer to 'again'
			_again_saved = true;
			_CopyInputArray(buffer, buffer2);
			_CopyParseArray(parse, parse2);
		}

		if(_sentencelength <= 0) {
			_sentencelength = -_sentencelength;
		} else {
			! _ParseAndPerformAction found a problem and
			! printed an error message. We set _sentencelength
			! to the end of the line to force new user input
			_sentencelength = parse->1;
		}
		if(action >= 0 && meta == false && _sentencelength>0) {
			EndTurnSequence();
        }

        if(deadflag ~= GS_PLAYING && deadflag ~= GS_WIN) {
        	! we died somehow, use entry routine to give
        	! a chance of resurrection
        	RunEntryPointRoutine(AfterLife);
		}

#Ifndef NO_SCORE;
		if(_score ~= score && notify_mode == true) {
			PrintMsg(MSG_PARSER_NEW_SCORE, _score);
		}
#Endif;

		_parsearraylength = parse->1;
		if(_parsearraylength > _sentencelength) {
			! the first sentence in the input  has been parsed
			! and executed. Now remove it from parse so that
			! the next sentence can be parsed
			_copylength = 2 * _parsearraylength + 1;
			for(_i = 1, _j = 2 * _sentencelength + 1: _j < _copylength: _i++, _j++)
				parse-->_i = parse-->_j;

			parse->1 = _parsearraylength - _sentencelength;
			_disallow_complex_again = true; ! cannot parse "x me.g.g.g"
		} else {
			! the input was just one sentence
			parse->1 = 0;
		}
#Ifdef DEBUG_TIMER;
	timer1 = 0-->2 - timer1;
	print "[After ParseAndPerformAction: ",timer1," jf]^";
#Endif;
		continue;
._abort_current_input;
		! skip all processing and force new input
		parse->1 = 0;
		_sentencelength = 0;
	}
	_UpdateScoreOrTime();
	@new_line;
	if(deadflag == GS_QUIT) @quit;
#ifV5;
	style bold;
#Endif;
	print "^  *** ";
	if(deadflag == GS_WIN) PrintMsg(MSG_YOU_HAVE_WON);
	else if(deadflag == GS_DEAD) PrintMsg(MSG_YOU_HAVE_DIED);
	else if(deadflag >= GS_DEATHMESSAGE) DeathMessage();
	print " ***^^";
#ifV5;
	style roman;
#Endif;
#Ifndef NO_SCORE;
	ScoreSub();
#Endif;
	for(::) {
		PrintMsg(MSG_RESTART_RESTORE_OR_QUIT);
		_ReadPlayerInput(true);
		verb_word = parse-->1;
#Ifdef OPTIONAL_PROVIDE_UNDO_FINAL;
		if(verb_word == 'undo') {
			PerformUndo();
		}
#Endif;
		if(verb_word == 'restart') @restart;
		if(verb_word == 'restore') RestoreSub();
		if(AMUSING_PROVIDED == 0 && deadflag == 2 && verb_word == 'amusing') Amusing();
		if(verb_word == 'quit') @quit;
#IfDef OPTIONAL_FULL_SCORE;
		if(verb_word == 'full') FullScoreSub();
#EndIf;
	}
];

! provide entry point routines if the user hasn't already:
! Routines marked NO are not supported in Puny, usually
! because the implementations differ too much.
!
#Stub AfterLife       0;
#Stub AfterPrompt     0;
#Stub Amusing         0;
#Stub BeforeParsing   0;
#Stub DeathMessage    0;
#Stub GamePostRoutine 0;
#Stub GamePreRoutine  0;
#Stub InScope         1;
#Stub LookRoutine     0;
#Stub NewRoom         0;
#Stub ParseNumber     2;
#Stub PrintTaskName   1;
#Stub PrintVerb       1;
#Stub TimePasses      0;
#Stub UnknownVerb     1;
!NO #Stub ChooseObjects   2;
!NO #Stub ParserError     1;
#Ifndef OPTIONAL_NO_DARKNESS;
#Stub DarkToDark      0;
#Endif;
