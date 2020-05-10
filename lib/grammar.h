! ######################### Grammar + Actions


[ LookSub _obj _ceil _player_parent;

	_ceil = GetVisibilityCeiling(player);

! print "Ceiling is object ", _ceil, ": ", (object) _ceil, ".^";
	! ### Print room name
	if(_ceil == location) {
		PrintObjName(location);
	} else {
		print (The) _ceil;
	}

	_player_parent = parent(player);
	if(_player_parent ~= _ceil) {
		if(_player_parent has supporter) print " (on ";
		else print " (in ";
		print (the) _player_parent, ")";
	}

	@new_line;

	! ### Print room description
	if(_ceil.description) {
		PrintOrRun(_ceil, description, 1);
	}

	PrintContents(" You can also see ", " here.", _ceil);
	@new_line;

	objectloop(_obj in _ceil) {
		if(_obj hasnt moved && _obj.initial ~= 0) {
			@new_line;
			PrintOrRun(_obj, initial);
		}
	}

];

[ ExamineSub;
	if(noun provides description) {
		PrintOrRun(noun, description);
	} else {
		PrintMsg(MSG_EXAMINE_NOTHING_SPECIAL);
	}
];

[ QuitSub;
	PrintMsg(MSG_AREYOUSUREQUIT);
	if(YesOrNo()) {
		deadflag = GS_QUIT;
	}
];

[ TryToTakeNoun;
    ! Try to transfer the given item to the player: return false
    ! if successful, true if unsuccessful, printing a suitable message
    ! in the latter case.
    ! People cannot ordinarily be taken.
    if(noun == player) { PrintMsg(MSG_TAKE_YOURSELF); rtrue; }
    if(noun has animate) { PrintMsg(MSG_TAKE_ANIMATE); rtrue; }
    if(noun has scenery) { PrintMsg(MSG_TAKE_SCENERY); rtrue; }
    if(noun has static) { PrintMsg(MSG_TAKE_STATIC); rtrue; }
	if(noun in player) { PrintMsg(MSG_TAKE_ALREADY_HAVE); rtrue; }
	if(IndirectlyContains(noun, player)) { PrintMsg(MSG_TAKE_PLAYER_PARENT, noun); rtrue; }
    if(AtFullCapacity(player)) { PrintMsg(MSG_TAKE_NO_CAPACITY); rtrue; }

	move noun to player;
	give noun moved;
	rfalse;
];

[ TakeSub;
	if(TryToTakeNoun() == 1) rtrue;
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
	PrintMsg(MSG_TAKE_SUCCESS);
];

[ EatSub;
    if(noun has animate) { PrintMsg(MSG_EAT_ANIMATE); rtrue; }
    PrintMsg(MSG_EAT_INEDIBLE);
];

[ DrinkSub;
	PrintMsg(MSG_DRINK_NOTHING_SUITABLE);
];

[ DropSub;
	if(noun notin player) { PrintMsg(MSG_DROP_NOT_HOLDING); rtrue; }
	move noun to location;
	if(AfterRoutines() == 1) rtrue;
    if(keep_silent) return;
	PrintMsg(MSG_DROP_DROPPED);
];

