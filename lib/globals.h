! Part of PunyInform, a standard library for writing interactive fiction using Inform 6.
!
System_file;

Constant PUNYINFORM_MAJOR_VERSION = 2;
Constant PUNYINFORM_MINOR_VERSION = 5;

#Ifndef VN_1634;
Message fatalerror "*** The PunyInform library needs Inform v6.34 or later to work ***";
#Endif; ! VN_

#IfnDef CUSTOM_ABBREVIATIONS;
Abbreviate "You ";
Abbreviate "thing";
Abbreviate "n't ";
Abbreviate "This game is now in its ";
Abbreviate " you ";
Abbreviate " descriptions of locations ";
Abbreviate "There is no";
Abbreviate " to ";
Abbreviate " that";
Abbreviate "lready ";
Abbreviate "which ";
Abbreviate "ed.";
Abbreviate "ing ";
Abbreviate "nderstand";
Abbreviate " the ";
Abbreviate "here";
Abbreviate " no";
Abbreviate "That";
Abbreviate "Inform";
Abbreviate "You're";
Abbreviate ".^";
Abbreviate "some";
Abbreviate "have";
Abbreviate "~ mode, ";
Abbreviate "would ";
Abbreviate "tion";
Abbreviate " fixed in place.";
Abbreviate "nexpect";
Abbreviate "are";
Abbreviate "can ";
Abbreviate ", ";
Abbreviate "yourself";
Abbreviate "before";
Abbreviate "I do";
Abbreviate "see";
Abbreviate "always gives ";
Abbreviate " ***";
Abbreviate "e.";
Abbreviate " has better ";
Abbreviate "Cannot put ";
Abbreviate "is clos";
Abbreviate " number";
Abbreviate "achieve";
Abbreviate "But";
Abbreviate " in";
Abbreviate " of";
Abbreviate "(first tak";
Abbreviate "The ";
Abbreviate " (";
Abbreviate "'s";
Abbreviate " by this.";
Abbreviate " happens.";
Abbreviate "lease ";
Abbreviate "switch";
Abbreviate " empty";
Abbreviate "moment";
Abbreviate "pecial";
Abbreviate "rest";
Abbreviate " error: ";
Abbreviate "(even if";
Abbreviate " itself.";
Abbreviate " an";
Abbreviate " on";
Abbreviate "hat";
#EndIf;

#IfDef STATUSLINE_TIME;
#IfDef STATUSLINE_SCORE;
Message fatalerror "Can't define both STATUSLINE_TIME and STATUSLINE_SCORE."
#EndIf;
#EndIf;

Constant Grammar__Version = 2;
Constant INDIV_PROP_START 64;
Constant NULL         = $ffff;

Constant PHASE1 = 1; ! just check if pattern matches (no side effects such as which? questions or indirect actions)
Constant PHASE2 = 2; ! parse the given pattern with side effects
Constant PHASE2_SUCCESS         = 0;
Constant PHASE2_ERROR           = 1;
Constant PHASE2_DISAMBIGUATION  = 2;


!Constant WORDSIZE 2; ! set by the compiler from Inform 6.30

Constant ALL_WORD     = 'all';
Constant EXCEPT_WORD1 = 'but';
Constant EXCEPT_WORD2 = 'except';
Constant AND_WORD     = 'and';
Constant THEN1__WD    = 'then';
Constant comma_word   = 'comma,';  ! An "untypeable word" used to substitute
                                   ! for commas in parse buffers

! Isolating words and punctuation for reuse and potential translation
Constant SOMETHING_STR = "something";
Constant SOMEONE_STR = "someone";

#Ifndef DEFAULT_CAPACITY;
Constant DEFAULT_CAPACITY = 100;
#Endif;

#Ifdef OPTIONAL_PROVIDE_UNDO;
#IfV3;
Message warning "*** Skipping Undo - not supported in v3 ***";
#IfNot;
Constant OPTIONAL_PROVIDE_UNDO_FINAL;
#Endif;
#Endif;


