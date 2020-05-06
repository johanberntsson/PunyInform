!
! Simple string messages
!
#Ifndef MSG_YOU_HAVE_WON;
Constant MSG_YOU_HAVE_WON "You have won";
#EndIf;
#Ifndef MSG_YOU_HAVE_DIED;
Constant MSG_YOU_HAVE_DIED "You have died";
#EndIf;
#Ifndef MSG_RESTART_RESTORE_OR_QUIT;
Constant MSG_RESTART_RESTORE_OR_QUIT "^Would you like to RESTART, RESTORE or QUIT? ";
#EndIf;
#Ifndef MSG_AREYOUSUREQUIT;
Constant MSG_AREYOUSUREQUIT "Are you sure you want to quit? ";
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
#Ifndef MSG_GIVE_PLAYER;
Constant MSG_GIVE_PLAYER "You already have it.";
#Endif;
#Ifndef MSG_ASK_SUCCESS;
Constant MSG_ASK_SUCCESS "There is no reply.";
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
#Ifndef MSG_ANSWER_SUCCESS;
Constant MSG_ANSWER_SUCCESS "There is no reply";
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
#Ifndef LibraryMessages;
Constant LibraryMessages 0;
#Endif;


[ PrintMsg p_msg p_dontprintnewline arg1;
	if(p_msg ofclass String) {
		print (string) p_msg;
		if(p_dontprintnewline == 0) 
			new_line;
		rtrue;
	}

	if(LibraryMessages(p_msg, p_dontprintnewline, arg1))
		rtrue;
		
	! Not a string, there should be code for the message here
	switch(p_msg) {
	MSG_OPEN_YOU_CANT, MSG_CLOSE_YOU_CANT, MSG_ENTER_YOU_CANT:
		"You can't ", (PrintVerb) verb_word, " that!";
	MSG_EXAMINE_NOTHING_SPECIAL:
		"There is nothing special about ", (the) noun, ".";
	MSG_TAKE_ANIMATE, MSG_EAT_ANIMATE:
		"I don't suppose ", (the) noun, " would care for that.";
	MSG_TAKE_PLAYER_PARENT:
		"First, you'd have to leave ", (the) noun, ".";
	MSG_DROP_NOT_HOLDING, MSG_SHOW_NOT_HOLDING, MSG_GIVE_NOT_HOLDING:
		"You aren't holding ", (ItOrThem) noun, ".";
    MSG_OPEN_SUCCESS, MSG_CLOSE_SUCCESS, MSG_ENTER_SUCCESS:
	    "You ", (PrintVerb) verb_word, " ", (the) noun, ".";
    MSG_THROW_FIRST_TAKING:
		"(first taking ", (the) noun, " off)";
    MSG_SHOW_SUCCESS:
        print_ret (The) second, " is unimpressed.";
    MSG_GIVE_SUCCESS:
        print_ret (The) second, " doesn't seem interested.";
    MSG_ASKFOR_SUCCESS, MSG_ASKTO_SUCCESS:
        print_ret (The) noun, " has better things to do.";
    MSG_EXIT_FIRST_LEAVE:
        "First you have to leave ", (the) parent(player),".";
    MSG_ENTER_NOT_OPEN, MSG_EXIT_NOT_OPEN:
        "You can't, since ",(the) noun, " is closed.";
    MSG_EXIT_SUCCESS:
	    "You leave ", (the) noun, ".";

	default:
		! No code found. Print an error message.
		RuntimeError("Unknown message#");
	}
];

[ ItorThem obj;
    if (obj == player)            { print "yourself"; return; }
    if (obj has pluralname)       { print "them"; return; }
    if (obj has animate) {
        if (obj has female)       { print "her"; return; }
        else
            if (obj hasnt neuter) { print "him"; return; }
    }
    print "it";
];


!
! Error messages
!
Constant ERR_TOO_MANY_TIMERS_DAEMONS "Too many timers/daemons";
Constant ERR_OBJECT_HASNT_PROPERTY "Object lacks that property";
Constant ERR_SCOPE_FULL "Scope full";

[RunTimeError p_err;
	print "[Run-time error: ";
	if(p_err ofclass String) print (string) p_err;
	! Add switch to support more complex messages
	"]";
];
