!
! Simple string messages
!
#Ifndef MSG_YOU_HAVE_WON;
Constant MSG_YOU_HAVE_WON "You have won.";
#EndIf;
#Ifndef MSG_YOU_HAVE_DIED;
Constant MSG_YOU_HAVE_DIED "You have died.";
#EndIf;
#Ifndef MSG_TAKE_YOURSELF;
Constant MSG_TAKE_YOURSELF "You are always self-possessed.";
#EndIf;
#Ifndef MSG_TAKE_SCENERY;
Constant MSG_TAKE_SCENERY "That's hardly portable.";
#EndIf;
#Ifndef MSG_TAKE_STATIC;
Constant MSG_TAKE_STATIC "That's fixed in place.";
#EndIf;
#Ifndef MSG_TAKE_ALREADY_HAVE;
Constant MSG_TAKE_ALREADY_HAVE "You already have that.";
#EndIf;
#Ifndef MSG_TAKE_NO_CAPACITY;
Constant MSG_TAKE_NO_CAPACITY "You are carrying too many things already.";
#EndIf;
#Ifndef MSG_TAKE_SUCCESS;
Constant MSG_TAKE_SUCCESS "Taken.";
#EndIf;
#Ifndef MSG_EAT_INEDIBLE;
Constant MSG_EAT_INEDIBLE "That's plainly inedible.";
#EndIf;
#Ifndef MSG_DRINK_NOTHING_SUITABLE;
Constant MSG_DRINK_NOTHING_SUITABLE "There's nothing suitable to drink here.";
#EndIf;
#Ifndef MSG_DROP_DROPPED;
Constant MSG_DROP_DROPPED "Dropped.";
#EndIf;
#Ifndef MSG_OPEN_ALREADY;
Constant MSG_OPEN_ALREADY "It's already open.";
#Endif;
#Ifndef MSG_THROW_ANIMATE;
Constant MSG_THROW_ANIMATE "Futile.";
#Endif;
#Ifndef MSG_THROW_SUCCESS;
Constant MSG_THROW_SUCCESS "You lack the nerve when it comes to the crucial moment.";
#Endif;
#Ifndef MSG_CLOSE_NOT_OPEN;
Constant MSG_CLOSE_NOT_OPEN "It isn't open.";
#Endif;
#Ifndef MSG_SMELL_SUCCESS;
Constant MSG_SMELL_SUCCESS "You smell nothing unexpected.";
#Endif;
#Ifndef MSG_TELL_PLAYER;
Constant MSG_TELL_PLAYER "You talk to yourself for a while.";
#Endif;
#Ifndef MSG_TELL_SUCCESS;
Constant MSG_TELL_SUCCESS "This provokes no reaction.";
#Endif;
#Ifndef MSG_ENTER_ALREADY;
Constant MSG_ENTER_ALREADY "But you are already there!";
#Endif;
#Ifndef MSG_EXIT_ALREADY;
Constant MSG_EXIT_ALREADY "But you aren't in anything at the moment!";
#Endif;
#Ifndef MSG_EXIT_NOT_ON;
Constant MSG_EXIT_NOT_ON "You aren't on that.";
#Endif;
#Ifndef MSG_EXIT_NOT_IN;
Constant MSG_EXIT_NOT_IN "You aren't in that.";
#Endif;
#Ifndef MSG_INVENTORY_EMPTY;
Constant MSG_INVENTORY_EMPTY "You are empty handed.";
#Endif;
#Ifndef MSG_GO_CANT_GO;
Constant MSG_GO_CANT_GO "You can't go that way.";
#Endif;
#Ifndef MSG_SAVE_SUCCESS;
Constant MSG_SAVE_SUCCESS "Ok.";
#Endif;
#Ifndef MSG_YES_OR_NO;
Constant MSG_YES_OR_NO "Please answer yes or no: ";
#Endif;
#Ifndef MSG_RESTART_CONFIRM;
Constant MSG_RESTART_CONFIRM "Are you sure you want to restart? ";
#Endif;
#Ifndef MSG_INSERT_ITSELF;
Constant MSG_INSERT_ITSELF "Cannot put something inside itself.";
#Endif;
#Ifndef MSG_INSERT_NOT_CONTAINER;
Constant MSG_INSERT_NOT_CONTAINER "That can't contain things.";
#Endif;
#Ifndef MSG_PUTON_NOT_SUPPORTER;
Constant MSG_PUTON_NOT_SUPPORTER "You can't put things on top of that.";
#Endif;
#Ifndef MSG_PUTON_ITSELF;
Constant MSG_PUTON_ITSELF "Cannot put something on itself.";
#Endif;
#Ifndef MSG_WAKE_SUCCESS;
Constant MSG_WAKE_SUCCESS "The dreadful truth is, this is not a dream.";
#Endif;
#Ifndef MSG_WAKEOTHER_SUCCESS;
Constant MSG_WAKEOTHER_SUCCESS "That seems unnecessary.";
#Endif;
#Ifndef MSG_KISS_PLAYER;
Constant MSG_KISS_PLAYER "If you think that'll help.";
#Endif;
#Ifndef MSG_KISS_SUCCESS;
Constant MSG_KISS_SUCCESS "Keep your mind on the game.";
#Endif;
#Ifndef MSG_ATTACK_NO_VIOLENCE;
Constant MSG_ATTACK_NO_VIOLENCE "Violence isn't the answer to this one.";
#Endif;

