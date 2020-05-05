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

#Ifndef LibraryMessages;
Constant LibraryMessages 0;
#Endif;


[ PrintMsg p_msg p_dontprintnewline;
	if(p_msg ofclass String) {
		if(p_dontprintnewline == 0) {
			print_ret (string) p_msg;
		}
		print (string) p_msg;
		rtrue;
	}

	if(LibraryMessages(p_msg))
		rtrue;
		
	! Not a string, there should be code for the message here
	switch(p_msg) {
	MSG_OPEN_YOU_CANT, MSG_CLOSE_YOU_CANT, MSG_ENTER_YOU_CANT:
		"You can't ", (PrintVerb) verb_word, " that!";
	}

	! No code found. Print an error message.
	RuntimeError("Unknown message#");
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
