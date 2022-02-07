System_file;

! Before including this file, define a word array called talk_array.
! For each NPC the player should be able to talk to, add a section starting
! with TM_NPC and the NPCs object ID. Then add one or more conversation topics.
! End the array with TM_NPC 0. Also, each NPC which the player should be able
! to talk to must have the property talk_start.
!
! To initialise the system, either:
!
! (a) (more convenient) In your Initialise routine , call InitTalk()
!     OR
! (b) (better performance):
!   1. Compile the game in DEBUG mode
!   2. Run the game and type "TALK SETUP"
!   3. Enter the printed values into the talk_start property for each NPC
!   4. Define the constant NO_INITTALK to skip the InitTalk routine, as it
!      won't be needed.
!   5. If you add or remove any values in talk_array, comment out the
!      NO_INITTALK constant and go to step 1 again.
!
! Recommendation: Stick to (a) until your game is ready for release, then switch
! to (b). This makes the game start a little faster and saves some bytes.
!
! A talk topic has the following form:
! ACTIVE [ID] TOPIC PLAYERSAYS NPCSAYS [FLAGREF|UNLOCKREF|ROUTINE]*
!
! ACTIVE is either 0 ( = TM_INACTIVE = not active) or
!   1000 ( = TM_ACTIVE  = active) or 1001 ( = TM_STALE, has been used).
! ID is a number (500-900) which can be used as a reference to activate
!   a topic in code or using an UNLOCKREF in talk_array.
! TOPIC is a string or routine for the topic name
! PLAYERSAYS is a string or routine for what the player says
! NPCSAYS is a string or routine for what the NPC replies
! FLAGREF is a number 1-499 for a flag# to be set.
! UNLOCKREF is either an ID (500-900) or a relative reference to a topic (901
!    to 999) that is activated by this topic. 901 means the next topic, 902 the
!    topic after that etc. The target topic has to have status
!    TM_INACTIVE (= 0) or TM_ACTIVE (= 1000) for this to work. When a topic
!    is used, it is set to status TM_STALE, and the only way to activate it
!    when it's stale is to call ReActivateTopic.
! ROUTINE is a routine to be run. In this routine, the global variable
!    current_talker refers to the NPC.
!
! Example of an array giving Linda one active topic (Weather), which will
! activate the next topic (Heat) and topic 500 (Herself):
!
! Array talk_array -->
! TM_NPC Linda
! 0 500 "Herself" "Tell me more about yourself!" "I'm just an average girl."
! 1000 "Weather" "How do you like the weather?" "It's too hot for me." 901 500
! 0 "Heat" "Say, don't you like hot weather?" "No, I prefer it cold."
! TM_NPC 0;
!
! Apart from activating topics using UNLOCKREFs in the talk_array, you can also
! use these routines:
! InitTalk()
!   Must be called before the player starts their first conversation, or code
!   calls any of the other procedures. Calling it in Initialise is a good idea.
! ActivateTopic(NPC, topic) => true for success, false for fail
!   Activates the topic if it's currently inactive or active (not stale!).
! ReActivateTopic(NPC, topic) => true for success, false for fail
!   Activates the topic, regardless of its status.
! InactivateTopic(NPC, topic) => true for success, false for fail
!   Inactivates the topic if it's currently inactive or active (not stale!).
! ReInactivateTopic(NPC, topic) => true for success, false for fail
!   Inactivates the topic, regardless of its status.
! GetTopicStatus(NPC, topic) => topic status (TM_INACTIVE, TM_ACTIVE, TM_STALE)
!   Returns the status of the topic.
!
! If you call these routines with DEBUG defined *and* RUNTIME_ERRORS > 0, you
! will be notified whenever a problem is detected. As usual, use
! RUNTIME_ERRORS = 2 to get the maximum amount of information. (This is the
! default when compiling in DEBUG mode)

