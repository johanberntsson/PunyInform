! ext_cheap_scenery.h, a library extension for PunyInform by Fredrik Ramsberg
!
! This library extension provides a way to implement simple scenery objects
! using just a single object for the entire game. This helps keep both the 
! object count and the dynamic memory usage down. Games are also faster
! when fewer objects are in scope.
!
! To use it, include this file after globals.h. Then add a property called
! cheap_scenery to the locations where you want to add cheap scenery objects.
! You can add any number of cheap scenery objects to one location in this way.
!
! For each scenery object, you provide an entry in the list, typically 
! consisting of two dictionary words (called word1 and word2), and a
! reaction string/routine. This cheap scenery entry will be matched if the 
! player types any combination of word1 and word2
!
! If only one word is needed, use the value 1 for word1. 
!
! There is a more flexible option, which allows you to specify up to nine 
! adjectives and nine nouns: You give a value (10 * adjectives + nouns), 
! followed by the adjectives and nouns, e.g: 
! 21 'small' 'green' 'bug' - this means there are two adjectives and one noun, 
! and this will match "small green bug", "green small bug", "small bug", 
! "green bug" and "bug", but not "small" or "small green" - at least one of 
! the nouns must be used by the player, optionally preceded by one or more of 
! the adjectives.
!
! Alternatively, an entry can start with CS_PARSE_NAME and then a routine 
! which will act as a parse_name routine. 
!
! Optionally, you can precede an entry with CS_THEM to say that this cheap
! scenery object should be considered a "them"-object by the parser. E.g.
! the player can type "EXAMINE CURTAINS.TAKE THEM". Another option is to
! mark some of the words with the plural flag, e.g. 'doors//p'. If a plural
! word is matched, the object is considered a "them"-object. For objects
! with a parse_name routine, the routine can set parser_action = ##PluralFound
! to signal that a plural word was matched.
!
! Additionally, you can start an entry with CS_ADD_LIST and then an object
! ID and a property name, to include the cheap scenery list held in that 
! property in the object.
!
! Finally, you can use the value CS_MAYBE_ADD_LIST, then a function, an 
! object ID and a property name, to say that if the function returns true,
! you want to include the cheap scenery list held in that property in the 
! object.
!
! If multiple cheap scenery objects are matched, the one matching the highest
! number of words in player input is the match that is used. If there's a tie,
! the first one matching this number of words is used.
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
! * Otherwise, word1 and word2 hold the two dictionary words specified for the
!     matched cheap scenery object, and routine = 0.
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
!             CS_ADD_LIST Library (inside_scenery)
!             CS_MAYBE_ADD_LIST [; if(LightSwitch has on) rtrue; ] Library (light_scenery)
!             'book' 'books//p' BOOKDESC
!             'shelf' 'shelves//p' "They're full of books."
!             CS_PARSE_NAME ParseNameAir "The air is oh so thin here."
!             CS_PARSE_NAME [ _i _w;
!                 cs_parse_name_id = CSP_LIBRARY;
!                 _w = NextWord();
!                 if(_w == 'big') { _i++; _w = NextWord();}
!                 if(_w == 'lovely') { _i++; _w = NextWord();}
!                 if(_w == 'library') { _i++; return _i;}
!                 return 0;
!             ] "It's truly glorious.",
!         light_scenery
!             1 'light' "The light is just stunning to watch.",
!         inside_scenery
!             'wall' 'walls//p' WallDesc
!              CS_THEM 'curtain' 'curtains' "The curtains are lovely."
!             1 'ceiling' "The ceiling is quite high up.",
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
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
Constant CS_ERR = "^[Cheap_scenery error #";
#Endif;

#Ifndef CS_DEFAULT_MSG;
Constant CS_DEFAULT_MSG "No need to concern yourself with that.";
#Endif;

Constant CS_NO_ADJ = 1;
Constant CS_PARSE_NAME = 100;
Constant CS_ADD_LIST = 101;
Constant CS_MAYBE_ADD_LIST = 102;

Constant CS_IT = 103;
Constant CS_THEM = 104;

Array CSData --> 7;
Constant CSDATA_OBJ = 0;
Constant CSDATA_PROP = 1;
Constant CSDATA_INDEX = 2;
Constant CSDATA_PARSE_NAME_ID = 3;
Constant CSDATA_MATCH_LENGTH = 4;
Constant CSDATA_PRONOUN = 5;
Constant CSDATA_PRONOUN_TEMP = 6;
!  CSData-->0: The object which holds list where we found a match
!  CSData-->1: The property where the list is stored
!  CSData-->2: The index into the list for the best match
!  CSData-->3: The value of cs_parse_name_id when match was made
!  CSData-->4: The length of the best match
!  CSData-->5: The pronoun for the match (CS_IT or CS_THEM)
!  CSData-->6: Used to pass a pronoun value between routines

