! ext_cheap_scenery.h, a library extension for PunyInform by Fredrik Ramsberg
!
! This library extension provides a way to implement simple scenery objects
! which can only be examined, using just a single object for the entire game.
! This helps keep both the object count and the dynamic memory usage down.
!
! To use it, include this file after globals.h. Then add a property called
! cheap_scenery to the locations where you want to add cheap scenery objects.
! You can add up to ten cheap scenery objects to one location in this way. For
! each scenery object, specify, in this order, one adjective, one noun, and one
! description string or a routine to print one. Instead of an adjective, you
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
! If you want to use the same description for a scenery object in several
! locations, declare a constant to hold that string, and refer to the constant
! in each location.
!
! Before including this extension, you can also define a string or routine
! called SceneryReply. If you do, it will be used whenever the player does
! something to a scenery object other than examining it. If it's a string, it's
! printed. If it's a routine it's called. If the routine prints something, it
! should return true, otherwise false. The routine is called with two
! parameters - the adjective/synonym and the noun listed in the cheap_scenery
! property which was matched.

!
! Example usage:

! [SceneryReply word1 word2 ;
!   Push:
!     if(location == RiverBank && word2 == 'water')
!         "If you mean you want to swim, you should say so."
!     "Now how would you do that?";
!   default:
!     rfalse;
! ];
!
! Include "ext_cheap_scenery.h";
!
! Constant SCN_WATER = "The water is so beautiful this time of year, all clear and glittering.";
! [SCN_SUN;
!   deadflag = 1;
!   "As you stare right into the sun, you feel a burning sensation in your eyes.
!     After a while, all goes black. With no eyesight, you have little hope of
!     completing your investigations.";
! ];
!
! Object RiverBank "River Bank"
!   with
!	 description "The river is quite wide here. The sun reflects in the blue water, the birds are
!      flying high up above.",
!	 cheap_scenery
!      'blue' 'water' SCN_WATER
!      'bird' 'birds' "They seem so carefree."
!      CS_NO_ADJ 'sun' SCN_SUN
!      CS_PARSE_NAME [ i; while(NextWord() == 'huge' or 'car' or 'park') i++; return i;] "Nice!"
!      'soil' 'dirt' "The soil is surprisingly dry here!",
!      CS_ADD_LIST RiverBank outside_scenery,
!    outside_scenery
!      CS_NO_ADJ 'air' "The air is fresh here"
!      'soil' 'dirt' "The soil is damp after yesterday's rain.",
!
!   has light;


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

Array CSData --> 5;
Constant CSDATA_OBJ = 0;
Constant CSDATA_PROP = 1;
Constant CSDATA_INDEX = 2;
Constant CSDATA_WORD_1 = 3;
Constant CSDATA_WORD_2 = 4;
!  CSData-->0: The object which holds list where we found a match
!  CSData-->1: The property where the list is stored
!  CSData-->2: The index into the list
!  CSData-->3: Word 1 in player input
!  CSData-->4: Word 2 in player input (may not have matched anything)

#Ifndef cheap_scenery;
Property individual cheap_scenery;
#Endif;

[ _ParseCheapScenery p_obj p_prop p_base_wn _w1 _w2 _i _sw1 _sw2 _len _ret _arr;
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
		description [ _k;
			_k = (CSData-->CSDATA_OBJ).&(CSData-->CSDATA_PROP)-->((CSData-->CSDATA_INDEX) + 2);
			if(_k ofclass Routine) {
				self = location;
				_k();
				rtrue;
			}
			print_ret (string) _k;
		],
#Ifdef SceneryReply;
		before [_i _w1;
#Ifnot;
		before [;
#Endif;
			Examine:
				rfalse;
			default:
#ifdef SceneryReply;
				if(SceneryReply ofclass string)
					print_ret (string) SceneryReply;
				_i = (CSData-->CSDATA_OBJ).&(CSData-->CSDATA_PROP);
				_w1 = CSData-->CSDATA_INDEX;
				if(SceneryReply(_i-->_w1, _i-->(_w1 + 1)))
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
