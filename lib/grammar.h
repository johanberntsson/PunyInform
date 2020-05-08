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
	PrintMsg(MSG_AREYOUSUREQUIT, true);
	if(YesOrNo()) {
		game_state = GS_QUIT;
	}
];

[ TryToTakeNoun;
    ! Try to transfer the given item to the player: return false
    ! if successful, true if unsuccessful, printing a suitable message
    ! in the latter case.
    ! People cannot ordinarily be taken.
    if(noun == player) return PrintMsg(MSG_TAKE_YOURSELF);
    if(noun has animate) return PrintMsg(MSG_TAKE_ANIMATE);
    if(noun has scenery) return PrintMsg(MSG_TAKE_SCENERY);
    if(noun has static) return PrintMsg(MSG_TAKE_STATIC);
	if(noun in player) return PrintMsg(MSG_TAKE_ALREADY_HAVE);
	if(IndirectlyContains(noun, player)) return PrintMsg(MSG_TAKE_PLAYER_PARENT);
    if(AtFullCapacity(player)) return PrintMsg(MSG_TAKE_NO_CAPACITY);

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
    if(noun has animate) return PrintMsg(MSG_EAT_ANIMATE);
    PrintMsg(MSG_EAT_INEDIBLE);
];

[ DrinkSub;
	PrintMsg(MSG_DRINK_NOTHING_SUITABLE);
];

