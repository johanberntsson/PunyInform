Attribute light;

Constant TT_PREPOSITION     = 2;    ! e.g. 'into'
Constant TT_ROUTINE_FILTER  = 3;    ! e.g. noun=CagedCreature
Constant TT_ATTR_FILTER     = 4;    ! e.g. edible
Constant TT_SCOPE           = 5;    ! e.g. scope=Spells
Constant TT_PARSE_ROUTIME   = 6;    ! a parse routine
Constant TT_END             = 15;   ! End of grammar line

Constant GS_PLAYING         = 1;
Constant GS_QUIT            = 2;
Constant GS_DEAD            = 3;

Global location = 0; ! Must be first global
Global game_state;

Constant MAX_INPUT_CHARS    = 78;
Constant MAX_INPUT_WORDS    = 20;

Array player_input_array->(MAX_INPUT_CHARS + 3);
Array parse_array->(2 + 4 * MAX_INPUT_WORDS);

[read_and_parse   result;
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

[parse_and_perform_action;
	if(parse_array->1 == 1 && parse_array-->1 == 'quit') {
		game_state = GS_QUIT;
	} else {
		print "Sorry, I didn't understand that.^";
	}
];



[main;
	print "PunyInform 0.0^^";
	game_start();
	game_state = GS_PLAYING;
	while(game_state == GS_PLAYING) {
		read_and_parse();
		parse_and_perform_action();
	}
];

