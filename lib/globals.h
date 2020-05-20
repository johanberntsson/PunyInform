! Part of PunyInform, a standard library for writing interactive fiction using Inform 6.
!
! CONSTANTS that can be set in the game to turn parts of PunyInform
! on or off (to save memory).
!
! FULL_DIRECTIONS: to allow ne, se, sw, and nw as directions
! ALLOW_WRITTEN_NUMBERS: to be able to parse one, two etc as numbers
! DEBUG_VERBS: enable some debuggin verbs for game development
!
#IfnDef CUSTOM_ABBREVIATIONS;
Abbreviate ". ";
Abbreviate ", ";
Abbreviate "You ";
Abbreviate "'t ";
Abbreviate "thing";
Abbreviate "_to/";
Abbreviate "_array";
Abbreviate "There is ";
Abbreviate "ection";
Abbreviate "Tha";
Abbreviate "lready";
Abbreviate "to ";
Abbreviate "can";
Abbreviate "'s ";
Abbreviate "tha";
Abbreviate "the ";
Abbreviate "re you ";
Abbreviate "ing ";
Abbreviate "you ";
Abbreviate "the";
Abbreviate "rea";
Abbreviate "nderstand ";
Abbreviate "The";
Abbreviate "re ";
Abbreviate "not ";
Abbreviate "ope";
Abbreviate "I d";
Abbreviate "ve ";
Abbreviate "this";
Abbreviate "ome";
Abbreviate "/when_";
Abbreviate "ut ";
Abbreviate "numbe";
Abbreviate "which";
Abbreviate "witch";
Abbreviate "ould ";
Abbreviate "objec";
Abbreviate "in ";
Abbreviate "rror: ";
Abbreviate "ine";
Abbreviate "ly ";
Abbreviate "ble";
Abbreviate "name";
Abbreviate "pla";
Abbreviate "rse";
Abbreviate "on ";
Abbreviate "res";
Abbreviate "los";
Abbreviate "of ";
Abbreviate "her";
Abbreviate "is ";
Abbreviate "hat";
Abbreviate "you";
Abbreviate "ected.";
Abbreviate "aren";
Abbreviate "_to";
Abbreviate "ing";
Abbreviate "est";
Abbreviate "for";
Abbreviate "see ";
Abbreviate "ed ";
Abbreviate "pen";
Abbreviate "int";
Abbreviate "as ";
#EndIf;

Constant Grammar__Version = 2;
Constant INDIV_PROP_START 64;
Constant NULL         = $ffff;

Constant PHASE1 = 1; ! just check if pattern matches (no side effects such as which? questions or indirect actions)
Constant PHASE2 = 2; ! parse the given pattern with side effects


!Constant WORDSIZE 2; ! set by the compiler from Inform 6.30

Constant ALL_WORD     = 'all';
Constant AND_WORD     = 'and';
Constant THEN_WORD    = 'then';
Constant comma_word   = 'comma,';  ! An "untypeable word" used to substitute
                                   ! for commas in parse buffers
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

! when you order a NPC, but the command isn't understood
! See: http://www.inform-fiction.org/manual/html/s18.html
Fake_Action Order; 
Fake_Action Receive;
Fake_Action ThrownAt;
Fake_Action NotUnderstood; 
Fake_Action PluralFound; 

Property additive before $ffff;
Property additive after  $ffff;
Property additive life   $ffff;

Property long n_to;  Property long s_to; !  Slightly wastefully, these are
Property long e_to;  Property long w_to; !  long (they might be routines)
Property long ne_to; Property long se_to;
Property long nw_to; Property long sw_to;
Property long u_to;  Property long d_to;
Property long in_to; Property long out_to;

Property door_to     alias n_to;     !  For economy: these properties are
Property when_closed alias s_to;     !  used only by objects which
Property with_key    alias e_to;     !  aren't rooms
Property door_dir    alias w_to;
Property invent      alias u_to;
Property add_to_scope alias se_to;
Property list_together alias sw_to;
Property react_before alias ne_to;
Property react_after  alias nw_to;
Property grammar     alias in_to;
Property orders      alias out_to;