! Define constants needed if compiled with the standard library
#Ifndef RUNTIME_ERRORS;
Constant RUNTIME_ERRORS = 2;
#Endif;
#Ifndef RTE_MINIMUM;
Constant RTE_MINIMUM = 0;
Constant RTE_NORMAL = 1;
Constant RTE_VERBOSE = 2;
Constant TM_NOT_PUNY;
#Endif;


Global current_talker;

#Ifdef TM_MSG_TOPICS_DEPLETED;
#Ifdef TM_MSG_EXIT;
Constant TM_SKIP_MSG_EXIT;
#Endif;
#Endif;

#Ifndef TM_SKIP_MSG_EXIT;
Constant TM_REUSABLE_MSG_EXIT "With that, you politely end the conversation.";
#Endif;

#Ifndef TM_MSG_YOU;
Constant TM_MSG_YOU "You";
#Endif;

#Ifndef TM_MSG_TALK_ABOUT_WHAT;
[ TM_MSG_TALK_ABOUT_WHAT;
	print "Talk to ", (the) current_talker, " about:^";
];
#Endif;
#Ifndef TM_MSG_TOPICS_DEPLETED;
Constant TM_MSG_TOPICS_DEPLETED TM_REUSABLE_MSG_EXIT;
#Endif;
#Ifndef TM_MSG_EXIT;
Constant TM_MSG_EXIT TM_REUSABLE_MSG_EXIT;
#Endif;
#Ifndef TM_MSG_NO_TOPICS;
Constant TM_MSG_NO_TOPICS "Right now, you wouldn't know what to talk about.";
#Endif;
#Ifndef TM_MSG_EXIT_OPTION;
Constant TM_MSG_EXIT_OPTION "[ENTER] End conversation";
#Endif;
#Ifndef TM_MSG_PAGE_OPTION;
Constant TM_MSG_PAGE_OPTION "[N] Next page";
#Endif;

Constant TM_INACTIVE 0;
Constant TM_ACTIVE 1000;
Constant TM_STALE 1001;
Constant TM_NPC 1002;

[ _TMPrintMsg p_msg p_no_newline;
	if(metaclass(p_msg) == Routine) {
		p_msg();
		rtrue;
	}
	print (string) p_msg;
	if(p_no_newline == false) new_line;
];

[ _TMCallOrPrint p_index p_no_newline;
	_TMPrintMsg(talk_array-->p_index, p_no_newline);
];

[ _SetTopic p_topic p_start p_value _val _neg;
	! p_topic is 1-99: Act on topic number P_TOPIC, counting from p_start
	! p_topic is 500-900: Act on topic with ID = P_TOPIC.
	! p_topic is (-900)-(-500): Act on topic with ID = -P_TOPIC. If p_value is
	!   TM_ACTIVE, set the topic to active even if it's currently in status
	!   TM_STALE.
	! p_start: Start from this index in the talk_menu array and work forward,
	!   until a TM_NPC token is found.
	! p_value is TM_ACTIVE: Set the topic to active, if it's currently in
	!   status TM_INACTIVE (0) or TM_ACTIVE (1000). If p_topic is negative, set
	!   it to active even if the topic is in status TM_STALE.
	! p_value is TM_INACTIVE: Set the topic to inactive, if it's currently in
	!   status TM_INACTIVE (0) or TM_ACTIVE (1000). If p_topic is negative, set
	!   it to inactive even if the topic is in status TM_STALE.
	! p_value is 1: Return the current status of the topic.
	! Return true if setting succeeded, false if not.
	!
	! Examples:
	! Activate the next topic from index 123:
	!   _SetTopic(1, 123, TM_ACTIVE);
	! Activate topic with ID 501, starting from index 123:
	!   _SetTopic(501, 123, TM_ACTIVE);
	! Reactivate topic with ID 501, starting from index 123:
	!   _SetTopic(-501, 123, TM_ACTIVE);
	! Inactivate topic with ID 501, starting from index 123:
	!   _SetTopic(501, 123, TM_INACTIVE);

	_neg = -p_topic;
	p_start--;
	while(true) {
		p_start++;
		_val = talk_array-->p_start;
		if(_val == TM_NPC) {
			rfalse; ! The topic wasn't found
		}
		if(_val ==TM_INACTIVE or TM_ACTIVE or TM_STALE) {
			if(p_topic < 100 && p_topic > 0) {
				if(p_topic-- == 1) jump found;
				continue;
			}
			p_start++;
			if(talk_array-->p_start == p_topic or _neg) {
				p_start--;
.found;
				if(p_value == 1)
					return talk_array-->p_start;
				if((p_value == TM_ACTIVE &&
							(_val == TM_INACTIVE or TM_ACTIVE || _neg > 0)) ||
						(p_value == TM_INACTIVE &&
							(_val == TM_INACTIVE or TM_ACTIVE || _neg > 0))) {
					talk_array-->p_start = p_value;
					rtrue; ! Success
				}
				rfalse;
			}
		}
	}
	rfalse; ! Fail
];

