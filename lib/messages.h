Constant MSG_YOU_HAVE_WON "You have won";
Constant MSG_YOU_HAVE_DIED "You have died";
Constant MSG_RESTART_RESTORE_OR_QUIT "^Would you like to RESTART, RESTORE or QUIT? ";

Constant MSG_YOU_CANT_VERB_THAT = 1;

[ PrintMsg p_msg;
	if(p_msg ofclass String) print_ret (string) p_msg;

	! Not a string, there should be code for the message here
	switch(p_msg) {
	MSG_YOU_CANT_VERB_THAT:
		"You can't ", (PrintVerb) verb_word, " that!";
	}

	! No code found. Print an error message.
	Error("Unknown message#");
];

Constant ERR_TOO_MANY_TIMERS_DAEMONS "Too many timers/daemons";
Constant ERR_OBJECT_HASNT_PROPERTY "The object has not that property";

[RunTimeError p_err;
    print "[Run-time error: ";
    if(p_err ofclass String) print (string) p_err;
    switch(p_err) {
        ! For future use
    }
    "]";
];
