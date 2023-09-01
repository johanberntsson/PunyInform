! Part of PunyInform: A small stdlib and parser for interactive fiction
! suitable for old-school computers such as the Commodore 64.
! Designed to be similar, but not identical, to the Inform 6 library.

! ######################### Parser
! PunyInform uses grammar version 2 which is easier to parse and economical

System_file;

[ YesOrNo n;
    for (::) {
        _ReadPlayerInput(true, true);
		n = parse -> 1;
		parse -> 1 = 0;
        if(n == 1) {
        	! one word reply
            if(parse --> 1 == 'yes' or 'y//') rtrue;
            if(parse --> 1 == 'no' or 'n//') rfalse;
        }
        PrintMsg(MSG_YES_OR_NO);
    }
];

[ _UserFilter _obj _noun _ret;
	!  UserFilter consults the user's filter (or checks on attribute)
	!  to see what already-accepted nouns are acceptable
    if(noun_filter > 0 && noun_filter < 49) {
        if (_obj has (noun_filter-1)) rtrue;
        rfalse;
    }
	_noun = noun; noun = _obj;
    _ret = indirect(noun_filter);
	noun = _noun;
	return _ret;
];

[ _FixIncompleteSentenceOrComplain p_pattern _token _type _noun _prep _second _num_nouns;
	! Called because sentence shorter than the pattern
	! Available data: wn, parse and p_pattern_token (last matched token)
	!
	! Either guess missing parts in the pattern and return true,
	! or print a suitable error message and return false
	!
	! INFORM:
	! lock: What do you want to lock?
	! lock door: What do you want to lock the toilet door with?
	! lock door with: What do you want to lock the toilet door with?
	! lock door on: I didn't understand that sentence.
	! give john: What do you want to give John?
	! jump at: I only understood you as far as wanting to jump.
	! jump over: What do you want to jump over?
	!
	! Inform tries the 'itobj' if second missing, and his/herobj
	! is creature missing (or if only one animate object in scope)

	! analyse the rest of the pattern to see if second and prep are expected
	for(_token = p_pattern + 3: _token->0 ~= TT_END: _token = _token + 3) {
		_type = _token -> 0;
		if(_type > 9) {
			_prep = _token;
		} else {
			++_num_nouns;
			if(_noun == 0) {
				_noun = _token;
			} else {
				_second = _token;
			}
		}
	}

	! try to guess missing parts in the pattern
	! return true if we could fix everything
#IfDef OPTIONAL_GUESS_MISSING_NOUN;
	if(_noun ~= 0 && noun == 0) {
		noun = _GuessMissingNoun(_noun -> 2, 0, 1);
	}
	if(_second ~= 0 && second == 0) {
		second = _GuessMissingNoun(_second -> 2, _prep, 2);
	}
	!print scope_stage, " ", phase2_necessary, " num_nouns ",_num_nouns, " noun ", _noun, "=", noun, ": second ", _second, "=", second, "^";
	if(_noun ~= 0  && noun ~= 0) {
		if(_num_nouns == 1) {
			!print "message complete: ", noun, "^";
			rtrue;
		}
		if(_num_nouns == 2 && _second ~=0 && second ~= 0) {
			!print "message complete: ", noun, " ", second, "^";
			rtrue;
		}
	} else if(_num_nouns == 2 && _second ~=0 && second ~= 0 && _noun->2 == TOPIC_OBJECT) {
		! for example: "say hello" (assuming X) -> noun = 0, second = X
		!print "message complete: ", noun, " ", second, "^";
		rtrue;
	}
#EndIf;

	! write an error message and return false
	_PrintPatternSyntax(p_pattern, _noun);
	rfalse;
];

