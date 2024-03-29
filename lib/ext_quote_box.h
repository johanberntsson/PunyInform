! ext_quote_box.h, a library extension for PunyInform by Fredrik Ramsberg.
!
! This is an extension to let games show a simple quote box.
! For z5+ games, the extension will try to center the quote box on the screen,
! by reading the screen width reported by the interpreter in the header.
! For z3, this information is not available. Instead, it can do it two ways:
! 1: The game programmer tells the extension to assume the screen has a certain
!      width and the extension uses this information to center the quote box.
! 2: The game programmer tells the extension to just indent the quote box a
!      fixed number of characters.

! To use (1), set the constant QUOTE_V3_SCREEN_WIDTH to the desired width, which
! has to be > 6.

! To use (2), set the constant QUOTE_V3_SCREEN_WIDTH to the desired number of
! characters to indent by, which must be in the range 0-6.

! By default, method (2) will be used, with 2 characters of indentation.

! To display a quote box, create a word array holding the number of lines, the
! number of characters in the longest line, and then a string per line, and call
! QuoteBox with the array name as the argument.

! Array quote_1 static --> 5 35
! "When I die, I want to go peacefully"
! "in my sleep like my grandfather."
! "Not screaming in terror, like the"
! "passengers in his car."
! "               -- Jack Handey";
!
! [AnyRoutine;
!   QuoteBox(quote_1);
! ];
!
! You can also add a second argument to QuoteBox. Setting it to true makes
! QuoteBox skip waiting for a keypress after the quote box has been shown:
!
!   QuoteBox(quote_1, true); ! Don't wait for keypress

System_file;

Constant EXT_QUOTE_BOX = 1;

#IfnDef RUNTIME_ERRORS;
Constant RUNTIME_ERRORS = 2;
#EndIf;
#IfnDef RTE_MINIMUM;
Constant RTE_MINIMUM = 0;
Constant RTE_NORMAL = 1;
Constant RTE_VERBOSE = 2;
#EndIf;
#Iftrue RUNTIME_ERRORS > RTE_MINIMUM;
Constant QB_ERR = "^[Quote_box error #";
#Endif;


Constant QUOTE_MAX_LENGTH = 80;

#IfV3;
Default QUOTE_V3_SCREEN_WIDTH = 2;

#Iftrue	QUOTE_V3_SCREEN_WIDTH == 6;
Constant QUOTE_INDENT_STRING = "      ";
#EndIf;
#Iftrue	QUOTE_V3_SCREEN_WIDTH == 5;
Constant QUOTE_INDENT_STRING = "     ";
#EndIf;
#Iftrue	QUOTE_V3_SCREEN_WIDTH == 4;
Constant QUOTE_INDENT_STRING = "    ";
#EndIf;
#Iftrue	QUOTE_V3_SCREEN_WIDTH == 3;
Constant QUOTE_INDENT_STRING = "   ";
#EndIf;
#Iftrue	QUOTE_V3_SCREEN_WIDTH == 2;
Constant QUOTE_INDENT_STRING = "  ";
#EndIf;
#Iftrue	QUOTE_V3_SCREEN_WIDTH == 1;
Constant QUOTE_INDENT_STRING = " ";
#EndIf;
#Iftrue	QUOTE_V3_SCREEN_WIDTH == 0;
Constant QUOTE_INDENT_STRING = "";
#EndIf;
#EndIf;

Array quote_buffer -> QUOTE_MAX_LENGTH + 3;

#Ifv3;
[ QuoteBox p_quote_data p_dont_pause _quote_lines _quote_width _screen_width _i _j _k _last_index;
#Ifnot;
[ QuoteBox p_quote_data p_dont_pause _quote_lines _quote_width _screen_width _i _k _last_index;
#Endif;
	_quote_lines = p_quote_data --> 0;
	_quote_width = p_quote_data --> 1;
#IfV5;
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
#IfTrue RUNTIME_ERRORS == RTE_VERBOSE;
	if(_quote_width > QUOTE_MAX_LENGTH) {
		print_ret (string) QB_ERR,"1: Tried to print quote wider than ", QUOTE_MAX_LENGTH, " characters]";
	}
#IfNot;
	if(_quote_width > QUOTE_MAX_LENGTH) {
		print_ret (string) QB_ERR,"1]";
	}
#EndIf;
#EndIf;
	_screen_width = 0->$21;
	_i = _quote_lines + 5;
	@erase_window -2;
	@split_window _i;
	@set_window 1;
!	@erase_window 1;
	@new_line;
	@new_line;
#IfNot;
	_screen_width = QUOTE_V3_SCREEN_WIDTH;
#EndIf;
	@new_line;
	font off;
	_last_index = 2 + _quote_lines;
	for(_i = 1 : _i <= _last_index : _i++) {
#IfDef QUOTE_INDENT_STRING;
		print (string) QUOTE_INDENT_STRING;
#IfNot;
		FastSpaces((_screen_width - _quote_width - 2) / 2);
#EndIf;
		_k =  p_quote_data-->_i;
		if(_i == 1 or _last_index) {
			_k = "";
#IfV3;
			for(_j = -2 : _j < _quote_width: _j++) @print_char '-';
#EndIf;
		}
#IfV5;
		style reverse;
#EndIf;
		@print_char ' ';
		print (string) _k;
#IfV5;
		@output_stream 3 quote_buffer;
		print (string) _k;
		@output_stream -3;
		FastSpaces(_quote_width + 1 - quote_buffer->1);
		style roman;
#EndIf;
		@new_line;
	}
	font on;

#IfV5;
	@set_window 0;
!	print "[Press any key to continue]";
	if(p_dont_pause == 0) {
		@read_char _i;
		@erase_window -1;
	} else
		@split_window 0;
	! Force a re-split to show game's statusline
#IfDef PUNYINFORM_MAJOR_VERSION;
	statusline_current_height = 0;
#IfNot;
	gg_statuswin_cursize = 0;
#EndIf;
#IfNot;
!	@new_line;
!	print "[ENTER]";
	if(p_dont_pause == 0) {
		quote_buffer -> 0 = 1;
		read quote_buffer quote_buffer;
	}
#EndIf;
];
