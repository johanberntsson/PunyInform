Include "scope.h";

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
Global reverse;
Global game_state;
Global wn;

Constant MAX_INPUT_CHARS     = 78;
Constant MAX_INPUT_WORDS     = 20;

Array player_input_array->(MAX_INPUT_CHARS + 3);
Array parse_array->(2 + 4 * MAX_INPUT_WORDS);

! ######################### Grammar + Actions
[QuitSub;
	game_state = GS_QUIT;
	"Quitting...";
];

[TakeSub;
	"Taking...";
];

Verb 'quit'
	* -> Quit
	* 'into'/'out'/'of'/'hello'/'bob' noun -> Quit
	* 'out' 'of' -> Take reverse;

! ######################### Parser

[read_player_input   result;
	print ">";
	parse_array->0 = MAX_INPUT_WORDS;
#IfV5;
	player_input_array->0 = MAX_INPUT_CHARS;
	player_input_array->1 = 0;
	@aread player_input_array parse_array -> result;
#IfNot;
	player_input_array->0 = MAX_INPUT_CHARS - 1;
	@sread player_input_array parse_array;
#EndIf;
];


! #IfTrue Grammar__Version == 1;
! [check_pattern pattern   i c;
! 	print "Params wanted: ", pattern->0,". ";
! 	for(i = 1: i < 7: i++) {
! 		if(pattern->i ~= 0) {
! 			print pattern->i,", ";
! 			c++;
! 		} else {
! 			break;
! 		}
! 	}
! 	print ": ", c, " tokens. Action#: ", pattern->7, "^";
! 	return pattern + 8;
! ];
! #IfNot;
! Grammar version 2
[check_pattern pattern   i action_number token_top token_next token_bottom;
    ! action number is the first two bytes
	action_number = pattern-->0;
	pattern = pattern + 2;
	action = action_number & $3ff;
	reverse = (action_number & $400 ~= 0);
	print "Action#: ", action, " Reverse: ", reverse, "^";

	for(i = 0: : i++) {
		if(pattern->0 == TT_END) break;
		token_top = (pattern->0 & $c0)/64; ! top (2 bits)
		token_next = (pattern->0 & $30)/16;  ! next (2 bits)
		token_bottom = pattern->0 & $0f; ! bottom (4 bits)
		print "Token#: ", i, " Type: ", pattern->0, " (top ", token_top, ", next ",token_next, ", bottom ",token_bottom, ") Next byte: ",(pattern + 1)-->0,"^";
		pattern = pattern + 3;
	}
!	print ": ", i, " tokens^";
	return pattern + 1; ! skip TT_END
];
! #EndIf;

[check_noun parse_pointer   i j n p obj result matches last_match current_word name_array name_array_len;
	! return -1 if no noun matches
	! return -2 if more than one match found
	! else return object number
	n = wn;
	p = parse_pointer;
	current_word = parse_pointer-->0;
	for(i = 0: i < MAX_SCOPE: i++) {
		obj = scope-->i;
		if(obj == nothing) continue;
		! the matching of name with parse_array doesn't work yet
		if(obj.#name > 1) {
			name_array = obj.&name;
			name_array_len = obj.#name / 2;
#IfV5;
			@scan_table current_word name_array name_array_len -> result ?success;
#IfNot;
			for(j = 0: j < name_array_len: j++) {
				if(name_array-->j == current_word) jump success;
			}
#EndIf;
			continue;
		}
.success;
		n++;
		p = p + 4;
		matches++;
		last_match = obj;

!		print "checking ", obj.&name-->0, " ", current_word, "^";
	}
	if(matches == 1) {
		wn = n;
		parse_pointer = p;
		return last_match;
	}
	if(matches > 1) return -2;
	return -1;
];

