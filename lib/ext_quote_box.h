#IfnDef QUOTE_V3_SCREEN_WIDTH;
Constant QUOTE_V3_SCREEN_WIDTH = -2;
#EndIf;

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

Array quote_buffer -> 83;

[ QuoteBox p_quote_data _quote_lines _quote_width _screen_width _i _j _k _last_index;
	_quote_lines = p_quote_data --> 0;
	_quote_width = p_quote_data --> 1;
#IfV5;
	_screen_width = 0->$21;
#IfNot;
	_screen_width = QUOTE_V3_SCREEN_WIDTH;
#EndIf;
	_i = _quote_lines + 4;
	@split_window _i;
	@set_window 1;
!	@erase_window 1;
	@new_line;
	@new_line;
	_last_index = 2 + _quote_lines;
	for(_i = 1 : _i <= _last_index : _i++) {
#IfDef QUOTE_INDENT_STRING;
		print (string) QUOTE_INDENT_STRING;
#IfNot;
		for(_j = (_screen_width - _quote_width - 2) / 2 : _j >= 0: _j--) @print_char ' ';
#EndIf;
		_k =  p_quote_data-->_i;
		if(_i == 1 or _last_index) {
			_k = "";
#IfV3;
			for(_j = -2 : _j < _quote_width: _j++) @print_char '-';
#EndIf;
		} else 
			_k =  p_quote_data-->_i;
#IfV5;
		style reverse;
#EndIf;
		@print_char ' ';
		print (string) _k;
#IfV5;
		@output_stream 3 quote_buffer;
		print (string) _k;
		@output_stream -3;
		for(_j = quote_buffer-->0: _j < _quote_width + 1 : _j++) @print_char ' ';
		style roman;
#EndIf;
		@new_line;
	}
	
!	print (string) p_line_1;
#IfV5;
	@read_char _i;
#IfNot;
	quote_buffer -> 0 = 0;
	read quote_buffer 0;
#EndIf;
	@split_window 0;
	@set_window 0;
];

