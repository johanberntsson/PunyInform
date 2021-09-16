! ext_waittime.h, a library extension for PunyInform by Fredrik Ramsberg
!
! This extension is based on waittime.h by L. Ross Raszewski, which in turn
! was based on TimeWait.h by Andrew Clover.
!
! Include this extension *after* including puny.h in your game.
!
! This library extension gives an enhanced Wait command, allowing the player
! to type commands like:
!
! wait for 5 minutes
! wait until 3 PM
! wait til 5 o'clock
! wait til 2:30
! wait til quarter to five    (Note: requires OPTIONAL_ALLOW_WRITTEN_NUMBERS)
! wait 3 turns
!
! In a game which doesn't have time on the statusline, a move is counted as
! one minute, and the player can't specify a time of day to wait until.
!
! The game author can check the variable waittime_waiting - if it's true it
! means that waiting is currently in progress. The author can choose to set this
! variable to false to abort the player's waiting prematurely, i.e. if code in
! a daemon decides that a person enters the room, or there's an explosion
! nearby. If something happens that alters the time in the game drastically,
! it is recommended that any waiting is aborted, or there could be problems
! if the player has typed i.e. "wait until 3:00" and this was at 2:30 AM but
! suddenly the time changed to 1 PM and 3:00 would now refer to 3 PM.
!
! The author can define the constants MAX_WAIT_MOVES and MAX_WAIT_MINUTES
! before including this extension, to say the maximum number of moves or
! minutes the player can ask to wait. The default is 1440 for both.

System_file;

Global waittime_waiting = false;

#Ifndef MAX_WAIT_MOVES;
Constant MAX_WAIT_MOVES 1440;
#Endif;
#Ifndef MAX_WAIT_MINUTES;
Constant MAX_WAIT_MINUTES 1440;
#Endif;

!Action routines for Wait command...

[ WaitMovesSub p_is_minutes _time_left _time_before _minutes_passed;
	meta = 1;

	! If it's not a time game, waiting in minutes is changed to moves instead.
	if (sys_statusline_flag == false) p_is_minutes = false;

	if (noun == 0) {
		"Time doesn't pass.";
	};

	if ((p_is_minutes == false && noun > MAX_WAIT_MOVES) ||
			(p_is_minutes && noun > MAX_WAIT_MINUTES)) {
		"That's too long to wait.";
	};

	print "Time passes.^";
	waittime_waiting = true;
	_time_left = noun;
	_minutes_passed = 1; ! Unless p_is_minutes is set, this value won't change

	while(_time_left > 0 && deadflag == GS_PLAYING && waittime_waiting) {
		_time_before = the_time;
		EndTurnSequence();
		_UpdateScoreOrTime();
		#IfV3;
			@show_status;
		#IfNot;
			DrawStatusLine();
		#EndIf;
		if(p_is_minutes) {
			_minutes_passed = the_time - _time_before;
			if(_minutes_passed < 0)
				_minutes_passed = _minutes_passed + 1440;
		}
		_time_left = _time_left - _minutes_passed;
	}
	if(AfterRoutines() == false && waittime_waiting == 0 && _time_left > 0 &&
			deadflag == GS_PLAYING)
		print "^(waiting stopped)^";
	waittime_waiting = 0;
];

[ WaitHoursSub;
	noun = noun * 60;
	WaitMovesSub(true);
];

[ WaitMinutesSub;
	WaitMovesSub(true);
];

[ WaitUntilSub;
	if (parsed_number >= the_time)
		noun = parsed_number - the_time;
	if (parsed_number < the_time) {
		parsed_number = the_time - parsed_number;
		noun = 1440 - parsed_number;
		print "(tomorrow)^^";
	}
	WaitMovesSub(true);
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
!is, the one that's just ahead of the current time.

[ ParseTime _i _j _k _flg _loop _dig _hr _mn;
	if (sys_statusline_flag == false) return GPR_FAIL;

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
				return GPR_FAIL;
			while (_loop < _k) {
				_dig = _j->_loop;
				if(_dig < '0' || _dig > '9')
					return GPR_FAIL;
				_mn = 10 * _mn + _dig - '0';
				_loop++;
			}                                        ! decode digital time
		} else {
			_j = NextWordStopped();
			if ((_j == 'o^clock' or -1) || (_j == 'am' or 'pm')) { ! then case (c) applies
				_hr = TryNumber(wn - 2);
				_mn = 0;
				if (_j ~= 'o^clock')
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
						_j = NextWordStopped();             ! ignore 'minutes'
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
		if ((_hr > 12) || (_hr < 1) || (_mn > 59) || (_mn < 0))
			parsed_number = -1;
		else {
			if (_hr==12)                                    ! now sort out am/pm
			_hr = 0;
			_i = NextWord();
			if (_i == 'pm')
				_hr = _hr + 12;
			else if (_i~='am') {                            ! am or pm implied, then?
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


#Ifdef STATUSLINE_SCORE;
	Extend 'wait'
		* 'for' number 'move'/'moves'/'turn'/'turns' -> WaitMoves
		* 'for' number 'minute'/'minutes'            -> WaitMinutes
		* 'for' number 'hour'/'hours'                -> WaitHours
	    * number 'minute'/'minutes'                  -> WaitMinutes
	    * number 'hour'/'hours'                      -> WaitHours
		* number 'move'/'moves'/'turn'/'turns'       -> WaitMoves
		* number                                     -> WaitMoves;
#Ifnot;
	Extend 'wait'
	    * 'until' parsetime                          -> WaitUntil
	    * 'til' parsetime                            -> WaitUntil
	    * 'till' parsetime                           -> WaitUntil
		* 'for' number 'move'/'moves'/'turn'/'turns' -> WaitMoves
		* 'for' number 'minute'/'minutes'            -> WaitMinutes
		* 'for' number 'hour'/'hours'                -> WaitHours
	    * number 'minute'/'minutes'                  -> WaitMinutes
	    * number 'hour'/'hours'                      -> WaitHours
		* number 'move'/'moves'/'turn'/'turns'       -> WaitMoves
		* number                                     -> WaitMoves;
#Endif;
