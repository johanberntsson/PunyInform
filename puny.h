! Always use grammar version 2 which is easier to parse and more economical
! See: section 8.6 in https://www.inform-fiction.org/source/tm/TechMan.txt
Constant Grammar__Version = 2;

Attribute light;

! Property name; ! This seems to be hardcoded in the Inform compiler
Property initial;
Property description;


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

#IfV3;
Constant DICT_BYTES_FOR_WORD = 4;
#IfNot;
Constant DICT_BYTES_FOR_WORD = 6;
#EndIf;

Global location = 1; ! Must be first global
Global status_field_1 = 0; ! Must be second global. Is used to show score or hours
Global status_field_2 = 0; ! Must be third global. Is used to show turns or minutes
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

Constant MAX_INPUT_CHARS     = 78;
Constant MAX_INPUT_WORDS     = 20;

Array player_input_array->(MAX_INPUT_CHARS + 3);
Array parse_array->(2 + 4 * (MAX_INPUT_WORDS + 1)); ! + 1 to make room for an extra word which is set to 0

! ######################### Include utility files

Include "scope.h";

! ######################### Grammar + Actions
[ LookSub _i _obj;
	! cheating for now
	print (string) location.description;
	new_line;
	for(_i = 0: _i < scope_objects: _i++) {
		_obj = scope-->_i;
		if(_obj ~= nothing) {
			new_line;
			if(_obj.initial) {
				print (string) _obj.initial;
			} else {
				print "There is a ",(name) scope-->_i, " here. ";
			}
			new_line;
		}
	}
];



[QuitSub;
	game_state = GS_QUIT;
	"Quitting...";
];

[TakeSub;
	"Taking...";
];

[DropSub;
	"Taking...";
];

Verb 'look'
	* -> Look;

Verb 'quit'
	* -> Quit
	* 'into'/'out'/'of'/'hello'/'bob' noun -> Quit
	* 'out' 'of' -> Take reverse;

Verb 'take' 'get'
	* noun -> Take;

Verb 'drop'
	* noun -> Drop;

! ######################### Parser

[read_player_input _result;
	print ">";
	parse_array->0 = MAX_INPUT_WORDS;
#IfV5;
	player_input_array->0 = MAX_INPUT_CHARS;
	player_input_array->1 = 0;
	@aread player_input_array parse_array -> _result;
#IfNot;
	player_input_array->0 = MAX_INPUT_CHARS - 1;
	@sread player_input_array parse_array;
#EndIf;
	input_words = parse_array -> 1;
	! Set word after last word in parse array to all zeroes, so it won't match any words.
	_result = 2 * input_words + 1;
	parse_array-->_result = 0;
	parse_array-->(_result + 1) = 0;
];


