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

! There are also functions to check if any of two or three flags are set or clear:
! if(AnyFlagIsSet(F_FED_PARROT, F_TICKET_OK, F_SAVED_CAT)) returns true if any of the three flags are set
! if(AnyFlagIsClear(F_FED_PARROT, F_TICKET_OK, F_SAVED_CAT)) returns true if any of the three flags are clear

! In all of these procedures, you can use a negative flag number, to mean "do
! the opposite." E.g. SetFlag(F_FED_PARROT, -F_SAVED_CAT) will set 
! F_FED_PARROT and clear F_SAVED_CAT, while 
! AnyFlagIsSet(F_FED_PARROT, -F_TICKET_OK) returns true if F_FED_PARROT is set
! or F_TICKET_OK is clear.

System_file;

Constant EXT_FLAGS = 1;

#Ifndef RUNTIME_ERRORS;
Constant RUNTIME_ERRORS = 2;
#Endif;
#Ifndef RTE_MINIMUM;
Constant RTE_MINIMUM = 0;
Constant RTE_NORMAL = 1;
Constant RTE_VERBOSE = 2;
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
	if(p_x < 0) p_x = -p_x;
#Ifdef DEBUG;
		if(p_x == 0) {
	#Iftrue RUNTIME_ERRORS == RTE_VERBOSE;
			print (string) FL_ERR,"2: Warning: Use of flag 0 is deprecated, and will not work as second or third argument to flag functions]^";
	#Ifnot;
			print (string) FL_ERR,"2 (warning)]^";
	#Endif;
		}
#Endif;
	if(p_x > FLAG_COUNT) {
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
	if(p_y) SetFlag(p_y, p_z);
	if(p_x < 0) {
		ClearFlag(-p_x);
		rtrue;
	}
#IfV5;
	@log_shift p_x (-3) -> _val;
	p_x = p_x & 7;
	@log_shift 1 p_x -> p_x;
	game_flags -> _val = game_flags -> _val | p_x;
#IfNot;
	_val = p_x / 8;
	game_flags -> _val = game_flags -> _val | flag_powers -> (p_x & 7);
#EndIf;
];

[ ClearFlag p_x p_y p_z _val;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(IncorrectFlagNumber(p_x)) rfalse;
#Endif;
	if(p_y) ClearFlag(p_y, p_z);
	if(p_x < 0) {
		SetFlag(-p_x);
		rtrue;
	}
#IfV5;
	@log_shift p_x (-3) -> _val;
	p_x = p_x & 7;
	@log_shift 1 p_x -> p_x;
	game_flags -> _val = game_flags -> _val & ~ p_x;
#IfNot;
	_val = p_x / 8;
	game_flags -> _val = game_flags -> _val & ~ flag_powers -> (p_x & 7);
#EndIf;
];

[ _FlagValue p_x _abs_p_x _val;
	_abs_p_x = p_x;
	if(p_x < 0) _abs_p_x = -p_x;
#IfV5;
	@log_shift _abs_p_x (-3) -> _val; ! Divide by 8
	_abs_p_x = _abs_p_x & 7;
	@log_shift 1 _abs_p_x -> _abs_p_x;
	_val = game_flags -> _val & _abs_p_x;
#IfNot;
	_val = _abs_p_x / 8;
	_val = game_flags -> _val & flag_powers -> (_abs_p_x & 7);
#EndIf;
	if(p_x >= 0) return _val;
	if(_val == 0) rtrue;
	rfalse;
];

[ FlagIsSet p_x p_y p_z;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(IncorrectFlagNumber(p_x)) rfalse;
#Endif;
	if(_FlagValue(p_x) == 0) rfalse;
	if(p_y) return FlagIsSet(p_y, p_z);
	rtrue;
];

[ FlagIsClear p_x p_y p_z;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
	if(IncorrectFlagNumber(p_x)) rfalse;
#Endif;
	if(_FlagValue(p_x)) rfalse;
	if(p_y) return FlagIsClear(p_y, p_z);
	rtrue;
];

[ AnyFlagIsSet p_x p_y p_z;
	if(FlagIsClear(p_x, p_y, p_z)) rfalse;
	rtrue;
];

[ AnyFlagIsClear p_x p_y p_z;
	if(FlagIsSet(p_x, p_y, p_z)) rfalse;
	rtrue;
];

