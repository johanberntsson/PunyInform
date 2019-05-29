#include "english.h";

! Always use grammar version 2 which is easier to parse and more economical
! See: section 8.6 in https://www.inform-fiction.org/source/tm/TechMan.txt
Constant Grammar__Version = 2;
Constant INDIV_PROP_START 64;
!Constant WORDSIZE 2;

Attribute light;
Attribute supporter;
Attribute container;
Attribute enterable;
Attribute transparent;
Attribute open;
Attribute openable;
Attribute concealed;
Attribute moved;
Attribute visited alias moved;
Attribute proper;

! Property name; ! This seems to be hardcoded in the Inform compiler
Property initial;
Property description;
Property short_name;
Property add_to_scope;
Property react_before;
Property before;

! directions
Property n_to;
Property s_to;
Property e_to;
Property w_to;
Property cant_go;
Array abbr_directions_array table 'n//' 's//' 'e//' 'w//';
Array full_directions_array table 'north' 'south' 'east' 'west';
Array direction_properties_array table n_to s_to e_to w_to;

! Header constants
Constant HEADER_DICTIONARY   = 4;    ! 2*4 = $8
Constant HEADER_STATIC_MEM   = 7;    ! 2*7 = $c

Constant TT_NOUN             = 1;    ! one or more words referring to an object
Constant TT_PREPOSITION      = 2;    ! e.g. 'into'
Constant TT_ROUTINE_FILTER   = 3;    ! e.g. noun=CagedCreature
Constant TT_ATTR_FILTER      = 4;    ! e.g. edible
Constant TT_SCOPE            = 5;    ! e.g. scope=Spells
Constant TT_PARSE_ROUTIME    = 6;    ! a parse routine
Constant TT_END              = 15;   ! End of grammar line

! $42 = Single prep, $62 = Beginning of list of alternatives, $72 = middle of list, $52 = end of list
Constant TOKEN_SINGLE_PREP   = $42;
Constant TOKEN_FIRST_PREP    = $62;
Constant TOKEN_MIDDLE_PREP   = $72;
Constant TOKEN_LAST_PREP     = $52;

Constant GS_PLAYING          = 1;
Constant GS_QUIT             = 2;
Constant GS_DEAD             = 3;
Constant GS_WIN              = 4;
Constant GS_DEATHMESSAGE     = 5;

Constant FORM_CDEF           = 1;
Constant FORM_DEF            = 2;
Constant FORM_INDEF          = 3;

#IfV3;
Constant DICT_BYTES_FOR_WORD = 4;
#IfNot;
Constant DICT_BYTES_FOR_WORD = 6;
Constant HDR_SCREENHCHARS    = $20;
Constant HDR_SCREENWCHARS    = $21;
Constant MOVES__TX = "Moves: ";
Constant SCORE__TX = "Score: ";
#EndIf;


Global location = 1; ! Must be first global
Global status_field_1 = 0; ! Must be second global. Is used to show score or hours
Global status_field_2 = 0; ! Must be third global. Is used to show turns or minutes
Global score;
Global turns;
Global player;
Global action;
Global noun;
Global second;
Global inp1;
Global inp2;
Global reverse;
Global game_state;
Global wn;
Global input_words;
Global scope_objects;
Global statusline_current_height = 0;
Global statusline_height     = 1;
Global statuswin_current     = false;
Global debug_flag            = 0;
Global clr_on                = false;
Global clr_bg                = 2;
Global clr_fg                = 8;
Global clr_bgstatus          = 2;
Global clr_fgstatus          = 8;

Constant MAX_INPUT_CHARS     = 78;
Constant MAX_INPUT_WORDS     = 20;

Array player_input_array->(MAX_INPUT_CHARS + 3);
Array parse_array->(2 + 4 * (MAX_INPUT_WORDS + 1)); ! + 1 to make room for an extra word which is set to 0

! ######################### Include utility files

#IfDef FLAG_COUNT;
Include "flags.h";
#EndIf;



Include "scope.h";



! ######################### Grammar + Actions
[ LookSub _obj _ceil;

	_ceil = GetVisibilityCeiling(player);

!	print "Ceiling is object ", _ceil, ": ", (object) _ceil, ".^";
	! ### Print room name
	if(_ceil ~= location) {
		print (The) _ceil;
	} else {
		PrintObjName(_ceil);
	}
	@new_line;

	! ### Print room description
	if(_ceil.description) {
		PrintOrRun(_ceil, description, 1);
	}

	PrintContents(" You can also see ", " here.^", _ceil);

	objectloop(_obj in _ceil) {
		if(_obj hasnt moved && _obj.initial ~= 0) {
			@new_line;
			PrintOrRun(_obj, initial);
		}
	}

];

