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
! use the value 1 in that position.
! 
! Note: If you want to use this library extension is a Z-code version 3 game, 
! you must NOT declare cheap_scenery as a common property, or it will only be 
! able to hold one scenery object instead of ten.
!
! If you want to use the same description for a scenery object in several locations,
! declare a constant to hold that string, and refer to the constant in each location.
!
! Before including this extension, you can also define a string or routine called 
! SceneryReply. If you do, it will be used whenever the player does something to a 
! scenery object other than examining it. If it's a string, it's printed. If it's a
! routine it's called. If the routine prints something, it should return true, 
! otherwise false. 
!
! Example usage:

! [SceneryReply;
!   Push:
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
!      'bird' 'birds' "They seem so careless."
!      1 'sun' SCN_SUN,
!   has light;


System_file;

Object CheapScenery "object"
	with
		article "an",
		number 0,
		parse_name [ w1 w2 i sw1 sw2 len;
			w1 = NextWordStopped();
			w2 = NextWordStopped();
			i = 0;
			len = location.#cheap_scenery / 2;
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
			if(len % 3 > 0)
#IfTrue RUNTIME_ERRORS == RTE_VERBOSE;
				"ERROR: cheap_scenery property of current location has incorrect # of values!^";
#IfNot;
				"cheap_scenery error 1!^";
#EndIf;
			while(i < len) {
				sw1 = location.&cheap_scenery-->(i+2);
				if(~~(sw1 ofclass String or Routine))
#IfTrue RUNTIME_ERRORS == RTE_VERBOSE;
					"ERROR: Element ",i+2," in cheap_scenery property of current location is not a string or routine!^",
						"Element: ", (name) sw1, "^";
#IfNot;
					"cheap_scenery error 2!^";
#EndIf;

				i = i + 3;
			}
			i = 0;
#endif;
			while(i < len) {
				sw1 = location.&cheap_scenery-->i;
				sw2 = location.&cheap_scenery-->(i+1);
				if(w1 == sw1 && w2 == sw2) {
					self.number = i;
					return 2;
				}
				if(w1 == sw1 or sw2) {
					self.number = i;
					return 1;
				}
				i = i + 3;
			}
			! It would make sense to return 0 here, but property
			! routines return 0 by default anyway.
		],
		description [k;
			k = location.&cheap_scenery-->(self.number + 2);
			if(k ofclass Routine) {
				k();
				rtrue;
			}
			print_ret (string) k;
		],
		before [;
			Examine, Search:
				rfalse;
			default:
				#ifdef SceneryReply;
				if(SceneryReply ofclass string)
					print_ret (string) SceneryReply;
				if(SceneryReply())
					rtrue;
				#endif;
				"Get a grip on yourself.";
		],
		found_in [;
			if(location provides cheap_scenery) rtrue;
		],
	has concealed scenery;

	