#Ifndef OPTIONAL_NO_DARKNESS;
Attribute light;
#Endif;
Attribute edible;
Attribute absent;
Attribute talkable;
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
Attribute scenery;
Attribute static;
Attribute animate;
Attribute clothing;
Attribute worn;
Attribute pluralname;
Attribute female;
Attribute neuter;
Attribute general;
Attribute switchable;
Attribute on;
Attribute door;
Attribute locked;
Attribute lockable;
Attribute workflag;
Attribute reactive;


! when you order a NPC, but the command isn't understood
! See: http://www.inform-fiction.org/manual/html/s18.html
Fake_Action Order;
Fake_Action Receive;
Fake_Action LetGo;
Fake_Action ThrownAt;
Fake_Action NotUnderstood;
Fake_Action PluralFound;
Fake_Action Going;

Property additive life   $ffff;
Property initial;
Property description;
Property cant_go;
Property number;
Property found_in;         !  For fiddly reasons this can't alias
Property time_left;
Property additive time_out $ffff;
Property short_name 0;
Property additive describe $ffff;
Property additive before $ffff;
Property additive after  $ffff;

Property n_to;  Property s_to; !  Slightly wastefully, these are
Property e_to;  Property w_to; !  (they might be routines)
#IfDef OPTIONAL_FULL_DIRECTIONS;
! These have to come in this position and order if we HAVE full directions
Property ne_to;
Property nw_to;
Property se_to;
Property sw_to;
#EndIf;
Property u_to;  Property d_to;
Property in_to; Property out_to;

#Ifndef OPTIONAL_FULL_DIRECTIONS;
! These have to come outside the other direction props if we DON'T HAVE full directions
Property ne_to;
Property nw_to;
Property se_to;
Property sw_to;
#EndIf;

Constant N_TO_CONST = n_to;
Constant OUT_TO_CONST = out_to;

Property additive each_turn $ffff;

Property door_to            alias n_to;     !  For economy: these properties
Property when_closed        alias s_to;     !  are used only by objects which
Property when_off           alias s_to;     !  aren't rooms
Property with_key           alias e_to;
Property door_dir           alias w_to;
Property orders             alias ne_to;
Property capacity           alias nw_to;
Property invent             alias se_to;
Property inside_description alias sw_to;
Property react_before       alias u_to;
Property react_after        alias d_to;
Property add_to_scope       alias in_to;
Property parse_name         alias out_to;
Property when_open          alias initial;
Property when_on            alias initial;
Property daemon             alias time_out;
Property article            alias cant_go;

Constant FAKE_N_OBJ = 10001;
Constant FAKE_S_OBJ = 10002;
Constant FAKE_E_OBJ = 10003;
Constant FAKE_W_OBJ = 10004;

#IfDef OPTIONAL_FULL_DIRECTIONS;

Constant FAKE_NE_OBJ = 10005;
Constant FAKE_NW_OBJ = 10006;
Constant FAKE_SE_OBJ = 10007;
Constant FAKE_SW_OBJ = 10008;
Constant FAKE_U_OBJ = 10009;
Constant FAKE_D_OBJ = 10010;
Constant FAKE_IN_OBJ = 10011;
Constant FAKE_OUT_OBJ = 10012;
Array abbr_direction_array static table 'n//' 's//' 'e//' 'w//' 'ne' 'nw' 'se' 'sw' 'u//' 'd//' 0 0;
Array full_direction_array static table 'north' 'south' 'east' 'west' 'northeast' 'northwest' 'southeast' 'southwest' 'up' 'down' 'in' 'out';
Array direction_properties_array static table n_to s_to e_to w_to ne_to nw_to se_to sw_to u_to d_to in_to out_to;
Array direction_name_array static table "north" "south" "east" "west" "northeast" "northwest" "southeast" "southwest" "up" "down" "in" "out";
#Ifdef OPTIONAL_SHIP_DIRECTIONS;
Array abbr_ship_direction_array static table 'f//' 'a//' 'sb' 'p//' 0 0 0 0 'u//' 'd//' 0 0;
Array full_ship_direction_array static table 'fore' 'aft' 'starboard' 'port' 0 0 0 0 'up' 'down' 'in' 'out';
#IfV3;
! These arrays say the position of the first and last direction which has 1, 2, 3, 4, 5 and 6+ letters respectively
Array _dir_start static -> 0  1  3  2  1 1 3;
Array _dir_end static ->   0 10 11 12 10 2 8;
#EndIf; ! V3
#IfNot; ! not OPTIONAL_SHIP_DIRECTIONS
#IfV3;
! These arrays say the position of the first and last direction which has 1, 2, 3, 4, 5 and 6+ letters respectively
Array _dir_start static -> 0  1  5 12  3 1 5;
Array _dir_end static ->   0 10 11 12 10 2 8;
#EndIf; ! V3
#EndIf; !If not OPTIONAL_SHIP_DIRECTIONS
Constant DIRECTION_COUNT = 12;

