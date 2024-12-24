! Part of PunyInform: A small stdlib and parser for interactive fiction
! suitable for old-school computers such as the Commodore 64.
! Designed to be similar, but not identical, to the Inform 6 library.

! Routines to update what is in scope
! http://www.inform-fiction.org/manual/html/s32.html#p244
!

System_file;

[ _PerformAddToScope p_obj _add_obj _i _len _addr _n;
	_addr = p_obj.&add_to_scope;
	if(_addr) {
		! routine or a list of objects
		if(UnsignedCompare(_addr-->0, top_object) > 0) {
			RunRoutines(p_obj, add_to_scope);
		} else {
#IfDef DEBUG_SCOPE;
			print "add_to_scope for ", (name) p_obj, " is list of objects:^";
#EndIf;
			_len = p_obj.#add_to_scope;
#Ifv3;
			_len = _len / 2;
#Ifnot;
			@log_shift _len (-1) -> _len; ! Divide by 2
#Endif;
			if(_len == 0) rtrue;
			_len--;
!			for(_i = 0: _i  < _len: _i++) {
._next_obj;
				_add_obj =  _addr --> _i;
				if(_add_obj) {
					_n = scope_objects;
					_PutInScope(_add_obj);
					if(scope_objects > _n && _add_obj has reactive)
						_PerformAddToScope(_add_obj);
					_SearchScope(child(_add_obj));
#IfDef DEBUG_SCOPE;
					print _i, ": ", _add_obj, "^";
#EndIf;
				}
!			}
			@inc_chk _i _len ?~_next_obj;

		}
	}
];

[ _SearchScope p_obj p_risk_duplicate p_no_add _child;
#IfDef DEBUG_SCOPE;
#IfDef DEBUG;
	if(p_obj) print "_SearchScope adding ",(object) p_obj," (", p_obj,") and siblings to scope. Action = ", (DebugAction) action, "^";
#IfNot;
	if(p_obj) print "_SearchScope adding ",(object) p_obj," (", p_obj,") and siblings to scope. Action = ", action, "^";
#EndIf;
#EndIf;
	while(p_obj) {
		if(scope_objects >= MAX_SCOPE) {
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
			_RunTimeError(ERR_SCOPE_FULL);
#EndIf;
			return;
		}

		_PutInScope(p_obj, p_risk_duplicate);
!		scope-->(scope_objects++) = p_obj;

		! Add_to_scope
		if(p_no_add == 0 && p_obj has reactive) _PerformAddToScope(p_obj);

		_child = child(p_obj);
		if(_child ~= 0 && (p_obj has supporter or transparent || (p_obj has container && p_obj has open)))
			_SearchScope(_child, p_risk_duplicate, p_no_add); ! Add contents

		p_obj = sibling(p_obj);
	}
];

[_PutInScope p_obj p_risk_duplicate _i;
#IfDef DEBUG_SCOPE;
#IfDef DEBUG;
	if(p_obj) print "_PutInScope adding ",(object) p_obj," (", p_obj,") to scope. Action = ", (DebugAction) action, "^";
#IfNot;
	if(p_obj) print "_PutInScope adding ",(object) p_obj," (", p_obj,") to scope. Action = ", action, "^";
#EndIf;
#EndIf;
	if(p_risk_duplicate == 0) {
#IfV5;
		@scan_table p_obj scope scope_objects -> _i ?~_object_wasnt_found;
		return;
._object_wasnt_found;
#IfNot;
		for(_i = 0: _i < scope_objects: _i++) {
			if(scope-->_i == p_obj) return;
		}
#EndIf;
	}
	! Check if there is room
	if(scope_objects >= MAX_SCOPE) {
#IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
		_RunTimeError(ERR_SCOPE_FULL);
#EndIf;
		return;
	}
	! Add it
	scope-->scope_objects = p_obj;
	scope_objects++;
];

