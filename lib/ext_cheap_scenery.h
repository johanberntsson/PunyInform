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
! use the value 1 in that position. You can also give 2 as the adjective value
! and give a routine which will act as a parse_name routine in the noun
! position.
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
!      1 'sun' SCN_SUN
!      2 [ i; while(NextWord() == 'huge' or 'car' or 'park') i++; return i;] "Nice!",
!
!   has light;


System_file;

#Ifndef RUNTIME_ERRORS;
Constant RUNTIME_ERRORS = 2;
#Endif;
#Ifndef RTE_MINIMUM;
Constant RTE_MINIMUM = 0;
Constant RTE_NORMAL = 1;
Constant RTE_VERBOSE = 2;
#Endif;

Object CheapScenery "object"
	with
		article "an",
		number 0,
		parse_name [ _w1 _w2 _i _sw1 _sw2 _len _base_wn _ret;
			_base_wn = wn;
			_w1 = NextWordStopped();
			_w2 = NextWordStopped();
			_len = location.#cheap_scenery / 2;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
			if(_len % 3 > 0) {
	#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
				"ERROR: cheap_scenery property of current location has
					incorrect # of values!^";
	#Ifnot;
				"ERROR: cheap_scenery #1!^";
	#Endif;
			}
#Endif;
			while(_i < _len) {
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
				if(metaclass(location.&cheap_scenery-->(_i+2)) ~= String or Routine) {
	#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
					"ERROR: Element ", _i+2, " in cheap_scenery property of
						current location is not a string or routine!^";
	#Ifnot;
					"ERROR: cheap_scenery #2!^";
	#Endif;
				}
#Endif;
				_sw1 = location.&cheap_scenery-->_i;
				_sw2 = location.&cheap_scenery-->(_i+1);
				if(_sw1 == 2) {
					wn = _base_wn;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
					if(metaclass(_sw2) ~= Routine) {
	#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
						"ERROR: Element ", _i+1, " in cheap_scenery property of
							current location should be a parse_name routine but
							isn't!^";
	#Ifnot;
						"ERROR: cheap_scenery #3!^";
	#Endif;
					}
#Endif;
					_ret = _sw2();
					if(_ret > 0) {
						self.number = _i;
						return _ret;
					}
				} else {
					if(_w1 == _sw1 && _w2 == _sw2) {
						self.number = _i;
						return 2;
					}
					if(_w1 == _sw1 or _sw2) {
						self.number = _i;
						return 1;
					}
				}
				_i = _i + 3;
			}
			! It would make sense to return 0 here, but property
			! routines return 0 by default anyway.
		],
		description [ _k;
			_k = location.&cheap_scenery-->(self.number + 2);
			if(_k ofclass Routine) {
				_k();
				rtrue;
			}
			print_ret (string) _k;
		],
#Ifdef SceneryReply;
		before [i w1;
#Ifnot;
		before [;
#Endif;
			Examine:
				rfalse;
			default:
				#ifdef SceneryReply;
				if(SceneryReply ofclass string)
					print_ret (string) SceneryReply;
				i = location.&cheap_scenery;
				w1 = self.number;
				if(SceneryReply(i-->w1, i-->(w1 + 1)))
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
