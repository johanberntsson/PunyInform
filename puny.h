! Always use grammar version 2 which is easier to parse and more economical
! See: section 8.6 in https://www.inform-fiction.org/source/tm/TechMan.txt
Constant Grammar__Version = 2;

Attribute light;

Constant TT_PREPOSITION      = 2;    ! e.g. 'into'
Constant TT_ROUTINE_FILTER   = 3;    ! e.g. noun=CagedCreature
Constant TT_ATTR_FILTER      = 4;    ! e.g. edible
Constant TT_SCOPE            = 5;    ! e.g. scope=Spells
Constant TT_PARSE_ROUTIME    = 6;    ! a parse routine
Constant TT_END              = 15;   ! End of grammar line

Constant GS_PLAYING          = 1;
Constant GS_QUIT             = 2;
Constant GS_DEAD             = 3;

#IfV3;
Constant DICT_BYTES_FOR_WORD = 4;
#IfNot;
Constant DICT_BYTES_FOR_WORD = 6;
#EndIf;

Global location = 0; ! Must be first global
Global action;
Global reverse;
Global game_state;

Constant MAX_INPUT_CHARS     = 78;
Constant MAX_INPUT_WORDS     = 20;

Array player_input_array->(MAX_INPUT_CHARS + 3);
Array parse_array->(2 + 4 * MAX_INPUT_WORDS);

! ######################### Grammar + Actions
[QuitSub;
	game_state = GS_QUIT;
];

[TakeSub a b c;
	game_state = GS_QUIT;
];

Verb 'quit'
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
[check_pattern pattern   i action_number;
	action_number = pattern-->0;
	action = action_number & $3ff;
	reverse = (action_number & $400 ~= 0);
	print "Action#: ", action, " Reverse: ", reverse, "^";
	pattern = pattern + 2;
	for(i = 0: : i++) {
		if(pattern->0 == TT_END) break;
		print "Token#: ", i, " Type: ", pattern->0, " Data: ",(pattern + 1)-->0,"^";
		pattern = pattern + 3;
	}
!	print ": ", i, " tokens^";
	return pattern + 1;
];
! #EndIf;

[parse_and_perform_action   verb word_data verb_num verb_grammar num_patterns i pattern;
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

	verb_num = 255 - (word_data->1);
	print "Verb#: ",verb_num,".^";
	verb_grammar = (0-->7)-->verb_num;
	print "Grammar address for this verb: ",verb_grammar,"^";
	num_patterns = verb_grammar->0;
	print "Number of patterns: ",num_patterns,"^";

	pattern = verb_grammar + 1;
	!pattern = verb_grammar;
	for(i = 0 : i < num_patterns : i++) {
		print "############ Pattern ",i,"^";
		pattern = check_pattern(pattern);
	}


	if(parse_array->1 == 1 && parse_array-->1 == 'quit') {
		! game_state = GS_QUIT;
		print "parse properly, no cheating allowed.^";
	} else {
		print "Sorry, I didn't understand that.^";
	}
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