#Ifndef MSG_UNABLE_TO_TURN;
Constant MSG_UNABLE_TO_TURN "You are unable to.";
#Endif;
#Ifndef MSG_NOTHING_OBVIOUS;
Constant MSG_NOTHING_OBVIOUS "Nothing obvious happens.";
#Endif;
#Ifndef MSG_LESS_THAN_COURTEOUS;
Constant MSG_LESS_THAN_COURTEOUS "That would be less than courteous.";
#Endif;

!
! complex messages (enumerated)
!

#Ifndef MSG_OPEN_YOU_CANT;
Constant MSG_OPEN_YOU_CANT = 1;
#Endif;
#Ifndef MSG_CLOSE_YOU_CANT;
Constant MSG_CLOSE_YOU_CANT = 2;
#Endif;
#Ifndef MSG_ENTER_YOU_CANT;
Constant MSG_ENTER_YOU_CANT = 3;
#Endif;
#Ifndef MSG_EXAMINE_NOTHING_SPECIAL;
Constant MSG_EXAMINE_NOTHING_SPECIAL = 4;
#Endif;
#Ifndef MSG_TAKE_ANIMATE;
Constant MSG_TAKE_ANIMATE = 5;
#Endif;
#Ifndef MSG_TAKE_PLAYER_PARENT;
Constant MSG_TAKE_PLAYER_PARENT = 6;
#Endif;
#Ifndef MSG_EAT_ANIMATE;
Constant MSG_EAT_ANIMATE = 7;
#Endif;
#Ifndef MSG_DROP_NOT_HOLDING;
Constant MSG_DROP_NOT_HOLDING = 8;
#Endif;
#Ifndef MSG_OPEN_SUCCESS;
Constant MSG_OPEN_SUCCESS = 9;
#Endif;
#Ifndef MSG_CLOSE_SUCCESS;
Constant MSG_CLOSE_SUCCESS = 10;
#Endif;
#Ifndef MSG_THROW_FIRST_TAKING;
Constant MSG_THROW_FIRST_TAKING = 11;
#Endif;
#Ifndef MSG_SHOW_NOT_HOLDING;
Constant MSG_SHOW_NOT_HOLDING = 12;
#Endif;
#Ifndef MSG_SHOW_SUCCESS;
Constant MSG_SHOW_SUCCESS = 13;
#Endif;
#Ifndef MSG_GIVE_NOT_HOLDING;
Constant MSG_GIVE_NOT_HOLDING = 14;
#Endif;
#Ifndef MSG_GIVE_SUCCESS;
Constant MSG_GIVE_SUCCESS = 15;
#Endif;
#Ifndef MSG_ASKFOR_SUCCESS;
Constant MSG_ASKFOR_SUCCESS = 16;
#Endif;
#Ifndef MSG_ASKTO_SUCCESS;
Constant MSG_ASKTO_SUCCESS = 17;
#Endif;
#Ifndef MSG_ENTER_SUCCESS;
Constant MSG_ENTER_SUCCESS = 18;
#Endif;
#Ifndef MSG_EXIT_FIRST_LEAVE;
Constant MSG_EXIT_FIRST_LEAVE = 19;
#Endif;
#Ifndef MSG_ENTER_NOT_OPEN;
Constant MSG_ENTER_NOT_OPEN = 20;
#Endif;
#Ifndef MSG_EXIT_NOT_OPEN;
Constant MSG_EXIT_NOT_OPEN = 21;
#Endif;
#Ifndef MSG_EXIT_SUCCESS;
Constant MSG_EXIT_SUCCESS = 22;
#Endif;
#Ifndef MSG_INVENTORY_SUCCESS;
Constant MSG_INVENTORY_SUCCESS = 23;
#Endif;
#Ifndef MSG_GO_FIRST_LEAVE;
Constant MSG_GO_FIRST_LEAVE = 24;
#Endif;
#Ifndef MSG_GIVE_PLAYER;
Constant MSG_GIVE_PLAYER 25;
#Endif;
#Ifndef MSG_SAVE_FAILED;
Constant MSG_SAVE_FAILED 26;
#Endif;
#Ifndef MSG_RESTORE_FAILED;
Constant MSG_RESTORE_FAILED 27;
#Endif;
#Ifndef MSG_RESTART_FAILED;
Constant MSG_RESTART_FAILED 28;
#Endif;
#Ifndef MSG_INSERT_SUCCESS;
Constant MSG_INSERT_SUCCESS 29;
#Endif;
#Ifndef MSG_INSERT_NOT_OPEN;
Constant MSG_INSERT_NOT_OPEN 30;
#Endif;
#Ifndef MSG_ASK_SUCCESS;
Constant MSG_ASK_SUCCESS 31;
#Endif;
#Ifndef MSG_ANSWER_SUCCESS;
Constant MSG_ANSWER_SUCCESS 32;
#Endif;
#Ifndef MSG_RESTART_RESTORE_OR_QUIT;
Constant MSG_RESTART_RESTORE_OR_QUIT 33;
#EndIf;
#Ifndef MSG_AREYOUSUREQUIT;
Constant MSG_AREYOUSUREQUIT 34;
#EndIf;
#Ifndef MSG_WEAR_ALREADY_WORN;
Constant MSG_WEAR_ALREADY_WORN 35;
#EndIf;
#Ifndef MSG_WEAR_NOT_CLOTHING;
Constant MSG_WEAR_NOT_CLOTHING 36;
#EndIf;
#Ifndef MSG_WEAR_NOT_HOLDING;
Constant MSG_WEAR_NOT_HOLDING 37;
#EndIf;
#Ifndef MSG_WEAR_SUCCESS;
Constant MSG_WEAR_SUCCESS 38;
#EndIf;
#Ifndef MSG_INSERT_ALREADY;
Constant MSG_INSERT_ALREADY 39;
#Endif;
#Ifndef MSG_INSERT_NO_ROOM;
Constant MSG_INSERT_NO_ROOM 40;
#Endif;
#Ifndef MSG_PUTON_ALREADY;
Constant MSG_PUTON_ALREADY 41;
#Endif;
#Ifndef MSG_PUTON_NO_ROOM;
Constant MSG_PUTON_NO_ROOM 42;
#Endif;
#Ifndef MSG_PUTON_SUCCESS;
Constant MSG_PUTON_SUCCESS 43;
#Endif;