#Ifdef InScope;
[ _UpdateScope p_actor p_reason _start_pos _i _j _obj _initial_scope_objects
		_current_scope_objects _risk_duplicates _scope_base _can_skip;
#Ifnot;
[ _UpdateScope p_actor p_reason _start_pos _i _j _obj _initial_scope_objects
		_current_scope_objects _risk_duplicates _scope_base;
#Endif;

#IfDef DEBUG_SCOPE;
	print "*** Call to UpdateScope for ", (the) p_actor, "^";;
#EndIf;

	scope_reason = p_reason;
	! check if scope is already calculated
	if(cached_scope_pov == p_actor && scope_modified == false &&
			((scope_stage ~= 2 && cached_scope_routine == 0) ||
			 (scope_stage == 2 && cached_scope_routine == scope_routine))) {
#IfDef InScope;
		_can_skip = true;
#IfNot;
		return;
#EndIf;
	}
	scope_modified = false;
	_initial_scope_objects = scope_objects; 
	scope_objects = 0;

	if(scope_stage == 2) {
		cached_scope_routine = scope_routine;
		! call scope_routine to add objects, then abort if it returns true
		if(indirect(scope_routine)) jump _done_updating_scope;

#Ifdef InScope;
		! scope_routine has added some objects that we don't want to overwrite
		_current_scope_objects = scope_objects;
#EndIf;
	} else {
		cached_scope_routine = 0;
		_risk_duplicates = 1;
	}

#Ifdef InScope;
	! give entry routine a chance to override
	_i = InScope(p_actor);
	if(_i ~= 0 || scope_objects > _current_scope_objects) {
		scope_modified = true; ! Force a hard scope update next call
		if(_i) jump _done_updating_scope;
		_risk_duplicates = 0;
	} else if(_can_skip && scope_objects == 0) {
		! Neither scope routine or InScope have modified scope
		scope_objects = _initial_scope_objects;
		return;
	}
#Endif;

	! the directions are always in scope
	_PutInScope(Directions, _risk_duplicates);

	! if we are in a container, add it to scope
	_start_pos = ScopeCeiling(p_actor);
	if(parent(_start_pos)) {
		_PutInScope(_start_pos, _risk_duplicates);
	}

	! Add all in player location (which may be inside an object)
	_scope_base = _start_pos;
#Ifdef OPTIONAL_NO_DARKNESS;
	if(p_actor hasnt transparent) {
		! The player's possessions are in scope to the actor
		_SearchScope(child(p_actor), _risk_duplicates, true);
	}
#Ifnot;
	if(location == thedark || p_actor hasnt transparent) {
		! only the player's possessions and whatever is in the dark are in scope
		if(location == thedark) {
			_PutInScope(p_actor, _risk_duplicates);
			_scope_base = location;
		}
		_SearchScope(child(p_actor), _risk_duplicates, true);
	}
#Endif;

	_SearchScope(child(_scope_base), _risk_duplicates, true);

!	_current_scope_objects = scope_objects;
!	print "WILL perform AddToScope for object ", _current_scope_objects, " to ", scope_objects - 1, "!^";
	_j = scope_objects;
	@dec_chk _j _current_scope_objects ?_done_updating_scope;
	_i = _current_scope_objects;
._next_add_to_scope;
!	for(_i = _current_scope_objects : _i < scope_objects : _i++) {
		_obj = scope-->_i;
!		print "PERFORMING AddToScope for object ", _obj, "!^";
		if(_obj has reactive)
			_PerformAddToScope(_obj);
!	}
	@inc_chk _i _j ?~_next_add_to_scope;

._done_updating_scope;
#Ifdef OPTIONAL_MANUAL_SCOPE_BOOST;
	if(p_actor == player) {
#Ifdef DEBUG_MANUAL_SCOPE_BOOST;
		print "UPDATING PLAYER SCOPE, RESET BOOST^";
#EndIf;
		react_before_in_scope = true;
		react_after_in_scope = true;
		each_turn_in_scope = true;
	}
#Endif;
	scope_copy_actor = 0;
	cached_scope_pov = p_actor;

#IfDef DEBUG_SCOPE;
	print "*** Updated scope from ", (the) _start_pos, ". Found ", scope_objects, " objects.^";
#EndIf;
];

#IfV5;
[GetScopeCopy p_actor p_reason _i;
#IfNot;
[GetScopeCopy p_actor p_reason _i _max;
#EndIf;

	_UpdateScope(p_actor, p_reason);

	if(scope_copy_actor ~= p_actor) {
#IfV5;
		@log_shift scope_objects 1 -> _i; ! _i = scope_objects * 2
		@copy_table scope scope_copy _i;
#IfNot;
		if(scope_objects) {
			_max = scope_objects - 1;
._copy_next_entry;
			scope_copy-->_i = scope-->_i;
			@inc_chk _i _max ?~_copy_next_entry;
		}
#EndIf;
		scope_copy_actor = p_actor;
	}
	scope_copy_objects = scope_objects;
	return scope_copy_objects;
];

[ ScopeCeiling p_actor p_stop_before _parent;
	! this routine is in I6 stdlib, but not in DM
	!
	for(:: p_actor = _parent) {
		_parent = parent(p_actor);
		!   print "Examining ", p_actor, "(", (object) p_actor, ") whose parent is ", _parent, "(", (object) _parent, ")...^";
		if(_parent == 0 or p_stop_before || (p_actor has container && p_actor hasnt transparent or open)) {
			return p_actor;
		}
	}
];

[ TouchCeiling p_actor _parent;
	! this routine is in I6 stdlib, but not in DM
	!
	for(:: p_actor = _parent) {
		_parent = parent(p_actor);
		!   print "Examining ", p_actor, "(", (object) p_actor, ") whose parent is ", _parent, "(", (object) _parent, ")...^";
		if(_parent == 0 || (p_actor has container && p_actor hasnt open)) {
			return p_actor;
		}
	}
];

Constant PlaceInScope = _PutInScope;
Constant AddToScope = _PutInScope;

! [ PlaceInScope p_obj _i;
	! ! DM: PlaceInScope(obj)
	! ! Used in "scope routines" (only) when scope_stage is set to 2 (only).
	! ! Places obj in scope for the token currently being parsed. No other
	! ! objects are placed in scope as a result of this, unlike the case of
	! ! ScopeWithin. No return value

	! ! skip if already added
! #IfV5;
	! @scan_table p_obj scope scope_objects -> _i ?~not_found;
	! return;
! .not_found;
! #IfNot;
	! for(_i = 0: _i < scope_objects: _i++) {
		! if(scope-->_i == p_obj) return;
	! }
! #EndIf;
	! ! add it
	! if(scope_objects >= MAX_SCOPE) {
! #IfTrue RUNTIME_ERRORS > RTE_MINIMUM;
		! _RunTimeError(ERR_SCOPE_FULL);
! #EndIf;
		! return;
	! }
	! scope-->(scope_objects++) = p_obj;
! ];

[ TestScope p_obj p_actor _i;
	! DM: TestScope(obj,actor)
	! Tests whether the object obj is in scope to the given actor. If no
	! actor is given, the actor is assumed to be the player.
	! The routine returns true or false.
	!print "TestScope ", (object) p_obj, "^";

	if(p_actor == 0)
		p_actor = player;

	_UpdateScope(p_actor, TESTSCOPE_REASON);
#IfV5;
	@scan_table p_obj scope scope_objects -> _i ?~_object_wasnt_found;
	rtrue;
._object_wasnt_found;
#IfNot;
	for(_i = 0: _i < scope_objects: _i++) {
		if(scope-->_i == p_obj) rtrue;
	}
#EndIf;
	rfalse;
];



[ _FindBarrier p_ancestor p_obj p_dontprint;
	while (p_obj ~= p_ancestor) {
		if (_g_check_take && p_obj hasnt container && p_obj hasnt supporter) {
						! We're going to return true here, we just need to write the correct message
						! But if we don't need to print anything, just return now
						if (p_dontprint) rtrue;

			if (p_obj has animate) {
				PrintMsg(MSG_TAKE_BELONGS, _g_item, p_obj); rtrue;
			}
			if (p_obj has transparent) {
				PrintMsg(MSG_TAKE_PART_OF, _g_item, p_obj); rtrue;
			}
			PrintMsg(MSG_TAKE_NOT_AVAILABLE); rtrue;
		}
		if(p_obj has container && p_obj hasnt open) {
			if(p_dontprint == false) PrintMsg(MSG_TOUCHABLE_FOUND_CLOSED, p_obj);
			rtrue;
		}
		p_obj = parent(p_obj);
	}
	rfalse;
];

[ ObjectIsUntouchable p_item p_dontprint p_checktake _ancestor _i;
	! DM: ObjectIsUntouchable(obj,flag)
	! Determines whether any solid barrier, that is, any container that is
	! not open, lies between the player and obj. If flag is true, this
	! routine never prints anything; otherwise it prints a message like
	! "You can't, because ! ... is in the way." if any barrier is found.
	! The routine returns true if a barrier is found, false if not.

	_g_item = p_item;
	_g_check_take = p_checktake;

	_UpdateScope(player, TESTSCOPE_REASON);

	_ancestor = CommonAncestor(player, p_item);
	if(_ancestor == 0) {
		_ancestor = p_item;
		while (_ancestor && (_i = _ObjectScopedBySomething(_ancestor)) == 0)
			_ancestor = parent(_ancestor);
		if(_i ~= 0) {
			if(ObjectIsUntouchable(_i, p_dontprint, p_checktake)) {
				! Item immediately added to scope
				rtrue;
			}
		}
	} else if(player ~= _ancestor) {
		_g_check_take = 0;
		if(_FindBarrier(_ancestor, parent(player), p_dontprint)) {
			! First, a barrier between the player and the ancestor.  The player
			! can only be in a sequence of enterable objects, and only closed
			! containers form a barrier.
			rtrue;
		}
		_g_check_take = p_checktake;
	}

	! Second, a barrier between the item and the ancestor.  The item can
	! be carried by someone, part of a piece of machinery, in or on top
	! of something and so on.
	if (p_item ~= _ancestor && _FindBarrier(_ancestor, parent(p_item), p_dontprint)) {
		rtrue;
	}
	rfalse;
];

[ _ObjectScopedBySomething p_obj _j _k _l _m;
	objectloop (_j has reactive && (_j.&add_to_scope ~= 0)) {
		_l = _j.&add_to_scope;
		if (_l-->0 ofclass Routine) continue;
#IfV5;
		_k = _j.#add_to_scope;
		@log_shift _k (-1) -> _k;
		@scan_table p_obj _l _k -> _m ?~_object_wasnt_found;
		return _j;
._object_wasnt_found;
#IfNot;
		_k = (_j.#add_to_scope)/WORDSIZE;
		for (_m=0 : _m<_k : _m++) if (_l-->_m == p_obj) return _j;
#EndIf;
	}
	rfalse;
];

[ ScopeWithin p_obj _child;
	! DM: ScopeWithin(obj)
	! Used in "scope routines" (only) when scope_stage is set to 2 (only).
	! Places the contents of obj in scope for the token currently being
	! parsed, and applies the rules of scope recursively so that contents of
	! see-through objects are also in scope, as is anything added to scope.
	!
	! Note that p_obj is NOT added to the scope, only its contents.
	!
	! No return value

	! is there a child?
	_child = child(p_obj);
	if(_child == nothing) return;

	! add the child (will also add all siblings)
	_SearchScope(_child);
];

[ LoopOverScope p_routine p_actor _i _scope_count;
	! DM: LoopOverScope(R,actor)
	! Calls routine p_routine(obj) for each object obj in scope for the
	! given actor. If no actor is given, the actor defaults to be the player.
	! No return value
	if(p_actor == 0)
		p_actor = player;

	_scope_count = GetScopeCopy(p_actor, LOOPOVERSCOPE_REASON);

	for(_i = 0: _i < _scope_count: _i++) p_routine(scope_copy-->_i);
];