[parse_and_perform_action   verb word_data verb_num verb_grammar num_patterns i pattern pattern_index token token_type data parse_pointer;
	if(parse_array->1 < 1) {
		"Come again?";
	}
	verb = parse_array-->1;
	if(verb < (0-->4)) {
		"That is not a verb I recognize.";
	}

	word_data = verb + DICT_BYTES_FOR_WORD;
	if((word_data->0) & 1 == 0) { ! This word does not have the verb flag set.
		"That is not a verb I recognize.";
	}

	! Now we know that the first word is a verb

! 	print "Parse array: ", parse_array, "^";
! 	print "Word count: ", parse_array->0, "^";
! 	print "Word 1: ", (parse_array + 2)-->0, "^";
! 	print "Word 2: ", (parse_array + 6)-->0, "^";
! 	print "Word 3: ", (parse_array + 10)-->0, "^";
	verb_num = 255 - (word_data->1);
	print "Verb#: ",verb_num,".^";
	verb_grammar = (0-->7)-->verb_num;
	print "Grammar address for this verb: ",verb_grammar,"^";
	num_patterns = verb_grammar->0;
	print "Number of patterns: ",num_patterns,"^";

! First print all patterns, for debug purposes
	pattern = verb_grammar + 1;
	for(i = 0 : i < num_patterns : i++) {
		print "############ Pattern ",i,"^";
		pattern = check_pattern(pattern);
	}

	@new_line;
	pattern = verb_grammar + 1;
	for(i = 0 : i < num_patterns : i++) {
		print "############ Pattern ",i," address ", pattern, "^";
		wn = 1;
		parse_pointer = parse_array + 6;
		pattern_index = pattern - 1;
		while(true) {
			pattern_index = pattern_index + 3;
			token = pattern_index -> 0;
			print "TOKEN: ", token, "^";
			data = (pattern_index + 1) --> 0;
			if(token == TT_END) {
				if(wn == parse_array -> 1) {
					jump parse_success;
				}
				break; ! Fail because the grammar line ends here but not the input
			}
			if(wn >= parse_array -> 1) { !Fail because input ends here but not the grammar line
				print "Fail, since grammar line has not ended but player input has.^";
				break;
			}
			token_type = token & $0f;
			if(token_type == TT_PREPOSITION) { ! $42 = Single prep, $62 = Beginning of list of alternatives, $72 = middle of list, $52 = end of list
				print "Preposition: ", data, "^";
				if(parse_pointer --> 0 == data) {
					print "Match!^";
					wn++;
					parse_pointer = parse_pointer + 4;
					while(token == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) { ! Alternative prepositions which are not at the end of the list
						print "Skipping one alternative...^";
						pattern_index = pattern_index + 3;
						token = pattern_index -> 0;
					}
					continue;
				}
				print "Failed prep: ", parse_pointer, ":", parse_pointer --> 0, " should have been ", data, "^";
				if(token == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) continue; ! First in a list or in the middle of a list of alternative prepositions, so keep parsing!
				break; ! Fail because this is the only or the last alternative preposition and the word in player input doesn't match it
			} else if(token_type == TT_NOUN) {
				! we expect a noun here
				! check all objects in 'scope', and see if any match.
				! If so, update wn and parse_pointer, and return success
				if(check_noun(parse_pointer) >= 0) {
					print "Noun match!^";
					continue;
				}
				print "Not a matching noun: ", parse_pointer, ":", parse_pointer --> 0, "^";
				break;
			}
			! This is a token we don't recognize, which means we fail at matching against this line
			print "Unknown token: ", token, "^";
			break;
		}
		! This pattern has failed.
		print "Pattern didn't match.^";
		! Scan to the end of this pattern
		while(pattern_index -> 0 ~= TT_END) pattern_index = pattern_index + 3;
		pattern = pattern_index + 1;
	}

	"Sorry, I didn't understand that.^";

.parse_success;
	print "Complete pattern match!^";
	action = (pattern --> 0) & $03ff;
	print "Performing action ", action, "^";
	indirect(#actions_table --> action);
];



[main;
	print "PunyInform 0.0^^";
	print "Adj: ", #adjectives_table;
	@new_line;
	print "Action: ", #actions_table;
	@new_line;

	game_start();
	game_state = GS_PLAYING;
	while(game_state == GS_PLAYING) {
		read_player_input();
		parse_and_perform_action();
	}
];

