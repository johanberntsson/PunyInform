! ext_flags.h, a library extension for PunyInform by Fredrik Ramsberg, adapted
! from flags.h by Adam Cadre.

! This is a simple system to provide on/off flags that only take up one bit of
! memory; thus, there's no need to waste memory (and global variables) by
! declaring a variable such as "doneflag" or some such, allocating an entire
! sixteen bits to a variable that will never be anything other than 0 or 1.

! Here's how to use it in your programs. After inclusing globals.h,
! set the constant FLAG_COUNT to the number of flags you need, then include
! ext_flags.h.
!
! When you start using a new flag, create a constant with a descriptive name.
! You may also want to add a comment, or keep a list on the side regarding
! the meaning of each flag.

! Constant F_FED_PARROT 1; ! Has the parrot been fed?
! Constant F_TICKET_OK 2; ! Has Hildegard booked her plane tickets with the correct credit card?
! Constant F_SAVED_CAT 3;   ! Has the player saved the cat in the tree?

! You get the idea. Note that the first flag is flag #1.

! Setting a flag on or off means calling the routine SetFlag(flag#) or
! ClearFlag(flag#). To indicate that the player has saved the cat,
! call "SetFlag(F_SAVED_CAT);", and to turn off that flag, call
! "ClearFlag(F_SAVED_CAT);"  (Minus the quote marks, of course.)

! Testing a flag is accomplished by calling FlagIsSet or FlagIsClear. So if you have
! a piece of code that should only be run if the parrot has been fed, you would
! enclose it in an "if (FlagIsSet(F_FED_PARROT)) { ... }" statement.
! Naturally, you can test if a flag is clear by calling FlagIsClear instead.

! Additionally, you can include one or two extra flags in each function call, e.g.
! SetFlag(F_FED_PARROT, F_TICKET_OK, F_SAVED_CAT) sets all three flags
! ClearFlag(F_FED_PARROT, F_TICKET_OK, F_SAVED_CAT) clears all three flags
! if(FlagIsSet(F_FED_PARROT, F_TICKET_OK, F_SAVED_CAT)) returns true if all three flags are set
! if(FlagIsClear(F_TICKET_OK, F_SAVED_CAT)) returns true if both flags are clear

System_file;

Constant EXT_FLAGS = 1;

#Ifndef RUNTIME_ERRORS;
Constant RUNTIME_ERRORS = 2;
#Endif;
#Ifndef RTE_MINIMUM;
Constant RTE_MINIMUM = 0;
Constant RTE_NORMAL = 1;
Constant RTE_VERBOSE = 2;
Constant TM_NOT_PUNY;
#Endif;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
Constant FL_ERR = "^[Flags error #";
#Endif;

#Ifv3;
Array flag_powers static ->
	$$00000001
	$$00000010
	$$00000100
	$$00001000
	$$00010000
	$$00100000
	$$01000000
	$$10000000;
#Endif;

Array game_flags -> (FLAG_COUNT + 1) / 8 + ((FLAG_COUNT + 1) & 7 > 0);

#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
[ IncorrectFlagNumber p_x;
#Ifdef DEBUG;
		if(p_x == 0) {
	#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
			print (string) FL_ERR,"2: Warning: Use of flag 0 is deprecated, and will not work as second or third argument to flag functions]^";
	#Ifnot;
			print (string) FL_ERR,"2 (warning)]^";
	#Endif;
		}
#Endif;
	if(p_x < 0 || p_x > FLAG_COUNT) {
#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
		print_ret (string) FL_ERR,"1: Tried to use flag ", p_x, 
			", but the highest flag number is ", FLAG_COUNT, "]";
#Ifnot;
		print_ret (string) FL_ERR,"1]";
#Endif;
	}
	rfalse;
];
#Endif;

[ SetFlag p_x p_y p_z _val;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(IncorrectFlagNumber(p_x)) rfalse;
#Endif;
#IfV5;
	@log_shift p_x (-3) -> _val;
	p_x = p_x & 7;
	@log_shift 1 p_x -> p_x;
	game_flags -> _val = game_flags -> _val | p_x;
#IfNot;
	_val = p_x / 8;
	game_flags -> _val = game_flags -> _val | flag_powers -> (p_x & 7);
#EndIf;
	if(p_y) SetFlag(p_y, p_z);
];

[ ClearFlag p_x p_y p_z _val;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(IncorrectFlagNumber(p_x)) rfalse;
#Endif;
#IfV5;
	@log_shift p_x (-3) -> _val;
	p_x = p_x & 7;
	@log_shift 1 p_x -> p_x;
	game_flags -> _val = game_flags -> _val & ~ p_x;
#IfNot;
	_val = p_x / 8;
	game_flags -> _val = game_flags -> _val & ~ flag_powers -> (p_x & 7);
#EndIf;
	if(p_y) ClearFlag(p_y, p_z);
];

[ FlagIsSet p_x p_y p_z _val;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(IncorrectFlagNumber(p_x)) rfalse;
#Endif;
#IfV5;
	@log_shift p_x (-3) -> _val;
	p_x = p_x & 7;
	@log_shift 1 p_x -> p_x;
	_val = game_flags -> _val & p_x;
#IfNot;
	_val = p_x / 8;
	_val = game_flags -> _val & flag_powers -> (p_x & 7);
#EndIf;
	if(_val == 0) rfalse;
	if(p_y) return FlagIsSet(p_y, p_z);
	rtrue;
];

[ FlagIsClear p_x p_y p_z _val;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(IncorrectFlagNumber(p_x)) rfalse;
#Endif;
#IfV5;
	@log_shift p_x (-3) -> _val;
	p_x = p_x & 7;
	@log_shift 1 p_x -> p_x;
	_val = game_flags -> _val & p_x;
#IfNot;
	_val = p_x / 8;
	_val = game_flags -> _val & flag_powers -> (p_x & 7);
#EndIf;
	if(_val) rfalse;
	if(p_y) return FlagIsClear(p_y, p_z);
	rtrue;
];