[ DropSub;
	if(noun notin player) return PrintMsg(MSG_DROP_NOT_HOLDING);
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
	if(second hasnt animate) return PrintMsg(MSG_THROW_ANIMATE);
	if(RunLife(second,##ThrowAt) ~= 0) rfalse;
	PrintMsg(MSG_THROW_SUCCESS);
];

[ OpenSub;
	if(noun hasnt openable) return PrintMsg(MSG_OPEN_YOU_CANT);
	if(noun has open) return PrintMsg(MSG_OPEN_ALREADY);
	give noun open;
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
	PrintMsg(MSG_OPEN_SUCCESS);
];

[ CloseSub;
	if(noun hasnt openable) return PrintMsg(MSG_CLOSE_YOU_CANT);
	if(noun hasnt open) return PrintMsg(MSG_CLOSE_NOT_OPEN);
	give noun ~open;
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
	PrintMsg(MSG_CLOSE_SUCCESS);
];

[ ShowSub;
    if (parent(noun) ~= player) return PrintMsg(MSG_SHOW_NOT_HOLDING);
    if (second == player) <<Examine noun>>;
    if (RunLife(second, ##Show) ~= 0) rfalse;
    PrintMsg(MSG_SHOW_SUCCESS);
];

[ SmellSub;
    PrintMsg(MSG_SMELL_SUCCESS);
];

[ GiveSub;
    if (parent(noun) ~= player) return PrintMsg(MSG_GIVE_NOT_HOLDING);
    if (second == player)  return PrintMsg(MSG_GIVE_PLAYER);
    if (RunLife(second, ##Give) ~= 0) rfalse;
    PrintMsg(MSG_GIVE_SUCCESS);
];

[ AskSub;
    if (RunLife(noun,##Ask) ~= 0) rfalse;
    PrintMsg(MSG_ASK_SUCCESS);
];

[ AskForSub;
    if (noun == player) <<Inventory>>;
    PrintMsg(MSG_ASKFOR_SUCCESS);
];

[ AskToSub;
    PrintMsg(MSG_ASKTO_SUCCESS);
];

[ TellSub;
    if (noun == player) return PrintMsg(MSG_TELL_PLAYER);
    if (RunLife(noun, ##Tell) ~= 0) rfalse;
    PrintMsg(MSG_TELL_SUCCESS);
];

[ EnterSub;
	if(noun hasnt enterable) return PrintMsg(MSG_ENTER_YOU_CANT);
	if(player in noun) return PrintMsg(MSG_ENTER_ALREADY);
	if(noun has container && noun hasnt open) return PrintMsg(MSG_ENTER_NOT_OPEN);
	PlayerTo(noun);
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
    PrintMsg(MSG_ENTER_SUCCESS);
];

[ ExitSub;
	if(noun == 0) noun = parent(player);
	if(player in location) return PrintMsg(MSG_EXIT_ALREADY);
	if(player notin noun) {
		if(IndirectlyContains(noun, player)) return PrintMsg(MSG_EXIT_FIRST_LEAVE);
		if(noun has supporter) return PrintMsg(MSG_EXIT_NOT_ON);
		return PrintMsg(MSG_EXIT_NOT_IN);
	}
	if(noun has container && noun hasnt open) return PrintMsg(MSG_EXIT_NOT_OPEN);
	PlayerTo(parent(noun));
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
    PrintMsg(MSG_EXIT_SUCCESS);
];

[ AnswerSub;
    if (second ~= 0 && RunLife(second,##Answer) ~= 0) rfalse;
    PrintMsg(MSG_ANSWER_SUCCESS);
];

[ InventorySub;
	if(child(player) == 0) return PrintMsg(MSG_INVENTORY_EMPTY);
    PrintMsg(MSG_INVENTORY_SUCCESS);
];

[ GoSub _prop;
	! when called Directions have been set properly
	_prop = Directions.selected_dir_prop;
	if(_prop == 0) return RuntimeError("Invalid direction prop in GoSub");
	GoDir(_prop);
	rtrue;
];

[ GoDir p_property _new_location;
	if(player notin location) return PrintMsg(MSG_GO_LEAVE_FIRST);
	if(location provides p_property) {
		@get_prop location p_property -> _new_location; ! works in z3 and z5
	}
	if(_new_location == 0) {
		if(location provides cant_go) {
			print_ret (string) location.cant_go;
		}
        return PrintMsg(MSG_GO_CANT_GO);
	}
	location = _new_location;
	PlayerTo(location);
	<Look>; ! Equivalent to PerformAction(##Look);
];

#IfV3;
[ SaveSub;
	@save ?save_success;
	return PrintMsg(MSG_SAVE_FAILED);
.save_success;
    PrintMsg(MSG_SAVE_SUCCESS);
#IfNot;
[ SaveSub _result;
	@save -> _result;
	if(_result == 0) return PrintMsg(MSG_SAVE_FAILED);
	PrintMsg(MSG_SAVE_SUCCESS); ! _result = 1: save ok, 2: Restore ok
#EndIf;
];

#IfV3;
[ RestoreSub;
	@restore ?restore_success; ! can't use @restore because of compiler test
	return PrintMsg(MSG_RESTORE_FAILED);
.restore_success; ! This is never reached, since a successful restore continues after save opcode.
#IfNot;
[ RestoreSub _flag;
	@restore -> _flag;
	! must have failed here so no need to check the flag
	PrintMsg(MSG_RESTORE_FAILED);
#EndIf;
];

[ RestartSub;
    PrintMsg(MSG_RESTART_CONFIRM, true);
	if(YesOrNo()) {
		@restart;
        PrintMsg(MSG_RESTART_FAILED);
	}
];

[ InsertSub _ancestor;
    if (parent(noun) == second) return PrintMsg(MSG_INSERT_ALREADY);
    _ancestor = CommonAncestor(noun, second);
    if (_ancestor == noun) return PrintMsg(MSG_INSERT_ITSELF);
    if (second ~= _ancestor) {
        if (second has container && second hasnt open) return PrintMsg(MSG_INSERT_NOT_OPEN);
    }
    if (second hasnt container) return PrintMsg(MSG_INSERT_NOT_CONTAINER);

    if (AtFullCapacity(noun, second)) return PrintMsg(MSG_INSERT_NO_ROOM);

    move noun to second;
    if (keep_silent) return;
    PrintMsg(MSG_INSERT_SUCCESS);
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
    if (noun == player) return PrintMsg(MSG_KISS_PLAYER);
    PrintMsg(MSG_KISS_SUCCESS);
];


Verb 'i' 'inventory'
	* -> Inventory;

Verb 'answer' 'say' 'shout' 'speak'
    * topic 'to' creature -> Answer;

Verb 'look' 'l'
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
	* noun -> Close;

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
    * multiexcept 'in'/'inside'/'into' noun     -> Insert;

Verb 'insert'
    * multiexcept 'in'/'into' noun              -> Insert;

[ ADirection; return (noun == Directions); ];

Verb 'go'
	* noun=ADirection -> Go
	* noun -> Enter;

Verb 'examine' 'x//'
	* noun -> Examine;

Verb meta 'quit'
	* -> Quit;

Verb meta 'save'
	* -> Save;

Verb meta 'restore'
	* -> Restore;

Verb meta 'restart'
	* -> Restart;

Verb 'wake' 'awake' 'awaken'
    *                                           -> Wake
    * 'up'                                      -> Wake
    * creature                                  -> WakeOther
    * creature 'up'                             -> WakeOther
    * 'up' creature                             -> WakeOther;

Verb 'kiss' 'embrace' 'hug'
    * creature                                  -> Kiss;