[ ThrowAtSub;
	if(ObjectIsUntouchable(noun)) return;
	if(second > 1) {
		action = ##ThrownAt;
		if (RunRoutines(second, before) ~= 0) { action = ##ThrowAt; rtrue; }
		action = ##ThrowAt;
	}
	if(noun has worn) {
        PrintMsg(MSG_THROW_FIRST_TAKING);
		if (noun has worn && noun in player) rtrue;
	}
	if(second hasnt animate) { PrintMsg(MSG_THROW_ANIMATE); rtrue; }
	if(RunLife(second,##ThrowAt) ~= 0) rfalse;
	PrintMsg(MSG_THROW_SUCCESS);
];

[ OpenSub;
	if(noun hasnt openable) { PrintMsg(MSG_OPEN_YOU_CANT); rtrue; }
	if(noun has open) { PrintMsg(MSG_OPEN_ALREADY); rtrue; }
	give noun open;
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
	PrintMsg(MSG_OPEN_SUCCESS);
];

[ CloseSub;
	if(noun hasnt openable) { PrintMsg(MSG_CLOSE_YOU_CANT); rtrue; }
	if(noun hasnt open) { PrintMsg(MSG_CLOSE_NOT_OPEN); rtrue; }
	give noun ~open;
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
	PrintMsg(MSG_CLOSE_SUCCESS);
];

[ ShowSub;
    if (parent(noun) ~= player) { PrintMsg(MSG_SHOW_NOT_HOLDING); rtrue; }
    if (second == player) { <Examine noun>; rtrue; }
    if (RunLife(second, ##Show) ~= 0) rfalse;
    PrintMsg(MSG_SHOW_SUCCESS);
];

[ SmellSub;
    PrintMsg(MSG_SMELL_SUCCESS);
];

[ GiveSub;
    if (parent(noun) ~= player) { PrintMsg(MSG_GIVE_NOT_HOLDING); rtrue; }
    if (second == player)  { PrintMsg(MSG_GIVE_PLAYER); rtrue; }
    if (RunLife(second, ##Give) ~= 0) rfalse;
    PrintMsg(MSG_GIVE_SUCCESS);
];

[ AskSub;
    if (RunLife(noun,##Ask) ~= 0) rfalse;
    PrintMsg(MSG_ASK_SUCCESS);
];

[ AskForSub;
    if (noun == player) { <Inv>; rtrue; }
    PrintMsg(MSG_ASKFOR_SUCCESS);
];

[ AskToSub;
    PrintMsg(MSG_ASKTO_SUCCESS);
];

[ TellSub;
    if (noun == player) { PrintMsg(MSG_TELL_PLAYER); rtrue; }
    if (RunLife(noun, ##Tell) ~= 0) rfalse;
    PrintMsg(MSG_TELL_SUCCESS);
];

[ EnterSub;
	if(noun hasnt enterable) { PrintMsg(MSG_ENTER_YOU_CANT); rtrue; }
	if(player in noun) { PrintMsg(MSG_ENTER_ALREADY); rtrue; }
	if(noun has container && noun hasnt open) { PrintMsg(MSG_ENTER_NOT_OPEN, noun); rtrue; }
	PlayerTo(noun);
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
    PrintMsg(MSG_ENTER_SUCCESS);
];

[ ExitSub;
	if(noun == 0) noun = parent(player);
	if(player in location) { PrintMsg(MSG_EXIT_ALREADY); rtrue; }
	if(player notin noun) {
		if(IndirectlyContains(noun, player)) { PrintMsg(MSG_EXIT_FIRST_LEAVE, parent(player)); rtrue; }
		if(noun has supporter) { PrintMsg(MSG_EXIT_NOT_ON); rtrue; }
		PrintMsg(MSG_EXIT_NOT_IN);
		rtrue;
	}
	if(noun has container && noun hasnt open) { PrintMsg(MSG_EXIT_NOT_OPEN, noun); rtrue; }
	PlayerTo(parent(noun));
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
    PrintMsg(MSG_EXIT_SUCCESS);
];

[ AnswerSub;
    if (second ~= 0 && RunLife(second,##Answer) ~= 0) rfalse;
    PrintMsg(MSG_ANSWER_SUCCESS);
];

[ InvSub;
	if(child(player) == 0) { PrintMsg(MSG_INVENTORY_EMPTY); rtrue; }
    PrintMsg(MSG_INVENTORY_SUCCESS);
];

[ WearSub;
	if (noun has worn) { PrintMsg(MSG_WEAR_ALREADY_WORN); rtrue; }
	if (noun hasnt clothing) { PrintMsg(MSG_WEAR_NOT_CLOTHING); rtrue; }
    if (parent(noun) ~= player) { PrintMsg(MSG_WEAR_NOT_HOLDING); rtrue; }
	give noun worn;
	PrintMsg(MSG_WEAR_SUCCESS);
];
	

[ GoSub _prop;
	! when called Directions have been set properly
	_prop = Directions.selected_dir_prop;
	if(_prop == 0) return RuntimeError(ERR_INVALID_DIR_PROP);
	GoDir(_prop);
];

[ GoDir p_property _new_location;
	if(player notin location) { PrintMsg(MSG_GO_FIRST_LEAVE, parent(player)); rtrue; }
	if(location provides p_property) {
		@get_prop location p_property -> _new_location; ! works in z3 and z5
	}
	if(_new_location ofclass String) {
		print_ret (string) _new_location;
	}
	if(_new_location == 0) {
		if(location provides cant_go) {
			print_ret (string) location.cant_go;
		}
        PrintMsg(MSG_GO_CANT_GO); 
		rtrue;
	}
	location = _new_location;
	PlayerTo(location);
	<Look>; ! Equivalent to PerformAction(##Look);
];

#IfV3;
[ SaveSub;
	@save ?save_success;
	PrintMsg(MSG_SAVE_FAILED); 
	rtrue;
.save_success;
    PrintMsg(MSG_SAVE_SUCCESS);
#IfNot;
[ SaveSub _result;
	@save -> _result;
	if(_result == 0) { PrintMsg(MSG_SAVE_FAILED); rtrue; }
	PrintMsg(MSG_SAVE_SUCCESS); ! _result = 1: save ok, 2: Restore ok
#EndIf;
];

#IfV3;
[ RestoreSub;
	@restore ?restore_success; ! can't use @restore because of compiler test
	PrintMsg(MSG_RESTORE_FAILED);
	rtrue;
.restore_success; ! This is never reached, since a successful restore continues after save opcode.
#IfNot;
[ RestoreSub _flag;
	@restore -> _flag;
	! must have failed here so no need to check the flag
	PrintMsg(MSG_RESTORE_FAILED);
#EndIf;
];

[ RestartSub;
    PrintMsg(MSG_RESTART_CONFIRM);
	if(YesOrNo()) {
		@restart;
        PrintMsg(MSG_RESTART_FAILED);
	}
];

#IfDef DEBUG_VERBS;
[ PredictableSub _i; 
	! sets the random seed, thus making randomness predictable
	! also a test of special and number, thus the fancy grammar
	!print special_word, " ", special_number," ",parsed_number,"^";
	if(special_word ~=0) "You can't send randomness to an object!";
	_i = parsed_number;
	if(_i == 0) _i = 100; ! default
	random(-_i);
	"The random seed is set to ", _i, ".";
];
#EndIf;

[ InsertSub _ancestor;
    if (second hasnt container) { PrintMsg(MSG_INSERT_NOT_CONTAINER); rtrue; }
    if (parent(noun) == second) { PrintMsg(MSG_INSERT_ALREADY); rtrue; }
    _ancestor = CommonAncestor(noun, second);
    if (_ancestor == noun) { PrintMsg(MSG_INSERT_ITSELF); rtrue; }
    if (second ~= _ancestor) {
        if (second has container && second hasnt open) { PrintMsg(MSG_INSERT_NOT_OPEN, second); rtrue; }
    }

    if (AtFullCapacity(noun, second)) { PrintMsg(MSG_INSERT_NO_ROOM); rtrue; }

    move noun to second;
    if (keep_silent) return;
    PrintMsg(MSG_INSERT_SUCCESS);
];

[ PutOnSub _ancestor;
    if (second hasnt supporter) { PrintMsg(MSG_PUTON_NOT_SUPPORTER); rtrue; }
    if (parent(noun) == second) { PrintMsg(MSG_PUTON_ALREADY); rtrue; }
    _ancestor = CommonAncestor(noun, second);
    if (_ancestor == noun) { PrintMsg(MSG_PUTON_ITSELF); rtrue; }

    if (AtFullCapacity(noun, second)) { PrintMsg(MSG_PUTON_NO_ROOM); rtrue; }

    move noun to second;
    if (keep_silent) return;
    PrintMsg(MSG_PUTON_SUCCESS);
];

[ WakeSub;
    PrintMsg(MSG_WAKE_SUCCESS);
];

[ WakeOtherSub;
    !if (ObjectIsUntouchable(noun)) return; ! TODO
    if (RunLife(noun, ##WakeOther) ~= 0) rfalse;
    PrintMsg(MSG_WAKEOTHER_SUCCESS);
];

[ KissSub;
    !if (ObjectIsUntouchable(noun)) return; ! TODO
    if (RunLife(noun, ##Kiss) ~= 0) rfalse;
    if (noun == player) { PrintMsg(MSG_KISS_PLAYER); rtrue; }
    PrintMsg(MSG_KISS_SUCCESS);
];

[ AttackSub;
    !if (ObjectIsUntouchable(noun)) return; !TODO
    if (noun has animate && RunLife(noun, ##Attack) ~= 0) rfalse;
	PrintMsg(MSG_ATTACK_SUCCESS);
];

[ TurnSub;
    !if (ObjectIsUntouchable(noun)) return; !TODO
    if (noun has static)   { PrintMsg(MSG_TURN_STATIC); rtrue; }
    if (noun has scenery)  { PrintMsg(MSG_TURN_SCENERY); rtrue; }
    if (noun has animate)  { PrintMsg(MSG_TURN_ANIMATE); rtrue; }
    PrintMsg(MSG_TURN_SUCCESS);
];

[ SwitchOnSub;
    !if (ObjectIsUntouchable(noun)) return; !TODO
    if (noun hasnt switchable) { PrintMsg(MSG_SWITCH_ON_NOT_SWITCHABLE); rtrue; }
    if (noun has on)           { PrintMsg(MSG_SWITCH_ON_ON); rtrue; } 
    give noun on;
    if (AfterRoutines() == 1) rtrue;
    if (keep_silent == 1) rtrue;
	PrintMsg(MSG_SWITCH_ON_SUCCESS);
];

[ SwitchOffSub;
    !if (ObjectIsUntouchable(noun)) return; !TODO
    if (noun hasnt switchable) { PrintMsg(MSG_SWITCH_OFF_NOT_SWITCHABLE); rtrue; }
    if (noun hasnt on)         { PrintMsg(MSG_SWITCH_OFF_NOT_ON); rtrue; }
    give noun ~on;
    if (AfterRoutines() == 1) rtrue;
    if (keep_silent == 1) rtrue;
	PrintMsg(MSG_SWITCH_OFF_SUCCESS);
];

[ PullSub;
    !if (ObjectIsUntouchable(noun)) return; !TODO
	if (noun has static)   { PrintMsg(MSG_PULL_STATIC); rtrue; }
    if (noun has scenery)  { PrintMsg(MSG_PULL_SCENERY); rtrue; }
    if (noun has animate)  { PrintMsg(MSG_PULL_ANIMATE); rtrue; }
    PrintMsg(MSG_PULL_SUCCESS); !Nothing obvious happens
];

[ PushSub;
    !if (ObjectIsUntouchable(noun)) return;
    if (noun has static)   { PrintMsg(MSG_PUSH_STATIC); rtrue; }
    if (noun has scenery)  { PrintMsg(MSG_PUSH_SCENERY); rtrue; }
    if (noun has animate)  { PrintMsg(MSG_PUSH_ANIMATE); rtrue; }
    PrintMsg(MSG_PUSH_SUCCESS);
];


Verb 'i//' 'inventory'
	* -> Inv;

Verb 'answer' 'say' 'shout' 'speak'
    * topic 'to' creature -> Answer;

Verb 'look' 'l//'
	* -> Look
	* 'at' noun -> Examine;

Verb 'open'
	* noun -> Open;

Verb 'show' 'display' 'present'
	* creature held                             -> Show reverse
	* held 'to' creature                        -> Show;

Verb 'smell'
    *                                           -> Smell
    * noun                                      -> Smell;

Verb 'give' 'feed' 'offer' 'pay'
	* held 'to' creature                        -> Give
	* creature held                             -> Give reverse
	* 'over' held 'to' creature                 -> Give;

Verb 'ask'
	* creature 'about' topic                    -> Ask
	* creature 'for' noun                       -> AskFor
	* creature 'to' topic                       -> AskTo
	* 'that' creature topic                     -> AskTo;

Verb 'tell'
	* creature 'about' topic                    -> Tell
	* creature 'to' topic                       -> AskTo;

Verb 'close'
	* noun -> Close
	* 'off' noun                                -> SwitchOff;

Verb 'get'
	* 'up' -> Exit
	* 'out' -> Exit
	* multi -> Take;

Verb 'take'
	* multi -> Take;

Verb 'throw'
	* held 'at'/'against'/'on'/'onto' noun      -> ThrowAt;

Verb 'eat'
    * held                                      -> Eat;

Verb 'drink'
    * noun                                      -> Drink;

Verb 'drop'
	* multiheld -> Drop
	* multiexcept 'in'/'into'/'down' noun       -> Insert;

Verb 'enter'
	* noun -> Enter;

Verb 'climb'
	* 'into'/'onto' noun    -> Enter
	* 'out' 'of'/'from' noun  -> Exit;

Verb 'jump'
	* 'into'/'onto' noun    -> Enter
	* 'out' 'of'/'from' noun  -> Exit
	* 'off' noun        -> Exit;

Verb 'exit' 'leave'
	* noun -> Exit;

Verb 'put'
    * multiexcept 'in'/'inside'/'into' noun     -> Insert
	* 'on' held									-> Wear;

Verb 'insert'
    * multiexcept 'in'/'into' noun              -> Insert;

[ ADirection; 
	return (noun == Directions);
];

Verb 'go'
	* noun=ADirection -> Go
	* noun -> Enter;

Verb 'examine' 'x//'
	* noun -> Examine;

Verb meta 'quit' 'q//'
	* -> Quit;

Verb meta 'save'
	* -> Save;

Verb meta 'restore'
	* -> Restore;

Verb meta 'restart'
	* -> Restart;

#IfDef DEBUG_VERBS;
! sets the random seed, thus making randomness predictable
! also a test of special and number, thus the fancy grammar
Verb meta 'random'
	* -> Predictable
	* special -> Predictable
	* 'to' number -> Predictable;
#EndIf;

Verb 'wake' 'awake' 'awaken'
    *                                           -> Wake
    * 'up'                                      -> Wake
    * creature                                  -> WakeOther
    * creature 'up'                             -> WakeOther
    * 'up' creature                             -> WakeOther;

Verb 'kiss' 'embrace' 'hug'
    * creature                                  -> Kiss;

Verb 'wear'
	* held										-> Wear;
Verb 'attack' 'break' 'crack' 'destroy'
     'fight' 'hit' 'kill' 'murder' 'punch'
     'smash' 'thump' 'torture' 'wreck'
    * noun                                      -> Attack;

Verb 'turn' 'rotate' 'screw' 'twist' 'unscrew'
    * noun                                      -> Turn
    * noun 'on'                                 -> SwitchOn
    * noun 'off'                                -> SwitchOff
    * 'on' noun                                 -> SwitchOn
    * 'off' noun                                -> SwitchOff;

Verb 'switch'
    * noun                                      -> SwitchOn
    * noun 'on'                                 -> SwitchOn
    * noun 'off'                                -> SwitchOff
    * 'on' noun                                 -> SwitchOn
    * 'off' noun                                -> SwitchOff;

Verb 'pull' 'drag'
    * noun                                      -> Pull;
Verb 'push' 'clear' 'move' 'press' 'shift'
    * noun                                      -> Push
    !TODO: * noun noun                                 -> PushDir
    !TODO: * noun 'to' noun                            -> Transfer
	;


[ PrintVerb p_v _string_name;
#IfV3;
	switch(p_v) {
		'restart': _string_name = "restart";
		'restore': _string_name = "restore";
		'display': _string_name = "display";
		'present': _string_name = "present";
		'embrace': _string_name = "embrace";
		'destroy': _string_name = "destroy";
		'torture': _string_name = "torture";
		'unscrew': _string_name = "unscrew";
	}
#EndIf;
	switch(p_v) {
		'x//', 'examine': _string_name = "examine";		! 'examine' is really only needed for z3, but this creates smaller code in z3
		'i//', 'inventory': _string_name = "inventory";	! 'inventory' is really only needed for z3, but this creates smaller code in z3
		'l//': _string_name = "look";
		'z//': _string_name = "wait";
		'q//': _string_name = "quit";
	}
	if(_string_name)
		print (string) _string_name;
	else
		print (address) p_v;
];

