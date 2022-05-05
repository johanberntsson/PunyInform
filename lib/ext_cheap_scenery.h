! ext_cheap_scenery.h, a library extension for PunyInform by Fredrik Ramsberg
!
! This library extension provides a way to implement simple scenery objects
! which can only be examined, using just a single object for the entire game.
! This helps keep both the object count and the dynamic memory usage down.
!
! To use it, include this file after globals.h. Then add a property called
! cheap_scenery to the locations where you want to add cheap scenery objects.
! You can add up to ten cheap scenery objects to one location in this way. For
! each scenery object, specify, in this order, an adjective, a noun, and a
! reaction string/routine. Instead of an adjective, you
! may give a synonym to the noun. If no adjective or synonym is needed,
! use the value CS_NO_ADJ (=1) in that position. You can also give
! CS_PARSE_NAME (=2) as the adjective value and give a routine which will
! act as a parse_name routine in the noun position. Finally, you can give
! CS_ADD_LIST (=3) as the adjective value and then an object ID and a
! property name, to include the cheap scenery list held in that property in
! the object in the current list.
!
! Note: If you want to use this library extension is a Z-code version 3 game,
! you must NOT declare cheap_scenery as a common property, or it will only be
! able to hold one scenery object instead of ten.
!
! The reaction can be either:
! * a string to be used as the description of the object
! * a routine which will act as a before routine for the object - this can be
!     used to trap the Examine action and print a dynamic description of the
!     object, but also to react to any other actions the player may try to
!     perform on the object.
!
! If you want to use the same description for a scenery object in several
! locations, declare a constant to hold that string, and refer to the constant
! in each location.
!
! Before including this extension, you can also define a string or routine
! called SceneryReply. If you do, it will be used whenever the player does
! something to a scenery object other than examining it. If it's a string, it's
! printed. If it's a routine it's called. If the routine prints something, it
! should return true, otherwise false. The routine is called with three
! parameters - word1, word2 and routine. These hold:
! * If the cheap scenery object was matched using a parse_name routine, and this
!     routine set cs_parse_name_id = n, then word1 = CS_PARSE_NAME, word2 = 0,
!     routine = n. The value n should be in the range 1-600.
! * If the cheap scenery object was matched using a parse_name routine, and this
!     routine did not set cs_parse_name_id, word1 = CS_PARSE_NAME,
!     word2 = 0 and routine = [routine address] (If you use a named routine,
!     the name is a constant equal to the routine address).
! * Otherwise, word1 = adjective, word2 = noun (the values given in the cheap
!     scenery property for the item matched) and routine = 0.
!
! Example usage: (from howto/cheapscenerydemo.inf in PunyInform distribution)

! ! Cheap Scenery Parse Name constants. Use values 1-600.
! Constant CSP_LIBRARY 1;
!
! [ SceneryReply word1 word2 routine;
!     ! We can check location, if we want different answers in different rooms
!     ! We can also check action, and there's even an implicit switch on action,
!     ! so we can do things like: Take: "You're crazy.";
!     switch(routine) {
!     ParseNameAir:
!         "You need the air to breathe, that's all.";
!     CSP_LIBRARY:
!         "The library is super-important. Better not mess with it.";
!     }
!     if(location == Library && word1 == 'book' && word2 == 'books')
!         "Leave the books to the people who care about them.";
!     rfalse;
! ];
!
! Include "ext_cheap_scenery.h";
! Include "puny.h";
!
! [ ParseNameAir;
!     if(NextWord() == 'air') return 1;
!     rfalse;
! ];
!
! [ WallDesc;
!     Examine:
!         "The walls are ",
!             (string) random("all white", "claustrophobia-inducing", "scary",
!                             "shiny"), " here.";
!     default:
!         ! A named routine will return true by default, so this is necessary
!         rfalse;
! ];
!
! Constant BOOKDESC "You're not interested in reading.";
!
! Object Library "The Library"
!     with
!         description "You are in a big lovely library. You can examine or try to
!             take the books, the shelves, the library, the air, the walls and
!             the ceiling.",
!         cheap_scenery
!             CS_ADD_LIST Library inside_scenery
!             'book' 'books' BOOKDESC
!             'shelf' 'shelves' "They're full of books."
!             CS_PARSE_NAME ParseNameAir "The air is oh so thin here."
!             CS_PARSE_NAME [ _i _w;
!                 cs_parse_name_id = CSP_LIBRARY;
!                 _w = NextWord();
!                 if(_w == 'big') { _i++; _w = NextWord();}
!                 if(_w == 'lovely') { _i++; _w = NextWord();}
!                 if(_w == 'library') { _i++; return _i;}
!                 return 0;
!             ] "It's truly glorious.",
!         inside_scenery
!             'wall' 'walls' WallDesc
!             CS_NO_ADJ 'ceiling' "The ceiling is quite high up.",
!     has light;

System_file;

Constant EXT_CHEAP_SCENERY = 1;

#Ifndef RUNTIME_ERRORS;
Constant RUNTIME_ERRORS = 2;
#Endif;
#Ifndef RTE_MINIMUM;
Constant RTE_MINIMUM = 0;
Constant RTE_NORMAL = 1;
Constant RTE_VERBOSE = 2;
#Endif;

Constant CS_NO_ADJ = 1;
Constant CS_PARSE_NAME = 2;
Constant CS_ADD_LIST = 3;