#IfNot; ! not OPTIONAL_FULL_DIRECTIONS

Constant FAKE_U_OBJ = 10005;
Constant FAKE_D_OBJ = 10006;
Constant FAKE_IN_OBJ = 10007;
Constant FAKE_OUT_OBJ = 10008;
Array abbr_direction_array static table 'n//' 's//' 'e//' 'w//' 'u//' 'd//' 0 0;
Array full_direction_array static table 'north' 'south' 'east' 'west' 'up' 'down' 'in' 'out';
Array direction_properties_array static table n_to s_to e_to w_to u_to d_to in_to out_to;
Array direction_name_array static table "north" "south" "east" "west" "up" "down" "in" "out";
#Ifdef OPTIONAL_SHIP_DIRECTIONS;
Array abbr_ship_direction_array static table 'f//' 'a//' 'sb' 'p//' 'u//' 'd//' 0 0;
Array full_ship_direction_array static table 'fore' 'aft' 'starboard' 'port' 'up' 'down' 'in' 'out';
#IfV3;
! These arrays say the position of the first and last direction which has 1, 2, 3, 4, 5 and 6+ letters respectively
Array _dir_start static -> 0 1 3 2 1 1 3;
Array _dir_end static ->   0 6 7 8 6 2 3;
#EndIf; ! V3
#IfNot; ! not OPTIONAL_SHIP_DIRECTIONS
#IfV3;
! These arrays say the position of the first and last direction which has 1, 2, 3, 4, 5 and 6+ letters respectively
Array _dir_start static -> 0 1 5 8 3 1 0;
Array _dir_end static ->   0 6 7 8 6 2 0;
#EndIf; ! V3
#EndIf; ! not OPTIONAL_SHIP_DIRECTIONS
Constant DIRECTION_COUNT = 8;

#EndIf; ! not OPTIONAL_FULL_DIRECTIONS

#IfDef OPTIONAL_ALLOW_WRITTEN_NUMBERS;
Array LanguageNumbers static table
    'one' 1 'two' 2 'three' 3 'four' 4 'five' 5
    'six' 6 'seven' 7 'eight' 8 'nine' 9 'ten' 10
    'eleven' 11 'twelve' 12 'thirteen' 13 'fourteen' 14 'fifteen' 15
    'sixteen' 16 'seventeen' 17 'eighteen' 18 'nineteen' 19 'twenty' 20;
#EndIf;


! Header constants
Constant HDR_DICTIONARY      = $8;   ! Actual address
Constant HDR_STATIC_MEM      = $e;   ! Actual address
Constant HDR_SCREENHCHARS    = $20;
Constant HDR_SCREENWCHARS    = $21;
Constant HDR_FLAGS2          = $10;  ! transcript, undo etc.

Constant TT_OBJECT           = 1;    ! one or more words referring to an object
                                     ! it is one of NOUN_TOKEN etc. below
Constant TT_PREPOSITION      = 2;    ! e.g. 'into'
Constant TT_ROUTINE_FILTER   = 3;    ! e.g. noun=CagedCreature
Constant TT_ATTR_FILTER      = 4;    ! e.g. edible
Constant TT_SCOPE            = 5;    ! e.g. scope=Spells
Constant TT_PARSE_ROUTINE    = 6;    ! a parse routine
Constant TT_END              = 15;   ! End of grammar line

