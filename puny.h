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
Global input_words;
Global scope_objects;

Constant MAX_INPUT_CHARS     = 78;
Constant MAX_INPUT_WORDS     = 20;

Array player_input_array->(MAX_INPUT_CHARS + 3);
Array parse_array->(2 + 4 * MAX_INPUT_WORDS);

! ######################### Include utility files

Include "scope.h";

! ######################### Grammar + Actions
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

Verb 'quit'
	* -> Quit
	* 'into'/'out'/'of'/'hello'/'bob' noun -> Quit
	* 'out' 'of' -> Take reverse;

Verb 'take' 'get'
	* noun -> Take;

Verb 'drop'
	* noun -> Drop;

! ######################### Parser

[read_player_input v_result;
	print ">";
	parse_array->0 = MAX_INPUT_WORDS;
#IfV5;
	player_input_array->0 = MAX_INPUT_CHARS;
	player_input_array->1 = 0;
	@aread player_input_array parse_array -> v_result;
#IfNot;
	player_input_array->0 = MAX_INPUT_CHARS - 1;
	@sread player_input_array parse_array;
#EndIf;
	input_words = parse_array -> 1;
];


[check_pattern p_pattern v_i v_action_number v_token_top v_token_next v_token_bottom;
    ! action number is the first two bytes
	v_action_number = p_pattern-->0;
	p_pattern = p_pattern + 2;
	action = v_action_number & $3ff;
	reverse = (v_action_number & $400 ~= 0);
	print "Action#: ", action, " Reverse: ", reverse, "^";

	for(v_i = 0: : v_i++) {
		if(p_pattern->0 == TT_END) break;
		v_token_top = (p_pattern->0 & $c0)/64; ! top (2 bits)
		v_token_next = (p_pattern->0 & $30)/16;  ! next (2 bits)
		v_token_bottom = p_pattern->0 & $0f; ! bottom (4 bits)
		print "Token#: ", v_i, " Type: ", p_pattern->0, " (top ", v_token_top, ", next ",v_token_next, ", bottom ",v_token_bottom, ") Next byte: ",(p_pattern + 1)-->0,"^";
		p_pattern = p_pattern + 3;
	}
!	print ": ", i, " tokens^";
	return p_pattern + 1; ! skip TT_END
];

[check_noun p_parse_pointer v_i v_j v_n v_p v_obj v_result v_matches v_last_match v_current_word v_name_array v_name_array_len v_best_score;
	! return -1 if no noun matches
	! return -2 if more than one match found
	! else return object number
	for(v_i = 0: v_i < scope_objects: v_i++) {
		v_n = wn;
		v_p = p_parse_pointer;
		v_current_word = p_parse_pointer-->0;
		v_obj = scope-->v_i;
!		if(v_obj == nothing) continue;
		if(v_obj.#name > 1) {
			v_name_array = v_obj.&name;
			v_name_array_len = v_obj.#name / 2;
			while(v_n < input_words) {
#IfV5;
				@scan_table v_current_word v_name_array v_name_array_len -> v_result ?success;
#IfNot;
				for(v_j = 0: v_j < v_name_array_len: v_j++) {
					if(v_name_array-->v_j == v_current_word) jump success;
				}
#EndIf;
				jump not_matched;
.success;
				v_n++;
				v_p = v_p + 4;
				if(v_n == v_best_score) v_matches++;
				if(v_n > v_best_score) {
					v_last_match = v_obj;
					v_best_score = v_n;
					v_matches = 1;
				}
			}
		}
.not_matched;

!		print "checking ", v_obj.&name-->0, " ", v_current_word, "^";
	}
	if(v_matches == 1) {
		wn = v_best_score;
!		parse_pointer = p;
		return v_last_match;
	}
	if(v_matches > 1) return -2;
	return -1;
];