#Ifndef cheap_scenery;
Property individual cheap_scenery;
#Endif;

Global cs_parse_name_id = 0;

[ _CSGetArr;
	return (CSData-->CSDATA_OBJ).&(CSData-->CSDATA_PROP) + 2 * (CSData-->CSDATA_INDEX);
];

[ _CSFindInArr p_value p_array p_count _i;
#Ifv5;
	@scan_table p_value p_array p_count -> _i ?~rfalse;
	rtrue;
!._didnt_match_word_in_array;
#Ifnot;
	for(_i = 0 : _i < p_count : _i++)
		if(p_array-->_i == p_value)
			rtrue;
	rfalse;
#Endif;
];

[_CSMatchNameList p_arr p_count _w _matched _base;
    _w = NextWord();
    if(p_count == 0) return 0;
    while(true) {
        _base = _matched;
		if(_CSFindInArr(_w, p_arr, p_count)) {
			_matched++; 
			if((_w-> #dict_par1) & 4) CSDATA-->CSDATA_PRONOUN_TEMP = CS_THEM;
			_w = NextWord();
		} else
			return _matched;
    }
];

[ CSHasAdjective p_word _arr _w1;
	_arr = _CSGetArr();
	_w1 = _arr-->0;
	if(_w1 < 1 || _w1 > 99) {
		if(_w1 == p_word)
			rtrue;
		rfalse;
	}
	return _CSFindInArr(p_word, _arr + 2, _w1 / 10);
];

[ CSHasNoun p_word _arr _w1;
	_arr = _CSGetArr();
	_w1 = _arr-->0;
	if(_w1 < 1 || _w1 > 99) {
		if(_arr-->1 == p_word)
			rtrue;
		rfalse;
	}
	return _CSFindInArr(p_word, _arr + 2 + 2 * (_w1 / 10), _w1 % 10);
];

[ CSHasWord p_word;
	return CSHasAdjective(p_word) | CSHasNoun(p_word);
];

[ _ParseCheapScenery p_obj p_prop p_base_wn _i _j _sw1 _sw2 _len _ret _arr _longest _next_i _self_bak;
	_longest = CSData-->CSDATA_MATCH_LENGTH;
	cs_parse_name_id = 0;
	_arr = p_obj.&p_prop;
	_len = p_obj.#p_prop;
#ifv5;
	@log_shift _len (-1) -> _len; ! Divide by 2
#Ifnot;
	_len = _len / 2;
#Endif;
	while(_i < _len) {
		CSDATA-->CSDATA_PRONOUN_TEMP = CS_IT;
		_sw1 = _arr-->_i;
		if(_sw1 == CS_THEM) {
			CSDATA-->CSDATA_PRONOUN_TEMP = CS_THEM;
			_i++;
			_sw1 = _arr-->_i;	
		}
		_sw2 = _arr-->(_i+1);
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
		if(_sw1 == 0) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
			print (string) CS_ERR, "5: First element of entry, at position ", _i, 
				" in property ", (property) p_prop, " of ", (name) p_obj,
				" should be a value 1-99, or a vocabulary word, but is 0]^" ;
#Ifnot;
			print (string) CS_ERR, "5]^";
#Endif;
			rfalse;
		}
#Endif;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
		if((_sw1 == CS_ADD_LIST &&
				(_sw2 < 2 || _sw2 > top_object)) ||
			(_sw1 == CS_MAYBE_ADD_LIST &&
				(_arr-->(_i+2) < 2 || _arr-->(_i+2) > top_object))	) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
			if(_sw1 == CS_MAYBE_ADD_LIST) _i++;
			print (string) CS_ERR,"2: Element ", _i+1, " in property ", (property) p_prop, " of ", (name) p_obj,
				" is part of a CS_ADD_LIST or CS_MAYBE_ADD_LIST entry and should be a valid
				object ID but isn't]^" ;
#Ifnot;
			print (string) CS_ERR, "2]^";
#Endif;
			rfalse;
		}
		if(_sw1 < 1 || _sw1 > CS_MAYBE_ADD_LIST && metaclass(_arr-->(_i+2)) ~= String or Routine) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
			print (string) CS_ERR,"3: Element ", _i+2, " in property ", (property) p_prop, " of ",
				(name) p_obj, " is not a string or routine]^";
#Ifnot;
			print (string) CS_ERR,"3]^";
#Endif;
			rfalse;
		}
#Endif;
		
		if(_sw1 == CS_ADD_LIST or CS_MAYBE_ADD_LIST) {
			if(_sw1 == CS_MAYBE_ADD_LIST) {
				_i++;
				if(_sw2() == false) jump _no_list;
				_sw2 = _arr-->(_i+1);
			}
			_ret = _ParseCheapScenery(_sw2, _arr-->(_i+2), p_base_wn);
			_longest = CSData-->CSDATA_MATCH_LENGTH;
		} else if(_sw1 == CS_PARSE_NAME) {
			wn = p_base_wn;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
			if(metaclass(_sw2) ~= Routine) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
				print (string) CS_ERR,"4: Element ", _i+1, " in property ", (property) p_prop, " of ",
				(name) p_obj, " should be a parse_name routine but isn't]^";
#Ifnot;
				print (string) CS_ERR,"4]^";
#Endif;
				rfalse;
			}
#Endif;
			_self_bak = self;
			self = location;
			parser_action = 0;
			_ret = _sw2();
			self = _self_bak;
			if(_ret > _longest) {
				_sw2 = CS_IT;
				if(parser_action == ##PluralFound)
					_sw2 = CS_THEM;
				CSDATA-->CSDATA_PRONOUN_TEMP = _sw2;	
				jump _cs_found_a_match;
			}
			cs_parse_name_id = 0;
		} else if(_sw1 > 0 && _sw1 < 100) {
			wn = p_base_wn;
			_sw2 = _sw1 / 10; ! Repurposing _sw2 as a temp var
			_j = _i + 1; ! Start of adjectives
			_ret = 0;
			if(_sw2 > 0) {
				_ret = _CSMatchNameList(_arr + _j + _j, _sw2);
				_j = _j + _sw2;
				wn--;
			}
			_sw2 = _sw1 % 10;
			_sw1 = _CSMatchNameList(_arr + _j + _j, _sw2);

#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
		if(metaclass(_arr-->(_j + _sw2)) ~= String or Routine) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
			print (string) CS_ERR,"3: Element ", _j + _sw2, " in property ", (property) p_prop, " of ",
				(name) p_obj, " is not a string or routine]^";
#Ifnot;
			print (string) CS_ERR,"3]^";
#Endif;
			rfalse;
		}
#Endif;

			_ret = _ret + _sw1;
			_next_i = _j + _sw2 - 2;
			if(_ret > _longest) {
				jump _cs_found_a_match;
			}
		} else {
			wn = p_base_wn;
			_ret = _CSMatchNameList(_arr + _i + _i, 2);
			if(_ret > _longest) {
._cs_found_a_match;
				_longest = _ret;
				CSData-->CSDATA_OBJ = p_obj;
				CSData-->CSDATA_PROP = p_prop;
				CSData-->CSDATA_INDEX = _i;
				CSData-->CSDATA_PARSE_NAME_ID = cs_parse_name_id;
				CSData-->CSDATA_MATCH_LENGTH = _longest;
				CSDATA-->CSDATA_PRONOUN = CSDATA-->CSDATA_PRONOUN_TEMP;
			}
!				jump _cs_found_a_match;
		}