Constant NOUN_OBJECT         = 0;    ! The elementary grammar tokens, and
Constant HELD_OBJECT         = 1;    ! the numbers compiled by Inform to
Constant MULTI_OBJECT        = 2;    ! encode them
Constant MULTIHELD_OBJECT    = 3;
Constant MULTIEXCEPT_OBJECT  = 4;
Constant MULTIINSIDE_OBJECT  = 5;
Constant CREATURE_OBJECT     = 6;
Constant SPECIAL_OBJECT      = 7;
Constant NUMBER_OBJECT       = 8;
Constant TOPIC_OBJECT        = 9;

Constant GPR_FAIL           = -1;   ! Return values from General Parsing
Constant GPR_PREPOSITION    = 0;    ! Routines
Constant GPR_NUMBER         = 1;
Constant GPR_MULTIPLE       = 2;
Constant GPR_REPARSE        = 10000;

Constant TOKEN_SINGLE_PREP   = $42; ! 66
Constant TOKEN_FIRST_PREP    = $62; ! 98
Constant TOKEN_MIDDLE_PREP   = $72; ! 114
Constant TOKEN_LAST_PREP     = $52; ! 82

Constant GS_QUIT             = -1;
Constant GS_PLAYING          = 0;
Constant GS_DEAD             = 1;
Constant GS_WIN              = 2;
Constant GS_DEATHMESSAGE     = 3;

Constant FORM_CDEF           = 1;
Constant FORM_DEF            = 2;
Constant FORM_INDEF          = 3;

#IfV3;
Constant DICT_BYTES_FOR_WORD = 4;
#IfNot;
Constant DICT_BYTES_FOR_WORD = 6;
Constant MOVES__TX = " Moves: ";
Constant SCORE__TX = " Score: ";
Constant TIME__TX = " Time: ";
#EndIf;

#Default Story        0;
#Default Headline     0;

Default MAX_SCORE           0;
Default AMUSING_PROVIDED    1;
Default MAX_CARRIED        32;

#IfDef INITIAL_LOCATION_VALUE;
Global location = INITIAL_LOCATION_VALUE;		! Must be the first global to show location name
#IfNot;
	#Ifdef OPTIONAL_NO_DARKNESS;
	Global location = Directions;						! Must be the first global to show location name
	#IfNot;
	Global location = thedark;						! Must be the first global to show location name
	#EndIf;
#EndIf;

#Ifdef NO_SCORE;
Global status_field_1 = NO_SCORE; ! Must be the second global to show score or hours
#Ifnot;
Global status_field_1 = 0; ! Must be the second global to show score or hours
#Endif;

Global status_field_2 = 0; ! Must be the third global to show turns or minutes
Global real_location;
#Ifndef NO_SCORE;
Global score;
#Endif;
Global undo_flag;
Global notify_mode = true;          ! Score notification
#Ifndef sys_statusline_flag;
Global sys_statusline_flag = 0;     ! non-zero if status line displays time
#Endif;
Global turns;                       ! Number of turns of play so far
Global the_time = 0;                ! Current time (in minutes since midnight)
Global time_rate = 1;               ! How often time is updated
Global time_step;                   ! By how much
Global lookmode = 1;
Global player;
Global actor;
Global wn;               ! word number within parse array
Global pattern_pointer;  ! token within current pattern
Global num_words;        ! number of words typed
Global action;           ! the current action
Global action_reverse;   ! if parameters are in reversed order
Global meta;             ! if the verb has the meta attribute or not
Global update_moved;     ! if _NoteObjectAcquisitions should update moved
Global verb_word;        ! verb word, eg 'take' in "take all"
Global verb_wordnum;     ! the position of the verb in the current sentence
Global consult_from;     ! Word that a "consult" topic starts on
Global consult_words;    ! ...and number of words in topic
Global num_words_parsed; ! Number of words successfully parsed
Global special_word;     ! Dictionary address for "special" token
Global special_number;   ! Number typed special orders, e.g "john, 34"
Global parsed_number;    ! For user-supplied parsing routines
Global noun_filter;      ! For noun filtering by user routines
Global noun;
Global second;
Global selected_direction;
Global selected_direction_index;
Global inp1;             ! the same as noun, except when noun is a number
Global inp2;             ! the same as second, except when second is a number
Global num_noun_groups;  ! how many noun groups the current sentence contains
Global parser_action;    ! needed for parse_name, ##PluralFound (see DM)
Global parser_check_multiple;    ! parser should check if multiheld/multiinside
Global parser_unknown_noun_found;! parser should report unknown word
Global parser_all_found;! parser encountered 'all'
Global parser_all_except_object; ! used to filter all but/except patterns
Global deadflag;
Global scope_modified;   ! true if daemons, each_turn etc has invalidated scope
Global scope_objects;
Global scope_routine;    ! for scope=Routine grammar
Global scope_stage;      ! for scope=Routine grammar: 1, 2 then 3
Global keep_silent;
Global itobj = 0;        ! The object which is currently "it"
Global himobj = 0;       ! The object which is currently "him"
Global herobj = 0;       ! The object which is currently "her"
Global themobj = 0;       ! The object which is currently "them"
Global top_object;
Global newline_flag;     ! Used by Look
Global also_flag;        ! Used by Look
Global inventory_stage;
Global phase2_necessary;
Global receive_action;
Global scope_copy_actor = 0;

