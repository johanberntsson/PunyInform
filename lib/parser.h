! ######################### Parser
! PunyInform uses grammar version 2 which is easier to parse and economical

[ _ReadPlayerInput p_no_prompt _result;
! #IfV5;
!   print "Width: ", HDR_SCREENWCHARS->0,"^";
! #EndIf;
#IfV5;
	style roman;
	@buffer_mode 0;
#EndIf;
	if(p_no_prompt == false) print "> ";
	! library entry routine
	AfterPrompt();
#IfV5;
	DrawStatusLine();
	player_input_array->1 = 0;
	@aread player_input_array parse_array -> _result;
	@buffer_mode 1;
#IfNot;
	if(player in location) {
		@sread player_input_array parse_array;
	} else {
		! need to adjust location to make the status line correct
		_result = location; location = ScopeCeiling(player);
		@sread player_input_array parse_array;
		location = _result;
	}
#EndIf;
	! Set word after last word in parse array to all zeroes, so it won't match any words.
	_result = 2 * (parse_array -> 1) + 1;
	parse_array-->_result = 0;
	parse_array-->(_result + 1) = 0;

	! call library entry routine
	BeforeParsing();
];

[ YesOrNo;
    for (::) {
        _ReadPlayerInput(true);
        if(parse_array -> 1 == 1) {
        	! one word reply
            if(parse_array --> 1 == 'yes' or 'y//') rtrue;
            if(parse_array --> 1 == 'no' or 'n//') rfalse;
        }
        PrintMsg(MSG_YES_OR_NO, true);
    }
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

[ TryNumber p_wordnum _i _j _num _len _mul _d _tot _digit;
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

    _i = p_wordnum*4+1; _j = parse_array->_i; _num = _j+player_input_array; _len = parse_array->(_i-1);

    ! allow for a entry point routine to override normal parsing
    _tot = ParseNumber(_num, _len); if(_tot ~= 0) return _tot;

	_i = _len;
	_mul=1; --_len;
    for (: _len >= 0 : _len--) {
        _digit = _num->_len;
        if(_digit < '0' || _digit > '9') jump baddigit;
		_d = _digit - '0';
        _tot = _tot + _mul * _d; _mul = _mul * 10;
    }
   	if (_i > 4) return 10000;
    return _tot;
.baddigit;
	return -1000;
];

[ _CopyInputArray p_src_input_array p_dst_input_array _i;
	!_n = MAX_INPUT_CHARS + 3;
	for(_i = 0: : _i++) {
		p_dst_input_array->_i = p_src_input_array->_i;
		! abort when 0 found in the text, which starts
		! from 1 in v1-4, and from 2 in v5-v8.
		if(p_dst_input_array->_i == 0 && _i>1) break;
	}
];

[ _CopyParseArray p_src_parse_array p_dst_parse_array _n _i;
	!_n = 2 + 4 * (MAX_INPUT_WORDS + 1); 
	_n = 2 + 4*p_src_parse_array->1;
	for(_i = 0: _i < _n: _i++)
		p_dst_parse_array->_i = p_src_parse_array->_i;
];

#IfDef USEINJECT;
[ _InjectInputAndParseArray p_param_array
		_src_input_array _dst_input_array 
		_src_parse_array _dst_parse_array 
		_src_start_word _dst_start_word _word_count 
		_n _i _j _k _m _char_count _pos_diff;
	_i = p_param_array - 4;
	for(_j = 2 : _j < 9 : _j++) {
		_m = _i-->_j;
		@store [_j] _m;
	}
	_src_start_word--;
	_dst_start_word--;
	
!	print "_src_start_word=",_src_start_word,"^";
!	print "_dst_start_word=",_dst_start_word,"^";
!	print "_word_count=",_word_count,"^";
!	_src_input_array = p_param_array-->0;
!	_dst_input_array = p_param_array-->1;
!	_src_parse_array = p_param_array-->2;
!	_dst_parse_array = p_param_array-->3;
!	_src_start_word = p_param_array-->4 - 1;
!	_dst_start_word = p_param_array-->5 - 1;
!	_word_count = p_param_array-->6;

	! If we are not to copy anything, signal success
	if(_word_count == 0)
		rtrue;

	! If there isn't enough room in the destination parse array, signal failure
	if(_dst_parse_array->1 + _word_count > MAX_INPUT_WORDS)
		rfalse;
!	print "Parse array room is OK!^";

	! If there isn't enough room in the destination input array, signal failure
#IfV5;
	_n = _dst_input_array->1;
#IfNot;
	while(_dst_input_array->++_n); ! {print _dst_input_array->_n, "-";}
	_n--;
#EndIf;
	_i = 4 * (_src_start_word + _word_count); ! Last word
	_j = _src_parse_array->(5 + 4 * _src_start_word); ! Start position of first word to be copied in source input buffer
!	print "i = ", _i, "^";
!	print "Final word length: ", _src_parse_array->(_i + 2), "Final word start: ", _src_parse_array->(_i + 3), "^";	
!	print "Final char: ", _src_parse_array->(_i + 2) + _src_parse_array->(_i + 3), "First char: ", 
!			_src_parse_array->(2 + 4 * (_src_start_word - 1) + 3), "^";	
	_char_count = _src_parse_array->_i + _src_parse_array->(_i + 1) - _j + 1; ! Add one for a space after each word
!	print "n = ", _n, ", char_count = ", _char_count, "^";
	if(_n + _char_count > MAX_INPUT_CHARS)
		rfalse;
!	print "Input array room is OK!^";

	_m = _dst_parse_array->(5 + 4 * _dst_start_word);  ! Position where new words are to be inserted in destination input buffer
	_pos_diff = _m - _j; 

	! Step 1: Make room in destination input array, and copy characters from source to destination
#IfV5;
	_src_input_array++;
	_dst_input_array++;
	_dst_input_array->0 = _dst_input_array->0 + _char_count;
#EndIf;
	! Make room in destination input array
	for(_i = _n + 1: _i > _m: _i--) ! Copy one extra byte - null-byte if z3
		_dst_input_array->(_i + _char_count) = _dst_input_array->_i;
	! Copy the words
	_k = _m + 1; 
	for(_i = _src_start_word : _i < _src_start_word + _word_count : _i++) {
		_m = 4 + 4 * _i;
		for(_j=0 : _j < _src_parse_array->_m : _j++) {
			_dst_input_array->(_k++) = _src_input_array->(_src_parse_array->(1 + _m) + _j + 1);
		}
		_dst_input_array->(_k++) = ' ';
	}

	! Step 2: Make room in destination parse array, and modify start position for all words after injection point
	_k = _dst_parse_array + 2;
!	print "k = ",_k,"^";
	for(_i = _dst_parse_array->1 - 1: _i >= _dst_start_word : _i--) {
!		print "Moving word #",_i,"^";
		_m = 4 * (_i + _word_count);
		for(_j=0 : _j < 3 : _j++)
			_k->(_m + _j) = _k->(4 * _i + _j);
		_k->(3 + _m) = _k->(3 + 4 * _i) + _char_count;
	}
	! Copy the parse data
	for(_i = 0 : _i < 4 * _word_count : _i++) {
!		print "Copying from pos ", (2 + 4 * _src_start_word - 4 + _i);
		_m = _src_parse_array->(2 + 4 * _src_start_word + _i);
		_j = 4 *_dst_start_word + _i;
		if(_i % 4 == 3)
			_k->_j = _m + _pos_diff;
		else	
			_k->_j = _m;
	}
	_dst_parse_array->1 = _dst_parse_array->1 + _word_count;
	rtrue;	
];
#Endif;

#IfDef DEBUG;

[ _PrintParseArray p_parse_array _i;
	print "PARSE_ARRAY: ", p_parse_array->1, " entries^";
	for(_i = 0; _i < p_parse_array -> 1; _i++) {
		print "Word " ,_i,
		" dict ",((p_parse_array + 2 + _i * 4) --> 0),
 		" len ",(p_parse_array + 2 + _i * 4) -> 2,
		" index ",(p_parse_array + 2 + _i * 4) -> 3, "^";
	}
];

[ _CheckPattern p_pattern _i _action_number _token_top _token_next _token_bottom;
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


! Keep the routines WordAddress, WordLength, NextWord and NextWordStopped just next to _CheckNoun, 
! since they will typically be called from parse_name routines, which are called from _CheckNoun

[ WordAddress p_wordnum;  ! Absolute addr of 'wordnum' string in buffer
	return player_input_array + parse_array->(p_wordnum*4+1);
];

[ WordLength p_wordnum;     ! Length of 'wordnum' string in buffer
	return parse_array->(p_wordnum*4);	
];

[ _PeekAtNextWord _i;
	_i = NextWord();
	--wn; ! wn was modified by NextWord, restore it
	return _i;
];

[ NextWord _i _j;
	if (wn <= 0 || wn > parse_array->1) { wn++; rfalse; }
	_i = wn*2-1; wn++;
	_j = parse_array-->_i;
	if (_j == ',//') _j = comma_word;
	if (_j == './/') _j = THEN_WORD;
	return _j;
];

[ NextWordStopped;
	if (wn > parse_array->1) { wn++; return -1; }
	return NextWord();
];

[ PronounNotice p_object;
	if(p_object == 0) return;
	if(p_object == player) return;
	if(p_object has pluralname) return;
	if(p_object has animate) {
		if(p_object has female) herobj = p_object;
		else if(p_object has neuter) itobj = p_object;
		else himobj = p_object;
	} else itobj = p_object;
	!print "he ", himobj, " she ", herobj, " it ", itobj, "^";
];

[ _UserFilter _obj;
	!  UserFilter consults the user's filter (or checks on attribute)
	!  to see what already-accepted nouns are acceptable
    if(noun_filter > 0 && noun_filter < 49) {
        if (_obj has (noun_filter-1)) rtrue;
        rfalse;
    }
    noun = _obj;
    return indirect(noun_filter);
];

[ _CheckNoun p_parse_pointer _i _j _n _p _obj _matches _last_match _last_match_len _current_word _name_array _name_array_len _best_score _result _stop;
#IfDef DEBUG_CHECKNOUN;
	print "Entering _CheckNoun!^";
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

	if(wn == nouncache_wn) {
		return nouncache_result;
	}
	nouncache_wn = wn;

	if((((p_parse_pointer-->0) -> #dict_par1) & 128) == 0) {
		! this word doesn't have the noun flag set,
		! so it can't be part of a noun phrase
		nouncache_result = 0;
		return 0;
	}

#IfDef OPTIONAL_DEBUG_VERBS;
	if(action_debug) {
		_name_array_len = Directions; _stop = top_object + 1;
	} else {
		_name_array_len = 0; _stop = scope_objects;
	}
	for(_i = _name_array_len: _i < _stop: _i++) {
		if(action_debug) _obj = _i; else _obj = scope-->_i;
#IfNot;
	for(_i = 0: _i < scope_objects: _i++) {
		_obj = scope-->_i;
#Endif;
		_n = wn;
		_p = p_parse_pointer;
		_current_word = p_parse_pointer-->0;
#IfDef DEBUG_CHECKNOUN;
		print "Testing ", (the) _obj, " _n is ", _n, "...^";
#EndIf;
		if(noun_filter ~= 0 && _UserFilter(_obj) == 0) {
			!print "noun_filter rejected ", (the) _obj,"^";
			jump not_matched;
		}
		if(parser_check_multiple == MULTIHELD_OBJECT && _obj notin player) {
			jump not_matched;
		}
		if(_obj provides parse_name) {
			_j = wn;
			_result = PrintOrRun(_obj, parse_name);
			if(_result == -1) jump try_name_match;
			_n = _n + _result; ! number of words consumed
			wn = _j;
			if(_n > wn && ObjectIsInvisible(_obj, true) == false) {
				if(_obj has concealed or scenery) {
					! don't consider for which, but remember
					! as last resort if nothing else matches
					if(_last_match == 0) {
						_last_match = _obj;
						_last_match_len = _n;
					}
					jump not_matched;
				}
				if(_n == _best_score) {
					_matches++;
					which_object-->_matches = _obj;
#IfDef DEBUG_CHECKNOUN;
					print "Same best score ", _best_score, ". Matches are now ", _matches,"^";
#EndIf;
				}
				if(_n > _best_score) {
#IfDef DEBUG_CHECKNOUN;
					print "New best score - matched with parse_name ", _n,"^";
#EndIf;
					_last_match = _obj;
					_best_score = _n;
					_matches = 1;
					which_object-->1 = _obj;
				}
			}
		} else {
.try_name_match;
			@get_prop_addr _obj name -> _name_array;
			if(_name_array) {
				! Assembler equivalent of _name_array_len = _obj.#name / 2  
				@get_prop_len _name_array -> _name_array_len;
#IfV5;
				@log_shift _name_array_len 1 -> _name_array_len;
#IfNot;
				@div _name_array_len 2 -> _name_array_len;
#EndIf;

				while(_IsSentenceDivider(_p) == false) {
#IfV5;
					@scan_table _current_word _name_array _name_array_len -> _result ?success;
#IfNot;
					_j = 0;
					@dec _name_array_len; ! This is needed for the loop. Do we need to undo it after?
.next_word_in_name_prop;
					if(_name_array-->_j == _current_word) jump success;
					@inc_chk _j _name_array_len ?~next_word_in_name_prop;
#EndIf;
					jump not_matched;
.success;
#IfDef DEBUG_CHECKNOUN;
					print " - matched ", (address) _current_word,"^";
#EndIf;
					_n++;
					_p = _p + 4;
					_current_word = _p-->0;
					if(_n >= _best_score && ObjectIsInvisible(_obj, true) == false) {
						if(_obj has concealed or scenery) {
							! don't consider for which, but remember
							! as last resort if nothing else matches
							if(_last_match == 0) {
								_last_match = _obj;
								_last_match_len = _n;
							}
							jump not_matched;
						}
						if(_n == _best_score) {
							_matches++;
							which_object-->_matches = _obj;
#IfDef DEBUG_CHECKNOUN;
							print "Same best score ", _best_score, ". Matches are now ", _matches,"^";
#EndIf;
						}
						if(_n > _best_score) {
							_matches = 1;
#IfDef DEBUG_CHECKNOUN;
							print "New best score ", _n, ". Old score was ", _best_score,". Matches is now ",_matches,".^";
#EndIf;
							_last_match = _obj;
							_best_score = _n;
							which_object-->1 = _obj;
						}
					}
				}
			}
		}
.not_matched;

!   print "checking ", _obj.&name-->0, " ", _current_word, "^";
	}

	if(_matches == 0 && _last_match > 0) {
		! only scenery or concealed objects matched.
		_matches = 1;
		_best_score = _last_match_len;
	}

	which_object->0 = _matches;
	which_object->1 = _best_score - wn;

	if(_matches == 1) {
#IfDef DEBUG_CHECKNOUN;
		print "Matched a single object: ", (the) _last_match,
			", num words ", which_object->1, "^";
#EndIf;
		nouncache_result = _last_match;
		return _last_match;
	}
#IfDef DEBUG_CHECKNOUN;
				print "Matches: ", _matches,", num words ", which_object->1, "^";
#EndIf;
	if(_matches > 1) {
		nouncache_result = -_matches;
		return -_matches;
	}
	nouncache_result = 0;
	return 0;
];

[ _AskWhichNoun p_noun_name p_num_matching_nouns _i; 
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
	print "? ";
];

[ _GetNextNoun p_parse_pointer p_phase _noun _oldwn _num_words_in_nounphrase _pluralword;
	! try getting a noun from the <p_parse_pointer> entry in parse_array
	! return:
	!   <noun number> if found
	!   0  if no noun found (but we didn't write an error message)
	!   -1 if we should give up parsing completely (because
	!      the player has entered a new command line).
	!   -2 if parsing failed, and error message written
	! 
	! Side effects:
	! - if found, then wn will be updated
	! - if plural matched, then parser_action set to ##PluralFound
	!
	! NOTE: you need to update parse_pointer after calling _GetNextNoun since
	! wn can change

	parser_action = 0;

	! special rule to avoid "all in"
	! (since 'in' in this combination is usually
	! a preposition but also matches a direction noun)
	if(p_parse_pointer-->0 == 'all' && 
		(p_parse_pointer + 4)-->0 == 'in') {
		++wn;
		return 0;
	}

	! skip 'the', 'all' etc
	while(p_parse_pointer --> 0 == 'a//' or 'the' or 'an' or ALL_WORD) {
#IfDef DEBUG_GETNEXTNOUN;
		print "skipping ",(address) p_parse_pointer --> 0,"^";
#Endif;
		++wn;
		p_parse_pointer = p_parse_pointer + 4;
	}

	! check for pronouns
	if(p_parse_pointer --> 0 == 'it' or 'him' or 'her') {
		switch(p_parse_pointer --> 0) {
		'it': _noun = itobj;
		'him': _noun = himobj;
		'her': _noun = herobj;
		}
		if(_noun == 0 && p_phase == PHASE2) {
			print "I don't know what ~",(address) p_parse_pointer --> 0, "~ refers to.^";
			return -2;
		} else if(TestScope(_noun) == false && p_phase == PHASE2) {
			print "You can't see ~",(address) p_parse_pointer --> 0, "~ (", (name) _noun, ") at the moment.^"; 
			return -2;
		}
		++wn; 
		return _noun;
	}

	! not a pronoun, continue
	_pluralword = ((p_parse_pointer-->0) -> #dict_par1) & 4;
#IfDef DEBUG_GETNEXTNOUN;
	print "Calling _CheckNoun(",p_parse_pointer,");^";
	if(p_parse_pointer-->0 > 2000) print (address) p_parse_pointer-->0, " ", _pluralword, "^";
#Endif;
#IfDef DEBUG_TIMER;
	timer2_start = $1c-->0;
#Endif;
	_noun = _CheckNoun(p_parse_pointer);
#IfDef DEBUG_TIMER;
	timer2_stop = $1c-->0 - timer2_start;
	print "[_CheckNoun took ",timer2_stop," jiffies]^";
#Endif;
	_num_words_in_nounphrase = which_object -> 1;

.recheck_noun;
	if(p_phase == PHASE1 && _noun < 0) {
		wn = wn + _num_words_in_nounphrase;
		_noun = 1; ! a random noun in phase 1 just to avoid which? question
	}
	if(_noun < 0) {
		if(_pluralword) {
			! we don't have to ask here, because the input was
			! "take books" or "take all books"
			parser_action = ##PluralFound;
			!wn = wn + which_object->1;
			return 0;
		}
		_AskWhichNoun(p_parse_pointer --> 0, -_noun);
		! read a new line of input
		! I need to use parse_array since NextWord
		! for parse_name and others hardcode this
		! usage, so I first store the old input into
		! temp arrays that I will restore if I can
		! disambiguate successfully.
		_CopyInputArray(player_input_array, temp_player_input_array);
		_CopyParseArray(parse_array, temp_parse_array);
		_ReadPlayerInput();
		! is this a reply to the question?
		!if((parse_array->1 == 1) &&  
		if((((parse_array + 2) --> 0) + DICT_BYTES_FOR_WORD)->0 & 1 == 0) {
			! the first word is not a verb. Assume
			! a valid reply and add the other 
			! entry into parse_array, then retry
			_oldwn = wn; ! wn is used in _CheckNoun, so save it
			wn = 1;
			_noun = _CheckNoun(parse_array+2);
			wn = _oldwn; ! and restore it after the call
			if(_noun > 0) {
				! we have successfully disambiguated the noun phrase.
				! now we need to restore the length of the
				! noun phrase so that it will be absorbed when we
				! return from the routine.
				which_object->1 = _num_words_in_nounphrase;
				! don't forget to restore the old arrays
				_CopyInputArray(temp_player_input_array, player_input_array);
				_CopyParseArray(temp_parse_array, parse_array);
				@new_line;
				jump recheck_noun;
			}
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

[ _UpdateNounSecond p_noun p_inp p_id;
	if(num_noun_groups == 0) {
		!print p_id, ": setting noun: ", p_noun, " inp1 ", p_inp, "^";
		noun = p_noun;
		inp1 = p_inp;
	} else if(num_noun_groups == 1){
		!print p_id, ": setting second: ", p_noun, " inp2 ", p_inp, "^";
		second = p_noun;
		inp2 = p_inp;
	}
	++num_noun_groups;
];

[ _IsSentenceDivider p_parse_pointer;
	! check if current parse_array block, indicated by p_parse_pointer,
	! is a period or other sentence divider
	return p_parse_pointer --> 0 == './/' or ',//' or 'and' or 'then';
];

[ ParseToken p_token_type p_token_data _parse_pointer;
	! DM defines ParseToken as ParseToken(tokentype,tokendata)
	! ParseToken is similar to a general parse routine,
	! and returns GPR_FAIL, GPR_MULTIPLE, GPR_NUMBER,
	! GPR_PREPOSITION, GPR_REPARSE or the object number
	return _ParseToken(-p_token_type, -p_token_data, PHASE1);
];

[ _GrabIfNotHeld p_noun;
	if(p_noun in player) return;
	if(p_noun has animate) return;
	print "(first taking ", (the) p_noun, ")^";
	keep_silent = true;
	<take p_noun>;
	keep_silent = false;
];

[ _ParseToken p_pattern_pointer p_parse_pointer p_phase _noun _i _token _token_type _token_data;
	! ParseToken is similar to a general parse routine,
	! and returns GPR_FAIL, GPR_MULTIPLE, GPR_NUMBER,
	! GPR_PREPOSITION, GPR_REPARSE or the object number
	! However, it also taks the current grammar token as input
	! while a general parse routine takes no arguments.
	! (this is mostly to avoid recalculating the values from wn
	! when the calling routine already has them at hand)

	if(p_pattern_pointer < 0) {
		! called from ParseToken (DM library API)
		_token = -p_pattern_pointer;
		_token_data = -p_parse_pointer;
		p_parse_pointer = parse_array + 2 + 4 * (wn - 1);
		p_pattern_pointer = 0;
	} else {
		_token = (p_pattern_pointer -> 0);
		_token_data = (p_pattern_pointer + 1) --> 0;
	}
	_token_type = _token & $0f;
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
		indirect(scope_routine);
	} else if(_token_type == TT_PARSE_ROUTINE) {
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
		parser_check_multiple = _token_data;

		if(_token_data == NOUN_OBJECT or HELD_OBJECT or CREATURE_OBJECT) {
			_noun = _GetNextNoun(p_parse_pointer, p_phase);
			if(_noun == -2) return GPR_FAIL;
			if(_noun == -1) return GPR_REPARSE;
			if(_noun == 0) {
				parser_unknown_noun_found = p_parse_pointer;
				return GPR_FAIL;
			}
			p_parse_pointer = parse_array + 2 + 4 * (wn - 1);
			if(_token_data == CREATURE_OBJECT && _noun hasnt animate) {
				if(p_phase == PHASE2) {
					print "You can only do that to something animate.^";
					return GPR_FAIL;
				}
			}
			if(_token_data == HELD_OBJECT && _noun notin player) {
				if(p_phase == PHASE2) {
					_GrabIfNotHeld(_noun);
					if(_noun notin player) return GPR_FAIL;
				}
			}
			return _noun;
		} else if(_token_data == MULTI_OBJECT or MULTIHELD_OBJECT or MULTIEXCEPT_OBJECT or MULTIINSIDE_OBJECT) {
			for(::) {
				_noun = _GetNextNoun(p_parse_pointer, p_phase);
				if(_noun == -2) return GPR_FAIL;
				if(_noun == -1) return GPR_REPARSE;
				if(_noun == 0) {
					if(parser_action == ##PluralFound) {
						! take books or take all books
						! copy which_object to multiple_objects
						for(_i = 0: _i < which_object->0: _i++) {
							multiple_objects --> 0 = 1 + (multiple_objects --> 0);
							multiple_objects --> (multiple_objects --> 0) = which_object--> (_i + 1);
						}
#IfDef DEBUG_PARSETOKEN;
						print "adding plural ", which_object->0, " ", which_object->1, " ", multiple_objects --> 0, "^";
#Endif;
						wn = wn + which_object->1;
						return GPR_MULTIPLE;
					}
					if(p_parse_pointer-->0 == ALL_WORD) {
						! take all etc.
						! Add all reasonable objects in scope
						! to the multiple_objects array
						_AddMultipleNouns(_token_data);
						parser_all_found = true;
						if(multiple_objects --> 0 == 0) {
							if(p_phase == PHASE2)
								PrintMsg(MSG_PARSER_NOTHING_TO_VERB);
							return GPR_FAIL;
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
				p_parse_pointer = parse_array + 2 + 4 * (wn - 1);
				multiple_objects --> 0 = 1 + (multiple_objects --> 0);
				multiple_objects --> (multiple_objects --> 0) = _noun;
				! check if we should continue: and or comma
				if(_PeekAtNextWord() == comma_word or AND_WORD) {
					++wn;
					p_parse_pointer = p_parse_pointer + 4;
					continue;
				} else break;
			}
			if(multiple_objects --> 0 == 0) {
				! no nouns found, so this pattern didn't match
				return GPR_FAIL;
			}
			return GPR_MULTIPLE;
		} else if(_token_data == TOPIC_OBJECT) {
			consult_from = wn;
			consult_words = 0;
			! topic continues until end of line or
			! until the word matches the preposition
			! defined in the next pattern
			!print (p_pattern_pointer + 3) -> 0, "^"; ! token
			!print (p_pattern_pointer + 4) --> 0, "^"; ! token_data 
			if(p_pattern_pointer > 0) {
				_i = (p_pattern_pointer + 4) --> 0; ! word to stop at
			} else {
				_i = NULL;
			}
			for(::) {
				++wn;
				++consult_words;
				p_parse_pointer = p_parse_pointer + 4;
				if(wn > parse_array->1 || p_parse_pointer --> 0 == _i) {
					! found the stop token, or end of line
					break;
				}
			}
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

[ _AddMultipleNouns p_multiple_objects_type   _i _addobj _obj;
	multiple_objects --> 0 = 0;
	for(_i = 0: _i < scope_objects: _i++) {
		_obj = scope-->_i;
		_addobj = false;
		switch(p_multiple_objects_type) {
		MULTIHELD_OBJECT:
			_addobj = _obj in player;
		MULTI_OBJECT, MULTIEXCEPT_OBJECT, MULTIINSIDE_OBJECT:
			_addobj = _obj hasnt scenery or concealed or static or animate;
		}
		if(action == ##Take && _obj in player) _addobj = false;
		if(_addobj) {
			multiple_objects --> 0 = 1 + (multiple_objects --> 0);
			multiple_objects --> (multiple_objects --> 0) = _obj;
			!print "Adding ", (name) _obj, "^";
		}
	}
];

[ _PrintPartialMatch p_start p_stop _start _stop _i;
	_i = (parse_array-2+(4*p_start));
	_start = _i->3; ! index to input line for first word
	if(p_stop > parse_array -> 1) {
		_stop = player_input_array->0; ! until the end of the input
	} else {
		_i = (parse_array-2+(4*p_stop));
		_stop = _i->2 + _i->3; ! until the index of the stop word + its length
	}
	for(_i = _start: _i < _stop: _i ++) {
		if(player_input_array -> _i == 0) break;
		print (char) player_input_array -> _i;
	}
];

[ _PrintUknownWord _i;
	for(_i = 0: _i < parser_unknown_noun_found->2: _i++) {
		print (char) player_input_array->(_i + parser_unknown_noun_found->3);
	}
];


#IfDef OPTIONAL_GUESS_MISSING_NOUN;
Constant GUESS_CREATURE = 0;
Constant GUESS_HELD = 1;
Constant GUESS_CONTAINER = 2;
Constant GUESS_THING = 3;
Constant GUESS_DOOR = 4;
Array guess_object-->5;
Array guess_num_objects->5;
[ _GuessMissingNoun p_type p_prep p_nounphrase_num _assumed _exclude _i _noun;
	for(_i = 0: _i < 5: _i++) guess_num_objects->_i = 0;

	if(p_nounphrase_num == 1) {
		_assumed = noun;
	} else {
		_assumed = second;
		if(_assumed == 0) _assumed = noun;
		_exclude = noun;
	}

	for(_i = 0: _i < scope_objects: _i++) {
		_noun = scope-->_i;
		if(_noun == player) continue;
		if(ObjectIsInvisible(_noun)) continue;
		if(_noun has door && _noun ~= _exclude) {
			guess_object-->GUESS_DOOR = _noun;
			guess_num_objects->GUESS_DOOR = 1 + guess_num_objects->GUESS_DOOR;
			!print "found door ", (the) _door, "^";
		}
		if(_noun has container && _noun ~= _exclude) {
			guess_object-->GUESS_CONTAINER = _noun;
			guess_num_objects->GUESS_CONTAINER = 1 + guess_num_objects->GUESS_CONTAINER;
			!print "found container ", (the) _container, "^";
		}
		if(_noun has animate && _noun ~= _exclude) {
			guess_object-->GUESS_CREATURE = _noun;
			guess_num_objects->GUESS_CREATURE = 1 + guess_num_objects->GUESS_CREATURE;
			!print "found creature ", (the) _creature, "^";
		} 
		if(_noun in player && _noun ~= _exclude) {
			guess_object-->GUESS_HELD = _noun;
			guess_num_objects->GUESS_HELD = 1 + guess_num_objects->GUESS_HELD;
			!print "found held ", (the) _held, "^";
		}
		if(_noun hasnt scenery or concealed && _noun ~= _exclude) {
			guess_object-->GUESS_THING = _noun;
			guess_num_objects->GUESS_THING = 1 + guess_num_objects->GUESS_THING;
			!print "found thing ", (the) _thing, "^";
		}
	}

	_noun = 0;
	switch(p_type) {
	HELD_OBJECT: 
		if(guess_num_objects->GUESS_HELD == 1)
			_noun = guess_object-->GUESS_HELD;
	CREATURE_OBJECT: 
		if(guess_num_objects->GUESS_CREATURE == 1)
			_noun = guess_object-->GUESS_CREATURE;
	default: 
		if(_noun == 0 && guess_num_objects->GUESS_CONTAINER == 1 &&
			action == ##Open or ##Close) {
			_noun = guess_object-->GUESS_CONTAINER;
		}
		if(_noun == 0 && guess_num_objects->GUESS_DOOR == 1 &&
			action == ##Lock or ##Unlock or ##Open or ##Close) {
			_noun = guess_object-->GUESS_DOOR;
		}
		if(_noun == 0 && guess_num_objects->GUESS_THING == 1) {
			_noun = guess_object-->GUESS_THING;
		}
	}

	!print p_nounphrase_num, " ",_noun, " ", noun, " ", second, "^";
	if(_noun == _assumed) _noun = 0;
	if(_noun) {
		print "(assuming ";
		if(p_prep) {
			print (address) (p_prep+1) --> 0, " ";
		} else {
		}
		print (the) _noun, ")^";
	}
	return _noun;
];

#EndIf;

[ _FixIncompleteSentenceOrComplain p_pattern _token _type _data _noun _prep _second _num_preps;
	! Called because sentence shorter than the pattern
	! Available data: wn, parse_array and p_pattern_token (last matched token)
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
	if(_noun ~= 0 && noun == 0) noun = _GuessMissingNoun(_noun -> 2, 0, 1);
	if(_second ~= 0 && second == 0) second = _GuessMissingNoun(_second -> 2, _prep, 2);
	if((_noun == 0 || noun ~= 0) && (_second == 0 || second ~= 0)) {
		!print "message complete: ", noun, " ", second, "^";
		rtrue;
	}
#EndIf;

	! write an error message and return false
	print "I think you wanted to say ~";
	print (verbname) verb_word;
	for(_token = p_pattern + 3: _token->0 ~= TT_END: _token = _token + 3) {
		_type = _token -> 0;
		_data = (_token + 1) --> 0;
		if(_type > 9) {
			if(_num_preps == 0) print " ", (address) _data;
			++_num_preps;
		} else {
			if(_noun == 0) {
				if(second == 0) print " something"; else print " ", (name) second;
			} else {
				if(noun ~= 0) {
					_noun = 0; ! avoid repeat (and we don't need _noun anymore)
					print " ",(name) noun;
				} else if(_token->2 == CREATURE_OBJECT) {
					print " someone";
				} else {
					print " something";
				}
			}
		}
	}
	print "~. Please try again.^";
	rfalse;
];

[ _ParsePattern p_pattern p_phase _pattern_pointer _parse_pointer _noun _i;
	! Check if the current pattern will parse, with side effects if PHASE2
	! _ParsePattern will return:
	!   -1 if need to reparse
	!   0..99 how many words were matched before the match failed
	!   100 if perfect match
	wn = verb_wordnum + 1;
	_parse_pointer = parse_array + 2 + 4*(verb_wordnum);
	_pattern_pointer = p_pattern - 1;
	num_noun_groups = 0;
	noun = 0;
	second = 0;
	inp1 = 0;
	inp2 = 0;
	special_number = 0;
	special_word = 0;
	parsed_number = 0;
	multiple_objects --> 0 = 0;
	parser_check_multiple = 0;
	parser_unknown_noun_found = 0;
	parser_all_found = false;
	action = (p_pattern --> 0) & $03ff;
	action_reverse = ((p_pattern --> 0) & $400 ~= 0);
#IfDef OPTIONAL_DEBUG_VERBS;
	action_debug = (action == ##Scope);
#EndIf;

	while(true) {
		_pattern_pointer = _pattern_pointer + 3;
#IfDef DEBUG_PARSEPATTERN;
		print "TOKEN: ", _pattern_pointer -> 0, " wn ", wn, " _parse_pointer ", _parse_pointer, "^";
#EndIf;

		if(((_pattern_pointer -> 0) & $0f) == TT_END) {
			if(_IsSentenceDivider(_parse_pointer)) {
				wn++;
				return 100; ! pattern matched
			}
			if(wn == 1 + parse_array->1) {
				return 100; ! pattern matched
			}
			if(p_phase == PHASE2) {
				print "I only understood you as far as ~";
				_PrintPartialMatch(verb_wordnum, wn - 1);
				print "~ but then you lost me.^";
			}
			return wn - verb_wordnum; ! Fail because the grammar line ends here but not the input
		}
		if(wn >= 1 + parse_array->1) {
#IfDef DEBUG_PARSEPATTERN;
			print "Fail, since grammar line has not ended but player input has.^";
#EndIf;
			if(p_phase == PHASE2) {
				!print "You need to be more specific.^";
				if(_FixIncompleteSentenceOrComplain(p_pattern - 1)) {
					! sentence was corrected
					return 100;
				}
			};
			return wn - verb_wordnum;!Fail because input ends here but not the grammar line
		}
#IfDef DEBUG_PARSEPATTERN;
		print "Calling ParseToken: token ", _pattern_pointer->0," type ", (_pattern_pointer->0) & $f, ", data ", (_pattern_pointer + 1) --> 0,"^";
#EndIf;
		_noun = _ParseToken(_pattern_pointer, _parse_pointer, p_phase);
		! the parse routine can change wn, so update _parse_pointer
		_parse_pointer = parse_array + 2 + 4 * (wn - 1);

		switch(_noun) {
		GPR_FAIL:
			if(_pattern_pointer->0 == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) {
				! First or in the middle of a list of alternative prepositions
#IfDef DEBUG_PARSEPATTERN;
				print "Preposition failed, but more options available so reparsing^";
#Endif;
				continue; ! keep parsing
			}

			! write error messages if PHASE2 as needed
			if(p_phase == PHASE2) {
				if(_pattern_pointer->0 == TOKEN_LAST_PREP or TOKEN_SINGLE_PREP) {
					! bad preposition
					print "I didn't understand that sentence.^";
				} else if(parser_unknown_noun_found > 0) {
					if(scope_routine ~= 0) {
						scope_stage = 3;
						indirect(scope_routine);
					} else if(parser_unknown_noun_found --> 0 > 0) {
						! is it one of the location.name words?
						inp1 = -1;
						if(location.name ~= 0) {
							for(_i = 0: _i < location.#name / 2: _i++) {
								if(parser_unknown_noun_found --> 0 ==
									location.&name --> _i) {
									inp1 = _i;
								}
							}
						}
						if(inp1 > -1) {
							print "You don't need to refer to ~";
							_PrintUknownWord();
							print "~ in this game.^";
						} else if(parser_unknown_noun_found-->0 == ALL_WORD) {
							print "You can't use multiple objects with that verb.^";
						} else {
							print "You can't see any such thing.^";
						}
					} else {
						print "Sorry, I don't understand what ~";
						_PrintUknownWord();
						print "~ means.^";
					}
				}
			}
			return wn - verb_wordnum; ! pattern didn't match
		GPR_PREPOSITION:
			! advance until the end of the list of prepositions
#IfDef DEBUG_PARSEPATTERN;
			print "-- preposition mached ", _pattern_pointer, " ", _pattern_pointer->0, "^";
#Endif;
			while(_pattern_pointer->0 ~= TOKEN_LAST_PREP or TOKEN_SINGLE_PREP) {
#IfDef DEBUG_PARSEPATTERN;
			print "-- increasing _pattern_pointer^";
#Endif;
				_pattern_pointer = _pattern_pointer + 3;
			}
		GPR_MULTIPLE:
			! multiple_objects contains the objects
			if(multiple_objects-->0 == 0) {
				_UpdateNounSecond(0, 0, 1);
			} else {
				_UpdateNounSecond(multiple_objects-->1, multiple_objects-->1, 1);
			}
		GPR_NUMBER:
			! parsed_number contains the new number
			if(p_phase == PHASE2 && parsed_number == -1000)  {
				print "I didn't understand that number.^";
				return wn - verb_wordnum; ! bad match
			}
			_UpdateNounSecond(parsed_number, 1, 2);
		GPR_REPARSE:
			return -1; ! the player_input and parse_array have changed
		default:
			! _noun was a valid noun
			_UpdateNounSecond(_noun, _noun, 3);
		}
	}
	! we should never reach this line
	! the while(true) loop is only exited by return statements
];

[ _ParseAndPerformAction _word_data _verb_grammar _i _pattern _pattern_pointer _parse_pointer _noun _score _best_score _best_pattern;
	! returns
	! 0: to reparse
	! 1/true: if error was found (so you can abort with "error...")
	! -n: if <n> words were used to find a match,
	!
	! taking periods and other sentence breaks into account.
	! For example, if the input is "l.l" then the parser
	! will stop after the first "l" has been mached, and
	! 1 is returned. If the input is "open box" then
	! the whole input is matched and 2 returned.

#IfDef DEBUG_TIMER;
	timer1_start = $1c-->0;
#Endif;
	if(_IsSentenceDivider(parse_array + 2))
		return -1;

	action = -1;
	which_object->1 = 0;
	actor = player;
	nouncache_wn = -1; ! clear noun cache

	if(scope_routine ~= 0) {
		! if true, then scope=Routine was executed
		! in the previous _ParseAndPerformAction,
		! which can have added stuff to the scope
		! Calling _ResetScope to force  a scope refresh
		!_ResetScope();
	}
	scope_routine = 0; ! prepare for a new scope=Routine

	if(parse_array->1 < 1) {
		"Come again?";
	}

	verb_wordnum = 1;

.reparse;
	verb_word = (parse_array - 2) --> (2 * verb_wordnum) ;
	if(verb_word < (0-->HEADER_DICTIONARY)) {
		! Not a verb. Try the entry point routine before giving up
		verb_word = UnknownVerb(verb_word);
		if(verb_word == 0) {
			! unknown word
#IfDef DEBUG_PARSEANDPERFORM;
			print "Case 1, Word ", verb_word, "^";
#EndIf;
			if(actor ~= player) jump treat_bad_line_as_conversation;
			"I don't understand that word.";
		}
	}

	_word_data = verb_word + DICT_BYTES_FOR_WORD;
	! check if it is a direction
	if((_word_data->0) & 1 == 0) { ! This word does not have the verb flag set.
		! try a direction instead
		wn = verb_wordnum;
		if(Directions.parse_name()) {
			action = ##Go;
			noun = Directions;
			inp1 = Directions;
			PerformPreparedAction();
			return -1;
		}
		! not a direction, check if beginning of a command
		_noun = _CheckNoun(parse_array+2);
		if(_noun > 0 && verb_wordnum == 1) {
			! The sentence starts with a noun, now 
			! check if comma afterwards
			wn = wn + which_object->1;
			_pattern = NextWord();
			if(_pattern == comma_word) {
				jump conversation;
			}
		}
		if(actor ~= player) jump treat_bad_line_as_conversation;
		"That is not a verb I recognize.";

.conversation;
		if(_noun hasnt animate && _noun hasnt talkable) {
			"You can't talk to ", (the) _noun, ".";
		}
		! See http://www.inform-fiction.org/manual/html/s18.html
		! set actor
		actor = _noun;
		!print "Trying to talk to to ", (the) _noun, ".^";
		verb_wordnum = wn;
		jump reparse;
	}

	! Now it is known word, and it is not a direction, in the first position
	meta = (_word_data->0) & 2;

!   print "Parse array: ", parse_array, "^";
!   print "Word count: ", parse_array->0, "^";
!   print "Word 1: ", (parse_array + 2)-->0, "^";
!   print "Word 2: ", (parse_array + 6)-->0, "^";
!   print "Word 3: ", (parse_array + 10)-->0, "^";
	_i = 255 - (_word_data->1); ! was _verb_num
	_verb_grammar = (0-->HEADER_STATIC_MEM)-->_i;

#IfDef DEBUG_PARSEANDPERFORM;
	print "Verb#: ",_i,", meta ",meta,".^";
	print "Grammar address for this verb: ",_verb_grammar,"^";
	print "Number of patterns: ", _verb_grammar->0 ,"^";

	! First print all patterns, for debug purposes
	_pattern = _verb_grammar + 1;
	for(_i = 0 : _i < _verb_grammar->0: _i++) {
		print "############ Pattern ",_i," ",_pattern,"^";
		_pattern = _CheckPattern(_pattern);
	}
	@new_line;
#EndIf;

	! Phase 1: look for best pattern without side effects
	_best_score = 0;
	_best_pattern = 0;
	_pattern = _verb_grammar + 1;
	for(_i = 0 : _i < _verb_grammar->0 : _i++) {
#IfDef DEBUG_PARSEANDPERFORM;
		print "### PHASE 1: Pattern ",_i," address ", _pattern, "^";
#EndIf;
		scope_stage = 0;
		_score = _ParsePattern(_pattern, PHASE1);
		! reset scope if _ParsePattern messed with it
		if(scope_stage > 0) {
			_ResetScope();
			_UpdateScope(player);
		}

#IfDef DEBUG_PARSEANDPERFORM;
		print "### PHASE 1: result ", _score, "^";
#EndIf;
		! note that _ParsePattern will never return -1 in PHASE1
		if(_score == 0) {
			! This pattern has failed.
#IfDef DEBUG_PARSEANDPERFORM;
			print "Pattern didn't match.^";
#EndIf;
		} else if(_score > _best_score) {
			_best_score = _score;
			_best_pattern = _pattern;
			!continue;
		}

		! check if pefect match found
		if(_best_score == 100) break;

		! Scan to the end of this pattern
		_pattern_pointer = _pattern + 2;
		while(_pattern_pointer -> 0 ~= TT_END) {
			_pattern_pointer = _pattern_pointer + 3;
		}
		_pattern = _pattern_pointer + 1;
	}

	! Phase 2: reparse best pattern and ask for additional info if
	! needed (which book? etc)
#IfDef DEBUG_PARSEANDPERFORM;
	print "### PHASE 2: Pattern address ", _best_pattern, "^";
#EndIf;
	_score = _ParsePattern(_best_pattern, PHASE2);
#IfDef DEBUG_PARSEANDPERFORM;
	print "### PHASE 2: result ", _score, "^";
#EndIf;
	if(_score == -1) rfalse; ! force a complete reparse
	if(_score == 100) jump parse_success;
	rtrue; ! ParsePattern wrote some error message

.treat_bad_line_as_conversation;
	! this is used when not understood and the actor is an NPC
	action = ##NotUnderstood;
	consult_from = wn;
	consult_words = parse_array->1 - wn + 1;
	special_number = TryNumber(wn);
	special_word = NextWord();
	! fall through to jump parse_success;

.parse_success;
	! we want to return how long the successfully sentence was
	! but wn can be destroyed by action routines, so store in _i
#IfDef DEBUG_TIMER;
	timer1_stop = $1c-->0 - timer1_start;
	print "[parsing took ",timer1_stop," jiffies]^";
#Endif;
	num_words_parsed = -(wn - 1);
	if(action_reverse) {
		_i = second;
		second = noun;
		noun = _i;
		inp1 = noun;
		inp2 = second;
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
		if(RunRoutines(player, orders)) rtrue;
		if(RunRoutines(actor, orders)) rtrue;
		if(action == ##NotUnderstood) {
			actor = player; action = ##Answer;
			jump parse_success;
		}
		if(RunLife(actor, ##Order)) rtrue;
		print (The) actor, " has better things to do.";
		return num_words_parsed;
	}

	if(multiple_objects --> 0 == 0) {
		! single action
		PerformPreparedAction();
	} else {
		! multiple action
		! (a) check the multiple list isn't empty;
		! (b) warn the player if it has been cut short because too long;
		! (c) generate a sequence of actions from the list
		!     (stopping in the event of death or movement away).
		_score = 0;
		for(_noun = 1: _noun <= multiple_objects --> 0 : _noun++) {
			inp1 = multiple_objects --> _noun;
			noun = inp1;
			switch(parser_check_multiple) {
			MULTIEXCEPT_OBJECT:
				if(noun == second) continue; ! eg take all except X
			MULTIINSIDE_OBJECT:
				if(noun notin second) continue; ! eg get all from X
			}
			if(action == ##Take && noun == parent(player)) continue; ! don' pick up the box when you are in it
			if(parser_all_found || multiple_objects --> 0 > 1) print (name) noun, ": ";
			++_score;
			PerformPreparedAction();
		}
		if(_score == 0) PrintMsg(MSG_PARSER_NOTHING_TO_VERB);
	}
	if(inp1 ~= 1) PronounNotice(noun);
	return num_words_parsed;
];