Array CSData --> 6;
Constant CSDATA_OBJ = 0;
Constant CSDATA_PROP = 1;
Constant CSDATA_INDEX = 2;
Constant CSDATA_WORD_1 = 3;
Constant CSDATA_WORD_2 = 4;
Constant CSDATA_PARSE_NAME_ID = 5;
!  CSData-->0: The object which holds list where we found a match
!  CSData-->1: The property where the list is stored
!  CSData-->2: The index into the list
!  CSData-->3: Word 1 in player input
!  CSData-->4: Word 2 in player input (may not have matched anything)
!  CSData-->5: The value of cs_parse_name_id when match was made

#Ifndef cheap_scenery;
Property individual cheap_scenery;
#Endif;

Global cs_parse_name_id = 0;

[ _ParseCheapScenery p_obj p_prop p_base_wn _w1 _w2 _i _sw1 _sw2 _len _ret _arr;
	cs_parse_name_id = 0;
! 	_base_wn = CheapScenery.inside_description;
	_w1 = CSData-->CSDATA_WORD_1;
	_w2 = CSData-->CSDATA_WORD_2;
	_arr = p_obj.&p_prop;
	_len = p_obj.#p_prop / 2;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(_len % 3 > 0) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
		"ERROR: cheap_scenery property of ", (name) p_obj,
			" has incorrect # of values!^";
#Ifnot;
		"ERROR: cheap_scenery #1!^";
#Endif;
	}
#Endif;
	while(_i < _len) {
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
		if(_arr-->_i == CS_ADD_LIST &&
				(_arr-->(_i+1) < 2 || _arr-->(_i+1) > top_object)) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
			"ERROR: Element ", _i+1, " in cheap_scenery property of ", (name) p_obj,
				" is part of a CS_ADD_LIST entry and should be a valid
				object ID but is ", _arr-->(_i+1), "!^" ;
#Ifnot;
			"ERROR: cheap_scenery #2!^";
#Endif;
		}
#Endif;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
		if(_arr-->_i ~= CS_ADD_LIST && metaclass(_arr-->(_i+2)) ~= String or Routine) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
			"ERROR: Element ", _i+2, " in cheap_scenery property of ",
				(name) p_obj, " is not a string or routine!^";
#Ifnot;
			"ERROR: cheap_scenery #3!^";
#Endif;
		}
#Endif;
		_sw1 = _arr-->_i;
		_sw2 = _arr-->(_i+1);
		if(_sw1 == CS_ADD_LIST) {
			_ret = _ParseCheapScenery(_sw2, _arr-->(_i+2), p_base_wn);
			if(_ret)
				return _ret;
		} else if(_sw1 == CS_PARSE_NAME) {
			wn = p_base_wn;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
			if(metaclass(_sw2) ~= Routine) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
				"ERROR: Element ", _i+1, " in cheap_scenery property of
					current location should be a parse_name routine but
					isn't!^";
#Ifnot;
				"ERROR: cheap_scenery #4!^";
#Endif;
			}
#Endif;
			self = location;
			_ret = _sw2();
			if(_ret > 0) {
				jump match;
			}
			cs_parse_name_id = 0;
		} else if(_w1 == _sw1 or _sw2) {
				_ret = 1;
				if(_w1 == _sw1 && _w2 == _sw2) {
					_ret = 2;
				}
				jump match;
		}
		_i = _i + 3;
	}
	return 0;
.match;
	CSData-->CSDATA_OBJ = p_obj;
	CSData-->CSDATA_PROP = p_prop;
	CSData-->CSDATA_INDEX = _i;
	CSData-->CSDATA_PARSE_NAME_ID = cs_parse_name_id;
	return _ret;
];


Object CheapScenery "object"
	with
		article "an",
		parse_name [ _base_wn;
			_base_wn = wn;
			CSData-->CSDATA_WORD_1 = NextWordStopped();
			CSData-->CSDATA_WORD_2 = NextWordStopped();
			return _ParseCheapScenery(location, cheap_scenery, _base_wn);
		],
#Ifdef SceneryReply;
		before [_i _k _w1pos _w1 _w2 _routine;
#Ifnot;
		before [_i _k _w1pos;
#Endif;
			_i = (CSData-->CSDATA_OBJ).&(CSData-->CSDATA_PROP);
			_w1pos = CSData-->CSDATA_INDEX;
			_k = _i-->(_w1pos + 2);
			if(action == ##Examine && _k ofclass String)
				print_ret (string) _k;

			if(_k ofclass Routine) {
				self = location;
				sw__var = action;
				if(_k())
					rtrue;
			}

#ifdef SceneryReply;
			if(SceneryReply ofclass string)
				print_ret (string) SceneryReply;
			_w1 = _i-->_w1pos;
			_w2 = _i-->(_w1pos + 1);
			if(_w1 == CS_PARSE_NAME) {
				_routine = CSData-->CSDATA_PARSE_NAME_ID;
				if(_routine == 0)
					_routine = _w2;
				_w2 = 0;
			}
			if(SceneryReply(_w1, _w2, _routine))
				rtrue;
#endif;
			"No need to concern yourself with that.";
		],
		found_in [;
			if(location provides cheap_scenery) rtrue;
		],
	has concealed scenery
#Ifdef OPTIONAL_REACTIVE_PARSE_NAME;
		reactive
#Endif;
;