#IfDef DEBUG;
Global dict_start;
Global dict_entry_size;
Global dict_end;
Global debug_flag = 0;
#EndIf;

#IfDef DEBUG_TIMER;
Global timer1 = 0;
Global timer2 = 0;
#EndIf;
#IfV5;
Global statusline_current_height = 0;
Global statusline_height     = 1;
Global statuswin_current     = false;
Global clr_on                = false;
Global clr_bg                = 2;
Global clr_fg                = 8;
Global clr_bgstatus          = 2;
Global clr_fgstatus          = 8;
#endif;

Global visibility_ceiling;

Global normal_directions_enabled = true;
#Ifdef OPTIONAL_SHIP_DIRECTIONS;
Global ship_directions_enabled = true;
#EndIf;

Global _g_check_visible = 0;
Global _g_check_take = 0;
Global _g_item = 0;

Default MAX_WHICH_OBJECTS     10;
Default MAX_MULTIPLE_OBJECTS  32;
Default MAX_INPUT_CHARS       78;
Default MAX_INPUT_WORDS       20;
Default MAX_SCOPE             32;

Global scope_pov;        ! Whose POV the scope is from (usually the player)
Array scope-->MAX_SCOPE; ! objects visible from the current POV
Array scope_copy-->MAX_SCOPE; ! Used to hold a copy of a scope list, for iteration

Constant WORD_HIGHBIT = $8000;
#Ifndef MAX_TIMERS;
Constant MAX_TIMERS  32;            ! Max number timers/daemons active at once
#Endif; ! MAX_TIMERS
Array  the_timers --> MAX_TIMERS;
Global active_timers;               ! Number of timers/daemons active
Global current_timer;               ! Index of the timer which is currently being executed

Global PrintMsg = _PrintMsg;        ! Using a global for this saves one byte per call

#Ifndef MAX_FLOATING_OBJECTS;
Constant MAX_FLOATING_OBJECTS  32;            ! Max number of objects that have found_in property
#Endif; ! MAX_FLOATING_OBJECTS
Array floating_objects --> MAX_FLOATING_OBJECTS + 1;

Array which_object-->MAX_WHICH_OBJECTS;       ! options for "which book?"
Array which_level-->MAX_WHICH_OBJECTS;        ! type of which_object
Array multiple_objects-->MAX_MULTIPLE_OBJECTS;! holds nouns when multi* used

Array buffer->(MAX_INPUT_CHARS + 3);
Array parse->(2 + 4 * (MAX_INPUT_WORDS + 1)); ! + 1 to make room for an extra word which is set to 0

! extra arrays to be able to ask for additional info (do you mean X or Y?)
Array buffer2->(MAX_INPUT_CHARS + 3);
Array parse2->(2 + 4 * (MAX_INPUT_WORDS + 1));
Array parse3->(2 + 4 * (MAX_INPUT_WORDS + 1));

Constant RTE_MINIMUM = 0;
Constant RTE_NORMAL  = 1;
Constant RTE_VERBOSE = 2;

#IfnDef RUNTIME_ERRORS;
#IfDef DEBUG;
Constant RUNTIME_ERRORS RTE_VERBOSE;
#IfNot;
Constant RUNTIME_ERRORS RTE_NORMAL;
#EndIf;
#EndIf;

