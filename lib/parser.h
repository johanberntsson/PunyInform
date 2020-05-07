! ######################### Parser
! https://www.inform-fiction.org/source/tm/TechMan.txt

[ ReadPlayerInput p_no_prompt _result;
! #IfV5;
!   print "Width: ", HDR_SCREENWCHARS->0,"^";
! #EndIf;
#IfV5;
	style roman;
	@buffer_mode 0;
#EndIf;
	if(p_no_prompt == false) print "> ";
	parse_array->0 = MAX_INPUT_WORDS;
#IfV5;
	DrawStatusLine();
	player_input_array->0 = MAX_INPUT_CHARS;
	player_input_array->1 = 0;
	@aread player_input_array parse_array -> _result;
	@buffer_mode 1;
#IfNot;
	player_input_array->0 = MAX_INPUT_CHARS - 1;
	if(player in location) {
		@sread player_input_array parse_array;
	} else {
		_result = location; location = GetVisibilityCeiling(player);
		@sread player_input_array parse_array;
		location = _result;
	}
#EndIf;
	! Set word after last word in parse array to all zeroes, so it won't match any words.
	_result = 2 * (parse_array -> 1) + 1;
	parse_array-->_result = 0;
	parse_array-->(_result + 1) = 0;
];

[ YesOrNo;
    for (::) {
        ReadPlayerInput(true);
        if(parse_array -> 1 == 1) {
        	! one word reply
            if(parse_array --> 1 == 'yes' or 'y//') rtrue;
            if(parse_array --> 1 == 'no' or 'n//') rfalse;
        }
        PrintMsg(MSG_YES_OR_NO, true);
    }
];

[ CopyInputArray p_src_input_array p_dst_input_array _n _i;
	_n = MAX_INPUT_CHARS + 3;
	for(_i = 0: _i < _n: _i++)
		p_dst_input_array->_i = p_src_input_array->_i;
];

[ CopyParserArray p_src_parse_array p_dst_parse_array _n _i;
	_n = 2 + 4 * (MAX_INPUT_WORDS + 1); 
	for(_i = 0: _i < _n: _i++)
		p_dst_parse_array->_i = p_src_parse_array->_i;
];

#IfDef DEBUG;

[ PrintParseArray p_parse_array _i;
	print "PARSE_ARRAY: ", p_parse_array->1, " entries^";
	for(_i = 0; _i < p_parse_array -> 1; _i++) {
		print "Word " ,_i,
		" dict ",((p_parse_array + 2 + _i * 4) --> 0),
 		" len ",(p_parse_array + 2 + _i * 4) -> 2,
		" index ",(p_parse_array + 2 + _i * 4) -> 3, "^";
	}
];


[ CheckPattern p_pattern _i _action_number _token_top _token_next _token_bottom;
	! action number is the first two bytes
	_action_number = p_pattern-->0;
	p_pattern = p_pattern + 2;
	action = _action_number & $3ff;
	reverse = (_action_number & $400 ~= 0);
	print "Action#: ", action, " Reverse: ", reverse, "^";

	for(_i = 0: : _i++) {
		if(p_pattern->0 == TT_END) break;
		_token_top = (p_pattern->0 & $c0)/64; ! top (2 bits)
		_token_next = (p_pattern->0 & $30)/16;  ! next (2 bits)
		_token_bottom = p_pattern->0 & $0f; ! bottom (4 bits)
		print "Token#: ", _i, " Type: ", p_pattern->0, " (top ", _token_top, ", next ",_token_next, ", bottom ",_token_bottom, ") Next byte: ",(p_pattern + 1)-->0,"^";
		p_pattern = p_pattern + 3;
	}
	! print ": ", i, " tokens^";
	return p_pattern + 1; ! skip TT_END
];
#EndIf;


! Keep the routines WordAddress, WordLength, NextWord and NextWordStopped just next to CheckNoun, 
! since they will typically be called from parse_name routines, which are called from CheckNoun

[ WordAddress p_wordnum;  ! Absolute addr of 'wordnum' string in buffer
	return player_input_array + parse_array->(p_wordnum*4+1);
];

[ WordLength p_wordnum;     ! Length of 'wordnum' string in buffer
	return parse_array->(p_wordnum*4);	
];

