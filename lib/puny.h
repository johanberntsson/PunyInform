! PunyInform: A small stdlib and parser for interactive fiction
! suitable for old-school computers such as the Commodore 64.
! Designed to be similar, but not identical, to the Inform 6 library.
!
! Reference documentation
! DM: http://www.inform-fiction.org/manual/html/dm4index.html
! Tech: https://www.inform-fiction.org/source/tm/TechMan.txt
! Z-machine: https://www.inform-fiction.org/zmachine/standards/z1point1
!
! Public routines (described in DM, available for a game developer)
! LIBRARY ROUTINES:
! - CommonAncestor
! - DrawStatusLine
! - IndirectlyContains
! - InScope
! - LoopOverScope
! - NextWord
! - NextWordStopped
! - NumberWord
! - NumberWords
! - ObjectIsUntouchable
! - PlayerTo
! - ParseToken
! - PlaceInScope
! - PronounNotice
! - ScopeWithin
! - SetTime
! - TestScope
! - TryNumber
! - WordAddress
! - WordLength
! - WordValue
! - YesOrNo
! OTHERS:
! - PrintOrRun
! - RunRoutines
! PRINT UTILITIES:
! - CTheyreorThats
! - ItorThem
! - IsOrAre
! PUNYINFORM ONLY:
! - OnOff
! - PrintMsg
! - RunTimeError

! comment/uncomment to restrict default debug messages behaviour
! (all can be overridden by adding them in the game source)
!Constant DEBUG_SCOPE;
!Constant DEBUG_CHECKNOUN;
!Constant DEBUG_GETNEXTNOUN;
!Constant DEBUG_PARSETOKEN;
!Constant DEBUG_PARSEPATTERN;
!Constant DEBUG_PARSEANDPERFORM;

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
	while (p_o2 ~= 0) {
		if (p_o1 == p_o2) rtrue;
		!        if (p_o2 ofclass Class) rfalse;
		p_o2 = parent(p_o2);
	}
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

#IfV5;

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
		if (clr_on && clr_bgstatus > 1) {
			@set_colour clr_fgstatus clr_bgstatus;
		} else {
			style reverse;
		}
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
		if (clr_on && clr_bgstatus > 1) {
			@set_colour clr_fg clr_bg;
		} else {
			style roman;
		}
		@set_window 0;
	}
	statuswin_current = false;
];


Array TenSpaces -> "          ";

[ _PrintSpacesOrMoveBack p_col p_string _current_col;
	p_col = (HDR_SCREENWCHARS->0) - p_col;

	@get_cursor cursor_pos;
	_current_col = cursor_pos --> 1;

	if(_current_col > p_col || cursor_pos --> 0 > 1) {
		_MoveCursor(1, p_col);
	} else {
		p_col = p_col - _current_col;
		while(p_col > 10) {
			@print_table TenSpaces 10;
			p_col = p_col - 10;
		}
		@print_table TenSpaces p_col;
	}
	if(p_string)
		print (string) p_string;
];

Constant ONE_SPACE_STRING = " ";