[ ExamineSub;
	if(noun provides description) {
!		print "Has desc...";
		PrintOrRun(noun, description);
	} else {
		"There is nothing special about ", (the) noun, ".";
	}
];

! could handle locatisation with constants, but need (string)
! Constant MSG_AREYOUSUREQUIT = "Are you sure you want to quit? ";

[ QuitSub;
    ! print (string) MSG_AREYOUSUREQUIT;
    print "Are you sure you want to quit? ";
    if(YesOrNo()) {
		game_state = GS_QUIT;
	}
];

[ TakeSub;
	if(noun in player) "You already have that.";
	if(IndirectlyContains(noun, player)) "First, you'd have to leave ", (the) noun, ".";
	move noun to player;
	give noun moved;
	score = score + 10;
	"Taken.";
];

[ DropSub;
	if(noun notin player) "You aren't holding that.";
	move noun to location;
	"Dropped.";
];

[ OpenSub;
	if(noun hasnt openable) "You can't open that.";
	if(noun has open) "It is already open.";
	give noun open;
	"You open ", (the) noun, ".";
];

[ CloseSub;
	if(noun hasnt openable) "You can't close that.";
	if(noun hasnt open) "It isn't open.";
	give noun ~open;
	"You close ", (the) noun, ".";
];

[ EnterSub;
	if(noun hasnt enterable) "You can't enter that.";
	if(player in noun) "But you are already there!";
	if(noun has container && noun hasnt open) "You can't, since it's closed.";
	PlayerTo(noun);
	"You enter ", (the) noun, ".";
];

[ ExitSub;
	if(player in location) "But you aren't in anything at the moment!";
	if(player notin noun) {
		if(IndirectlyContains(noun, player)) "First you have to leave ", (the) parent(player),".";
		if(noun has supporter) "You aren't on that.";
		"You aren't in that.";
	}
	if(noun has container && noun hasnt open) "You can't, since it's closed.";
	PlayerTo(parent(noun));
	"You leave ", (the) noun, ".";
];

[ InventorySub;
	if(child(player) == 0)
		"You are empty handed.";
	PrintContents("You are holding ", ".^", player);
];

[ GoSub _dir _i;
    ! called "go <dir>", so direction is the second word
    _dir = parse_array-->3;
	for(_i = 1 : _i <= abbr_directions_array-->0 : _i++) {
		if(_dir == abbr_directions_array --> _i or full_directions_array --> _i) {
			GoDir(direction_properties_array --> _i);
			rtrue;
		}
	}
    ! this should never happen, but just in case
    "You can't go that way!";
];