[ _PrintPatternSyntax p_pattern p_noun _num_preps _token _type _data;
	! write what pattern we expected, something like:
	! "I think you wanted to say "put all in something". Please try again."
	PrintMsg(MSG_PARSER_BAD_PATTERN_PREFIX);
	print (verbname) verb_word;
	for(_token = p_pattern + 3: _token->0 ~= TT_END: _token = _token + 3) {
		_type = (_token -> 0) & $0f;
		_data = (_token + 1) --> 0;
		! check if this is a new list of prepositions
		if(_token->0 == TOKEN_FIRST_PREP or TOKEN_SINGLE_PREP) _num_preps = 0;
		if(_type == TT_PREPOSITION) {
			! only write the first item in a list of alternative prepositions
			if(_num_preps == 0) print " ", (address) _data;
			++_num_preps;
		} else {
			@print_char ' ';
			if(p_noun == 0) {
				if(_type == TT_ROUTINE_FILTER && _data == ADirection) {
					print (string) SOMEDIRECTION_STR;
				} else if(second == 0) {
					if(_type == TT_OBJECT && _token->2 == CREATURE_OBJECT) {
						print (string) SOMEONE_STR;
					} else {
						print (string) SOMETHING_STR;
					}
				} else print (name) second;
			} else {
				if(noun ~= 0 && metaclass(noun) ~= ROUTINE) {
					p_noun = 0; ! avoid repeat (we don't need p_noun anymore)
					if(parser_all_found) print "all"; else print (name) noun;
				} else if(_type == TT_OBJECT && _token->2 == CREATURE_OBJECT) {
					print (string) SOMEONE_STR;
				} else {
					print (string) SOMETHING_STR;
				}
			}
		}
	}
	PrintMsg(MSG_PARSER_BAD_PATTERN_SUFFIX);
];

[ _AskWhichNoun p_num_matching_nouns _i;
	print "Do you mean ";
	for(_i = 1 : _i <= p_num_matching_nouns : _i++) {
		if(_i > 1) {
			if(_i == p_num_matching_nouns) {
				print " or ";
			} else {
				print ", ";
			}
		}
		print (the) which_object --> _i;
	}
	print "?";
];

[ _CreatureTest obj;
	! Will this obj do for a "creature" token?
    if (actor ~= player) rtrue;
    if (obj has animate) rtrue;
    if (obj has talkable && action == ##Ask or ##Answer or ##Tell or ##AskFor)
		rtrue;
    rfalse;
];

[ _ParseTopic p_wn p_parse_pointer p_preposition;
	consult_from = p_wn;
	consult_words = 0;
	for(::) {
		++p_wn;
		++consult_words;
		p_parse_pointer = p_parse_pointer + 4;
		if(p_wn > parse->1) {
			! end of line
			rfalse;
		}
		if(p_parse_pointer --> 0 == p_preposition) {
			! found the stop token
			rtrue;
		}
	}
];

#Ifv5;
[ _ReadPlayerInput p_no_prompt p_no_statusline _result;
#Ifnot;
[ _ReadPlayerInput p_no_prompt _result; ! NOTE: Must have at least 2 vars, so calls with 2 params don't break!
#EndIf;
#IfV5;
	style roman;
	@buffer_mode 0;
#EndIf;
	if(p_no_prompt == false) PrintMsg(MSG_PROMPT);
	! library entry routine
	RunEntryPointRoutine(AfterPrompt);
#IfV5;
	if(p_no_statusline == false) DrawStatusLine();
	buffer->1 = 0;
	if (clr_on && clr_fginput > 1) {
		@set_colour clr_fginput clr_bg;
	}
	@aread buffer parse -> _result;
	@buffer_mode 1;
	if (clr_on && clr_fginput > 1) {
		@set_colour clr_fg clr_bg;
	}
#IfNot;
	@sread buffer parse;
#EndIf;

	! call library entry routine
	RunEntryPointRoutine(BeforeParsing);

	num_words = parse -> 1;
	! Set word after last word in parse array to all zeroes, so it won't match any words.
	_result = 2 * (parse -> 1) + 1;
	parse-->_result = 0;
	parse-->(_result + 1) = 0;
];

#Ifdef OPTIONAL_ALLOW_WRITTEN_NUMBERS;
[ NumberWord p_o _i _n;
    ! try to parse  "one" up to "twenty".
    _n = LanguageNumbers-->0;
    for(_i = 1 : _i <= _n : _i = _i + 2)
        if (p_o == LanguageNumbers-->_i) return LanguageNumbers-->(_i+1);
    return 0;
];
#Endif;

[ TryNumber p_wordnum _i _j _num _len _d _tot _digit;
	!  Takes word number p_wordnum and tries to parse it as an
	! unsigned decimal number, returning
	!
	!  -1000                if it is not a number
	!  the number           if it has between 1 and 4 digits
	!  10000                if it has 5 or more digits.
    _i = wn; wn = p_wordnum; _j = NextWord(); wn = _i;
#Ifdef OPTIONAL_ALLOW_WRITTEN_NUMBERS;
    _j = NumberWord(_j); if (_j >= 1) return _j;
#Endif;

    _i = p_wordnum*4+1; _j = parse->_i; _num = _j+buffer; _len = parse->(_i-1);

    ! allow for a entry point routine to override normal parsing
    _tot = ParseNumber(_num, _len); if(_tot ~= 0) return _tot;

    ! this uses Horner's algorithm: 2421 = 10*(10*(10*(2)+4)+2)+1
    for (_i=0: _i<_len: _i++) {
        _digit = _num->_i;
        if(_digit < '0' || _digit > '9') jump _trynumber_baddigit;
        _d = _digit - '0';
        if(_len <=4) _tot = _tot*10 + _d;
    }
   	if (_len > 4) return 10000;
    return _tot;
._trynumber_baddigit;
	return -1000;
];

[ _CopyInputArray p_src_input_array p_dst_input_array _i;
	!_n = MAX_INPUT_CHARS + 3;
	for(_i = 0: : _i++) {
		p_dst_input_array->_i = p_src_input_array->_i;
		! abort when 0 found in the text, which starts
		! from 1 in v1-4, and from 2 in v5-v8.
#IfV5;
		if(p_dst_input_array->_i == 0 && _i>2) break;
#IfNot;
		if(p_dst_input_array->_i == 0 && _i>1) break;
#EndIf;
	}
];

[ _CopyParseArray p_src_parse_array p_dst_parse_array _n _i;
	!_n = 2 + 4 * (MAX_INPUT_WORDS + 1);
	_n = 2 + 4*p_src_parse_array->1;
	for(_i = 0: _i < _n: _i++)
		p_dst_parse_array->_i = p_src_parse_array->_i;
];

[ _PatternRanking p_pattern _i;
	! Return a biased pattern ranking:
	! this is used to select more basic patterns in some situations,
	! and since when the parsning fails but there are two possible
	! patterns, once with preposition and one with basic object slots,
	! then give bias to the more basic form (without prepositions).
	! This gives better error messages.
	_i = (p_pattern-->0 & $400); ! give bias to action_reverse
	p_pattern = p_pattern + 2;
	for(::) {
		if(p_pattern->0 == TT_END) {
			return _i;
		} else if(p_pattern->0 == TOKEN_SINGLE_PREP or TOKEN_FIRST_PREP) {
			_i = _i + 2; ! give bias to patterns without prepositions
		} else if(p_pattern->0 ~= TOKEN_MIDDLE_PREP or TOKEN_LAST_PREP) {
			! Alternative prepositions should be skipped, and all other
			! tokens should add to the length
			++_i;
		}
		p_pattern = p_pattern + 3;
	}
];

#IfDef DEBUG;

[ _PrintParseArray p_parse_array _i;
	print "PARSE_ARRAY: ", p_parse_array->1, " entries^";
	for(_i = 0 : _i < p_parse_array -> 1 : _i++) {
		print _i, ": wn ", (_i + 1),
		" dict ",((p_parse_array + 2 + _i * 4) --> 0),
		" (",(address) ((p_parse_array + 2 + _i * 4) --> 0),") ",
 		" len ",(p_parse_array + 2 + _i * 4) -> 2,
		" index ",(p_parse_array + 2 + _i * 4) -> 3, "^";
	}
];

[ _PrintGrammarPattern p_pattern _i _action_number _token_top _token_next _token_bottom;
	! action number is the first two bytes
	_action_number = p_pattern-->0;
	p_pattern = p_pattern + 2;
	action = _action_number & $3ff;
	action_reverse = (_action_number & $400 ~= 0);
!	print "Action#: ", action, " Reverse: ", reverse, "^";
	print "Action#: ", action, "^";

	for(_i = 0: : _i++) {
		if(p_pattern->0 == TT_END) break;
		_token_top = (p_pattern->0 & $c0)/64; ! top (2 bits)
		_token_next = (p_pattern->0 & $30)/16;  ! next (2 bits)
		_token_bottom = p_pattern->0 & $0f; ! bottom (4 bits)
		print "Token#: ", _i, " Type: ", p_pattern->0, " (top ", _token_top, ", next ",_token_next, ", bottom ",_token_bottom, ") data: " ,(p_pattern + 1)-->0;
		if((p_pattern + 1)-->0>4000) print" " ,(address) (p_pattern + 1)-->0;
		@new_line;
		p_pattern = p_pattern + 3;
	}
	! print ": ", i, " tokens^";
	return p_pattern + 1; ! skip TT_END
];
#EndIf;


! Keep the routines WordAddress, WordLength, NextWord and NextWordStopped just next to _ParseNounPhrase,
! since they will typically be called from parse_name routines, which are called from _ParseNounPhrase

[ WordAddress p_wordnum;    ! Absolute addr of 'wordnum' string in buffer
	return buffer + parse->(p_wordnum*4+1);
];

[ WordLength p_wordnum;     ! Length of 'wordnum' string in buffer
	return parse->(p_wordnum*4);
];

[ WordValue p_wordnum;      ! Dictionary value of 'wordnum' string in buffer
    return parse-->(p_wordnum*2-1);
];

[ NumberWords;              ! Number of parsed strings in buffer
    return parse->1;
];

[ _PeekAtNextWord _i;
	_i = NextWord();
	--wn; ! wn was modified by NextWord, restore it
	return _i;
];

[ NextWord _i _j;
	if (wn <= 0 || wn > parse->1) { wn++; rfalse; }
	_i = wn*2-1; wn++;
	_j = parse-->_i;
	if (_j == ',//') _j = comma_word;
	if (_j == './/') _j = THEN1__WD;
	return _j;
];

[ NextWordStopped;
	if (wn > parse->1) { wn++; return -1; }
	return NextWord();
];

[ _CalculateObjectLevel p_obj _score;
	! set priority levels for inclusion in which?
	! queries. Low level objects will only be
	! considered if there are no higher levels in play
	! This routine should return a value between 0 and 990.
	! The last digit of the return value must be 0.
	if(meta == true) {
		! if meta then add all matching objects
		return 900;
	}
	if (p_obj has concealed) return 200;

	_score = 500;

	if(p_obj has scenery) _score = 300;
	else if(action == ##Take && p_obj in actor) {
		! take gives low priority for already held objects
		_score = 400;
	} else if(object_token_type == HELD_OBJECT or MULTIHELD_OBJECT or MULTIEXCEPT_OBJECT && p_obj notin actor) {
		! low priority for not held objects
		_score = 400;
		if(object_token_type == MULTIEXCEPT_OBJECT && p_obj in second) {
			! lower priority for object in/on the second object
			_score = _score - 10;
		}
	} else if(object_token_type == MULTIINSIDE_OBJECT && parent(p_obj) == location or actor) {
		! low priority for objects directly in location
		_score = 400;
	} else if(object_token_type == MULTIINSIDE_OBJECT && second ~= 0 && parent(p_obj) ~= second) {
		! lower priority for objects not inside the object
		_score = 400;
	}

	if(p_obj ~= Directions)
		_score = _score + 10;
	
	return _score;
];

Constant _PARSENP_CHOOSEOBJ_WEIGHT = 1000;

#Ifdef ParseNoun;
[ _ParseNounPhrase p_parse_pointer p_expecting_single_noun
		_i _j _k _p _obj _matches _best_word_count _current_word
		_name_array _name_array_len _best_score _result _parse_noun_words;
#Ifnot;
[ _ParseNounPhrase p_parse_pointer p_expecting_single_noun
		_i _j _k _p _obj _matches _best_word_count _current_word
		_name_array _name_array_len _best_score _result;
#Endif;
#IfDef DEBUG_PARSENOUNPHRASE;
	print "Entering _ParseNounPhrase, first word is ";
	_i = WordValue(wn);
	if(_i == 0) print "[Unknown]";
	else print (address) _i;
	new_line;
#EndIf;
	! return 0 if no noun matches
	! return -n if more n matches found (n > 1)
	! else return object number
	! side effects:
	! - uses parser_check_multiple
	! - which_object
	!     - stores number of objects in -> 0
	!     - stores number of words consumed in -> 1
	!     - stores all matching nouns if more than one in -->1 ...

	! don't check if wn out of range
	if(wn > num_words) return 0;

	! this is needed after a which question, so that we
	! can answer 'the pink book' and similar
	while(p_parse_pointer --> 0 == 'a//' or 'the' or 'an') {
		wn = wn + 1;
		p_parse_pointer = p_parse_pointer + 4;
	}

	if((((p_parse_pointer-->0) -> #dict_par1) & 128) == 0) {
		! this word doesn't have the noun flag set,
		! so it can't be part of a noun phrase
		return 0;
	}
	_k = wn;

#IfDef DEBUG;
	if(meta) {
		_name_array_len = Directions; parser_one = top_object + 1;
	} else {
		_name_array_len = 0; parser_one = scope_objects;
	}
	for(_i = _name_array_len: _i < parser_one: _i++) {
		if(meta) _obj = _i; else _obj = scope-->_i;
#IfNot;
	for(_i = 0: _i < scope_objects: _i++) {
		_obj = scope-->_i;
#Endif;
		if(parser_check_multiple && _obj == Directions && selected_direction ~= 0) continue;
		wn = _k;
		_p = p_parse_pointer;
		_current_word = p_parse_pointer-->0;
#Ifdef ParseNoun;
		_parse_noun_words = 0;
#Endif;
#IfDef DEBUG_PARSENOUNPHRASE;
		print "Testing ", (the) _obj, "...^";
#EndIf;
		if((noun_filter == 0 || _UserFilter(_obj) ~= 0)) {
#Ifdef ParseNoun;
			_result = ParseNoun(_obj);
			if(_result >= 0) {
				jump _register_candidate;
			}
			else if(wn > _k) {
				_parse_noun_words = wn - _k;
				_p = p_parse_pointer + 4 * _parse_noun_words;
				_current_word = _p-->0;
			}
#Endif;
			_result = 0;
#Ifdef DEBUG;
			! Problem: parse_name is an alias of sw_to, and debug verbs can
			! reference any object in the game, some of which are rooms.
			! Solution: If the object seems to be a room, we will not call
			! parse_name, unless DebugParseNameObject(object) returns true
#Ifdef OPTIONAL_REACTIVE_PARSE_NAME;
			if(_obj has reactive && _obj.parse_name ofclass Routine) {
#Ifnot;
			if(_obj.parse_name ofclass Routine) {
#Endif;
				if(meta == 0 || parent(_obj) ~= 0
						|| (_obj provides describe or life or found_in)
						|| DebugParseNameObject(_obj)) {
#IfV3;
					if(debug_flag & 1) print (name) _obj, ".parse_name()^";
#EndIf;
					_result = _obj.parse_name();
				}
#Ifnot; ! Not DEBUG
#Ifdef OPTIONAL_REACTIVE_PARSE_NAME;
			if(_obj has reactive && _obj.parse_name) {
#Ifnot;
			if(_obj.parse_name) {
#Endif;
				_result = _obj.parse_name();
#Endif;

#Ifdef ParseNoun;
				wn = _k + _parse_noun_words;
#Ifnot;
				wn = _k;
#Endif;
				if(_result >= 0) {
					jump _register_candidate;
				}
			}

			_result = 0;
!  Try matching name array
			@get_prop_addr _obj name -> _name_array;
			if(wn <= num_words && _name_array) {
				_result = 0;
				! Assembler equivalent of _name_array_len = _obj.#name / 2
				@get_prop_len _name_array -> _name_array_len;
#IfV5;
				@log_shift _name_array_len (-1) -> _name_array_len;
#IfNot;
				@div _name_array_len 2 -> _name_array_len;
#EndIf;
#IfDef DEBUG_PARSENOUNPHRASE;
				print "Trying to find ", (address) _current_word," in name (length ",_name_array_len,").^";
#EndIf;
#IfV3;
				@dec _name_array_len; ! This is needed for the loop.
#EndIf;
				while(_IsSentenceDivider(_p) == false) {
#IfV5;
					@scan_table _current_word _name_array _name_array_len -> _j ?_word_found_in_name_prop;
#IfNot;
					_j = 0;
._next_word_in_name_prop;
					@loadw _name_array _j -> sp;
					@je sp _current_word ?_word_found_in_name_prop;
					@inc_chk _j _name_array_len ?~_next_word_in_name_prop;
#EndIf;
					jump _register_candidate;
._word_found_in_name_prop;
#IfDef DEBUG_PARSENOUNPHRASE;
					print " - matched ", (address) _current_word,"^";
#EndIf;
					_result++;
					_p = _p + 4;
					_current_word = _p-->0;
				}
			}
._register_candidate;
#Ifdef ParseNoun;
			_result = _result + _parse_noun_words;
#Endif;
			if(_result ~= 0 && _result >= _best_word_count) {
				! This object is matched with at least as many words as the longest matches this far
				_j = _CalculateObjectLevel(_obj);
#Ifdef ChooseObjects;
				! give ChooseObjects a chance to modify the score
				_j = _j + _PARSENP_CHOOSEOBJ_WEIGHT * ChooseObjects(_obj, 2);
#Endif;

				if(_j == _best_score && _result == _best_word_count) {
					_matches++;
					which_object-->_matches = _obj;
#IfDef DEBUG_PARSENOUNPHRASE;
					print "Same best result: word count ", _result, ", score ", _best_score, ". Matches are now ", _matches,"^";
#EndIf;
				} else if(_j > _best_score || _result > _best_word_count) {
#IfDef DEBUG_PARSENOUNPHRASE;
					print "New best result: word count ", _result, ", score ", _j , "^";
#EndIf;
					_best_word_count = _result;
					_best_score = _j;
					_matches = 1;
					which_object-->1 = _obj;
				}
			}
		}
	}

	for(_i = p_parse_pointer + (_best_word_count - 1) * 4: _i >= p_parse_pointer: _i = _i - 4) {
		_j = _i-->0;
		if(_j && (_j-> #dict_par1) & 4) parser_action = ##PluralFound;
	}

#Ifdef ChooseObjectsFinal;
	if(_matches > 1 && parser_action ~= ##PluralFound && parser_all_found == 0) {
		if(parser_phase == PHASE1) {
			phase2_necessary = PHASE2_SUCCESS;
		} else {
			! ChooseObjectsFinal may call ChooseObjectsPick(n) or ChooseObjectsDiscard(n) to modify the array.
			parser_one = _matches;
			ChooseObjectsFinal(which_object + 2, _matches);
			_matches = parser_one;
		}
	}
#Endif;

	wn = _k;
	which_object->0 = _matches;
	which_object->1 = _best_word_count;

	if(p_expecting_single_noun) {
#Ifdef ChooseObjectsFinal;
		! if p_expecting_single_noun is true, then try calling ChooseObjectsFinal
		! to reduce a set of indistiguishable nouns to a single one.
		if(_matches > 1) {
			parser_one = which_object -> 0;
 			ChooseObjectsFinal(which_object + 2, parser_one);
			parser_action = 0; ! we expect a single object, so no plural
			_matches = parser_one;
		}
#EndIf;
	}

	if(_matches == 1) {
		_result = which_object-->1;
#IfDef DEBUG_PARSENOUNPHRASE;
		print "Matched a single object: ", (the) _result,
			", num words ", which_object->1, ", wn ", wn, "^";
#EndIf;
		return _result;
	}
#IfDef DEBUG_PARSENOUNPHRASE;
		print "Matches: ", _matches,", num words ", which_object->1, "^";
#EndIf;
	if(_matches > 1) {
		return -_matches;
	}
	return 0;
];

#Ifdef ChooseObjectsFinal;
! ChooseObjectsFinal may call ChooseObjectsFinal_Pick(n) or ChooseObjectsFinal_Discard(n) to modify the array.

[ ChooseObjectsFinal_Pick p_n _arr;
	_arr = which_object + 2;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(p_n < 0 || p_n > parser_one - 1)
		_RunTimeError(ERR_ILLEGAL_CHOOSEOBJNO);
#EndIf;
	_arr-->0 = _arr-->p_n;
	parser_one = 1;
];

[ ChooseObjectsFinal_Discard p_n _i _arr;
	_arr = which_object + 2;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(p_n < 0 || p_n > parser_one - 1)
		_RunTimeError(ERR_ILLEGAL_CHOOSEOBJNO);
#EndIf;
	parser_one--;
	for(_i=p_n: _i<parser_one: _i++) {
		_arr-->_i = _arr-->(_i+1);
	}
];
#Endif; !Ifdef ChooseObjectsFinal


[ _GetNextNoun p_parse_pointer p_expecting_single_noun _noun _oldwn
		_num_words_in_nounphrase _pluralword _i _j _k _m _all_found;
	! try getting a noun from the <p_parse_pointer> entry in parse
	! return:
	!   <noun number> if found
	!   0  if no noun found (but we didn't write an error message)
	!   -1 if we should give up parsing completely (because
	!      the player has entered a new command line).
	!   -2 if parsing failed, and error message written
	!
	! Side effects:
	! - if found, then wn will be set to the first word in the noun phrase,
	!       skipping any articles
	! - if plural matched, then parser_action set to ##PluralFound
	!
	! NOTE: you need to update parse_pointer after calling _GetNextNoun since
	! wn can change

	parser_action = 0;

	! special rule to avoid "all in"
	! (since 'in' in this combination is usually
	! a preposition but also matches a direction noun)
	if(p_parse_pointer-->0 == ALL_WORD &&
		(p_parse_pointer + 4)-->0 == 'in') {
		++wn;
		return 0;
	}

	! skip 'all' etc
	while(p_parse_pointer --> 0 == ALL_WORD or EXCEPT_WORD1 or EXCEPT_WORD2) {
		if(p_parse_pointer --> 0 == ALL_WORD) {
			parser_all_found = true; !TODO: do we really need the global?
			_all_found = true;
		}
#IfDef DEBUG_GETNEXTNOUN;
		print "skipping ",(address) p_parse_pointer --> 0,"^";
#Endif;
		++wn;
		p_parse_pointer = p_parse_pointer + 4;
	}

	! check for pronouns
	if(p_parse_pointer --> 0 == 'it' or 'him' or 'her' or 'them') {
		switch(p_parse_pointer --> 0) {
		'it': _noun = itobj;
		'him': _noun = himobj;
		'her': _noun = herobj;
		'them': _noun = themobj;
		}
		if(_noun == 0) {
			phase2_necessary = PHASE2_ERROR;
			if(parser_phase == PHASE2) {
				PrintMsg(MSG_PARSER_NO_IT, p_parse_pointer --> 0);
!				print "I don't know what ~",(address) p_parse_pointer --> 0, "~ refers to.^";
				return -2;
			}
		} else if(TestScope(_noun) == false) {
			phase2_necessary = PHASE2_ERROR;
			if(parser_phase == PHASE2) {
				PrintMsg(MSG_PARSER_CANT_SEE_IT, p_parse_pointer --> 0, _noun);
!				print "You can't see ~",(address) p_parse_pointer --> 0, "~ (", (name) _noun, ") at the moment.^";
				return -2;
			}
	 	}
		if((noun_filter ~= 0 && _UserFilter(_noun) == 0)) {
			! a filter was defined, but failed for it/his/her
			return 0;
		}
		++wn;
		return _noun;
	}

	! not a pronoun, continue
#IfDef DEBUG_GETNEXTNOUN;
	print "Calling _ParseNounPhrase(",p_parse_pointer,");^";
	if(p_parse_pointer-->0 > 2000) print (address) p_parse_pointer-->0, " ", _pluralword, "^";
#Endif;
	_noun = _ParseNounPhrase(p_parse_pointer, p_expecting_single_noun);
	_num_words_in_nounphrase = which_object -> 1;

	! check if the noun phrase contains a plural word
	_pluralword = false;
	if(parser_action == ##PluralFound) _pluralword = true;

._getnextnoun_recheck_noun;
	if(_noun < 0) {
		if(_pluralword || _all_found) {
			! we don't have to ask here, because the input was
			! "take books" or "take all books"
			phase2_necessary = PHASE2_ERROR;
			parser_action = ##PluralFound;
			wn = wn + _num_words_in_nounphrase;
			return 0;
		}
		if(parser_phase == PHASE1) {
			phase2_necessary = PHASE2_DISAMBIGUATION;
			wn = wn + _num_words_in_nounphrase;
			return 1; ! a random noun in phase 1 just to avoid which? question
		}
		_AskWhichNoun(-_noun);
		! read a new line of input
		! I need to use parse since NextWord
		! for parse_name and others hardcode this
		! usage, so I first store the old input into
		! temp arrays that I will restore if I can
		! disambiguate successfully.
		_CopyInputArray(buffer, buffer2);
		_CopyParseArray(parse, parse2);
		@new_line;
		@new_line;
		_ReadPlayerInput(); ! ReadPlayerInput destroys num_words
		! is this a reply to the question?
		_j = parse + 2;
		_k = parse + 6;
		if((_j-->0 ~= 0 && (_j-->0)->#dict_par1 & 129 == 128)) {
			! the first word is not a verb, but is a noun. Assume
			! a valid reply and add the other
			! entry into parse, then retry

			! add the word we got stuck at to the temp parse buffer
			! this is to be able to handle a room with a transparent
			! box, a opaque box and a transparent chest, while
			! processing "take transparent". If the player responds
			! 'box' then we don't know if it is the transparent or
			! opqaue box unless we also add the 'transparent' word
			! before calling _ParseNounPhrase

			_oldwn = wn; ! wn is used in _ParseNounPhrase, so save it

			! add the original words to the new input. For example,
			! if the input was 'take book' and we added 'red'
			! then the complete new input to test should be
			! 'red' 'book'
			!
			! since more than one word may be given we need to loop
			! over and test each word
			_CopyParseArray(parse, parse3);
			for(_i = 0 : _i < parse3 -> 1 : _i++) {
				!print "Testing ", (address) (parse3 + 2 + _i * 4) --> 0, "^";
				_j-->0 = (parse3 + 2 + _i * 4) --> 0;

				! note that we have to add this is correct order otherwise
				! parse_name routines may not work, so we need to test
				! both ways.

#IfDef DEBUG;
				!_PrintParseArray(parse);
#Endif;
				_m = (parse2 + 4 * _oldwn - 2)-->0;
				parse->1 = 2;
				if(_m == _j-->0) {
					! don't allow repeated words (red red etc)
					_m = 0;
					parse->1 = 1;
				}
				_k-->0 = _m;
#IfDef DEBUG;
				!_PrintParseArray(parse);
#Endif;
				wn = 1;
				_noun = _ParseNounPhrase(_j, p_expecting_single_noun);
				if(_noun <= 0) {
					! the normal word order didn't work. Try the other way
					!print "testing other word order^";
#IfDef DEBUG;
					!_PrintParseArray(parse);
#Endif;
					_m = _j-->0;
					_j-->0 = (parse2 + 4 * _oldwn - 2)-->0;
					parse->1 = 2;
					if(_m == _j-->0) {
						! don't allow repeated words (red red etc)
						_m = 0;
						parse->1 = 1;
					}
					_k-->0 = _m;
#IfDef DEBUG;
					!_PrintParseArray(parse);
#Endif;
					wn = 1;
					_noun = _ParseNounPhrase(_j, p_expecting_single_noun);
				}
				if(_noun == Directions && which_object->1 < parse->1) {
					! _ParseNounPhrase only matched a direction and
					! should be treated as new input instead
					parse->1 = which_object->1;
					return -1;
				}
				wn = _oldwn; ! restore wn after the _ParseNounPhrase calls
				if(_noun > 0) {
					! we have successfully disambiguated the noun phrase.
					! now we need to restore the length of the
					! noun phrase so that it will be absorbed when we
					! return from the routine.
					! don't forget to restore the old arrays
					_CopyInputArray(buffer2, buffer);
					_CopyParseArray(parse2, parse);
                    num_words = parse -> 1;
					jump _getnextnoun_recheck_noun;
				}
			}
			PrintMsg(MSG_PARSER_CANT_DISAMBIGUATE);
			return -2;
		}
		! completely new input.
		return -1; ! start from the beginning
	} else if(_noun > 0) {
#IfDef DEBUG_GETNEXTNOUN;
		print "Noun match! ", _noun, " ", which_object->1, "^";
#EndIf;
		wn = wn + _num_words_in_nounphrase;
		return _noun;
	} else {
		! this is not a recognized word at all
#IfDef DEBUG_GETNEXTNOUN;
		print "it wasn't a noun^";
#EndIf;
		return 0;
	}
];

[ _UpdateNounSecond p_noun p_inp;
	if(num_noun_groups == 0) {
		noun = p_noun;
		inp1 = p_inp;
	} else if(num_noun_groups == 1){
		second = p_noun;
		inp2 = p_inp;
	}
	++num_noun_groups;
];

[ _IsSentenceDivider p_parse_pointer;
	! check if current parse block, indicated by p_parse_pointer,
	! is a period or other sentence divider
	if(p_parse_pointer --> 0 == './/' or ',//' or AND_WORD or THEN1__WD) rtrue;
	rfalse;
];

[ ImplicitGrabIfNotHeld p_noun _ks;
	! return true if p_noun isn't held by the player at the end of the call
	! (so that you can use it like: if(_ImplicitGrabIfNotHeld(...)) { }
	! Return false if actor isn't player or p_noun is 0
	if(actor ~= player || p_noun == 0) rfalse;
	if(p_noun in player) rfalse;
	PronounNotice(p_noun);
	if(no_implicit_actions) { PrintMsg(MSG_AUTO_TAKE_NOT_HELD, p_noun); rtrue; }
	PrintMsg(MSG_AUTO_TAKE, p_noun);
	_ks = keep_silent;
	keep_silent = true;
	PerformAction(##Take, p_noun);
	keep_silent = _ks;
	if (p_noun notin player) rtrue;
	rfalse;
];

[ ImplicitDisrobeIfWorn p_noun _ks;
	! return true if p_noun isn't held by the player at the end of the call
	! (so that you can use it like: if(ImplicitDisrobeIfWorn(...)) rtrue;
	if(actor ~= player || p_noun == 0) rfalse;
	if(p_noun notin player || p_noun hasnt worn) rfalse;
	PronounNotice(p_noun);
	if(no_implicit_actions) { PrintMsg(MSG_AUTO_DISROBE_WORN, p_noun); rtrue; }
	if(keep_silent == false)
		PrintMsg(MSG_AUTO_DISROBE, p_noun);
	_ks = keep_silent;
	keep_silent = true;
	PerformAction(##Disrobe, p_noun);
	keep_silent = _ks;
	if (p_noun notin player || p_noun has worn) rtrue;
	rfalse;
];

[ ParseToken p_token_type p_token_data;
	! DM defines ParseToken as ParseToken(tokentype,tokendata)
	! ParseToken is similar to a general parse routine,
	! and returns GPR_FAIL, GPR_MULTIPLE, GPR_NUMBER,
	! GPR_PREPOSITION, GPR_REPARSE or the object number
	return _ParseToken(p_token_type, p_token_data, -1);
];

[ _ParseToken p_pattern_pointer p_parse_pointer _noun _i _token
		_token_type _token_data _old_wn _j _k _parse_plus_2 _num_already_added;
	! ParseToken is similar to a general parse routine,
	! and returns GPR_FAIL, GPR_MULTIPLE, GPR_NUMBER,
	! GPR_PREPOSITION, GPR_REPARSE or the object number
	! However, it also taks the current grammar token as input
	! while a general parse routine takes no arguments.
	! (this is mostly to avoid recalculating the values from wn
	! when the calling routine already has them at hand)
	object_token_type = -1;
	_parse_plus_2 = parse + 2;
	if(_noun < 0) {
		! called from ParseToken (DM library API)
		! since third argument is -1 instead of default 0
		_token = p_pattern_pointer;
		_token_data = p_parse_pointer;
		p_parse_pointer = _parse_plus_2 + 4 * (wn - 1);
		p_pattern_pointer = 0;
	} else {
		_token = (p_pattern_pointer -> 0);
		_token_data = (p_pattern_pointer + 1) --> 0;
	}
	_token_type = _token & $0f;
	if(_token_type ~= TT_PREPOSITION or TT_SCOPE) _UpdateScope(actor);
	! first set up filters, if any
	noun_filter = 0;
	if(_token_type == TT_ROUTINE_FILTER) {
		noun_filter = _token_data;
		_token_type = TT_OBJECT;
		_token_data = NOUN_OBJECT;
	} else if(_token_type == TT_ATTR_FILTER) {
		noun_filter = 1 + _token_data;
		_token_type = TT_OBJECT;
		_token_data = NOUN_OBJECT;
	} else if(_token_type == TT_SCOPE) {
		_token_type = TT_OBJECT;
		scope_routine = _token_data;
		! check what type of routine (single or multi)
		scope_stage = 1;
		_i = indirect(scope_routine);
		if(_i == 1)
			_token_data = MULTI_OBJECT;
		else
			_token_data = NOUN_OBJECT;
		! trigger add to scope
		scope_stage = 2;
		_UpdateScope();
		! not a real error, just to make sure that phase2 runs
		phase2_necessary = PHASE2_SCOPE;
	} else if(_token_type == TT_PARSE_ROUTINE) {
		! allow the 'general parsing routine' to do all instead.
		! it returns object or GRP_FAIL, ...; just like _ParseToken
		return  indirect(_token_data);
	}
	! then parse objects or prepositions
	if(_token_type == TT_PREPOSITION) {
#IfDef DEBUG_PARSETOKEN;
		print "Preposition: _token ", _token, " _token_type ", _token_type, ": data ", _token_data;
		if(_token_data > 1000) {
			print " '", (address) _token_data, "'";
		} else {
			print " (this is not a preposition!)";
		}
		@new_line;
#EndIf;
		if(p_parse_pointer --> 0 == _token_data) {
#IfDef DEBUG_PARSETOKEN;
			print "Match!^";
#EndIf;
			wn++;
			return GPR_PREPOSITION;
		}
#IfDef DEBUG_PARSETOKEN;
		print "Failed prep: ", p_parse_pointer, ": ", (address) p_parse_pointer --> 0, " doesn't match ", (address) _token_data, "^";
#EndIf;
		return GPR_FAIL;
	} else if(_token_type == TT_OBJECT) {
		! here _token_data will be one of
		! NOUN_OBJECT, HELD_OBJECT, MULTI_OBJECT, MULTIHELD_OBJECT,
		! MULTIEXCEPT_OBJECT, MULTIINSIDE_OBJECT, CREATURE_OBJECT,
		! SPECIAL_OBJECT, NUMBER_OBJECT or TOPIC_OBJECT
		!
		! remember if except or inside found, so we can filter later
		object_token_type = _token_data;
		if(_token_data == MULTI_OBJECT or MULTIHELD_OBJECT or MULTIEXCEPT_OBJECT or MULTIINSIDE_OBJECT) {
			parser_check_multiple = _token_data;
		}

		if(_token_data == NOUN_OBJECT or HELD_OBJECT or CREATURE_OBJECT) {
			if(_PeekAtNextWord() == ALL_WORD or EXCEPT_WORD1 or EXCEPT_WORD2) {
				! we don't accept all/all-but with held or creature
				if(scope_stage == 2 && parser_phase == PHASE2) {
					PrintMsg(MSG_PARSER_NOT_MULTIPLE_VERB);
				}
				return GPR_FAIL;
			}
			_noun = _GetNextNoun(p_parse_pointer, true);
			if(_noun == -2) {
				return GPR_FAIL;
			}
			if(_noun == -1) {
				return GPR_REPARSE;
			}
			if(_noun == 0) {
				! Here we got a list of nouns when we only expected
				! one. Let's give ChooseObjectsFinal a chance
				! to reduce the numbers before failing
#Ifdef ChooseObjectsFinal;
				parser_one = which_object -> 0;
				if(parser_one > 1) {
					ChooseObjectsFinal(which_object + 2, parser_one);
				}
				if(parser_one == 1) {
					_noun = which_object --> 1;
				} else {
#Endif;
					parser_unknown_noun_found = p_parse_pointer;
					return GPR_FAIL;
#Ifdef ChooseObjectsFinal;
				}
#Endif;
			}
			p_parse_pointer = _parse_plus_2 + 4 * (wn - 1);
			if(_token_data == CREATURE_OBJECT && _CreatureTest(_noun) == 0)  {
				phase2_necessary = PHASE2_ERROR;
				if(parser_phase == PHASE2) {
					PronounNotice(_noun);
					PrintMsg(MSG_PARSER_ONLY_TO_ANIMATE);
				}
				return GPR_FAIL;
			}
			if(_token_data == HELD_OBJECT && actor == player && _noun notin actor) {
				phase2_necessary = PHASE2_ERROR;
				if(parser_phase == PHASE2) {
#Ifdef DisallowTakeAnimate;
					if(_noun hasnt scenery or static &&
							(_noun hasnt animate || DisallowTakeAnimate(_noun) == false) &&
							ImplicitGrabIfNotHeld(_noun)) {
						return GPR_FAIL;
					}
#Ifnot;
					if(_noun hasnt scenery or static or animate && ImplicitGrabIfNotHeld(_noun)) {
						return GPR_FAIL;
					}
#Endif;
				}
			}
			return _noun;
		} else if(_token_data == MULTI_OBJECT or MULTIHELD_OBJECT or MULTIEXCEPT_OBJECT or MULTIINSIDE_OBJECT) {
			for(::) {
				_old_wn = wn;
				_noun = _GetNextNoun(p_parse_pointer, false);
				if(_noun == -2) {
					return GPR_FAIL;
				}
				if(_noun == -1) {
					return GPR_REPARSE;
				}
				if(_noun == 0 || parser_all_found) {
					! Here it is either a plural, 'all' or not understood
					!
					if(parser_action == ##PluralFound) {
						! take books [but xxx] or take all books [but xxx]
						parser_all_found = true;

						! There could be nouns that have already been added
						! (such as 'take blue and books' - and multiple_objects
						! will contain BlueBook here). Keep track of this
						_num_already_added = multiple_objects-->0;

						! copy which_object to multiple_objects
						for(_i = 0: _i < which_object->0: _i++) {
							! don't add if already in multiple_objects
							_k = 1;
							for(_j = 0: _j < multiple_objects-->0: _j++) {
								if(multiple_objects --> _j == which_object--> (_i + 1)) {
									_k = 0;
									! this was already added
									!print _i, " ", _j, " ", (the) multiple_objects --> _j, " == ", (the) which_object--> (_i + 1), "^";
								}
							}
							if(_k) {
								multiple_objects --> 0 = 1 + (multiple_objects --> 0);
								multiple_objects --> (multiple_objects --> 0) = which_object--> (_i + 1);
							}
						}
#IfDef DEBUG_PARSETOKEN;
						print "adding plural ", which_object->0, " ", which_object->1, " ", multiple_objects --> 0, "^";
#Endif;
						! check if 'take all Xs but Y'
						p_parse_pointer = _parse_plus_2 + 4 * (wn - 1);
						if(_PeekAtNextWord() == EXCEPT_WORD1 or EXCEPT_WORD2) {
							wn = wn + 1;
							! here we only want to consider the Xs
							! so we clear scope and copy Xs into scope
							! before GetNextNoun. Later we need to restore scope
							scope_objects = multiple_objects --> 0;
							for(_noun = 0: _noun < multiple_objects --> 0 : _noun++) {
								scope-->_noun = multiple_objects-->(_noun + 1);
							}
							_noun = _GetNextNoun(p_parse_pointer + 4, false);
							scope_modified = true; ! restore scope
							if(_noun <= 0) {
								if(parser_phase == PHASE2) {
									PrintMsg(MSG_PARSER_NOTHING_TO_VERB, wn);
								}
								return GPR_FAIL;
							}
							! is the exception object included in the
							! current NP? If it refers to one of the
							! objects that were already declared before
							! we started parsing this NP (e.g. get books
							! but X), then it should not be used since
							! the "but" object only refers to this NP.
							_k = 1;
							for(_i = 0: _i < _num_already_added: _i++) {
								if(_noun == multiple_objects-->(_i + 1)) {
									_k = 0;
								}
							}
							if(_k) {
								parser_all_except_object = _noun;
							}
							! allow 'take all Xs but Y one'
							p_parse_pointer = _parse_plus_2 + 4 * (wn - 1);
							if(_PeekAtNextWord() == 'one') {
								wn = wn + 1;
							}
						}
						! check if get [all] Xs (and|,) Y...
						if(_PeekAtNextWord() == comma_word or AND_WORD) {
							! check the next word after "and" or comma
							if((wn + 1) > parse->1) {
								!print "and followed by nothing^";
								! there is no word, so the pattern fails
								return GPR_FAIL;
							}
							_i = (_parse_plus_2 + 4*wn ) --> 0; ! Word value
							if(_i ~= 0 && _i -> DICT_BYTES_FOR_WORD & 1 == 0) {
								! this is not a verb so we assume it is a list
								! of nouns instead. Continue to parse
								++wn;
								p_parse_pointer = _parse_plus_2 + 4 * (wn - 1);
								!print "and followed by a noun^";
								parser_all_found = false;
								continue;
							}
							!print "and followed by a verb^";
						}

						return GPR_MULTIPLE;
					}
					if(p_parse_pointer-->0 == ALL_WORD) {
						! take all etc.
						! note that 'all' has already updated
						! wn in GetNextNoun
						!
						! Add all reasonable objects in scope
						! to the multiple_objects array
						p_parse_pointer = _parse_plus_2 + 4 * (wn - 2);
						if(p_parse_pointer-->0 == EXCEPT_WORD1 or EXCEPT_WORD2) {
							!print "take all but^";
							if(parser_phase == PHASE2) {
								PrintMsg(MSG_PARSER_UNKNOWN_SENTENCE);
							}
							return GPR_FAIL;
						}
						p_parse_pointer = _parse_plus_2 + 4 * (_old_wn);
						if(p_parse_pointer-->0 == EXCEPT_WORD1 or EXCEPT_WORD2) {

							!print "take all but <noun>^";
							parser_all_except_object = _noun;
						} else if(_noun > 0) {
							!print "take all <noun>^";
							return _noun;
						}

						_AddMultipleNouns(_token_data);
						parser_all_found = true;
						if(multiple_objects --> 0 == 0) {
							if(parser_phase == PHASE2) {
								PrintMsg(MSG_PARSER_NOTHING_TO_VERB, wn);
								return GPR_FAIL;
							}
							return GPR_MULTIPLE;
						} else if(multiple_objects --> 0 == 1) {
							! single object
							_noun = multiple_objects --> 1;
							return _noun;
						} else {
							! multiple objects
							return GPR_MULTIPLE;
						}
					}
					parser_unknown_noun_found = p_parse_pointer;
					return GPR_FAIL;
				}
				! adding a single object - check if it's already there
				_j = false;
				_k = multiple_objects-->0;
				if(_k > 0) {
					for(_i = 1: _i <= _k: _i++) {
						if(multiple_objects-->_i == _noun) {
							_j = true;
							break;
						}
					}
				}
				! If _j == false, the object is not in the list, so add it!
				if(_j == false) {
					p_parse_pointer = _parse_plus_2 + 4 * (wn - 1);
					multiple_objects --> 0 = 1 + (multiple_objects --> 0);
					multiple_objects --> (multiple_objects --> 0) = _noun;
				}
				! check if we should continue: and or comma followed by a noun
				if(_PeekAtNextWord() == comma_word or AND_WORD) {
					! check the next word after "and" or comma
					if((wn + 1) > parse->1) {
						!print "and followed by nothing^";
						! there is no word, so the pattern fails
						return GPR_FAIL;
					}
					_i = (_parse_plus_2 + 4*wn ) --> 0; ! Word value
					if(_i ~= 0 && _i -> DICT_BYTES_FOR_WORD & 1 == 0) {
						! this is not a verb so we assume it is a list
						! of nouns instead. Continue to parse
						++wn;
						p_parse_pointer = _parse_plus_2 + 4 * (wn - 1);
						!print "and followed by a noun^";
						continue;
					}
					!print "and followed by a verb^";
				}
				break;
			}
			if(multiple_objects --> 0 == 0) {
				! no nouns found, so this pattern didn't match
				return GPR_FAIL;
			}
			return GPR_MULTIPLE;
		} else if(_token_data == TOPIC_OBJECT) {
			! topic continues until end of line or
			! until the word matches the preposition
			! defined in the next pattern(s)
			! p_pattern_pointer now points at <topic>
			! p_pattern_pointer + 3 could be the preposition
			!print (p_pattern_pointer + 3) -> 0, "^"; ! token
			!print (p_pattern_pointer + 4) --> 0, "^"; ! token_data
			consult_words = 0;
			if(p_pattern_pointer ~= 0) {
				! loop over all possible prepositions and update wn
				! if topic and preposition matched
				for(_i = 3: (p_pattern_pointer + _i)->0 >= TOKEN_SINGLE_PREP: _i = _i + 3) {
					!print (address) (p_pattern_pointer + _i + 1) --> 0, "^";
					if(_ParseTopic(wn, p_parse_pointer, (p_pattern_pointer + _i + 1) --> 0)) {
						break;
					}
				}
			}
			if(consult_words == 0) {
				! topic with preposition wasn't found, so the
				! remaining part of the sentence as topic
				_ParseTopic(wn, p_parse_pointer, -1);
			}
			wn = wn + consult_words;
			return GPR_NUMBER;
		} else if(_token_data == SPECIAL_OBJECT) {
			parsed_number = TryNumber(wn);
			special_word = NextWord(); ! will make wn++
			if(parsed_number == -1000) parsed_number = special_word;
			return GPR_NUMBER;
		} else if(_token_data == NUMBER_OBJECT) {
			parsed_number = TryNumber(wn++);
			return GPR_NUMBER;
		}
	}
];

[ _AddMultipleNouns p_multiple_objects_type   _i _addobj _obj _p _ceil;
	multiple_objects --> 0 = 0;
	for(_i = 0: _i < scope_objects: _i++) {
		_obj = scope-->_i;
		_addobj = false;
		switch(p_multiple_objects_type) {
		! MULTIEXCEPT_OBJECT, MULTIINSIDE_OBJECT:
		! we don't know yet know what 'second' is, so we
		! add all reasonable objects and filter later
		MULTIHELD_OBJECT, MULTIEXCEPT_OBJECT:
			_addobj = _obj in actor && _obj hasnt worn;
		MULTI_OBJECT:
			_p = parent(_obj);
			_ceil = TouchCeiling(player);
			_addobj = false;
			if((_p == _ceil || (_p ~= 0 && _p in _ceil && _p has scenery or static && _p hasnt concealed && _p has container or supporter)) && _obj hasnt scenery or concealed or static or animate)
				_addobj = true;
			if(scope_stage == 2) _addobj = true; ! added by scope routine
		MULTIINSIDE_OBJECT:
			_p = parent(_obj);
			_ceil = TouchCeiling(player);
			_addobj = false;
			if(_p ~= 0 && _p has container or supporter && _obj hasnt scenery or concealed or static or animate)
				_addobj = true;
!			_addobj = _obj hasnt scenery or concealed or static or animate &&
!				(_p == 0 || parent(_p) == 0 || _p has container or supporter);
		}
		if(action == ##Take && _obj in player) _addobj = false;
#Ifdef ChooseObjects;
		! give ChooseObjects a chance to override
		switch(ChooseObjects(_obj, _addobj)) {
			2: _addobj = 0; ! force rejection
			1: _addobj = 1; ! force acceptance
		}
#Endif;
		if(_addobj) {
			multiple_objects --> 0 = 1 + (multiple_objects --> 0);
			multiple_objects --> (multiple_objects --> 0) = _obj;
			!print "Adding ", (name) _obj, "^";
		}
	}
];

[ _PrintPartialMatch p_start p_stop _start _stop _i;
	_i = (parse-2+(4*p_start));
	_start = _i->3; ! index to input line for first word
	if(p_stop > parse -> 1) {
		_stop = buffer->0; ! until the end of the input
	} else {
		_i = (parse-2+(4*p_stop));
		_stop = _i->2 + _i->3; ! until the index of the stop word + its length
	}
	for(_i = _start: _i < _stop: _i ++) {
		if(buffer -> _i == 0) break;
		print (char) buffer -> _i;
	}
];

[ _PrintUnknownWord _i;
	for(_i = 0: _i < parser_unknown_noun_found->2: _i++) {
		print (char) buffer->(_i + parser_unknown_noun_found->3);
	}
];


#IfDef OPTIONAL_GUESS_MISSING_NOUN;
Constant GUESS_CREATURE = 0;
Constant GUESS_HELD = 1;
Constant GUESS_CONTAINER = 2;
Constant GUESS_THING = 3;
Constant GUESS_DOOR = 4;
Array guess_object-->5;
[ _GuessMissingNoun p_type p_prep p_nounphrase_num _assumed _exclude _i _noun
	_door_count _container_count _creature_count _held_count _thing_count;

	if(p_nounphrase_num == 1) {
		_assumed = noun;
	} else {
		_assumed = second;
		if(_assumed == 0) _assumed = noun;
		_exclude = noun;
	}

	for(_i = 0: _i < scope_objects: _i++) {
		_noun = scope-->_i;
		if(_noun == player || _noun has concealed ||
			TestScope(_noun, player) == false) {
			continue;
		}
		if(_noun has door && _noun ~= _exclude) {
			guess_object-->GUESS_DOOR = _noun;
			_door_count++;
		}
		if(_noun has container && _noun ~= _exclude) {
			guess_object-->GUESS_CONTAINER = _noun;
			_container_count++;
		}
		if(_noun has animate && _noun ~= _exclude) {
			guess_object-->GUESS_CREATURE = _noun;
			_creature_count++;
		}
		if(_noun in actor && _noun ~= _exclude) {
			guess_object-->GUESS_HELD = _noun;
			_held_count++;
		}
		if(_noun hasnt scenery && _noun ~= _exclude) {
			guess_object-->GUESS_THING = _noun;
			_thing_count++;
		}
	}

	_noun = 0;
	switch(p_type) {
	HELD_OBJECT:
		if(_held_count == 1)
			_noun = guess_object-->GUESS_HELD;
	CREATURE_OBJECT:
		if(_creature_count == 1)
			_noun = guess_object-->GUESS_CREATURE;
	TOPIC_OBJECT:
		! we can't guess anything when parsing a topic
	default:
		if(_noun == 0 && _container_count == 1 &&
			action == ##Open or ##Close) {
			_noun = guess_object-->GUESS_CONTAINER;
		}
		if(_noun == 0 && _door_count == 1 &&
			action == ##Lock or ##Unlock or ##Open or ##Close) {
			_noun = guess_object-->GUESS_DOOR;
		}
		if(_noun == 0 && _thing_count == 1) {
			_noun = guess_object-->GUESS_THING;
		}
	}

	if(_noun == _assumed) _noun = 0;
	if(_noun) {
		print "(assuming ";
		if(p_prep) {
			print (address) (p_prep+1) --> 0, " ";
		}
		print (the) _noun, ")^";
	}
	return _noun;
];

#EndIf;

[ _ParsePattern p_pattern _parse_pointer _noun _i _j _k _word _type _current_wn _old_dir_index;
	! Check if the current pattern will parse, with side effects if PHASE2
	! _ParsePattern will return:
	!   -1 if need to reparse
	!   0..99 how many words were matched before the match failed
	!   100 if perfect match
	wn = verb_wordnum + 1;
	_parse_pointer = parse + 2 + 4*(verb_wordnum);
	pattern_pointer = p_pattern - 1;
	num_noun_groups = 0;
	noun = 0;
	consult_from = 0;
	consult_words = 0;
	inp1 = 0;
	inp2 = 0;
	special_number = 0;
	special_word = 0;
	parsed_number = 0;
	multiple_objects --> 0 = 0;
	parser_check_multiple = 0;
	parser_unknown_noun_found = 0;
	parser_all_found = false;
	parser_all_except_object = 0;
	selected_direction_index = 0;
	selected_direction = 0;
	action = (p_pattern --> 0) & $03ff;
	action_to_be = action; ! compatibility (referenced in DM4 for ChooseObjects)
	action_reverse = ((p_pattern --> 0) & $400 ~= 0);
	phase2_necessary = PHASE2_SUCCESS;

	while(true) {
		pattern_pointer = pattern_pointer + 3;
#IfDef DEBUG_PARSEPATTERN;
		print "TOKEN: ", pattern_pointer -> 0, " wn ", wn, " _parse_pointer ", _parse_pointer, "^";
#EndIf;

		scope_stage = 0;
		_type = ((pattern_pointer -> 0) & $0f);
		if(_type == TT_END) {
			if(_IsSentenceDivider(_parse_pointer)) {
				! check if dictionary word after sentence divider
				if(parse->1 > wn && (_parse_pointer + 4)-->0 == 0) {
					! uknown word, so probably an unknown word in a
					! list matching the multi token, such as
					! 'get box and SDASDASD'
					if(parser_phase == PHASE2) {
						parser_unknown_noun_found = _parse_pointer + 4;
						PrintMsg(MSG_PARSER_DONT_UNDERSTAND_WORD);
					} else {
						phase2_necessary = PHASE2_ERROR;
					}
					return wn;
				} else if(parse->1 > wn && ((((_parse_pointer + 4)-->0) + DICT_BYTES_FOR_WORD)->0 & 1) == 0) {
					_current_wn = wn;
					wn++;
					if(Directions.parse_name()) {
						return 100; ! Next is a direction, so this is fine
					}
					! neither a verb nor a direction, so probably a list
					! of nouns without a matching multi token
					if(parser_phase == PHASE2) {
						PrintMsg(MSG_PARSER_NOT_MULTIPLE_VERB);
					} else {
						phase2_necessary = PHASE2_ERROR;
					}
					return _current_wn;
				} else {
					wn++;
					return 100; ! pattern matched
				}
			}
			!if(_IsSentenceDivider(_parse_pointer)) {
			!	wn++;
			!	return 100; ! pattern matched
			!}
			if(wn == 1 + parse->1) {
				return 100; ! pattern matched
			}
			! Fail because the grammar line ends here but not the input
			if(parser_phase == PHASE2) {
				! last resort when no other error message printed
				PrintMsg(MSG_PARSER_UNKNOWN_SENTENCE);
			}
			return wn - verb_wordnum;
		}

		! parse_routine doesn't match anything and is always allowed
		if(wn >= 1 + parse->1 && _type ~= TT_PARSE_ROUTINE) {
#IfDef DEBUG_PARSEPATTERN;
			print "Fail, since grammar line has not ended but player input has.^";
#EndIf;
			if(parser_phase == PHASE2) {
				!print "You need to be more specific.^";
				if(_FixIncompleteSentenceOrComplain(p_pattern - 1)) {
					! sentence was corrected
					return 100;
				}
			};
			return wn - verb_wordnum;!Fail because input ends here but not the grammar line
		}

#IfDef DEBUG_PARSEPATTERN;
		print "Calling ParseToken: token ", pattern_pointer->0," type ", (pattern_pointer->0) & $f, ", data ", (pattern_pointer + 1) --> 0,"^";
#EndIf;
		_current_wn = wn;
		_old_dir_index = selected_direction_index;
		_noun = _ParseToken(pattern_pointer, _parse_pointer);
		! the parse routine can change wn, so update _parse_pointer
		_parse_pointer = parse + 2 + 4 * (wn - 1);

		switch(_noun) {
		GPR_FAIL:
			if(_type == TT_PARSE_ROUTINE) {
				if(parser_phase == PHASE2) {
				    if(scope_stage == 2) {
						scope_stage = 3;
						indirect(scope_routine);
                    } else {
                        PrintMsg(MSG_PARSER_UNKNOWN_SENTENCE);
                    }
				}
				return _current_wn - 1;
			}
			if(pattern_pointer->0 == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) {
				! First or in the middle of a list of alternative prepositions
#IfDef DEBUG_PARSEPATTERN;
				print "Preposition failed, but more options available so reparsing^";
#Endif;
				continue; ! keep parsing
			}
			if(parser_unknown_noun_found ~= 0) {
				if(parser_phase == PHASE2) {
					_word = parser_unknown_noun_found --> 0;
					if(scope_routine ~= 0) {
						scope_stage = 3;
						indirect(scope_routine);
					} else if(_word ~= 0) {
						! is it one of the location.name words?
						inp1 = -1;
						@get_prop_addr location name -> _k;
						if(_k) {
							@get_prop_len _k -> _j;
		#IfV5;
							@log_shift _j (-1) -> _j;
		#IfNot;
							@div _j 2 -> _j;
		#EndIf;
							for(_i = 0: _i < _j: _i++) {
								if(_word == (_k-->_i)) {
									inp1 = _i;
								}
							}
						}
						if(inp1 > -1) {
							PrintMsg(MSG_PARSER_NO_NEED_REFER_TO);
						} else if(_word == ALL_WORD) {
							PrintMsg(MSG_PARSER_NOT_MULTIPLE_VERB);
						} else {
							PrintMsg(MSG_PARSER_NOSUCHTHING);
						}
					} else if((parser_unknown_noun_found->2) == 0) {
						PrintMsg(MSG_PARSER_UNKNOWN_SENTENCE);
					} else {
						PrintMsg(MSG_PARSER_DONT_UNDERSTAND_WORD);
					}
				} else if(phase2_necessary ~= PHASE2_SCOPE) {
					! give higher score to unknown words matches
					! so that for examine 'get goblin' and 'take goblin'
					! works the same when goblin isn't in scope.
					phase2_necessary = PHASE2_ERROR;
					wn = wn + 1;
				}
			} else {
				if(parser_phase == PHASE2) {
					if(pattern_pointer->0 == TOKEN_SINGLE_PREP) {
						PrintMsg(MSG_PARSER_UNKNOWN_SENTENCE);
					}
				}
			}
			return wn - verb_wordnum; ! pattern didn't match
		GPR_PREPOSITION:
			! advance until the end of the list of prepositions
#IfDef DEBUG_PARSEPATTERN;
			print "-- preposition mached ", pattern_pointer, " ", pattern_pointer->0, "^";
#Endif;
			_type = ((pattern_pointer -> 0) & $0f);
			while(_type ~= TT_END && _type ~= TT_PARSE_ROUTINE &&
				(pattern_pointer->0 ~= TOKEN_LAST_PREP or TOKEN_SINGLE_PREP)) {
#IfDef DEBUG_PARSEPATTERN;
			print "-- increasing pattern_pointer^";
#Endif;
				pattern_pointer = pattern_pointer + 3;
				_type = ((pattern_pointer -> 0) & $0f);
			}
		GPR_MULTIPLE:
			! multiple_objects contains the objects
			if(multiple_objects-->0 == 0) {
				_UpdateNounSecond(0, 0);
				! 'all' matched zero objects in scope. It is still a perfect
				! match of course but we need to force phase2 to write
				! a suitable message.
				phase2_necessary = PHASE2_ERROR;
			} else {
				_UpdateNounSecond(multiple_objects-->1, multiple_objects-->1);
			}
		GPR_NUMBER:
			! parsed_number contains the new number
			if(parsed_number == -1000)  {
				if(parser_phase == PHASE2) {
					PrintMsg(MSG_PARSER_BAD_NUMBER);
				}
				return wn - verb_wordnum; ! bad match
			}
			_UpdateNounSecond(parsed_number, 1);
		GPR_REPARSE:
			return -1; ! the player_input and parse have changed
		default:
			! _noun was a valid noun
			if(_noun == Directions) {
				if(noun == Directions) {
					! This is second, and noun is already Directions.
					if(parser_phase == PHASE2)
						PrintMsg(MSG_PARSER_NOT_MULTIPLE_DIRS);
					phase2_necessary = PHASE2_ERROR;
					return wn - verb_wordnum;
				}
			} else {
				selected_direction_index = _old_dir_index;
				selected_direction =
					direction_properties_array -> selected_direction_index;
			}
			_UpdateNounSecond(_noun, _noun);
		}
	}
	! we should never reach this line
	! the while(true) loop is only exited by return statements
];

[ PronounNotice p_object;
	if(p_object == 0 or player or Directions) return;
	if(p_object has pluralname) {
		themobj = p_object;
	} else if(p_object has animate) {
		if(p_object has female) herobj = p_object;
		else if(p_object has neuter) itobj = p_object;
		else himobj = p_object;
	} else itobj = p_object;
	!print "he ", himobj, " she ", herobj, " it ", itobj, "^";
];

[ _ParseAndPerformAction _word_data _verb_grammar _i _j _pattern _noun _score _best_score _best_pattern _best_phase2 _best_second _action _verb_offset _selected_direction _selected_direction_index;
	! returns
	! 1/true: if error was found
	! -n: if <n> words were used to find a match,
	!
	! taking periods and other sentence breaks into account.
	! For example, if the input is "l.l" then the parser
	! will stop after the first "l" has been mached, and
	! 1 is returned. If the input is "open box" then
	! the whole input is matched and 2 returned.

	multiple_objects-->0 = 0;
	selected_direction_index = 0;
	selected_direction = 0;
	action = -1;
	meta = false;
	which_object->1 = 0;
	actor = player;
	noun = 0; ! needed since _ParsePattern not always called
	second = 0;
	consult_from = 0;
	consult_words = 0;
	usual_grammar_after = 0;
	_verb_offset = 0;
	scope_routine = 0;
	noun_filter = 0;
	object_token_type = -1;

	verb_wordnum = 1;

	if(_IsSentenceDivider(parse + 2) || parse->1 < 1) {
		PrintMsg(MSG_PARSER_NO_INPUT);
		return -1;
	}

._perform_reparse;
	verb_word = (parse - 2) --> (2 * verb_wordnum);
._perform_reparse_2;
	if(UnsignedCompare(verb_word, (HDR_DICTIONARY-->0)) == -1) {
		! Not a verb. Try the entry point routine before giving up
		verb_word = UnknownVerb(verb_word);
		if(verb_word == 0) {
			! unknown word
#IfDef DEBUG_PARSEANDPERFORM;
			print "Case 1, Word ", verb_word, "^";
#EndIf;
			jump _first_word_unknown;
		}
	}

	_word_data = verb_word + DICT_BYTES_FOR_WORD;
	! check if it is a direction
	if((_word_data->0) & 1 == 0) { ! This word does not have the verb flag set.
		! try a direction instead
		wn = verb_wordnum;
		_i = Directions.parse_name();
		if(_i) {
			wn = wn + _i; ! number of words in direction command
			! check if separator or end of line
			_i = wn - 1; ! keep for error message since wn changed by NextWord
			_pattern = NextWord();
			if(_pattern == 0 or comma_word or THEN1__WD) {
				action = ##Go;
				noun = Directions;
				inp1 = Directions;
				jump _parsing_was_successful;
			}
			! bad direction command, such as "n n"
			PrintMsg(MSG_PARSER_PARTIAL_MATCH, _i);
			rtrue;
		}
		! not a direction, check if beginning of a command
		_noun = _ParseNounPhrase(parse+2, false);
		if(_noun > 0 && verb_wordnum == 1) {
			! The sentence starts with a noun, now
			! check if comma afterwards
			wn = wn + which_object->1;
			_pattern = NextWord();
			if(_pattern == comma_word) {
				jump _this_is_conversation;
			}
		}
		! fall through to first_word_unknown below
		!jump first_word_unknown;

._first_word_unknown;
		if(actor ~= player) jump _treat_bad_line_as_conversation;
		if(parse-->3 == ',//') { PrintMsg(MSG_PARSER_UNKNOWN_PERSON); rtrue; }
		PrintMsg(MSG_PARSER_UNKNOWN_VERB);
		rtrue;


._this_is_conversation;
		if(_noun hasnt animate && _noun hasnt talkable) {
			PrintMsg(MSG_PARSER_CANT_TALK, _noun);
			rtrue;
		}
		! See http://www.inform-fiction.org/manual/html/s18.html
		! set actor
		actor = _noun;
		verb_wordnum = wn;
		verb_word = (parse - 2) --> (2 * verb_wordnum);
		if(actor provides grammar) {
			parser_one = 0;
			_i = actor.grammar();
			! 0 = carry on as usual
			! 1 = grammar parsed it completely
			! verb = use this verb's grammar instead
			! -verb = try using this verb's grammar first, then the original
			if((_i ~= 0 or 1) &&
				(parser_one ~= 0 ||
				(UnsignedCompare(_i, dict_start) < 0 ||
				UnsignedCompare(_i, dict_end) >= 0 ||
				(_i - dict_start) % dict_entry_size ~= 0))) {
				! returned -'verb'
				usual_grammar_after = verb_word;
				_i = -_i;
			}
			if(_i == 1) {
				++wn; ! to account for the correctly parsed verb
				jump _parsing_was_successful;
			}
			if(_i ~= 0) {
				! _i == 'verb', so use its grammar instead
				verb_word = _i;
				! decrease verb_wordnum to try the new grammar rule,
				! but keep track of the offset so we can
				! restore in case the rule fails.
				_verb_offset = 1;
				verb_wordnum = verb_wordnum - _verb_offset;
				jump _perform_reparse_2;
			}
		}
		jump _perform_reparse;
	}

	! Now it is known word, and it is not a direction, in the first position
	meta = (_word_data->0) & 2;

!   print "Parse array: ", parse, "^";
!   print "Word count: ", parse->0, "^";
!   print "Word 1: ", (parse + 2)-->0, "^";
!   print "Word 2: ", (parse + 6)-->0, "^";
!   print "Word 3: ", (parse + 10)-->0, "^";
	_i = 255 - (_word_data->1); ! was _verb_num
	_verb_grammar = (HDR_STATICMEMORY-->0)-->_i;

#IfDef DEBUG_PARSEANDPERFORM;
	print "Verb#: ",_i,", meta ",meta,".^";
	print "Grammar address for this verb: ",_verb_grammar,"^";
	print "Number of patterns: ", _verb_grammar->0 ,"^";

	! First print all patterns, for debug purposes
	_pattern = _verb_grammar + 1;
	for(_i = 0 : _i < _verb_grammar->0: _i++) {
		print "############ Pattern ",_i," ",_pattern,"^";
		_pattern = _PrintGrammarPattern(_pattern);
	}
	@new_line;
#EndIf;

	! Phase 1: look for best pattern without side effects
	_best_score = 0;
	_best_pattern = 0;
	parser_phase = PHASE1;
	_pattern = _verb_grammar + 1;
	for(_i = 0 : _i < _verb_grammar->0 : _i++) {
#IfDef DEBUG_PARSEANDPERFORM;
		print "### PHASE 1: Pattern ",_i," address ", _pattern, "^";
#EndIf;
		_score = _ParsePattern(_pattern);

		! Special rule to convert AskTo to action, topic
		if(action == ##AskTo && _score == 100) {
			_noun = noun;
			wn = consult_from;
			jump _this_is_conversation;
		}

#IfDef DEBUG_PARSEANDPERFORM;
		print "### PHASE 1: result ", _score, " phase2_necessary ", phase2_necessary, " wn ", wn, "^";
#EndIf;
		! note that _ParsePattern will never return -1 in PHASE1
		if(_score == 0) {
			! This pattern has failed.
#IfDef DEBUG_PARSEANDPERFORM;
			print "Pattern didn't match.^";
#EndIf;
		} else if(
				(_score > _best_score &&
				((_best_phase2 ~= PHASE2_SCOPE && _score ~= 100) ||
				_score == 100)
				)
				||
				(_score <= _best_score && phase2_necessary == PHASE2_SCOPE)
				||
				(
				! we override previous best if this pattern is equally
				! good but it doesn't require reparsing (it will
				! produce some error message in phase 2), and
				! the previously best pattern did.
				_score == _best_score &&
				_best_phase2 > 0 &&  ! if previous best requested reparsing
				phase2_necessary == 0 &&  ! and we don't request reparsing
				consult_from == 0 ! and this pattern didn't include 'topic'
				)
				||
				(
				! we override previous best if this pattern is equally
				! good but shorter, so that for example get without a
				! noun picks "get multi" instead of "get 'out' 'off' noun"
				_score == _best_score && _best_pattern ~= 0 &&
				_PatternRanking(_pattern) < _PatternRanking(_best_pattern)
				)
				)
			{
			_best_score = _score;
			_best_pattern = _pattern;
			_best_phase2 = phase2_necessary;
			_best_second = second; ! need to save for multiexcept in phase 2
#IfDef DEBUG_PARSEANDPERFORM;
		print "### PHASE 1: new best pattern ", _i, " ", _best_phase2, " ", _score, "^";
#EndIf;
			! check if pefect match found
			if(_best_score == 100) break;
		}

		! Scan to the end of this pattern
		_j  = _pattern + 2;
		while(_j  -> 0 ~= TT_END) {
			_j  = _j  + 3;
		}
		_pattern = _j  + 1;
	}

	! skip phase 2 if last pattern matched perfectly
	! (since all data is then already setup and there
	! are no side effects to consider)
	parser_phase = PHASE2;
#IfDef DEBUG_PARSEANDPERFORM;
	print "### After phase 1, _best_score = ", _best_score, ", _best_phase2 = ", _best_phase2, "^";
#EndIf;
	if(_best_score == 100 && _best_phase2 == PHASE2_SUCCESS) {
#IfDef DEBUG_PARSEANDPERFORM;
		print "### Skipping phase 2^";
#EndIf;
		jump _parsing_was_successful;
	}
	if(_verb_offset ~= 0) {
		! restore the normal verb_wordnum after trying a
		! rule suggested by the NPC's grammar property
		! which, at this point, has failed.
		verb_wordnum = verb_wordnum + _verb_offset;
		_verb_offset = 0;
	}
	if(usual_grammar_after ~= 0) {
		! we have parsed a grammar given by the grammar property
		! which failed, so we should reparse using the original
		! grammar instead.
		verb_word = usual_grammar_after;
		usual_grammar_after = 0;
		jump _perform_reparse_2;
	}
	if(player ~= actor) jump _treat_bad_line_as_conversation;
	if(_best_score < parse->1) {
		if(_best_score == 0) {
			PrintMsg(MSG_PARSER_UNKNOWN_SENTENCE);
		} else if(_best_phase2 > 0) {
			! call again to generate suitable error message
			second = _best_second;
			_score = _ParsePattern(_best_pattern);
		} else {
			! parser_unknown_word is set when we tried to parse
			! a noun but were found a word that was didn't match
			! any object in scope. This word can be completely
			! crazy (not in the dictionary) or a valid word in
			! another context but not right now. Reasons may
			! be that it matches something that isn't in scope,
			! or this word isn't a noun word.
			if(parser_unknown_noun_found ~= 0 &&
				parser_unknown_noun_found-->0 == 0) {
				! this is not a dictionary word.
				PrintMsg(MSG_PARSER_DONT_UNDERSTAND_WORD);
			} else if((((_best_pattern - 1 + wn*3 )-> 0) & $0f) == TT_END) {
				! the sentence matched the pattern
				if((parse - 2 + wn*4)-->0 == ALL_WORD) {
					PrintMsg(MSG_PARSER_NOT_MULTIPLE_VERB);
				} else {
					PrintMsg(MSG_PARSER_NOSUCHTHING);
				}
			} else {
				! we didn't match the pattern at all
				_i = ((_best_pattern - 1 + wn*3 )-> 0) & $0f;
				if(_i == TT_PREPOSITION or TT_OBJECT) {
					! missing preposition
					_PrintPatternSyntax(_best_pattern -1);
				} else {
					! some other problem
					PrintMsg(MSG_PARSER_PARTIAL_MATCH, wn - 1);
				}
			}
		}
		action = 0;
		rtrue;
	}

	! Phase 2: reparse best pattern and ask for additional info if
	! needed (which book? etc)

#IfDef DEBUG_PARSEANDPERFORM;
	print "### PHASE 2: Pattern address ", _best_pattern, "^";
#EndIf;
	_score = _ParsePattern(_best_pattern);
#IfDef DEBUG_PARSEANDPERFORM;
	print "### PHASE 2: result ", _score, "^";
#EndIf;
	if(_score == -1) rfalse; ! force a complete reparse
	if(_score == 100) jump _parsing_was_successful;
	action = -1; ! to stop each_turn etc.
	rtrue; ! ParsePattern wrote some error message

._treat_bad_line_as_conversation;
	! this is used when not understood and the actor is an NPC
	action = ##NotUnderstood;
	consult_from = verb_wordnum; ! usually 3 (<person> , <command>)
	consult_words = parse->1 - consult_from + 1;
	second = actor;
	wn = consult_from;
	special_number = TryNumber(wn);
	special_word = NextWord();
	! fall through to jump parse_success;

._parsing_was_successful;
	! we want to return how long the successfully sentence was
	! but wn can be destroyed by action routines, so store in _i

	num_words_parsed = -(wn - 1);
	if(action_reverse) {
		_i = second;
		second = noun;
		noun = _i;
		inp1 = noun;
		inp2 = second;
	}

	! do some special transformations
	if(action == ##Tell && noun == player && actor ~= player) {
		! Convert "P, tell me about X" to "ask P about X"
		noun = actor; actor = player; action = ##Ask;
	}
	if(action == ##AskFor && noun ~= player && actor == player) {
		! Convert "ask P for X" to "P, give X to me"
		actor = noun; noun = second; second = player; action = ##Give;
	}

	! prepare noun and second to point at dictionary words
	! from the consult topic, if possible
	if(consult_from && action == ##Answer or ##Ask or ##Tell or ##NotUnderstood) {
		if(0 == noun or second) {
			for(_i=0 : _i < consult_words : _i++) {
				_noun = (parse-->(2 * (consult_from + _i) - 1));
				if(action == ##NotUnderstood || _noun ~= 'a//' or 'an' or 'the') {
					if(noun == 0)
						noun = _noun;
					else ! At this point we know that second is 0
						second = _noun;
					break;
				}
			}
		}
	}

	if(actor ~= player) {
		! The player's "orders" property can refuse to allow conversation
		! here, by returning true.  If not, the order is sent to the
		! other person's "orders" property.  If that also returns false,
		! then: if it was a misunderstood command anyway, it is converted
		! to an Answer action (thus "floyd, grrr" ends up as
		! "say grrr to floyd").  If it was a good command, it is finally
		! offered to the Order: part of the other person's "life"
		! property, the old-fashioned way of dealing with conversation.

		sw__var = action;
		if(_best_phase2 == PHASE2_DISAMBIGUATION || parser_all_found) {
			! the order contained a failed disambiguation
			PrintMsg(MSG_PARSER_BE_MORE_SPECIFIC);
			rtrue;
		}
		if(RunRoutines(actor, orders)) rtrue;
		if(RunRoutines(player, orders)) rtrue;
		if(action == ##NotUnderstood) {
			! convert to <answer topic second>
			action = ##Answer;
			second = actor;
			inp2 = actor;
			if(BeforeRoutines()) rtrue;
			if(RunLife(actor, action)) rtrue;
		    PrintMsg(MSG_ASK_DEFAULT);
		} else {
			if(RunLife(actor, ##Order)) rtrue;
		    PrintMsg(MSG_ORDERS_WONT, actor);
		}
		if(num_words_parsed < 0) return -num_words_parsed;
		return num_words_parsed;
	}

	if(multiple_objects --> 0 == 0) {
		! single action
		if(inp1 > 1) PronounNotice(noun);
		PerformPreparedAction();
	} else {
		! multiple action
		! (a) check the multiple list isn't empty;
		! (b) warn the player if it has been cut short because too long;
		! (c) generate a sequence of actions from the list
		!     (stopping in the event of death or movement away).
		if(parser_check_multiple == MULTIINSIDE_OBJECT && second has container && second hasnt open) {
        	PrintMsg(MSG_PARSER_CONTAINER_ISNT_OPEN, second);
		} else {
			_score = 0;
			_action = action;
			_selected_direction = selected_direction;
			_selected_direction_index = selected_direction_index;
			for(_noun = 1: _noun <= multiple_objects --> 0 : _noun++) {
				action = _action; ! This may have been altered by a previous interation for multitokens
				inp1 = multiple_objects --> _noun;
				noun = inp1;

				! disallow objects mentioned in 'all except/but X' patterns
				if(noun == parser_all_except_object) continue;

				switch(parser_check_multiple) {
				MULTIEXCEPT_OBJECT:
					! stop us from putting noun in second, for example
					! > take sack
					! > put all in sack
					! however, if this is the only object then allow it to get
					! messages like 'Cannot put something on itself.'
					if(noun == second && parser_all_found) {
						continue;
					}
				MULTIINSIDE_OBJECT:
					! stop us from trying to take things that are not in
					! the container
					! however, if this is the only object then allow it to get
					! the appropriate messages.
					if(noun notin second && parser_all_found) {
						continue;
					}
				}

				! don't try to drop things you don't carry
				if(action == ##Drop && noun notin player && parser_all_found) continue;

				! don't pick up the box when you are in it
				! however, if this is the only object then allow it to
				! get the 'you have to leave it' message.
				if(action == ##Take && noun == parent(player) && parser_all_found) continue;

				! don't pick up held objects if other objects available
				! however, if this is the only object then allow it to
				! get the 'you already have it' message.
                !if(action == ##Take && noun in player && (multiple_objects --> 0 > 1 || parser_all_found)) continue;

                ! make sure that selected_direction* is only active when
                ! the processed noun (or second) is a direction
                if(noun == Directions || second == Directions) {
                	selected_direction = _selected_direction;
                	selected_direction_index = _selected_direction_index;
				} else {
					selected_direction = 0; selected_direction_index = 0;
				}

				if(parser_all_found || multiple_objects --> 0 > 1) print (name) noun, ": ";
				if(inp1 > 1) PronounNotice(noun);
				PerformPreparedAction();
				++_score;
			}
			if(_score == 0) PrintMsg(MSG_PARSER_NOTHING_TO_VERB, wn);
		}
	}
	return num_words_parsed;
];