[ PeekAtNextWord _i _j _wn;
	_i = wn*2-1;
	_j = parse_array-->_i;
	if (_j == ',//') _j = comma_word;
	if (_j == './/') _j = THEN_WORD;
	return _j;
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

[ CheckNoun p_parse_pointer p_parse_length _i _j _n _p _obj _matches _last_match _current_word _name_array _name_array_len _best_score _result;
	! return 0 if no noun matches
	! return -n if more n matches found (n > 1)
	! else return object number
	! side effects: 
	!   which_object
	!     - stores all matching nouns if more than one
	!     - stores number of words consumed if single match 
	for(_i = 0: _i < scope_objects: _i++) {
		_n = wn;
		_p = p_parse_pointer;
		_current_word = p_parse_pointer-->0;
		_obj = scope-->_i;
#IfDef DEBUG;
		print "Testing ", (the) _obj, " _n is ", _n, "...^";
#EndIf;
		!   if(_obj == nothing) continue;
		if(_obj provides parse_name) {
			_j = wn;
			_result = PrintOrRun(_obj, parse_name);
			_n = _n + _result; ! number of words consumed
			wn = _j;
			if(_n > wn) {
				if(_n == _best_score) {
					which_object->_matches = _obj;
					_matches++;
#IfDef DEBUG;
					print "Same best score ", _best_score, ". Matches are now ", _matches,"^";
#EndIf;
				}
				if(_n > _best_score) {
#IfDef DEBUG;
					print "New best score - matched with parse_name ", _n,"^";
#EndIf;
					_last_match = _obj;
					_best_score = _n;
					_matches = 1;
					which_object->0 = _obj;
				}
			}
		} else if(_obj.#name > 1) {
			_name_array = _obj.&name;
			_name_array_len = _obj.#name / 2;

			while(_n <= p_parse_length && IsSentenceDivider(_p) == false) {
			!while(_n <= parse_array->1 && IsSentenceDivider(_p) == false) {
				if(_current_word == nothing) return 0; ! not in dictionary
#IfV5;
				@scan_table _current_word _name_array _name_array_len -> _result ?success;
#IfNot;
				for(_j = 0: _j < _name_array_len: _j++) {
					if(_name_array-->_j == _current_word) jump success;
				}
#EndIf;
				jump not_matched;
.success;
#IfDef DEBUG;
				print " - matched ", (address) _current_word,"^";
#EndIf;
				_n++;
				_p = _p + 4;
				_current_word = _p-->0;
				if(_n == _best_score) {
					which_object->_matches = _obj;
					_matches++;
#IfDef DEBUG;
					print "Same best score ", _best_score, ". Matches are now ", _matches,"^";
#EndIf;
				}
				if(_n > _best_score) {
					_matches = 1;
#IfDef DEBUG;
					print "New best score ", _n, ". Old score was ", _best_score,". Matches is now ",_matches,".^";
#EndIf;
					_last_match = _obj;
					_best_score = _n;
					which_object->0 = _obj;
				}
			}
		}
.not_matched;

!   print "checking ", _obj.&name-->0, " ", _current_word, "^";
	}
	if(_matches == 1) {
		which_object->0 = _best_score - wn;
#IfDef DEBUG;
		print "Matched a single object: ", (the) _last_match,
			", len ", which_object->0, "^";
#EndIf;
		return _last_match;
	}
#IfDef DEBUG;
				print "Matches: ", _matches,"^";
#EndIf;
	if(_matches > 1) return -_matches;
	return 0;
];

[ AskWhichNoun p_noun_name p_num_matching_nouns _i; 
	print "Which ", (address) p_noun_name, " do you mean? ";
	for(_i = 0 : _i < p_num_matching_nouns : _i++) {
		if(_i == 0) {
			print (The) which_object->_i;
		} else {
			if (_i == p_num_matching_nouns - 1) {
				print " or ";
			} else {
				print ", ";
			}
			print (the) which_object->_i;
		}
	}
	"?";
];

[ GetNextNoun p_parse_pointer _noun _oldwn;
	! try getting a noun from the <p_parse_pointer> entry in parse_array
	! return <noun number> if found, 0 if no noun found, or -1
	! if we should give up parsing completely (because the
	! player has entered a new command line).
	! 
	! Side effects:
	! - if found, then wn will be updated
	! NOTE: you need to update parse_pointer after calling GetNextNoun since
	! wn can change
	_noun = CheckNoun(p_parse_pointer, parse_array->1);
.recheck_noun;
	if(_noun < 0) {
		AskWhichNoun(p_parse_pointer --> 0, -_noun);
		! read a new line of input
		! I need to use parse_array since NextWord
		! for parse_name and others hardcode this
		! usage, so I first store the old input into
		! temp arrays that I will restore if I can
		! disambiguate successfully.
		CopyInputArray(player_input_array, temp_player_input_array);
		CopyParserArray(parse_array, temp_parse_array);
		ReadPlayerInput();
		! is this a reply to the question?
		if((parse_array->1 == 1) &&  
			(((parse_array + 2) --> 0) + DICT_BYTES_FOR_WORD)->0 & 1 == 0) {
			! only one word, and it is not a verb. Assume
			! a valid reply and add the other 
			! entry into parse_array, then retry
			_oldwn = wn; ! wn is used in CheckNoun, so save it
			wn = 1;
			_noun = CheckNoun(parse_array+2, 1);
			wn = _oldwn; ! and restore it after the call
			if(_noun > 0) {
				! TODO: here I assume that only one word was
				! give in the original input
				! > take book
				! which book, the blue or the green
				! > green
				! and then I skip book when accepting the
				! new reply. Will fail if more then
				! one noun word before disambiguation
				which_object->0 = 1;
				! don't forget to restore the old arrays
				CopyInputArray(temp_player_input_array, player_input_array);
				CopyParserArray(temp_parse_array, parse_array);
				jump recheck_noun;
			}
		}
		! completely new input.
		return -1; ! start from the beginning
	} else if(_noun > 0) {
#IfDef DEBUG;
		print "Noun match! ", _noun, " ", which_object -> 0, "^";
#EndIf;
		wn = wn + which_object->0;
		return _noun;
	} else {
		! this is not a recognized word at all
		return 0;
	}
];

[ IsSentenceDivider p_parse_pointer;
	! check if current parse_array block, indicated by p_parse_pointer,
	! is a period or other sentence divider
	return p_parse_pointer --> 0 == './/';
];

[ ParseAndPerformAction _verb _word_data _verb_num _verb_grammar _i _pattern _pattern_index _token _token_type _token_data _parse_pointer _noun_tokens _noun _check_held _check_creature;
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

	action = -1;
	which_object --> 0 = 0;
	if(IsSentenceDivider(parse_array + 2))
		return -1;

	actor = player;
	UpdateScope(GetVisibilityCeiling(player));

	if(parse_array->1 < 1) {
		"Come again?";
	}

	verb_wordnum = 1;

.reparse;
	_verb = (parse_array + 2 + 4 * (verb_wordnum - 1)) --> 0;
	if(_verb < (0-->HEADER_DICTIONARY)) {
		! unknown word
#IfDef DEBUG;
		print "Case 1, Word ", _verb, "^";
#EndIf;
		"I don't understand that word.";
	}

	_word_data = _verb + DICT_BYTES_FOR_WORD;
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
		_noun = CheckNoun(parse_array+2, parse_array->1);
		if(_noun > 0) {
			! The sentence starts with a noun, now 
			! check if comma afterwards
			wn = wn + which_object->0;
			_pattern = NextWord();
			if(_pattern == comma_word) {
				jump conversation;
			}
		}
		"That is not a verb I recognize.";
.conversation;
		if(_noun hasnt animate) {
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
	verb_word = _verb;
	meta = (_word_data->0) & 2;

!   print "Parse array: ", parse_array, "^";
!   print "Word count: ", parse_array->0, "^";
!   print "Word 1: ", (parse_array + 2)-->0, "^";
!   print "Word 2: ", (parse_array + 6)-->0, "^";
!   print "Word 3: ", (parse_array + 10)-->0, "^";
	_verb_num = 255 - (_word_data->1);
	_verb_grammar = (0-->HEADER_STATIC_MEM)-->_verb_num;

#IfDef DEBUG;
	print "Verb#: ",_verb_num,", meta ",meta,".^";
	print "Grammar address for this verb: ",_verb_grammar,"^";
	print "Number of patterns: ", _verb_grammar->0 ,"^";

	! First print all patterns, for debug purposes
	_pattern = _verb_grammar + 1;
	for(_i = 0 : _i < _verb_grammar->0: _i++) {
		print "############ Pattern ",_i,"^";
		_pattern = CheckPattern(_pattern);
	}
	@new_line;
#EndIf;

	_pattern = _verb_grammar + 1;
	for(_i = 0 : _i < _verb_grammar->0 : _i++) {
#IfDef DEBUG;
		print "############ Pattern ",_i," address ", _pattern, "^";
#EndIf;
		wn = verb_wordnum + 1;
		_parse_pointer = parse_array + 2 + 4*(verb_wordnum);
		_pattern_index = _pattern - 1;
		_noun_tokens = 0;
		noun = 0;
		second = 0;
		inp1 = 0;
		inp2 = 0;
		multiple_objects --> 0 = 0;
		_check_held = 0;
		_check_creature = 0;

		while(true) {
			_pattern_index = _pattern_index + 3;
			_token = (_pattern_index -> 0);
			_token_type = _token & $0f;
			_token_data = (_pattern_index + 1) --> 0;
#IfDef DEBUG;
			print "TOKEN: ", _token, " wn ", wn, " _parse_pointer ", _parse_pointer, " _token_data ", _token_data, "^";
#EndIf;

			if(_token_type == TT_END) {
				if(IsSentenceDivider(_parse_pointer)) {
					wn++;
					jump parse_success;
				}
				if(wn == 1 + parse_array->1) {
					jump parse_success;
				}
				break; ! Fail because the grammar line ends here but not the input
			}
			if(wn >= 1 + parse_array->1) { !Fail because input ends here but not the grammar line
#IfDef DEBUG;
				print "Fail, since grammar line has not ended but player input has.^";
#EndIf;
				break;
			}
#IfDef DEBUG;
			print "token type ", _token_type, ", data ",_token_data,"^";
#EndIf;
			if(_token_type == TT_PREPOSITION) { 
#IfDef DEBUG;
				print "Preposition: ", _token_data, "^";
#EndIf;
				if(_parse_pointer --> 0 == _token_data) {
#IfDef DEBUG;
					print "Match!^";
#EndIf;
					wn++;
					_parse_pointer = _parse_pointer + 4;
					while(_token == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) { ! Alternative prepositions which are not at the end of the list
#IfDef DEBUG;
						print "Skipping one alternative...^";
#EndIf;
						_pattern_index = _pattern_index + 3;
						_token = _pattern_index -> 0;
					}
					continue;
				}
#IfDef DEBUG;
				print "Failed prep: ", _parse_pointer, ":", _parse_pointer --> 0, " should have been ", _token_data, "^";
#EndIf;
				if(_token == TOKEN_FIRST_PREP or TOKEN_MIDDLE_PREP) continue; ! First in a list or in the middle of a list of alternative prepositions, so keep parsing!
				break; ! Fail because this is the only or the last alternative preposition and the word in player input doesn't match it
			} else if(_token_type == TT_OBJECT) {
				! here _token_data will be one of 
				! NOUN_OBJECT, HELD_OBJECT, MULTI_OBJECT, MULTIHELD_OBJECT,
				! MULTIEXCEPT_OBJECT, MULTIINSIDE_OBJECT, CREATURE_OBJECT,
				! SPECIAL_OBJECT, NUMBER_OBJECT or TOPIC_OBJECT
				!
				! first take care of take all/drop all
				if(_parse_pointer-->0 == ALL_WORD &&
					_token_data == MULTI_OBJECT or MULTIHELD_OBJECT) {
					! take all etc.
					! absort the "all" keyword
					wn = wn + 1;
					_parse_pointer = _parse_pointer + 4;
					! Add all reasonable objects in scope
					! to the multiple_objects array
					AddMultipleNouns(_token_data);
					if(multiple_objects --> 0 == 0) {
						print "Nothing to do!^";
						return -wn;
					}
					continue;
				}
				if(_token_data == NOUN_OBJECT or HELD_OBJECT or CREATURE_OBJECT) {
					_noun = GetNextNoun(_parse_pointer);
					if(_noun == 0) break;
					if(_noun == -1) rfalse;
					_parse_pointer = parse_array + 2 + 4 * (wn - 1);
					if(_token_data == CREATURE_OBJECT && _noun hasnt animate)
						_check_creature = _noun;
					if(_token_data == HELD_OBJECT && _noun notin player) {
						_check_held = _noun;
					}
					if(_noun_tokens == 0) {
						noun = _noun;
						inp1 = _noun;
					} else if(_noun_tokens == 1){
						second = _noun;
						inp2 = _noun;
					}
					_noun_tokens++;
				} else if(_token_data == MULTI_OBJECT or MULTIHELD_OBJECT) {
					for(::) {
						if(PeekAtNextWord() == comma_word or AND_WORD) {
							wn = wn + 1;
							_parse_pointer = _parse_pointer + 4;
							continue;
						}
						_noun = GetNextNoun(_parse_pointer);
						if(_noun == 0) break;
						if(_noun == -1) rfalse;
						_parse_pointer = parse_array + 2 + 4 * (wn - 1);
						multiple_objects --> 0 = 1 + (multiple_objects --> 0);
						multiple_objects --> (multiple_objects --> 0) = _noun;
					}
					if(multiple_objects --> 0 == 0) {
						! no nouns found, so this pattern didn't match
						break;
					}
				} else if(_token_data == TOPIC_OBJECT) {
					! TODO: for now, only absorb one topic word
					consult_from = wn;
					consult_words = 1;
					wn = wn + 1;
					_parse_pointer = _parse_pointer + 4;
				!TODO } else if(_token_data == MULTIEXCEPT_OBJECT) {
				!TODO } else if(_token_data == MULTIINSIDE_OBJECT) {
				!TODO } else if(_token_data == SPECIAL_OBJECT) {
				!TODO } else if(_token_data == NUMBER_OBJECT) {
				} else {
					RunTimeError("unexpected _token_data");
					break;
				}
			!TODO } else if(_token_type == TT_ROUTINE_FILTER) {
			!TODO } else if(_token_type == TT_ATTR_FILTER) {
			!TODO } else if(_token_type == TT_SCOPE) {
			!TODO } else if(_token_type == TT_PARSE_ROUTINE) {
			} else {
					RunTimeError("unexpected _token_type");
					break;
			}
		}
		! This pattern has failed.
#IfDef DEBUG;
		print "Pattern didn't match.^";
#EndIf;
		! Scan to the end of this pattern
		while(_pattern_index -> 0 ~= TT_END) _pattern_index = _pattern_index + 3;
		_pattern = _pattern_index + 1;
	}

	"Sorry, I didn't understand that.";

.parse_success;
	action = (_pattern --> 0) & $03ff;

	if(actor ~= player) {
		! The player's "orders" property can refuse to allow conversation
		! here, by returning true.  If not, the order is sent to the
		! other person's "orders" property.  If that also returns false,
		! then: if it was a misunderstood command anyway, it is converted
		! to an Answer action (thus "floyd, grrr" ends up as
		! "say grrr to floyd").  If it was a good command, it is finally
		! offered to the Order: part of the other person's "life"
		! property, the old-fashioned way of dealing with conversation.
		_i = RunRoutines(player, orders);
		if (_i == 0) {
			_i = RunRoutines(actor, orders);
			if (_i == 0) {
				if(action == ##NotUnderstood) {
					_pattern-->3 = actor; actor = player; action = ##Answer;
					jump parse_success;
				}
				if(RunLife(actor, ##Order) == 0) {
					print_ret (The) actor, " has better things to do.";
				}
			}
		}
		return -(wn - 1);
	}
	if(_check_held > 0) {
		print "(first taking ", (the) _check_held, ")^^";
		keep_silent = true;
		<take _check_held>;
		keep_silent = false;
		if(_check_held notin player) rtrue;
	}
	if(_check_creature > 0 && _check_creature hasnt animate)
		"You can only do that to something animate.";

	if(multiple_objects --> 0 == 0) {
		! single action
		PerformPreparedAction();
	} else {
		! multiple action
		! (a) check the multiple list isn't empty;
		! (b) warn the player if it has been cut short because too long;
		! (c) generate a sequence of actions from the list
		!     (stopping in the event of death or movement away).
		for(_noun = 1: _noun <= multiple_objects --> 0 : _noun++) {
			inp1 = multiple_objects --> _noun;
			noun = inp1;
			print (name) noun, ": ";
			PerformPreparedAction();
		}
	}
	return -(wn - 1);
];

[ AddMultipleNouns multiple_objects_type   _i _addobj _obj;
	multiple_objects --> 0 = 0;
	for(_i = 0: _i < scope_objects: _i++) {
		_obj = scope-->_i;
		_addobj = false;
		switch(multiple_objects_type) {
		MULTI_OBJECT:
			_addobj = _obj hasnt scenery or concealed;
		MULTIHELD_OBJECT:
			_addobj = _obj in player;
		}
		if(_addobj) {
			multiple_objects --> 0 = 1 + (multiple_objects --> 0);
			multiple_objects --> (multiple_objects --> 0) = _obj;
			!print "Adding ", (name) _obj, "^";
		}
	}
];
