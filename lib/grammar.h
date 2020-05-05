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
		"There is nothing special about ", (the) noun, ".";
	}
];

[ QuitSub;
	PrintMsg(MSG_AREYOUSUREQUIT);
	if(YesOrNo()) {
		game_state = GS_QUIT;
	}
];

[ TryToTakeObject item;
    ! Try to transfer the given item to the player: return false
    ! if successful, true if unsuccessful, printing a suitable message
    ! in the latter case.
    ! People cannot ordinarily be taken.
    if(item == player) "You are always self-possessed.";
    if(item has animate) "I don't suppose ", (the) item, " would care for that.";
	if(item has scenery) "That's hardly portable.";
	if(item has static) "That's fixed in place.";
	if(noun in player) "You already have that.";
	if(IndirectlyContains(noun, player)) "First, you'd have to leave ", (the) noun, ".";
    if(AtFullCapacity(player)) "You are carrying too many things already.";

	move noun to player;
	give noun moved;
	rfalse;
];

[ TakeSub;
	if(TryToTakeObject(noun) == 1) rtrue;
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
	"Taken.";
];

[ DropSub;
	if(noun notin player) "You aren't holding that.";
	move noun to location;
	if(AfterRoutines() == 1) rtrue;
    if(keep_silent) return;
	"Dropped.";
];