#IfDef OPTIONAL_SCORED;
Attribute scored;
Default OBJECT_SCORE        4;
Default ROOM_SCORE          5;
#IfDef OPTIONAL_FULL_SCORE;
Global things_score;
Global places_score;
#Endif;
#Endif;

#IfDef OPTIONAL_FULL_SCORE;
#IfDef TASKS_PROVIDED;
Default NUMBER_TASKS        1;
Array  task_done -> NUMBER_TASKS;
#Ifndef task_scores;
Array task_scores -> 1;
#Endif;
#Endif;
#Endif;

Object Directions
	with
		description "A look in that direction reveals nothing new.",
		short_name [;
			if(selected_direction_index)
				print (string) direction_name_array-->selected_direction_index;
			else
				print "direction";
			rtrue;
		],
#IfV5;
		parse_name [_parse _len _i _w _arr;
#IfNot;
		parse_name [_parse _len _i _w _w1 _w2;
#EndIf;
			_parse = parse+4*wn-2;
			_w = _parse-->0;
			if(_w == 'floor' or 'ground') {
#IfDef OPTIONAL_FULL_DIRECTIONS;
				selected_direction_index = 10;
#IfNot;
				selected_direction_index = 6;
#EndIf;
        		jump match2;
			}

#IfV5;
			_len = DIRECTION_COUNT;
			_arr = abbr_direction_array + 2;
			if(normal_directions_enabled) {
				@scan_table _w _arr _len -> _i ?success;
				! not found, try full
				_arr = full_direction_array + 2;
				@scan_table _w _arr _len -> _i ?success;
			}
#IfDef OPTIONAL_SHIP_DIRECTIONS;
			if(ship_directions_enabled) {
				! not found, try abbreviated ship directions
				_arr = abbr_ship_direction_array + 2;
				@scan_table _w _arr _len -> _i ?success;
				! not found, try full ship directions
				_arr = full_ship_direction_array + 2;
				@scan_table _w _arr _len -> _i ?success;
			}
#EndIf;
			! no match
			selected_direction_index = 0;
			selected_direction = 0;
			return 0;
.success;
			selected_direction_index = _i - _arr + 2;
			@log_shift selected_direction_index (-1) -> selected_direction_index; ! Divide by 2
.match2;
			selected_direction = direction_properties_array --> selected_direction_index;
			return 1;
#IfNot;
			! This is V3
#IfnDef OPTIONAL_SHIP_DIRECTIONS;
			if(normal_directions_enabled == 0)
				jump fail;
#EndIf;
			_w1 = _parse->2; ! length of typed word
			if(_w1 > 6)
				_w1 = 6;
			_i = _dir_start->_w1;
			if(_i > 0) {
				_len = _dir_end->_w1;
!				print "Testing from ", _i, " to ", _len, "^";
!				for(_i = 1 : _i <= _len : _i++) {


.checkNextDir;
#IfDef OPTIONAL_SHIP_DIRECTIONS;
				if(normal_directions_enabled) {
					@loadw abbr_direction_array _i -> _w1;
					@loadw full_direction_array _i -> _w2;
					@je _w _w1 _w2 ?match;
				}
				if(ship_directions_enabled) {
					@loadw abbr_ship_direction_array _i -> _w1;
					@loadw full_ship_direction_array _i -> _w2;
					@je _w _w1 _w2 ?match;
				}
#IfNot;
				! No ship directions, and we know normal_directions_enabled==1
				@loadw abbr_direction_array _i -> _w1;
				@loadw full_direction_array _i -> _w2;
				@je _w _w1 _w2 ?match;
#EndIf;
				@inc_chk _i _len ?~checkNextDir;
                jump fail;
.match;
    			selected_direction_index = _i;
.match2;
    			selected_direction = direction_properties_array --> selected_direction_index;
    			return 1;
			}
.fail;
      		! No direction was matched
			selected_direction_index = 0;
			selected_direction = 0;
			return 0;
#EndIf;
		]
has scenery proper
#Ifdef OPTIONAL_REACTIVE_PARSE_NAME;
		reactive
#Endif;
;
