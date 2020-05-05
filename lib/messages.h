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
	MSG_DROP_NOT_HOLDING:
		"You aren't holding ", (ItOrThem) noun, ".";	
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