[check_pattern p_pattern _i _action_number _token_top _token_next _token_bottom;
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

[check_noun p_parse_pointer _i _j _n _p _obj _result _matches _last_match _current_word _name_array _name_array_len _best_score;
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

[parse_and_perform_action _verb _word_data _verb_num _verb_grammar _num_patterns _i _pattern _pattern_index _token _token_type _data _parse_pointer _noun_tokens _noun;
	if(parse_array->1 < 1) {
		"Come again?";
	}
	_verb = parse_array-->1;
	if(_verb < (0-->4)) {
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
	print "Verb#: ",_verb_num,".^";
	_verb_grammar = (0-->7)-->_verb_num;
	print "Grammar address for this verb: ",_verb_grammar,"^";
	_num_patterns = _verb_grammar->0;
	print "Number of patterns: ",_num_patterns,"^";

	! First print all patterns, for debug purposes
	_pattern = _verb_grammar + 1;
	for(_i = 0 : _i < _num_patterns : _i++) {
		print "############ Pattern ",_i,"^";
		_pattern = check_pattern(_pattern);
	}

	@new_line;
	_pattern = _verb_grammar + 1;
	for(_i = 0 : _i < _num_patterns : _i++) {
		print "############ Pattern ",_i," address ", _pattern, "^";
		wn = 1;
		_parse_pointer = parse_array + 6;
		_pattern_index = _pattern - 1;
		_noun_tokens = 0;
		noun = 0;
		second = 0;
		while(true) {
			_pattern_index = _pattern_index + 3;
			_token = _pattern_index -> 0;
			print "TOKEN: ", _token, "^";
			_data = (_pattern_index + 1) --> 0;
			if(_token == TT_END) {
				if(wn == parse_array -> 1) {
					jump parse_success;
				}
				break; ! Fail because the grammar line ends here but not the input
			}
			if(wn >= parse_array -> 1) { !Fail because input ends here but not the grammar line
				print "Fail, since grammar line has not ended but player input has.^";
				break;
			}
			_token_type = _token & $0f;
			if(_token_type == TT_PREPOSITION) { ! $42 = Single prep, $62 = Beginning of list of alternatives, $72 = middle of list, $52 = end of list
				print "Preposition: ", _data, "^";
				if(_parse_pointer --> 0 == _data) {
					print "Match!^";
					wn++;
					_parse_pointer = _parse_pointer + 4;
					while(_token == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) { ! Alternative prepositions which are not at the end of the list
						print "Skipping one alternative...^";
						_pattern_index = _pattern_index + 3;
						_token = _pattern_index -> 0;
					}
					continue;
				}
				print "Failed prep: ", _parse_pointer, ":", _parse_pointer --> 0, " should have been ", _data, "^";
				if(_token == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) continue; ! First in a list or in the middle of a list of alternative prepositions, so keep parsing!
				break; ! Fail because this is the only or the last alternative preposition and the word in player input doesn't match it
			} else if(_token_type == TT_NOUN) {
				! we expect a noun here
				! check all objects in 'scope', and see if any match.
				! If so, update wn and parse_pointer, and return success
				_noun = check_noun(_parse_pointer);
				if(_noun > 0) {
					print "Noun match!^";
					if(_noun_tokens == 0) {
						noun = _noun;
					} else if(_noun_tokens == 1){
						second = _noun;
					}
					_noun_tokens++;
					continue;
				}
				print "Not a matching noun: ", _parse_pointer, ":", _parse_pointer --> 0, "^";
				break;
			}
			! This is a token we don't recognize, which means we fail at matching against this line
			print "Unknown token: ", _token, "^";
			break;
		}
		! This pattern has failed.
		print "Pattern didn't match.^";
		! Scan to the end of this pattern
		while(_pattern_index -> 0 ~= TT_END) _pattern_index = _pattern_index + 3;
		_pattern = _pattern_index + 1;
	}

	"Sorry, I didn't understand that.^";

.parse_success;
	action = (_pattern --> 0) & $03ff;
	perform_action();
];

[action_primitive; indirect(#actions_table-->action); ];

[perform_action;
	print "Performing action ", action, "^";
! Add check for before routines and fake actions later
!    if ((BeforeRoutines() == false) && action < 4096)
        action_primitive();
];

[perform_action_safe p_action p_noun p_second _sa _sn _ss;
    _sa = action; _sn = noun; _ss = second;
    action = p_action; noun = p_noun; second = p_second;
	perform_action();
    action = _sa; noun = _sn; second = _ss;
];

[R_Process p_action p_noun p_second _s1 _s2;
    _s1 = inp1; _s2 = inp2;
    inp1 = p_noun; inp2 = p_second;
    perform_action_safe(p_action, p_noun, p_second);
    inp1 = _s1; inp2 = _s2;
];


[main;
	print "PunyInform 0.0^^";

	game_state = GS_PLAYING;
	game_start();
	<Look>; ! Equivalent to perform_action(##Look);

	while(game_state == GS_PLAYING) {
		read_player_input();
		parse_and_perform_action();
	}
];