[ GoDir p_property _new_location;
	if(location provides p_property) {
		_new_location = location.p_property; ! doesn't work in z3
!		@get_prop location p_property -> _new_location; ! works in z3 and z5
	}
	if(_new_location == 0) {
		if(location provides cant_go) {
			print_ret (string) location.cant_go;
		}
		"You can't go that way.";
	}
	location = _new_location;
	PlayerTo(location);
	<Look>; ! Equivalent to PerformAction(##Look);
];

[ SaveSub _result;
#IfV3;
	@save ?save_success;
	"Save failed.";
.save_success;
	"Ok.";
#IfNot;
	@save -> _result;
	if(_result == 0) "Save failed.";
	"Ok."; ! _result = 1: save ok, 2: Restore ok
#EndIf;
];

[ RestoreSub _flag;
#IfV3;
	@restore ?restore_success; ! can't use @restore because of compiler test
	"Restore failed.";
.restore_success; ! This is never reached, since a successful restore continues after save opcode.
#IfNot;
	@restore -> _flag;
	! must have failed here so no need to check the flag
	"Restore failed";
#EndIf;
];

[ YesOrNo _i;
	for (::)
	{
#IfV3;
		@sread player_input_array parse_array;
#IfNot;
		DrawStatusLine();
		@aread player_input_array parse_array -> _i;
#EndIf;
		_i = parse_array --> 1;
		if (_i == 'yes' or #n$y) rtrue;
		if (_i == 'no' or #n$n) rfalse;
		print "Please answer yes or no.^> ";
	}
];

[ RestartSub;
	print "Are you sure you want to restart? ";
	if(YesOrNo()) {
		@restart;
		"Failed.";
	}
];

Verb 'i' 'inventory'
	* -> Inventory;

Verb 'look' 'l'
	* -> Look
	* 'at' noun -> Examine;

Verb 'open'
	* noun -> Open;

Verb 'close'
	* noun -> Close;

Verb 'take' 'get'
	* noun -> Take;

Verb 'drop'
	* noun -> Drop;

Verb 'enter'
	* noun -> Enter;

Verb 'climb'
	* 'into'/'onto' noun 		-> Enter
	* 'out' 'of'/'from' noun 	-> Exit;

Verb 'jump'
	* 'into'/'onto' noun 		-> Enter
	* 'out' 'of'/'from' noun 	-> Exit
	* 'off' noun 				-> Exit;

Verb 'exit' 'leave'
	* noun -> Exit;

Verb 'go'
	* 'north'/'south' -> Go
	* 'n//'/'s//' -> Go
	* noun -> Enter;

Verb 'examine' 'x//'
	* noun -> Examine;

Verb meta 'quit'
	* -> Quit;

Verb meta 'save'
	*      -> Save;

Verb meta 'restore'
	*      -> Restore;

Verb meta 'restart'
	*      -> Restart;

! ######################### Helper routines

[ GetVisibilityCeiling p_actor _parent;
	for(:: p_actor = _parent) {
		_parent = parent(p_actor);
!		print "Examining ", p_actor, "(", (object) p_actor, ") whose parent is ", _parent, "(", (object) _parent, ")...^";
		if(_parent == 0 || (p_actor has container && p_actor hasnt transparent or open)) {
			return p_actor;
		}
	}
];

[ IndirectlyContains p_o1 p_o2;
    ! Does o1 indirectly contain o2?  (Same as testing if o1 is one of the ancestors of o2.)
    while (p_o2 ~= 0) {
        if (p_o1 == p_o2) rtrue;
!        if (p_o2 ofclass Class) rfalse;
        p_o2 = parent(p_o2);
    }
    rfalse;
];


#IfV5;

Array cursor_pos --> 2;

[ StatusLineHeight p_height;
	if (statusline_current_height ~= p_height) {
		@split_window p_height;
		statusline_current_height = p_height;
	}
];


[ MoveCursor line column;  ! 1-based postion on text grid
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

[ MainWindow;
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

#IfNot; !IfV5

! [ MoveCursor line column;  ! 1-based postion on text grid
! 	if (~~statuswin_current) {
! 		@set_window 1;
! 		style reverse;
! 	}
! 	if (line == 0) {
! 		line = 1;
! 		column = 1;
! 	}
! 	@set_cursor line column;
! 	statuswin_current = true;
! ];

! [ MainWindow;
! 	if (statuswin_current) {
! 		style roman;
! 		@set_window 0;
! 	}
! 	statuswin_current = false;
! ];
#EndIf;


#IfV3;
[ DrawStatusLine;
	@show_status;
];
#IfNot;

[ PrintSpacesOrMoveBack p_col p_space_before _col;
	@get_cursor cursor_pos;
	_col = cursor_pos --> 1;
	if(_col > p_col || (_col == p_col && p_space_before ~= 0) || cursor_pos --> 0 > 1) {
		MoveCursor(1, p_col - 1);
		print (char) ' ';
		rtrue;
	}
	p_col = p_col - _col;
    while(p_col >= 5) {
	    print "     ";
	    p_col = p_col - 5;
    }

	@jl p_col 1 ?rtrue;
.one_more;
	@print_char ' ';
	@dec_chk (p_col) 1 ?~one_more;
];

[ DrawStatusLine _width _visibility_ceiling;

    ! If there is no player location, we shouldn't try to draw status window
    if (location == nothing || parent(player) == nothing)
        return;

    StatusLineHeight(statusline_height);
    MoveCursor(1, 1);
    print " ";

    _width = HDR_SCREENWCHARS->0;

!     _i = width;
!     while(_i>=10) {
! 	    print "          ";
! 	    _i = _i - 10;
!     }
!     spaces _i;

!     MoveCursor(1, 2);
!     if (location == thedark) {
!         print (name) location;
!     }
!     else {
!         FindVisibilityLevels();
!         if (visibility_ceiling == location)
	_visibility_ceiling = GetVisibilityCeiling(player);
!	print (object) _visibility_ceiling;
	if (_visibility_ceiling == location) {
		print (name) _visibility_ceiling;
	} else {
		print (The) _visibility_ceiling;
	}

!     if (sys_statusline_flag && width > 53) {
!         MoveCursor(1, posa);
!         print (string) TIME__TX;
!         LanguageTimeOfDay(sline1, sline2);
!     }
!     else {
	if (_width > 24) {
		if (_width < 29) {
			! Width is 25-28, only print score as "0", no moves
			PrintSpacesOrMoveBack(_width - 2, 1);
			print status_field_1;
		} else {
			if (_width > 66) {
				! Width is 67-, print "Score: 0 Moves: 0"
				PrintSpacesOrMoveBack(_width - 26, 1);
				print (string) SCORE__TX, status_field_1;
				PrintSpacesOrMoveBack(_width - 13);
				print (string) MOVES__TX;
			} else {
				if (_width > 35) {
					! Width is 36-66, print "Score: 0/0"
					PrintSpacesOrMoveBack(_width - 13, 1);
					print (string) SCORE__TX;
				} else {
					! Width is 29-35, print "0/0"
					PrintSpacesOrMoveBack(_width - 6, 1);
				}
				print status_field_1, "/";
			}
			print status_field_2;
		}
	}
	! Regardless of what kind of status line we have printed, print spaces to the end.
	PrintSpacesOrMoveBack(_width + 1);
    MainWindow(); ! set_window
];
#Endif;



[ PrintObjName p_obj p_form _done;
	if(p_obj hasnt proper) {
		if(p_form == FORM_CDEF) {
			print "The ";
		} else if(p_form == FORM_DEF) {
			print "the ";
		} else if(p_form == FORM_INDEF) {
			print "a ";
		}
	}
	if(p_obj.short_name) {
		_done = PrintOrRun(p_obj, short_name);
	}
	if(_done == 0) {
		print (object) p_obj;
	}
];

[ PrintContents p_first_text p_last_text p_obj _obj _printed_first_text _printed_any_objects _last_obj;
! 	print "Objectlooping...^";
	objectloop(_obj in p_obj) {
!		print "Considering ", (object) _obj, "...^";
!		if(_obj has concealed) print "Is concealed."; else print "Isnt concealed.";
		if(_obj hasnt concealed && (_obj has moved || _obj.initial == 0)) {
			if(_printed_first_text == 0) {
				print (string) p_first_text;
				_printed_first_text = 1;
			}
			! Push obj onto queue, printing the object that is shifted out, if any
			if(_last_obj) {
				if(_printed_any_objects) print ", ";
				PrintObjName(_last_obj, FORM_INDEF);
				_printed_any_objects = 1;
			}
			_last_obj = _obj;
		}
	}
	if(_last_obj) {
		if(_printed_any_objects) print " and ";
		PrintObjName(_last_obj, FORM_INDEF);
		print (string) p_last_text;
	}

!
! 	for(_i = 0: _i < scope_objects: _i++) {
! 		_obj = scope-->_i;
! 		if(_obj ~= nothing) {
! 			@new_line;
! 			_text = _obj.initial;
! 			if(_text) {
! 				PrintOrRun(_text);
! 			} else {
! 				print "There is a ",(name) scope-->_i, " here. ";
! 			}
! 			@new_line;
! 		}
! 	}
];

[ RunRoutines p_obj p_prop;
    if (p_obj.&p_prop == 0 && p_prop >= INDIV_PROP_START) rfalse;
    return p_obj.p_prop();
];


[ PrintOrRun p_obj p_prop p_no_string_newline;
    if (p_obj.#p_prop > WORDSIZE) return RunRoutines(p_obj,p_prop);
	if(p_obj.p_prop ofclass String) {
		print (string) p_obj.p_prop;
		if(p_no_string_newline == 0) @new_line;
		rtrue;
	}
	else if(p_obj.p_prop ofclass Routine) {
		return RunRoutines(p_obj, p_prop);
	}
];

[ PlayerTo p_loc _p;
	move Player to p_loc;
	for(location = p_loc: (_p = parent(location)): location = _p);
!	print (object) location;
];

! ######################### Parser

[ ReadPlayerInput _result;
! #IfV5;
! 	print "Width: ", HDR_SCREENWCHARS->0,"^";
! #EndIf;

	print "^>";
	parse_array->0 = MAX_INPUT_WORDS;
#IfV5;
	DrawStatusLine();
	player_input_array->0 = MAX_INPUT_CHARS;
	player_input_array->1 = 0;
	@aread player_input_array parse_array -> _result;
#IfNot;
	player_input_array->0 = MAX_INPUT_CHARS - 1;
	if(player in location) {
		@sread player_input_array parse_array;
	} else {
		_result = location; location = GetVisibilityCeiling(player);
		@sread player_input_array parse_array;
		location = _result;
	}
#EndIf;
	input_words = parse_array -> 1;
	! Set word after last word in parse array to all zeroes, so it won't match any words.
	_result = 2 * input_words + 1;
	parse_array-->_result = 0;
	parse_array-->(_result + 1) = 0;
];


#IfDef DEBUG;
[ CheckPattern p_pattern _i _action_number _token_top _token_next _token_bottom;
    ! action number is the first two bytes
	_action_number = p_pattern-->0;
	p_pattern = p_pattern + 2;
	action = _action_number & $3ff;
	reverse = (_action_number & $400 ~= 0);
	print "Action#: ", action, " Reverse: ", reverse, "^";

	for(_i = 0: : _i++) {
		if(p_pattern->0 == TT_END) break;
		_token_top = (p_pattern->0 & $c0)/64; ! top (2 bits)
		_token_next = (p_pattern->0 & $30)/16;  ! next (2 bits)
		_token_bottom = p_pattern->0 & $0f; ! bottom (4 bits)
		print "Token#: ", _i, " Type: ", p_pattern->0, " (top ", _token_top, ", next ",_token_next, ", bottom ",_token_bottom, ") Next byte: ",(p_pattern + 1)-->0,"^";
		p_pattern = p_pattern + 3;
	}
!	print ": ", i, " tokens^";
	return p_pattern + 1; ! skip TT_END
];
#EndIf;

[ CheckNoun p_parse_pointer _i _j _n _p _obj _matches _last_match _current_word _name_array _name_array_len _best_score _result;
	! return -1 if no noun matches
	! return -2 if more than one match found
	! else return object number
	for(_i = 0: _i < scope_objects: _i++) {
		_n = wn;
		_p = p_parse_pointer;
		_current_word = p_parse_pointer-->0;
		_obj = scope-->_i;
!		if(_obj == nothing) continue;
		if(_obj.#name > 1) {
			_name_array = _obj.&name;
			_name_array_len = _obj.#name / 2;
			while(_n < input_words) {
#IfV5;
				@scan_table _current_word _name_array _name_array_len -> _result ?success;
#IfNot;
				for(_j = 0: _j < _name_array_len: _j++) {
					if(_name_array-->_j == _current_word) jump success;
				}
#EndIf;
				jump not_matched;
.success;
				_n++;
				_p = _p + 4;
				if(_n == _best_score) _matches++;
				if(_n > _best_score) {
					_last_match = _obj;
					_best_score = _n;
					_matches = 1;
				}
			}
		}
.not_matched;

!		print "checking ", _obj.&name-->0, " ", _current_word, "^";
	}
	if(_matches == 1) {
		wn = _best_score;
!		parse_pointer = p;
		return _last_match;
	}
	if(_matches > 1) return -2;
	return -1;
];

[ ParseAndPerformAction _verb _word_data _verb_num _verb_grammar _num_patterns _i _pattern _pattern_index _token _token_type _data _parse_pointer _noun_tokens _noun;

	action = -1;

	UpdateScope(GetVisibilityCeiling(player));

	if(parse_array->1 < 1) {
		"Come again?";
	}

	! check if it is a direction
	for(_i = 1 : _i <= abbr_directions_array-->0 : _i++) {
		_verb = parse_array-->1;
		if(_verb == abbr_directions_array --> _i or full_directions_array --> _i) {
			!print "Found direction ",(address) abbr_directions_array --> _i, "^";
			action = ##Go;
			GoDir(direction_properties_array --> _i);
			rtrue;
		}
	}


	! not a direction, try a verb command
	_verb = parse_array-->1;
	if(_verb < (0-->HEADER_DICTIONARY)) {
		"That is not a verb I recognize.";
	}

	_word_data = _verb + DICT_BYTES_FOR_WORD;
	if((_word_data->0) & 1 == 0) { ! This word does not have the verb flag set.
		"That is not a verb I recognize.";
	}

	! Now we know that the first word is a verb

! 	print "Parse array: ", parse_array, "^";
! 	print "Word count: ", parse_array->0, "^";
! 	print "Word 1: ", (parse_array + 2)-->0, "^";
! 	print "Word 2: ", (parse_array + 6)-->0, "^";
! 	print "Word 3: ", (parse_array + 10)-->0, "^";
	_verb_num = 255 - (_word_data->1);
	_verb_grammar = (0-->HEADER_STATIC_MEM)-->_verb_num;
	_num_patterns = _verb_grammar->0;

#IfDef DEBUG;
	print "Verb#: ",_verb_num,".^";
	print "Grammar address for this verb: ",_verb_grammar,"^";
	print "Number of patterns: ",_num_patterns,"^";

	! First print all patterns, for debug purposes
	_pattern = _verb_grammar + 1;
	for(_i = 0 : _i < _num_patterns : _i++) {
		print "############ Pattern ",_i,"^";
		_pattern = CheckPattern(_pattern);
	}
	@new_line;
#EndIf;

	_pattern = _verb_grammar + 1;
	for(_i = 0 : _i < _num_patterns : _i++) {
#IfDef DEBUG;
		print "############ Pattern ",_i," address ", _pattern, "^";
#EndIf;
		wn = 1;
		_parse_pointer = parse_array + 6;
		_pattern_index = _pattern - 1;
		_noun_tokens = 0;
		noun = 0;
		second = 0;
		inp1 = 0;
		inp2 = 0;

		while(true) {
			_pattern_index = _pattern_index + 3;
			_token = _pattern_index -> 0;
#IfDef DEBUG;
			print "TOKEN: ", _token, "^";
#EndIf;
			_data = (_pattern_index + 1) --> 0;
			if(_token == TT_END) {
				if(wn == parse_array -> 1) {
					jump parse_success;
				}
				break; ! Fail because the grammar line ends here but not the input
			}
			if(wn >= parse_array -> 1) { !Fail because input ends here but not the grammar line
#IfDef DEBUG;
				print "Fail, since grammar line has not ended but player input has.^";
#EndIf;
				break;
			}
			_token_type = _token & $0f;
			if(_token_type == TT_PREPOSITION) { ! $42 = Single prep, $62 = Beginning of list of alternatives, $72 = middle of list, $52 = end of list
#IfDef DEBUG;
				print "Preposition: ", _data, "^";
#EndIf;
				if(_parse_pointer --> 0 == _data) {
#IfDef DEBUG;
					print "Match!^";
#EndIf;
					wn++;
					_parse_pointer = _parse_pointer + 4;
					while(_token == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) { ! Alternative prepositions which are not at the end of the list
#IfDef DEBUG;
						print "Skipping one alternative...^";
#EndIf;
						_pattern_index = _pattern_index + 3;
						_token = _pattern_index -> 0;
					}
					continue;
				}
#IfDef DEBUG;
				print "Failed prep: ", _parse_pointer, ":", _parse_pointer --> 0, " should have been ", _data, "^";
#EndIf;
				if(_token == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) continue; ! First in a list or in the middle of a list of alternative prepositions, so keep parsing!
				break; ! Fail because this is the only or the last alternative preposition and the word in player input doesn't match it
			} else if(_token_type == TT_NOUN) {
				! we expect a noun here
				! check all objects in 'scope', and see if any match.
				! If so, update wn and parse_pointer, and return success
				_noun = CheckNoun(_parse_pointer);
				if(_noun == -2) {
					print "Which ", (address) _parse_pointer --> 0, "? ";
					"You have to be more specific.";
					break;
				}
				if(_noun > 0) {
#IfDef DEBUG;
					print "Noun match!^";
#EndIf;
					if(_noun_tokens == 0) {
						noun = _noun;
						inp1 = _noun;
					} else if(_noun_tokens == 1){
						second = _noun;
						inp2 = _noun;
					}
					_noun_tokens++;
					continue;
				}
#IfDef DEBUG;
				print "Not a matching noun: ", _parse_pointer, ":", _parse_pointer --> 0, "^";
#EndIf;
				break;
			} else if(_token_type == TT_ROUTINE_FILTER ) {
				print "TODO: fix routine filter";
				break;
			}
			! This is a token we don't recognize, which means we fail at matching against this line
#IfDef DEBUG;
			print "Unknown token: ", _token, "^";
#EndIf;
			break;
		}
		! This pattern has failed.
#IfDef DEBUG;
		print "Pattern didn't match.^";
#EndIf;
		! Scan to the end of this pattern
		while(_pattern_index -> 0 ~= TT_END) _pattern_index = _pattern_index + 3;
		_pattern = _pattern_index + 1;
	}

	"Sorry, I didn't understand that.";



.parse_success;
	action = (_pattern --> 0) & $03ff;
	PerformPreparedAction();
];

[ ActionPrimitive; indirect(#actions_table-->action); ];

[ PerformPreparedAction;
	!print "Performing action ", action, "^";
	sw__var = action;
    if ((BeforeRoutines() == false) && action < 4096) {
	    ActionPrimitive();
	}
];

[ BeforeRoutines _i _obj;
	! react_before - Loops over the scope to find possible react_before routines
	! to run in each object, if it's found stop the action by returning true
 	for(_i = 0: _i < scope_objects: _i++) {
 		_obj = scope-->_i;
 		if (_obj provides react_before) {
 			if(PrintOrRun(_obj, react_before)) {
 				rtrue;
 			}
 		}
 	}
	if(location provides before && RunRoutines(location, before)) {
		rtrue;
 	}
	if(inp1 provides before && RunRoutines(inp1, before)) {
		rtrue;
 	}
	rfalse;
];

[ PerformAction p_action p_noun p_second _sa _sn _ss;
    _sa = action; _sn = noun; _ss = second;
    action = p_action; noun = p_noun; second = p_second;
	PerformPreparedAction();
    action = _sa; noun = _sn; second = _ss;
];

[ R_Process p_action p_noun p_second _s1 _s2;
    _s1 = inp1; _s2 = inp2;
    inp1 = p_noun; inp2 = p_second;
    PerformAction(p_action, p_noun, p_second);
    inp1 = _s1; inp2 = _s2;
];

[ CDefArt p_obj _result;
	PrintObjName(p_obj, FORM_CDEF);
];

[ DefArt p_obj _result;
	PrintObjName(p_obj, FORM_DEF);
];

[ IndefArt p_obj _result;
	PrintObjName(p_obj, FORM_INDEF);
];


#IfV3;
! These routines are implemented by Veneer, but the default implementations give compile errors for z3

[ FindIndivPropValue p_obj p_property _x _prop_id;
  _x = p_obj.3;
	if (_x == 0) rfalse;
!  print "Table for ", (object) obj, " is at ", (hex) x, "^";
	while (true) {
		_prop_id = _x-->0;
		if(_prop_id == 0) break;
		if(_prop_id & 32767 == p_property) break;
!	print (hex) x-->0, " (", (property) x-->0, ")  length ", x->2, ": ";
!       for (n = 0: n< (x->2)/2: n++)
!           print (hex) (x+3)-->n, " ";
!       new_line;
      _x = _x + _x->2 + 3;
  }
  return _x;
];

[ RV__Pr p_object p_property _value _address;
	if(p_object == 0) {
		print "*Error: Tried to read property ";
		Print__PName(p_property);
		" of nothing.*";
	}
	if(p_object provides p_property) {
		if(p_property >= INDIV_PROP_START) {
			_address = FindIndivPropValue(p_object, p_property);
			return (_address + 3)-->0;
		}
		@get_prop p_object p_property -> _value;
		return _value;
	}
	print "[Error: Tried to read undefined property ";
	Print__PName(p_property);
	print " of ";
	PrintShortName(p_object);
	print "]";
	rfalse;
];

!      CA__Pr:  call, that is, print-or-run-or-read, a property:
!                      this exactly implements obj..prop(...).  Note that
!                      classes (members of Class) have 5 built-in properties
!                      inherited from Class: create, recreate, destroy,
!                      remaining and copy.  Implementing these here prevents
!                      the need for a full metaclass inheritance scheme.      */

[ CA__Pr obj id a    x y z s s2 n m;
!	print "CA_Pr obj = ", obj,", id = ", id,", a = ", a, "^";
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
!	print "CA_Pr(2) obj = ", obj,", id = ", id,", a = ", a, "^";
! 	@check_arg_count 3 ?~A__x;y++;@check_arg_count 4 ?~A__x;y++;
! 	@check_arg_count 5 ?~A__x;y++;@check_arg_count 6 ?~A__x;y++;
! 	@check_arg_count 7 ?~A__x;y++;@check_arg_count 8 ?~A__x;y++;.A__x;
! 	#ifdef INFIX;if (obj has infix__watching) n=1;#endif;
	#ifdef DEBUG;if (debug_flag & 1 ~= 0) n=1;#endif;
! 	if (n==1) {
! 		n=debug_flag & 1; debug_flag=debug_flag-n;
! 		print "[ ~", (name) obj, "~.", (property) id, "(";
! 		switch(y) {
! 		1:
! 			print a; 2: print a,",",b; 3: print a,",",b,",",c;
! 		4:
! 			print a,",",b,",",c,",",d;
! 		5:
! 			print a,",",b,",",c,",",d,",",e;
! 		6:
! 			print a,",",b,",",c,",",d,",",e,",",f;
! 		}
! 		print ") ]^"; debug_flag = debug_flag + n;
! 	}
	if (id > 0 && id < INDIV_PROP_START) {
!		print "CA_Pr OK obj = ", obj,", id = ", id,", a = ", a, "^";
		x = obj.&id;
		if (x==0) {
			x=$000a-->0 + 2*(id-1); n=2;
		} else n = obj.#id;
	} else {
		if (id>=64 && id<69 && obj in Class) {
!			print "CA_Pr ERROR0 obj = ", obj,", id = ", id,", a = ", a, "^";
			RT__Err("Class create etc", obj, id); return;
			!			return Cl__Ms(obj,id,y,a,b,c);
		}
!		print "CA_Pr(2.1) obj = ", obj,", id = ", id,", n = ", n, "^";
		x = obj..&id;
!		print "CA_Pr(2.2) obj = ", obj,", id = ", id,", x = ", x, "^";
		if (x == 0) {
!			print "CA_Pr ERROR1 obj = ", obj,", id = ", id,", a = ", a, "^";
			.Call__Error;
			RT__Err("send message", obj, id); return;
		}
!		print "Reading n at ", x-1,": ", 0->(x-1), "^";
		n = 0->(x-1);
		if (id&$C000==$4000)
			switch (n&$C0) { 0: n=1; $40: n=2; $80: n=n&$3F; }
	}
!	print "CA_Pr(3) obj = ", obj,", id = ", id,", a = ", a, "^";
	for (:2*m<n:m++) {
!		print "Considering routine at ", x+2*m,": ", x-->m, "^";
		if (x-->m==$ffff) rfalse;
		switch(Z__Region(x-->m)) {
		2:
			s = sender; sender = self; self = obj; s2 = sw__var;
! 			switch(y) {
! 			0:
! 				z = indirect(x-->m);
! 			1:
 				z = indirect(x-->m, a);
! 			2:
! 				z = indirect(x-->m, a, b);
! 			3:
!				z = indirect(x-->m, a, b, c);
! 			4:
! 				z = indirect(x-->m, a, b, c, d);
! 			5:
! 				z = indirect(x-->m, a, b, c, d, e);
! 			6:
! 				z = indirect(x-->m, a, b, c, d, e, f);
! 			}
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

Object DefaultPlayer "you"
	has concealed;

[ PrintMsg msg;
	print (string) msg;
];

[ main i;
	print "PunyInform 0.0^^";

	player = DefaultPlayer;
	game_state = GS_PLAYING;
	Initialise();
	if(parent(player) == 0) PlayerTo(location);
	<Look>; ! Equivalent to PerformAction(##Look);

	while(game_state == GS_PLAYING) {
		status_field_1 = score;
		status_field_2 = turns;
		ReadPlayerInput();
		ParseAndPerformAction();
		if(action >= 0) turns++;
	}
	if(game_state == GS_QUIT) @quit;
	if(game_state == GS_WIN) PrintMsg(MSG_YOUHAVEWON);
	else if(game_state == GS_DEAD) PrintMsg(MSG_YOUHAVEDIED);
	else DeathMessage();
	print ".^";
	for (::)
	{
		PrintMsg(MSG_RESTARTRESTOREORQUIT);
#IFV3; 
		read player_input_array parse_array; 
#IFNOT;
		read player_input_array parse_array DrawStatusLine;
#ENDIF;
		i=parse_array-->1;
		if (i=='restart') @restart;
		if (i=='restore') RestoreSub();
		if (i=='quit') @quit;
	}
];

! todo: This has to be at the end of the file in case no DeathMessage was
! defined. Do we need to specify two includes (puny_begin.h and puny_end.h)?
#Stub DeathMessage      0;