[ _PrintStatusLineTime p_width _h _pm;
	if (p_width > 29) {
		if (p_width > 39) {
			if (p_width > 66) {
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

[ _PrintStatusLineScore p_width _pos;
	_pos = p_width; ! Just to get rid of warnings
#Ifdef OPTIONAL_SL_NO_SCORE;
#Ifndef OPTIONAL_SL_NO_MOVES;
	! Show moves only
	if (p_width > 25) {
		if (p_width < 30) {
			! Width is 25-29, only print moves as "0"
			_PrintSpacesOrMoveBack(4, ONE_SPACE_STRING);
		} else {
			! Width is 30-, print "Moves: 0"
			_pos = 11;
			if (p_width > 52) {
				! Width is 53+, leave some space to the right
				_pos = 15;
			}
			_PrintSpacesOrMoveBack(_pos, MOVES__TX);
		}
		print status_field_2;
	}
#Endif; ! Ifndef NO_MOVES
#Ifnot;
	! Show score and maybe moves
	if (p_width > 24) {
		if (p_width < 30) {
			! Width is 25-29, only print score as "0", no moves
			_PrintSpacesOrMoveBack(3, ONE_SPACE_STRING);
			print status_field_1;
		} else {
#Ifdef OPTIONAL_SL_NO_MOVES;
	! Show score only
			! Width is 30-, print "Score: 0"
			_pos = 13;
			if(p_width < 55) _pos = 10;
			_PrintSpacesOrMoveBack(_pos, SCORE__TX);
			print status_field_1;
#Ifnot;
	! Show score + moves
			if (p_width > 66) {
				! Width is 67-, print "Score: 0 Moves: 0"
				_PrintSpacesOrMoveBack(28, SCORE__TX);
				print status_field_1;
				_PrintSpacesOrMoveBack(14, MOVES__TX);
			} else {
				if (p_width > 36) {
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

[ DrawStatusLine _width _visibility_ceiling;
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

	_width = HDR_SCREENWCHARS->0;

	_StatusLineHeight(statusline_height);
	_MoveCursor(1, 1); ! This also sets the upper window as active.
	if(_width > 66) @print_char ' ';

!     _MoveCursor(1, 2);
!     if (location == thedark) {
!         print (name) location;
!     }
!     else {
!         FindVisibilityLevels();
!         if (visibility_ceiling == location)
	_visibility_ceiling = ScopeCeiling(player);
! print (object) _visibility_ceiling;
#Ifdef OPTIONAL_NO_DARKNESS;
	if (_visibility_ceiling == location)
		_PrintObjName(location); ! If it's light, location == real_location
else
	print (The) _visibility_ceiling;
#Ifnot;
	if (location == thedark || _visibility_ceiling == location)
		_PrintObjName(location); ! If it's light, location == real_location
else
	print (The) _visibility_ceiling;
#Endif;

#Ifdef STATUSLINE_TIME;
	_PrintStatusLineTime(_width);
#Ifnot;
	#Ifdef STATUSLINE_SCORE;
		_PrintStatusLineScore(_width);
	#Ifnot;
		if (sys_statusline_flag) {
			_PrintStatusLineTime(_width);
		} else {
			_PrintStatusLineScore(_width);
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
#EndIf;
#EndIf;

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
		! print " "; ! not ok, adds space in room name
	}
	PrintShortName(p_obj);
];

[ _PrintAfterEntry p_obj;
	if(p_obj has container && P_obj hasnt open) print " (which is closed)";
	if(p_obj has container && (p_obj has open || p_obj has transparent)) {
		if(child(p_obj) == nothing)
			print " (which is empty)";
		else
			if(PrintContents(" (which contains ", p_obj)) print ")";
	}
	if(p_obj has supporter && child(p_obj) ~= nothing) {
		if(PrintContents(" (on which is ", p_obj)) print ")";
	}
!	if(p_obj has light && action == ##Inv) print " (providing light)";
#Ifndef OPTIONAL_NO_DARKNESS;
	if(p_obj has light && p_obj hasnt animate) print " (providing light)";
#Endif;
	if(p_obj has worn && action == ##Inv) print " (worn)";
];

[ PrintContents p_first_text p_obj p_check_workflag _obj _printed_first_text _printed_any_objects _last_obj _show_obj;
!   print "Objectlooping...^";
	objectloop(_obj in p_obj) {
!print "Considering ", (object) _obj, "...^";
!if(_obj has concealed) print "Is concealed."; else print "Isnt concealed.";
		_show_obj =
			_obj ~= parent(player) && ! don't print container when player in it
			(p_check_workflag == false || _obj has workflag);
		if(action ~= ##Inv) {
			! don't show concealed or scenery in the normal case (look etc.),
			! but allow it when listing inventory.
			if(_obj has concealed or scenery) _show_obj = false;
		}

		if(_show_obj) {
!			(_obj.&describe == 0 || _obj notin parent(player)) &&
!			(_obj has moved || _obj.initial == 0 || _obj notin parent(player))) {
			if(_printed_first_text == 0) {
				if(p_first_text ofclass String)
					print (string) p_first_text;
				else if(p_first_text ~= 0)
					p_first_text(p_obj);
				_printed_first_text = 1;
			}
			! Push obj onto queue, printing the object that is shifted out, if any
			if(_last_obj) {
				if(_printed_any_objects) print ", ";
				_PrintContentsPrintAnObj(_last_obj);
				_printed_any_objects = 1;
			}
			_last_obj = _obj;
		}
	}
	if(_last_obj) {
		if(_printed_any_objects) print " and ";
		_PrintContentsPrintAnObj(_last_obj);
	}

	return _printed_first_text;
];

[ _PrintContentsPrintAnObj p_obj _inv _skip;
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
	}
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
		if(p_no_string_newline == 0) @new_line;
	}
];

[ MoveFloatingObjects _i _j _len _obj _present;
	while((_obj = floating_objects-->_i) ~= 0) {
		_len = _obj.#found_in;
		if(_obj has absent)
			_present = 0;
		else if(_len == 2 && UnsignedCompare(_obj.found_in, top_object) > 0) {
			_present = RunRoutines(_obj, found_in);
		} else {
			_present = 0;
			_j = _obj.&found_in;
#IfV5;
			@log_shift _len (-1) -> _len;
			@scan_table real_location _j _len -> _present ?~no_success; ! The position is only a throw-away value here.
			_present = 1;
.no_success;
#IfNot;
			_len = _len / 2;
			_len = _len - 1;
.next_value;
				if(_j-->_len == real_location) {
					_present = 1;
					jump after_loop;
				}
			@dec_chk _len 0 ?~next_value;
.after_loop;
#EndIf;

		}
		if(_present)
			move _obj to real_location;
		else
			remove _obj;
		_i++;
	}
	! It's not certain that scope has been modified, but PlayerTo relies on it
	! being set.
	scope_modified = true;
];

[ PlayerTo p_loc p_flag _old_loc _old_real_loc _old_lookmode _old_parent _vc _old_vc;
!	print "PlayerTo, moving player to ", (the) p_loc, ".^";
	_old_loc = location;
	_old_real_loc = real_location;
	move Player to p_loc;
	real_location = superparent(p_loc);
	location = real_location;
	MoveFloatingObjects(); ! Also sets scope_modified = true;
#Ifndef OPTIONAL_NO_DARKNESS;
	_UpdateDarkness();
#Endif;
.recheck_vc;
	_old_vc = visibility_ceiling;
#Ifdef OPTIONAL_NO_DARKNESS;
	_vc = ScopeCeiling(player);
#Ifnot;
	if(location == thedark)
		_vc = thedark;
	else {
		_vc = ScopeCeiling(player);
	}
#Endif;
	if(_vc == location)
		visibility_ceiling = _vc;

	if(visibility_ceiling == location && visibility_ceiling ~= _old_vc) {
		if(location provides initial) {
			_old_parent = parent(player);
			location.initial();
			if(parent(player) ~= _old_parent)
				jump recheck_vc;
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
!	print "_LookForLightInObj, Examining: ", (the) p_obj, "^";
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

[ RunEachTurn _i _obj _scope_count _max;
	! Run all each_turn routines for location and all objects in scope.
#IfDef DEBUG;
#IfV3;
	if(debug_flag & 1 && location has reactive && location.&each_turn ~= 0) print "(", (name) location, ").each_turn()^";
#EndIf;
#EndIf;
	_scope_count = GetScopeCopy();
	RunRoutines(location, each_turn);

	if(_scope_count) {
		_max = _scope_count - 1;
.next_entry;
		if(deadflag >= GS_DEAD) rtrue;
		_obj = scope_copy-->_i;
		if(_obj has reactive && _obj.&each_turn ~= 0) {
#IfDef DEBUG;
#IfV3;
			if(debug_flag & 1) print "(", (name) _obj, ").each_turn()^";
#EndIf;
#EndIf;
			RunRoutines(_obj, each_turn);
		}
		@inc_chk _i _max ?~next_entry;
	}
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

[ BeforeRoutines _i _obj _scope_count _max;
	! react_before - Loops over the scope to find possible react_before routines
	! to run in each object, if it's found stop the action by returning true
	_scope_count = GetScopeCopy();
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

	if(_scope_count) {
		_max = _scope_count - 1;
.next_entry;
		_obj = scope_copy-->_i;
		if (_obj has reactive && _obj.&react_before ~= 0) {
#IfDef DEBUG;
#IfV3;
			if(debug_flag & 1) print "(", (name) _obj, ").react_before()^";
#EndIf;
#EndIf;
			if(RunRoutines(_obj, react_before)) {
				rtrue;
			}
		}
		@inc_chk _i _max ?~next_entry;
	}
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

[ AfterRoutines _i _obj _scope_count _max;
	! react_after - Loops over the scope to find possible react_before routines
	! to run in each object, if it's found stop the action by returning true
	_scope_count = GetScopeCopy();

	if(_scope_count) {
		_max = _scope_count - 1;
.next_entry;
		_obj = scope_copy-->_i;
		if (_obj has reactive && _obj.&react_after ~= 0) {
#IfDef DEBUG;
#IfV3;
			if(debug_flag & 1) print "(", (name) _obj, ").react_after()^";
#EndIf;
#EndIf;
			if(RunRoutines(_obj, react_after)) rtrue;
		}
		@inc_chk _i _max ?~next_entry;
	}
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
	_i = active_timers++;
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
	if (_i >= MAX_TIMERS) RunTimeError(ERR_TOO_MANY_TIMERS_DAEMONS);
#EndIf;
	if (p_timer > 0) {
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
		if (p_obj.&time_left == 0) {
			RunTimeError(ERR_OBJECT_HASNT_PROPERTY); return;
		}
#EndIf;
		p_obj.time_left = p_timer;
	}
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
		the_timers-->(_i+1) = the_timers-->_i;
		_obj = the_timers-->_i & ~WORD_HIGHBIT;
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
		if (the_timers-->_i == p_array_val) jump FoundTSlot4;
	rfalse;
.FoundTSlot4;
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
	if (p_obj == p_array_val) { ! This is a timer, not a daemon
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
		if (p_obj.&time_left == 0) {
			RunTimeError(ERR_OBJECT_HASNT_PROPERTY); return;
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
			RT__Err("Class create etc", obj, id); return;
!     return Cl__Ms(obj,id,y,a,b,c);
		}
!   print "CA_Pr(2.1) obj = ", obj,", id = ", id,", n = ", n, "^";
		x = obj..&id;
!   print "CA_Pr(2.2) obj = ", obj,", id = ", id,", x = ", x, "^";
		if (x == 0) {
!     print "CA_Pr ERROR1 obj = ", obj,", id = ", id,", a = ", a, "^";
			.Call__Error;
			RT__Err("send message", obj, id); return;
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
	if(update_moved) {
		_NoteObjectAcquisitions();
		update_moved = false;
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
		RunTimeError(ERR_NOT_DIR_PROP);
#EndIf;
	return p_dir_prop - N_TO_CONST + FAKE_N_OBJ;
];

[ FakeObjToDirProp p_fake_obj;
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(p_fake_obj < FAKE_N_OBJ || p_fake_obj > FAKE_OUT_OBJ)
		RunTimeError(ERR_NOT_FAKE_OBJ);
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
				RunTimeError(ERR_TOO_MANY_FLOATING);
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

		_UpdateScoreOrTime();
		if(_sentencelength > 0) @new_line;

		_UpdateScope(player);
#Ifndef NO_SCORE;
		_score = score;
#Endif;
#Ifdef DEBUG_TIMER;
	timer1 = 0-->2 - timer1;
	print "[Before ReadPlayerInput took ",timer1," jiffies]^";
#Endif;
		if(parse->1 == 0) {
			_ReadPlayerInput();
			_disallow_complex_again = false;
#Ifdef OPTIONAL_PROVIDE_UNDO_FINAL;
			if(parse-->1 == 'undo') {
				PerformUndo();
				@new_line;
				jump abort_input;
			}
			@save_undo _i;
			undo_flag = 2;
			if(_i == -1) undo_flag = 0;
			if(_i == 0) undo_flag = 1;
			if(_i == 2) {
				! undo has just been issued
				PrintMsg(MSG_UNDO_DONE);
				@new_line;
				jump abort_input;
			}
#Endif;
		}
		_parser_oops = parser_unknown_noun_found;
.do_it_again;
#Ifdef DEBUG_TIMER;
	timer1 = 0-->2;
#Endif;
		_sentencelength = _ParseAndPerformAction();
#Ifdef DEBUG_TIMER;
	timer1 = 0-->2 - timer1;
	print "[ParseAndPerformAction took ",timer1," jiffies]^";
	timer1 = 0-->2;
#Endif;
		if(action == ##OopsCorrection) {
			if(_again_saved && _parser_oops > 0) {
				_CopyInputArray(buffer2, buffer);
				_CopyParseArray(parse2, parse);
				num_words = parse -> 1;
				_parser_oops-->0 = special_word;
				_again_saved = false;
				jump do_it_again;
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
				jump do_it_again;
			} else {
				PrintMsg(MSG_PARSER_NOTHING_TO_AGAIN);
			}
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
		if(action >= 0 && meta == false) {
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
	print "[After ParseAndPerformAction took ",timer1," jiffies]^";
#Endif;
		continue;
.abort_input;
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