[ ActivateTopic p_npc p_topic p_start;
	if(p_start == 0) p_start = p_npc.talk_start;
	p_start = _SetTopic(p_topic, p_start, TM_ACTIVE);
	#Ifdef DEBUG;
	#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
		if(p_start == 0) {
	#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
			print "ERROR: Talk_menu: Could not activate topic ", p_topic, " for NPC ", (name) p_npc, "!^";
	#Ifnot;
			print "ERROR: Talk_menu #3!^";
	#Endif;
			rfalse;
		}
	#Endif;
	#Endif;
	return p_start;
];

[ ReActivateTopic p_npc p_topic;
	return ActivateTopic(p_npc, -p_topic);
];

[ InactivateTopic p_npc p_topic p_start;
	if(p_start == 0) p_start = p_npc.talk_start;
	p_start = _SetTopic(p_topic, p_start, TM_INACTIVE);
	#Ifdef DEBUG;
	#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
		if(p_start == 0) {
	#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
			print "ERROR: Talk_menu: Could not inactivate topic ", p_topic, " for NPC ", (name) p_npc, "!^";
	#Ifnot;
			print "ERROR: Talk_menu #3!^";
	#Endif;
			rfalse;
		}
	#Endif;
	#Endif;
	return p_start;
];

[ ReInactivateTopic p_npc p_topic;
	return InActivateTopic(p_npc, -p_topic);
];

[ GetTopicStatus p_npc p_topic p_start;
	if(p_start == 0) p_start = p_npc.talk_start;
	return _SetTopic(p_topic, p_start, 1);
];


#Ifv5;
Array TenDashes -> "----------";
[ FastDashes p_dashes;
	while(p_dashes > 10) {
		@print_table TenDashes 10 1;
		p_dashes = p_dashes - 10;
	}
	@print_table TenDashes p_dashes 1;
];
#Endif;

#Ifndef NO_INITTALK;
[ InitTalk _i _val;
	! Initialise the conversation system
	_i = 0;
	while(true) {
		_val = talk_array-->_i;
		if(_val == TM_NPC) {
			_i++;
			_val = talk_array-->_i;
			if(_val == 0) break;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
			if(~~(_val provides talk_start)) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
				"ERROR: Talk_menu: Object ", _val, " (", (name) _val, "), which has
					talk topics in talk_array, doesn't provide talk_start!";
#Ifnot;
				"ERROR: Talk_menu #2!";
#Endif;
			}
#Endif;
			_val.talk_start = _i + 1;
		}
 		_i++;
	}
];
#Endif;