Property long initial;
Property when_open   alias initial;
Property when_on     alias initial;
Property when_off    alias when_closed;
Property inside_description alias d_to;
Property long description;
Property additive describe $ffff;

Property cant_go;
Property article alias cant_go;

Property long found_in;         !  For fiddly reasons this can't alias

Property long time_left;
Property long number;
Property additive time_out $ffff;
Property daemon alias time_out;
Property additive each_turn $ffff;

Property capacity 100;

Property long short_name 0;
Property long parse_name 0;


! ! directions
! Property n_to;
! Property s_to;
! Property e_to;
! Property w_to;
! #IfDef FULL_DIRECTIONS;
! Property ne_to;
! Property nw_to;
! Property se_to;
! Property sw_to;
! #EndIf;
! Property u_to;
! Property d_to;
! Property in_to;
! Property out_to;

! Property cant_go;
! Property door_to alias n_to;
! Property door_dir alias s_to;
Constant FULL_DIRECTIONS;

Constant FAKE_N_OBJ = 10001;
Constant FAKE_S_OBJ = 10002;
Constant FAKE_E_OBJ = 10003;
Constant FAKE_W_OBJ = 10004;
#IfDef FULL_DIRECTIONS;
Constant FAKE_NE_OBJ = 10005;
Constant FAKE_NW_OBJ = 10006;
Constant FAKE_SE_OBJ = 10007;
Constant FAKE_SW_OBJ = 10008;
Constant FAKE_U_OBJ = 10009;
Constant FAKE_D_OBJ = 10010;
Constant FAKE_IN_OBJ = 10011;
Constant FAKE_OUT_OBJ = 10012;
Array abbr_direction_array table 'n//' 's//' 'e//' 'w//' 'ne' 'nw' 'se' 'sw' 'u//' 'd//' ',,' ',,';
Array full_direction_array table 'north' 'south' 'east' 'west' 'northeast' 'northwest' 'southeast' 'southwest' 'up' 'down' 'in' 'out';
Array direction_properties_array table n_to s_to e_to w_to ne_to nw_to se_to sw_to u_to d_to in_to out_to;
Array direction_name_array table "north" "south" "east" "west" "northeast" "northwest" "southeast" "southwest" "up" "down" "in" "out";
#IfNot;
Constant FAKE_U_OBJ = 10005;
Constant FAKE_D_OBJ = 10006;
Constant FAKE_IN_OBJ = 10007;
Constant FAKE_OUT_OBJ = 10008;
Array abbr_direction_array table 'n//' 's//' 'e//' 'w//' 'u//' 'd//' ',,' ',,';
Array full_direction_array table 'north' 'south' 'east' 'west' 'up' 'down' 'in' 'out';
Array direction_properties_array table n_to s_to e_to w_to u_to d_to in_to out_to;
Array direction_name_array table "north" "south" "east" "west" "up" "down" "in" "out";
#EndIf;

! ! Property name; ! 'name' seems to be hardcoded in the Inform compiler
! Property initial;
! Property describe alias e_to;
! Property description;
! Property short_name;
! Property add_to_scope;
! Property react_after;
! Property react_before;
! Property after;
! Property before;
! Property parse_name;
! Property capacity;
! Property article;
! Property found_in;

! ! Daemons and timers

! Property additive time_out NULL;
! Property daemon alias time_out;
! Property time_left;

#IfDef ALLOW_WRITTEN_NUMBERS;
Array LanguageNumbers table
    'one' 1 'two' 2 'three' 3 'four' 4 'five' 5
    'six' 6 'seven' 7 'eight' 8 'nine' 9 'ten' 10
    'eleven' 11 'twelve' 12 'thirteen' 13 'fourteen' 14 'fifteen' 15
    'sixteen' 16 'seventeen' 17 'eighteen' 18 'nineteen' 19 'twenty' 20;