[parse_and_perform_action v_verb v_word_data v_verb_num v_verb_grammar v_num_patterns v_i v_pattern v_pattern_index v_token v_token_type v_data v_parse_pointer;
	if(parse_array->1 < 1) {
		"Come again?";
	}
	v_verb = parse_array-->1;
	if(v_verb < (0-->4)) {
		"That is not a verb I recognize.";
	}

	v_word_data = v_verb + DICT_BYTES_FOR_WORD;
	if((v_word_data->0) & 1 == 0) { ! This word does not have the verb flag set.
		"That is not a verb I recognize.";
	}

	! Now we know that the first word is a verb

! 	print "Parse array: ", parse_array, "^";
! 	print "Word count: ", parse_array->0, "^";
! 	print "Word 1: ", (parse_array + 2)-->0, "^";
! 	print "Word 2: ", (parse_array + 6)-->0, "^";
! 	print "Word 3: ", (parse_array + 10)-->0, "^";
	v_verb_num = 255 - (v_word_data->1);
	print "Verb#: ",v_verb_num,".^";
	v_verb_grammar = (0-->7)-->v_verb_num;
	print "Grammar address for this verb: ",v_verb_grammar,"^";
	v_num_patterns = v_verb_grammar->0;
	print "Number of patterns: ",v_num_patterns,"^";

	! First print all patterns, for debug purposes
	v_pattern = v_verb_grammar + 1;
	for(v_i = 0 : v_i < v_num_patterns : v_i++) {
		print "############ Pattern ",v_i,"^";
		v_pattern = check_pattern(v_pattern);
	}

	@new_line;
	v_pattern = v_verb_grammar + 1;
	for(v_i = 0 : v_i < v_num_patterns : v_i++) {
		print "############ Pattern ",v_i," address ", v_pattern, "^";
		wn = 1;
		v_parse_pointer = parse_array + 6;
		v_pattern_index = v_pattern - 1;
		while(true) {
			v_pattern_index = v_pattern_index + 3;
			v_token = v_pattern_index -> 0;
			print "TOKEN: ", v_token, "^";
			v_data = (v_pattern_index + 1) --> 0;
			if(v_token == TT_END) {
				if(wn == parse_array -> 1) {
					jump parse_success;
				}
				break; ! Fail because the grammar line ends here but not the input
			}
			if(wn >= parse_array -> 1) { !Fail because input ends here but not the grammar line
				print "Fail, since grammar line has not ended but player input has.^";
				break;
			}
			v_token_type = v_token & $0f;
			if(v_token_type == TT_PREPOSITION) { ! $42 = Single prep, $62 = Beginning of list of alternatives, $72 = middle of list, $52 = end of list
				print "Preposition: ", v_data, "^";
				if(v_parse_pointer --> 0 == v_data) {
					print "Match!^";
					wn++;
					v_parse_pointer = v_parse_pointer + 4;
					while(v_token == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) { ! Alternative prepositions which are not at the end of the list
						print "Skipping one alternative...^";
						v_pattern_index = v_pattern_index + 3;
						v_token = v_pattern_index -> 0;
					}
					continue;
				}
				print "Failed prep: ", v_parse_pointer, ":", v_parse_pointer --> 0, " should have been ", v_data, "^";
				if(v_token == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) continue; ! First in a list or in the middle of a list of alternative prepositions, so keep parsing!
				break; ! Fail because this is the only or the last alternative preposition and the word in player input doesn't match it
			} else if(v_token_type == TT_NOUN) {
				! we expect a noun here
				! check all objects in 'scope', and see if any match.
				! If so, update wn and parse_pointer, and return success
				if(check_noun(v_parse_pointer) >= 0) {
					print "Noun match!^";
					continue;
				}
				print "Not a matching noun: ", v_parse_pointer, ":", v_parse_pointer --> 0, "^";
				break;
			}
			! This is a token we don't recognize, which means we fail at matching against this line
			print "Unknown token: ", v_token, "^";
			break;
		}
		! This pattern has failed.
		print "Pattern didn't match.^";
		! Scan to the end of this pattern
		while(v_pattern_index -> 0 ~= TT_END) v_pattern_index = v_pattern_index + 3;
		v_pattern = v_pattern_index + 1;
	}

	"Sorry, I didn't understand that.^";

.parse_success;
	print "Complete pattern match!^";
	action = (v_pattern --> 0) & $03ff;
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