#Ifv5;
[ RunTalk p_npc _i _j _n _val _tm_start _height _width _offset _count _more;
#Ifnot;
[ RunTalk p_npc _i _j _n _val _tm_start;
#Endif;
	current_talker = p_npc;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(~~(p_npc provides talk_start)) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
		"ERROR: Talk_menu: Object ", p_npc, " (", (name) p_npc, ") doesn't provide talk_start!";
#Ifnot;
		"ERROR: Talk_menu #1!";
#Endif;
	}
#Endif;
	! Prepare upper window
#Ifv5;
	! Find out or guess the height of the screen
	_height = HDR_SCREENHLINES->0;
	_width = HDR_SCREENWCHARS->0;
	if(_height < 2) _height = 25;
	! Figure out best height for upper window. Note that _height now changes
	! meaning, from holding the height of the screen to holding the height of
	! the upper window
	if(_height > 31) _height = 16;
	else if(_height > 13) @log_shift _height (-1) -> _height; !Division by 2
	else _height = 7;
!	@erase_window $ffff;
	@split_window _height;
#Endif;

	! Print all valid lines to say

.restart_talk_after_line;

#Ifv5;
	_offset = 0;
#Endif;

.restart_talk;

#Ifv5;
	@erase_window 1;
	DrawStatusLine();
	@set_window 1;
	@set_cursor 2 1;
	_count = 0;
	_more = 0;
#Endif;
	_tm_start = p_npc.talk_start;
	_i = _tm_start - 1;
	_n = 0;
	while(true) {
		_i++;
		_val = talk_array-->_i;
		if(_val == TM_NPC) break;
		if(_val == TM_ACTIVE) {
#Ifv5;
			if(_count >= _height - 6) { _more = 1; break; }
			_n++;
			if(_n <= _offset) continue;
			_count++;
!			print "Talk to ", (the) p_npc, " about:^";
			if(_count == 1) _TMPrintMsg(TM_MSG_TALK_ABOUT_WHAT);
			print "  ", _count % 10, ": ";
#Ifnot;
			_n++;
!			print "Talk to ", (the) p_npc, " about:^";
			if(_n == 1) _TMPrintMsg(TM_MSG_TALK_ABOUT_WHAT);
			print "  ", _n, ": ";
#Endif;
			_i++;
			_val = talk_array-->_i;
			if(_val < 1000) _i++; ! An ID was found
			_TMCallOrPrint(_i);
		}
	}
	if(_n == 0) {
#Ifv5;
		@set_window 0;
#Endif;
		_val = TM_MSG_NO_TOPICS;
!		"Right now, you wouldn't know what to talk about.";
		if(_j)
			_val = TM_MSG_TOPICS_DEPLETED;
!			"With that, you politely end the conversation.";
		_TMPrintMsg(_val);
#Ifv5;
		jump end_of_talk;
#Ifnot;
		rtrue;
#Endif;
	}

#Ifv5;
	_i = _height - 1;
	@set_cursor _i 1;
	FastDashes(_width);
	_i = _height - 3;
	@set_cursor _i 1;
#Endif;
	_TMPrintMsg(TM_MSG_EXIT_OPTION, true);
#Ifv5;
	if(_more || _offset) {
		print "  ";
		_TMPrintMsg(TM_MSG_PAGE_OPTION, true);
	}
	new_line;
#Ifnot;
	new_line;
#Endif;

	! Ask player to choose a line to say

	_j = 0;
	while(_j == 0) {
#IfV5;
!		_j = _height - 1;
!		@set_cursor _j 1;
!		PrintMsg(MSG_PROMPT);
	!	DrawStatusLine();
!		buffer->1 = 0;
!		@aread buffer parse -> _val;
		@read_char -> _val;

		if(_val == 13 or 'q' or 'Q' or 'x' or 'X') {
			@set_window 0;
			_TMPrintMsg(TM_MSG_EXIT);
!			"With that, you politely end the conversation.";
			jump end_of_talk;
		}
		if(_val == 'n' or 'N' or 130) {
			if(_more) {
				_offset = _offset + _height - 4;
				jump restart_talk;
			}
			else if(_offset) {
				_offset = 0;
				jump restart_talk;
			}
		}
		_j = _val - 48;
		if(_j == 0) _j = 10;
		if(_j >= 0 && _j <= 10)
			_j = _j + _offset;
#IfNot;
		! This is v3
		PrintMsg(MSG_PROMPT);
		@sread buffer parse;
		num_words = parse->1;

!		_ReadPlayerInput(true);

		if(num_words == 0) {
			_TMPrintMsg(TM_MSG_EXIT);
!			"With that, you politely end the conversation.";
			rtrue;
		}
		_j = TryNumber(1);
#EndIf;
		if(_j < 1 || _j > _n) _j = 0;
	}

	! Print the line and reply

	_i = _tm_start - 1;
	_n = 0;
	while(true) {
		_i++;
		_val = talk_array-->_i;
!		print "_i is ", _i, ", _val is ", _val, "^";
		if(_val == TM_ACTIVE) {
			_n++;
			if(_n < _j) continue;
			! This is the entry we're looking for
			talk_array-->_i = TM_STALE;
			_i++;
			_val = talk_array-->_i;
			if(_val < 1000) _i++; ! An ID was found
			_i++;
#Ifv5;
			@set_window 0;
#Endif;
			_TMPrintMsg(TM_MSG_YOU, true);
			print ": ";
			_TMCallOrPrint(_i);
			_i++;
			print (The) p_npc, ": ";
			_TMCallOrPrint(_i);
			break;
		}
	}

	! Apply effects

	_j = _i;
	while(true) {
		_j++;
		_val = talk_array-->_j;
!		print "Performing action ", _val, " at pos ", _j, "^";
		if(_val == 0 or TM_ACTIVE or TM_STALE or TM_NPC) {
			break;
		}
		if(metaclass(_val) == Routine) {
			_TMCallOrPrint(_j);
			continue;
		}
		if(_val < 500) {
			SetFlag(_val);
			continue;
		}
		if(_val < 901) {
			ActivateTopic(p_npc, _val);
			continue;
		}
		if(_val < 1000) {
			ActivateTopic(p_npc, _val - 900, _j);
			continue;
		}
		#Ifdef DEBUG;
			#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
				#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
					print "ERROR: Talk_menu: Action ", _val, " was not understood for ", (name) p_npc, ".";
				#Ifnot;
					print "ERROR: Talk_menu #4!";
				#Endif;
			#Endif;
		#Endif;
	}

	new_line;
	jump restart_talk_after_line;
.end_of_talk;
#Ifv5;
	@erase_window 1;
	@split_window 0;
	@set_window 0;
#Ifdef PUNYINFORM_MAJOR_VERSION;
	statusline_current_height = 0;
#Ifnot;
	gg_statuswin_cursize = 0;
#Endif;
#Endif;
];