#Ifndef MSG_NOT_SWITCHABLE;
Constant MSG_NOT_SWITCHABLE 35;
#EndIf;
#Ifndef MSG_ALREADY_ON_OFF;
Constant MSG_ALREADY_ON_OFF 36;
#EndIf;
#Ifndef MSG_SWITCH_ON_OFF_SUCCESS;
Constant MSG_SWITCH_ON_OFF_SUCCESS 37;
#EndIf;
#Ifndef MSG_FIXED_IN_PLACE;
Constant MSG_FIXED_IN_PLACE 38;
#EndIf;


#Ifndef LibraryMessages;
Constant LibraryMessages 0;
#Endif;


[ PrintMsg p_msg p_arg1;
	if(p_msg ofclass String)
		print_ret (string) p_msg;

	if(LibraryMessages(p_msg, p_arg1))
		rtrue;

	! Not a string, there should be code for the message here
	switch(p_msg) {
	MSG_OPEN_YOU_CANT, MSG_CLOSE_YOU_CANT, MSG_ENTER_YOU_CANT, MSG_WEAR_NOT_CLOTHING:
		"You can't ", (PrintVerb) verb_word, " that!";
	MSG_EXAMINE_NOTHING_SPECIAL:
		"There is nothing special about ", (the) noun, ".";
	MSG_TAKE_ANIMATE, MSG_EAT_ANIMATE:
		"I don't suppose ", (the) noun, " would care for that.";
	MSG_TAKE_PLAYER_PARENT, MSG_GO_FIRST_LEAVE, MSG_EXIT_FIRST_LEAVE:
		"First, you'd have to leave ", (the) p_arg1, ".";
	MSG_DROP_NOT_HOLDING, MSG_SHOW_NOT_HOLDING, MSG_GIVE_NOT_HOLDING, MSG_WEAR_NOT_HOLDING:
		"You aren't holding ", (ItOrThem) noun, ".";
    MSG_OPEN_SUCCESS, MSG_CLOSE_SUCCESS, MSG_ENTER_SUCCESS:
	    "You ", (PrintVerb) verb_word, " ", (the) noun, ".";
    MSG_THROW_FIRST_TAKING:
		"(first taking ", (the) noun, " off)";
    MSG_GIVE_SUCCESS, MSG_SHOW_SUCCESS:
        print_ret (The) second, " doesn't seem interested.";
    MSG_ASKFOR_SUCCESS, MSG_ASKTO_SUCCESS:
        print_ret (The) noun, " has better things to do.";
    MSG_ENTER_NOT_OPEN, MSG_EXIT_NOT_OPEN, MSG_INSERT_NOT_OPEN:
        "You can't, since ",(the) p_arg1, " is closed.";
    MSG_EXIT_SUCCESS:
	    "You leave ", (the) noun, ".";
    MSG_INVENTORY_SUCCESS:
	    PrintContents("You are holding ", ".^", player);
		rtrue;
    MSG_GIVE_PLAYER:
        "You already have ", (ItOrThem) noun, ".";
    MSG_SAVE_FAILED, MSG_RESTORE_FAILED, MSG_RESTART_FAILED:
        "Failed ", (PrintVerb) verb_word, ".";
	MSG_INSERT_ALREADY, MSG_PUTON_ALREADY:
		"Already there.";
	MSG_INSERT_NO_ROOM, MSG_PUTON_NO_ROOM:
		"There is no more room.";
    MSG_INSERT_SUCCESS:
        "You put ", (the) noun, " into ", (the) second, ".";
    MSG_PUTON_SUCCESS:
        "You put ", (the) noun, " on ", (the) second, ".";
	MSG_ASK_SUCCESS, MSG_ANSWER_SUCCESS: 
		"There is no reply.";
	MSG_WEAR_ALREADY_WORN:	
		"You are already wearing ", (ItOrThem) noun, ".";
    MSG_WEAR_SUCCESS:
	    "You are now wearing ", (the) noun, ".";
	MSG_RESTART_RESTORE_OR_QUIT:
		print "^Would you like to RESTART, RESTORE or QUIT? ";
		rtrue;
	MSG_AREYOUSUREQUIT:
		print "Are you sure you want to quit? ";
		rtrue;
	MSG_NOT_SWITCHABLE:
		print_ret (ctheyreorthats) noun, " not something you can switch.";
	MSG_ALREADY_ON_OFF:
		print_ret (ctheyreorthats) noun, " already ", (OnOff) noun, ". ";
	MSG_SWITCH_ON_OFF_SUCCESS:
		"You switch ", (the) noun, " ", (OnOff) noun, ". "; 
	!TODO: Use same for Pull,Push,Turn: 
	MSG_FIXED_IN_PLACE:
		if (noun has pluralname) print "Those are "; else print "It is ";
		"fixed in place.";
	default:
		! No code found. Print an error message.
		RuntimeError(ERR_UNKNOWN_MSGNO);
	}
];