[ ThrowAtSub;
	if(ObjectIsUntouchable(noun)) return;
	if(second > 1) {
		action = ##ThrownAt;
		if (RunRoutines(second, before) ~= 0) { action = ##ThrowAt; rtrue; }
		action = ##ThrowAt;
	}
	if(noun has worn) {
		print "(first taking ", (the) noun, " off)^";
		if (noun has worn && noun in player) rtrue;
	}
	if(second hasnt animate) return "Futile.";
	if(RunLife(second,##ThrowAt) ~= 0) rfalse;
	"You lack the nerve when it comes to the crucial moment.";
];

[ OpenSub;
	if(noun hasnt openable) {
		PrintMsg(MSG_OPEN_YOU_CANT);
		rtrue;
	}
	if(noun has open) "It's already open.";
	give noun open;
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
	"You open ", (the) noun, ".";
];

[ CloseSub;
	if(noun hasnt openable) {
		PrintMsg(MSG_CLOSE_YOU_CANT);
		rtrue;
	}
	if(noun hasnt open) "It isn't open.";
	give noun ~open;
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
	"You close ", (the) noun, ".";
];

[ ShowSub;
    if (parent(noun) ~= player) "You aren't holding ", (the) noun, ".";
    if (second == player) <<Examine noun>>;
    if (RunLife(second, ##Show) ~= 0) rfalse;
    print_ret (The) second, " is unimpressed.";
];

[ GiveSub;
    if (parent(noun) ~= player) "You aren't holding ", (the) noun, ".";
    if (second == player)  return "You already have it.";
    if (RunLife(second, ##Give) ~= 0) rfalse;
    print_ret (The) second, " doesn't seem interested.";
];

[ AskSub;
    if (RunLife(noun,##Ask) ~= 0) rfalse;
    "There is no reply.";
];

[ AskForSub;
    if (noun == player) <<Inventory>>;
    print_ret (The) noun, " has better things to do.";
];

[ AskToSub;
    print_ret (The) noun, " has better things to do.";
];

[ TellSub;
    if (noun == player) "You talk to yourself a while.";
    if (RunLife(noun, ##Tell) ~= 0) rfalse;
    "This provokes no reaction.";
];

[ EnterSub;
	if(noun hasnt enterable) {
		PrintMsg(MSG_ENTER_YOU_CANT);
		rtrue;
	}
	if(player in noun) "But you are already there!";
	if(noun has container && noun hasnt open) "You can't, since it's closed.";
	PlayerTo(noun);
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
	"You enter ", (the) noun, ".";
];

[ ExitSub;
	if(noun == 0) noun = parent(player);
	if(player in location) "But you aren't in anything at the moment!";
	if(player notin noun) {
		if(IndirectlyContains(noun, player)) "First you have to leave ", (the) parent(player),".";
		if(noun has supporter) "You aren't on that.";
			"You aren't in that.";
	}
	if(noun has container && noun hasnt open) "You can't, since ",(the) noun, " is closed.";
	PlayerTo(parent(noun));
	if(AfterRoutines() == 1) rtrue;
    if (keep_silent) return;
	"You leave ", (the) noun, ".";
];

[ AnswerSub;
    if (second ~= 0 && RunLife(second,##Answer) ~= 0) rfalse;
    "There is no reply";
];

[ InventorySub;
	if(child(player) == 0)
	"You are empty handed.";
	PrintContents("You are holding ", ".^", player);
];

[ GoSub _prop;
	! when called Directions have been set properly
	_prop = Directions.selected_dir_prop;
	if(_prop == 0) return RuntimeError("Invalid direction prop in GoSub");
	GoDir(_prop);
	rtrue;
];

[ GoDir p_property _new_location;
	if(player notin location) "You need to leave ", (the) parent(player), " first.";
	if(location provides p_property) {
		@get_prop location p_property -> _new_location; ! works in z3 and z5
	}
	if(_new_location == 0) {
		if(location provides cant_go) {
			print_ret (string) location.cant_go;
		}
		"You can't go that way.";
	}
	location = _new_location;
	PlayerTo(location);
	<Look>; ! Equivalent to PerformAction(##Look);
];

#IfV3;
[ SaveSub;
	@save ?save_success;
	"Save failed.";
.save_success;
	"Ok.";
#IfNot;
[ SaveSub _result;
	@save -> _result;
	if(_result == 0) "Save failed.";
	"Ok."; ! _result = 1: save ok, 2: Restore ok
#EndIf;
];

#IfV3;
[ RestoreSub;
	@restore ?restore_success; ! can't use @restore because of compiler test
	"Restore failed.";
.restore_success; ! This is never reached, since a successful restore continues after save opcode.
#IfNot;
[ RestoreSub _flag;
	@restore -> _flag;
	! must have failed here so no need to check the flag
	"Restore failed";
#EndIf;
];

[ YesOrNo _words _reply;
	for (::)
	{
#IfV3;
		@sread player_input_array parse_array;
#IfNot;
		DrawStatusLine();
		player_input_array -> 1 = 0;
		@aread player_input_array parse_array -> _reply;
#EndIf;
		_words = parse_array -> 1;
		_reply = parse_array --> 1;
		if(_words == 1) {
			if (_reply == 'yes' or 'y//') rtrue;
			if (_reply == 'no' or 'n//') rfalse;
		}
		print "Please answer yes or no.^> ";
	}
];

[ RestartSub;
	print "Are you sure you want to restart? ";
	if(YesOrNo()) {
		@restart;
		"Failed.";
	}
];

[ InsertSub _ancestor;
    if (parent(noun) == second) "Already there.";
    _ancestor = CommonAncestor(noun, second);
    if (_ancestor == noun) "Cannot put something inside itself.";
    if (second ~= _ancestor) {
        if (second has container && second hasnt open) "Closed.";
    }
    if (second hasnt container) "That can't contain things.";

    if (AtFullCapacity(noun, second)) "There is no more room.";

    move noun to second;
    if (keep_silent) return;
    "You put ", (the) noun, " into ", (the) second, ".";
];

[ WakeSub;
	"The dreadful truth is, this is not a dream.";
];

[ WakeOtherSub;
    !if (ObjectIsUntouchable(noun)) return; ! TODO
    if (RunLife(noun, ##WakeOther) ~= 0) rfalse;
    "That seems unnecessary.";
];

[ KissSub;
    !if (ObjectIsUntouchable(noun)) return; ! TODO
    if (RunLife(noun, ##Kiss) ~= 0) rfalse;
    if (noun == player) "If you think that'll help.";
    "Keep your mind on the game.";
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