[ TalkSub;
#Ifdef TM_NOT_PUNY;
   if (noun==player) { L__M(##Tell, 1, noun); rtrue; }
   if (~~(noun provides talk_start)) { L__M(##Show, 2, second); rtrue; }
#Ifnot;
   if (noun==player) { PrintMsg(MSG_TELL_PLAYER); rtrue; }
   if (~~(noun provides talk_start)) { second = noun; PrintMsg(MSG_SHOW_DEFAULT); rtrue; }
#Endif;
   RunTalk(noun);
   AfterRoutines();
];

Verb 'talk' 'converse' 'interview' 'interrogate'
    *       'to'/'with' creature             ->Talk
    *       creature                         ->Talk;


#Ifdef DEBUG;
[ TalkSetupSub _i _len _val;
	! Initialise the conversation system
	_len = talk_array-->0;
!		print _len;
	_i = 0;
	while(true) {
		_val = talk_array-->_i;
		if(_val == TM_NPC) {
			_i++;
			_val = talk_array-->_i;
			if(_val == 0) break;
			if(~~(_val provides talk_start)) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
				print "ERROR: Talk_menu: Object ", _val, " (", (name) _val,
					"), which has talk topics in talk_array, doesn't
					provide talk_start!^";
#Ifnot;
				print "ERROR: Talk_menu #2!^";
#Endif;
			}
			print "Object ", _val, " (", (name) _val, "): talk_start = ", _i + 1, "^";
		}
 		_i++;
	}
];

Extend 'talk'
	* 'setup' -> TalkSetup;
#Endif;