[OnOff obj;
	if(obj has on) print "on";
	else print "off";
	return;
];

[ CTheyreorThats obj;
    if (obj == player)             { print "You're"; return; }
    if (obj has pluralname)        { print "They're"; return; }
    if (obj has animate) {
        if (obj has female)        { print "She's"; return; }
        else if (obj hasnt neuter) { print "He's"; return; }
    }
    print "That's";
];

[ ItorThem obj;
    if (obj == player) { print "yourself"; rtrue; } 
    if (obj has pluralname) { print "them"; rtrue; }
    if (obj has animate) {
        if (obj has female) { print "her"; rtrue; }
        else if (obj hasnt neuter) { print "him"; rtrue; }
    }
    print "it";
];


!
! Error messages
!
Constant ERR_TOO_MANY_TIMERS_DAEMONS 1;
Constant ERR_OBJECT_HASNT_PROPERTY 2;
Constant ERR_SCOPE_FULL 3;
Constant ERR_UNKNOWN_MSGNO 4;
Constant ERR_INVALID_DIR_PROP 5;

[RunTimeError p_err;
	print "[Puny error: ";
#IfDef RUNTIME_ERRORS;
	switch(p_err) {
	ERR_TOO_MANY_TIMERS_DAEMONS:
		print "Too many timers/daemons";
	ERR_OBJECT_HASNT_PROPERTY:
		print "Object lacks that property";
	ERR_SCOPE_FULL:
		print "Scope full";
	ERR_UNKNOWN_MSGNO:
		print "Unknown message#"
	ERR_INVALID_DIR_PROP:
		print "Invalid direction prop in GoSub";
	default:
		print "Unknown error (", p_err, ")";
	}
#IfNot;
	print p_err;
#EndIf;
	"]";
];
