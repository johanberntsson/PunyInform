!-------------------------------------------------------------------------
! WaitTime - an Inform library to provide understanding of English time
!            strings, and give an enhanced Wait command. Time games only!
!            #include it just after Grammar.
!            by L. Ross Raszewski, 1996. Based on TimeWait.h by Andrew
!           Clover.
!The ParseTime and WaitMoves functions are taken from TimeWait with only
!minimal modification.  I worked this out after I found that I couldn't get
!Andrew's library to work.  I updated some of the code which I believe was,
!outdated, and changed all the Wait subs so that they call WaitMovesSub.
!tw_waiting is used both as a sort of pseudo-global as Mr. Clover intended,
!and as an indicator.  I set up my programs to update the time through the
!TimePass routine by featuring the line:
!       if (tw_waiting hasnt locked) SetTime(the_time+1);
!       give tw_waiting ~locked;
!If an attempt is made to "Wait 0", or a function is called that is not meant
!to take any time, tw_waiting is given "locked", and the clock is not advanced.
!As with Andrew's TimeWait, any message that should halt the waiting process
!can give tw_waiting the "on" attribute, and discontinue waiting.  TimeWait
!featured a call to Time(), and wouldn't compile until I changed it to
!TimePasses().  Even when I changed it, the program was still prone to getting
!stuck in infinite loops, which is why I converted the time passage into moves.
!the command "Wait 10" or "Wait 10 minutes" should now be equivalent to the player
!Typing "Wait" ten times.  During testing, I realized that calling TimePasses()
!did not perform all the tasks that occur at the end of the turn, so I changed it
!again to End_Turn_Sequence().  I think this will make it operate satisfactorily.
!Also, I have added a MAX_WAIT, which should be defined as a constant before
!inclusion.  Attempts to wait more than that many (minutes) will generate a
!"That's too long" message.  Inspired by "A Mind Forever Voyaging", I added
!a response to "Wait 0".  I'm not sure, but I think having a time passage rate
!other than 0 will give the program problems.  If you want more than 1 minute
!to pass for each turn, some modification of the code will be necessary (Don't
!ask me how, I'm surprised I even got this far!).
!-------------------------------------------------------------------------
object tw_waiting "tw";
#IFNDEF MAX_WAIT; Constant MAX_WAIT 1000; #ENDIF;
!Action routines for Wait command...

[ WaitMovesSub _i;
	if (noun > MAX_WAIT) {
		give tw_waiting locked;
		"That's too long to wait.";
	};
	if (noun == 0) {
		print "Time doesn't pass.^";
		give tw_waiting locked;
		meta = 1;
		rtrue;
	};
	if (noun == 1)
		<<Wait>>;
	print "Time passes.^";
	give tw_waiting ~on;
	for (_i = noun : (_i > 1) && (deadflag == GS_PLAYING) && (tw_waiting hasnt on) : _i--) {
		EndTurnSequence();
		_UpdateScoreOrTime();
		#IfV3;
			@show_status;
		#IfNot;
			DrawStatusLine();
		#EndIf;
	}
	if ((tw_waiting has on) && (_i > 1) && (deadflag == GS_PLAYING))
		print "^(waiting stopped)^";
];

[ WaitHoursSub;
	noun = noun * 60;
	WaitMovesSub();
];

[ WaitUntilSub;
	if (parsed_number >= the_time)
		noun = parsed_number - the_time;
	if (parsed_number < the_time) {
		parsed_number = the_time - parsed_number;
		noun = 1440 - parsed_number;
		print "(tomorrow)^^";
	}
	WaitMovesSub();
];

!ParseTime takes data from the next words (using wn) and returns a
!the_time format time number, or -1 for unrecognisable. It can recognise
!time expressed in any of the following formats:
!!a. <"0"-"59">|<"one"-"twenty">|"half"|"quarter" ["minutes"|"minute"]
!   "past"|"to" <"1"-"12">|<"one"-"twelve"> ["am"|"pm"]
!b. <"1"-"12">|<"one"-"twelve"> ["o'clock"] ["am"|"pm"]
!c. <"1"-"12">|<"one"-"twelve"> <"0"-"59">|<"one"-"twenty"> ["am"|"pm"]
!d. <"1"-"12">":"<"0"-"59"> ["am"|"pm"]
!e. "midnight"|"midday"|"noon"
!!If no am/pm is specified, the next time likely to come up is chosen; that
!is, the one that's just ahead of the current time. However, if this
!happens, the tw_waiting object is given the 'general' attribute. Thus you
!can change the time returned by twelve hours in an action like SetClock
!if necessary.
!!The next dictionary command is there to allow us to compare a typed
!string with "o'clock", something we can't normally do as it is bounded by
!single quotes.

Constant tw_oclock 'o^clock';

[ ParseTime _i _j _k _flg _loop _dig _hr _mn;
	give tw_waiting ~general;
	_i = NextWord();
	if (_i == 'midday' or 'noon' or 'midnight') {    ! then case (e) applies
		if (_i == 'midnight')
			_hr = 0;
		else
			_hr = 12;
		_mn = 0;
		parsed_number = (_hr * 60 + _mn);
	} else {                                        ! test for case (d)
		_k = (wn - 1) * 4 + 1;
		_j = parse->_k;
		_j = _j + buffer;
		_k = parse->(_k - 1);
		_flg = 0;
		for (_loop = 0 : _loop < _k : _loop++) {
			_dig = _j->_loop;
			if (_dig == ':')
				_flg = 1;
		}
		if ((_k > 2) && (_k < 6) && (_flg == 1)) {     ! then case (d) applies
			_hr = 0;
			_mn = 0;
			_loop = 0;
.tw_diglph;
			_dig = _j->_loop;
			_loop++;
			if(_dig >= '0' && _dig <= '9') {
				_hr = 10 * _hr + _dig - '0';
				jump tw_diglph;
			}
			if (_dig ~= ':')
				return -1;
			while (_loop < _k) {
				_dig = _j->_loop;
!				_mn = _mn * 10;
				if(_dig < '0' || _dig > '9')
					return -1;
				_mn = 10 * _mn + _dig - '0';
				_loop++;
			}                                        ! decode digital time
		} else {
			_j = NextWordStopped();
			if ((_j == tw_oclock or -1) || (_j == 'am' or 'pm')) { ! then case (c) applies
				_hr = TryNumber(wn - 2);
				_mn = 0;
				if (_j ~= tw_oclock)
					wn--;
			} else {
				_k = TryNumber(wn - 1);
				if (_k ~= -1000) {                     ! then case (b) applies
					_mn = _k;
					_hr = TryNumber(wn - 2);
				} else {                            ! well, must be case (a)
					_mn = TryNumber(wn-2);
					if (_i == 'quarter')
						_mn = 15;
					if (_i == 'twenty-five')
						_mn = 25;
					if (_i == 'half' or 'thirty')
						_mn = 30;
					if (_j == 'minute' or 'minutes') {
!						print "Yay ", _mn, "minutes!^";
						_j = NextWordStopped();             ! ignore 'minutes'
!						if(_j < 0) {
!							parsed_number = (the_time + _mn) % 1440;
!							print "parsed_number is ", parsed_number, "^";
!							rtrue;
!						}
					}
					_hr = TryNumber(wn);
					wn++;
					if (_j ~= 'past' or 'to')
						_hr = -1;
					if (_j == 'to') {
						_hr--;
						_mn = 60 - _mn;
						if (_hr == 0)
							_hr = 12;
					}
				}
			}
		}
!		print "hr is ", _hr, " mn is ", _mn, "^";
		if ((_hr > 12) || (_hr < 1) || (_mn > 59) || (_mn < 0))
			parsed_number = -1;
		else {
!			print "Oki!^";
			if (_hr==12)                                    ! now sort out am/pm
			_hr = 0;
			_i = NextWord();
			if (_i == 'pm')
				_hr = _hr + 12;
			else if (_i~='am') {                            ! am or pm implied, then?
!				print "Oki2^";
				give tw_waiting general;
				wn--;
				_i = (_hr * 60 + _mn);
				_j = ((_hr + 12) * 60 + _mn);
				_i = _i - the_time;
				_j = _j - the_time;
				if (_i < 0)
					_i = _i + (24 * 60);
				if (_j < 0)
					_j = _j + (24 * 60);
				if (_i > _j)
					_hr = _hr + 12;
			}
			parsed_number = (_hr * 60 + _mn);
		}
	}
	if (parsed_number == -1)
		return GPR_FAIL;
	else
		return GPR_NUMBER;
];


!Now the grammar for the new Wait actions.
!You can use parsetime in other new actions. For example, you could perhaps
!allow setting of watches, etc. with grammar like:
!!extend "set" first
!    * is_timepiece "to" parsetime -> SetClock;

Extend 'wait'
    * 'until' parsetime           -> WaitUntil
    * 'til' parsetime             -> WaitUntil
    * 'till' parsetime            -> WaitUntil
	* 'for' number 'minute'/'minutes' -> WaitMoves
	* 'for' number 'hour'/'hours' -> WaitHours
    * number 'minute'/'minutes'   -> WaitMoves
    * number 'hour'/'hours'       -> WaitHours
    * number                      -> WaitMoves
    * parsetime                   -> WaitUntil;