._no_list;
		if(_next_i) {
			_i = _next_i;
			_next_i = 0;
		}
		_i = _i + 3;
	}
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(_i > _len) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
		print (string) CS_ERR,"1: Property ", (property) p_prop, " of ", (name) p_obj, 
			" extends beyond property length - check entries with 3+ words]^";
#Ifnot;
		print (string) CS_ERR,"1]^";
#Endif;
		rfalse;
	}
#Endif;
	return _longest;
];


Object CheapScenery "object"
	with
		article "an",
		parse_name [ _ret;
			CSData-->CSDATA_MATCH_LENGTH = 0;
			_ret = _ParseCheapScenery(location, cheap_scenery, wn);
			if(CSDATA-->CSDATA_PRONOUN == CS_THEM) {
				give self pluralname;
				if(itobj == self) itobj = 0;
			} else {
				give self ~pluralname;
				if(themobj == self) themobj = 0;
			}
			return _ret;
		],
#Ifdef SceneryReply;
		before [_i _k _w1pos _w1 _w2 _routine _self_bak;
#Ifnot;
		before [_i _k _self_bak;
#Endif;
			_i = _CSGetArr();
			_k = _i-->0;
			if(_k > 0 && _k < 100)
				_k = 1 + (_k / 10) + (_k % 10);
			else
				_k = 2;
			_k = _i-->_k;
			if(action == ##Examine && _k ofclass String)
				print_ret (string) _k;

			if(_k ofclass Routine) {
				_self_bak = self;
				self = location;
				sw__var = action;
				if(_k())
					rtrue;
				self = _self_bak;
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
			} else if(_w1 > 0 && _w1 < 100) {
				_k = _w1 / 10;
				_w1 = 0;
				if(_k) ! There is at least one adjective
					_w1 = _w2;
				_w2 = _i-->(_w1pos + 1 + _k);
			}
			if(SceneryReply(_w1, _w2, _routine))
				rtrue;
#endif;
			print_ret (string) CS_DEFAULT_MSG;
		],
		react_after [;
			Go:
				if(itobj == self) itobj = 0;
				if(themobj == self) themobj = 0;
		],
		found_in [;
			if(location provides cheap_scenery) rtrue;
		],
	has concealed scenery
#Ifdef OPTIONAL_REACTIVE_PARSE_NAME;
		reactive
#Endif;
;

#Ifdef DEBUG;

!			if(CSDebugIsWord(_val
!			(UnsignedCompare(_i, dict_start) < 0 ||

[CSDebugIsDictWord p_val;
    if (UnsignedCompare(p_val, dict_start) >= 0 &&
            UnsignedCompare(p_val, dict_end) < 0 &&
            (p_val - dict_start) % dict_entry_size == 0)
		rtrue;
	rfalse;
];

[ CSDebugPrintObjRef p_obj p_prop p_index;
	new_line;
	print (The) p_obj, " (", p_obj, "), ";
	print (property) p_prop, " property";
	print ", element ", p_index, ": ";
];

[ CSDebugHelper p_obj p_prop _arr _len _i _j _val _val2 _done;
	_arr = p_obj.&p_prop;
	_len = p_obj.#p_prop / 2;
	_i = -1;
	while(++_i < _len) {
		_done = false;
		_val = _arr-->_i;
		if(_val == CS_THEM) {
			_val = _arr-->++_i;
			if(_val == CS_ADD_LIST or CS_MAYBE_ADD_LIST) {
				CSDebugPrintObjRef(p_obj, p_prop, _i);
				"Element following CS_THEM can't be CS_ADD_LIST or CS_MAYBE_ADD_LIST.";
			}
		}
		if(_val == CS_MAYBE_ADD_LIST) {
			_val = _arr-->++_i;
			if(~~(_val ofclass Routine)) {
				CSDebugPrintObjRef(p_obj, p_prop, _i);
				"Element following CS_MAYBE_ADD_LIST must be a routine.";
				
			}
			_val = CS_ADD_LIST; ! Check the rest as if it was a CS_ADD_LIST entry
		}
		if(_val == CS_ADD_LIST) {
			_val = _arr-->++_i;
			if(_val < 2 || _val > top_object) {
				CSDebugPrintObjRef(p_obj, p_prop, _i);
				"Element following CS_ADD_LIST or CS_MAYBE_ADD_LIST must be an object ID.";
			}
			_val2 = _arr-->++_i;
			CSDebugHelper(_val, _val2);
			_done = true;
		}
		if(_done == false && _val == CS_PARSE_NAME) {
			_val = _arr-->++_i;
			if(~~(_val ofclass Routine)) {
				CSDebugPrintObjRef(p_obj, p_prop, _i);
				"Element following CS_PARSE_NAME must be a routine.";
			}
			_val = _arr-->++_i;
			if(~~(_val ofclass String) && ~~(_val ofclass Routine)) {
				CSDebugPrintObjRef(p_obj, p_prop, _i);
				"Expected a reaction string or routine in this position.";
			}
			_done = true;
		}
		if(_done == false && CSDebugIsDictWord(_val)) {
			_i--;
			_val = 2; ! Let the generic 1-99 clause handle it
		}
		if(_done == false && _val > 0 && _val < 100) {
			if(_val % 10 == 0) {
				CSDebugPrintObjRef(p_obj, p_prop, _i + _j);
				"This value (", _val, ") indicates that there are no nouns,
					which means this entry can never be matched.";
			}
			_val = _val / 10 + _val % 10;
			for(_j = 1: _j <= _val : _j++) {
				_val2 = _arr-->(_i + _j);
				if(CSDebugIsDictWord(_val2)==false) {
					CSDebugPrintObjRef(p_obj, p_prop, _i + _j);
					"Expected a dictionary word in this position.";
				}
			}
			_i = _i + _val + 1;
			_val = _arr-->_i;
			if((~~(_val ofclass String)) && ~~(_val ofclass Routine)) {
				CSDebugPrintObjRef(p_obj, p_prop, _i);
				"Expected a reaction string or routine in this position.";
			}
			_done = true;
		}
		
		
		if(_done == false) {
			CSDebugPrintObjRef(p_obj, p_prop, _i);
			"Unknown element in this position.";
		}
	}
	if(_i > _len) {
		CSDebugPrintObjRef(p_obj, p_prop, _i);
		"Element(s) missing at end of list?";
	}
	print "#";
];

[ DebugCheapScenerySub _obj;
	print "Testing all cheap_scenery arrays in game:^";
	objectloop(_obj provides cheap_scenery) {
		if(parent(_obj)) {
			print (The) _obj, "(", _obj, ") provides cheap_scenery, but doesn't appear to be a location.^";
			continue;
		}
		CSDebugHelper(_obj, (cheap_scenery));
	}
	"^Cheap scenery test complete.";
];

Verb meta 'cstest'
	* -> DebugCheapScenery;
#Endif;