#EndIf;


! Header constants
Constant HEADER_DICTIONARY   = 4;    ! 2*4 = $8
Constant HEADER_STATIC_MEM   = 7;    ! 2*7 = $c

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

! $42 = Single prep, $62 = Beginning of list of alternatives, $72 = middle of list, $52 = end of list
Constant TOKEN_SINGLE_PREP   = $42;
Constant TOKEN_FIRST_PREP    = $62;
Constant TOKEN_MIDDLE_PREP   = $72;
Constant TOKEN_LAST_PREP     = $52;

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
Constant HDR_SCREENHCHARS    = $20;
Constant HDR_SCREENWCHARS    = $21;
Constant MOVES__TX = "Moves: ";
Constant SCORE__TX = "Score: ";
#EndIf;

Default MAX_SCORE           0;
Default NUMBER_TASKS        1;
Default TASKS_PROVIDED      1;
Default AMUSING_PROVIDED    1;
Default MAX_CARRIED         100;
Default SACK_OBJECT         0;

Array  task_done -> NUMBER_TASKS;

Global location = 1; ! Must be first global
Global status_field_1 = 0; ! Must be second global. Is used to show score or hours
Global status_field_2 = 0; ! Must be third global. Is used to show turns or minutes
Global score;
Global turns;
Global darkness;
Global player;
Global actor;
Global wn;               ! word number within parse
Global action;           ! the current action
Global action_debug;     ! action is one of the debugging verbs
Global action_reverse;   ! if parameters are in reversed order
Global meta;             ! if the verb has the meta attribute or not
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
Global deadflag;
Global scope_objects;
Global scope_routine;    ! for scope=Routine grammar
Global scope_stage;      ! for scope=Routine grammar: 1, 2 then 3
Global keep_silent;
Global itobj = 0;        ! The object which is currently "it"
Global himobj = 0;       ! The object which is currently "him"
Global herobj = 0;       ! The object which is currently "her"
Global top_object;
#IfDef DEBUG_TIMER;
Global timer1_start = 0;
Global timer1_stop = 0;
Global timer2_start = 0;
Global timer2_stop = 0;
#EndIf;
Global nouncache_wn;
Global nouncache_result;
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

#ifdef DEBUG;
Global debug_flag            = 0;
#endif;

Constant MAX_WHICH_OBJECTS   = 10;
Constant MAX_MULTIPLE_OBJECTS= 32;
Constant MAX_INPUT_CHARS     = 78;
Constant MAX_INPUT_WORDS     = 20;

Constant WORD_HIGHBIT = $8000;
#Ifndef MAX_TIMERS;
Constant MAX_TIMERS  32;            ! Max number timers/daemons active at once
#Endif; ! MAX_TIMERS
Array  the_timers --> MAX_TIMERS;
Global active_timers;               ! Number of timers/daemons active
Global current_timer;               ! Index of the timer which is currently being executed

#Ifndef MAX_FLOATING_OBJECTS;
Constant MAX_FLOATING_OBJECTS  32;            ! Max number of objects that have found_in property
#Endif; ! MAX_FLOATING_OBJECTS
Array floating_objects --> MAX_FLOATING_OBJECTS + 1;

Array which_object-->MAX_WHICH_OBJECTS;       ! options for "which book?"
Array multiple_objects-->MAX_MULTIPLE_OBJECTS;! holds nouns when multi* used

Array player_input_array->(MAX_INPUT_CHARS + 3);
Array parse_array->(2 + 4 * (MAX_INPUT_WORDS + 1)); ! + 1 to make room for an extra word which is set to 0

! extra arrays to be able to ask for additional info (do you mean X or Y?)
Array temp_player_input_array->(MAX_INPUT_CHARS + 3);
Array temp_parse_array->(2 + 4 * (MAX_INPUT_WORDS + 1)); 

#IfDef DEBUG;
#IfnDef RUNTIME_ERRORS;
Constant RUNTIME_ERRORS;
#EndIf;
#EndIf;

