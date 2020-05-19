! Routines to update what is in scope
! http://www.inform-fiction.org/manual/html/s32.html#p244
!

Constant MAX_SCOPE = 32;

Global scope_pov;        ! Whose POV the scope is from (usually the player)
Array scope-->MAX_SCOPE; ! objects visible from the current POV

[ _SearchScope p_obj _child _add_this _i _len _addr;
	while(p_obj) {
#IfDef DEBUG_SCOPE;
		print "Adding ",(object) p_obj," (", p_obj,") to scope. Action = ", action, "^";
#EndIf;
		if(scope_objects >= MAX_SCOPE) {
			RunTimeError(ERR_SCOPE_FULL);
			return;
		}			

		scope-->(scope_objects++) = p_obj;

		_addr = p_obj.&add_to_scope;
		if(_addr) {
			! routine or a list of objects
			if(UnsignedCompare(_addr-->0, top_object) > 0) {
				RunRoutines(p_obj, add_to_scope);
			} else {
#IfDef DEBUG_SCOPE;
				print "add_to_scope for ", (name) p_obj, " is list of objects:^";
#EndIf;
				_len = p_obj.#add_to_scope / WORDSIZE;
				for(_i = 0: _i  < _len: _i++) {
					_child =  _addr --> _i;
					if(_child) {
						_SearchScope(_child);
#IfDef DEBUG_SCOPE;
						print _i, ": ", _child, "^";
#EndIf;
					}
				}
			}
		}
		_child = child(p_obj);
		_add_this = _child ~= 0 && (p_obj has supporter || p_obj has transparent || (p_obj has container && p_obj has open));
		if(_add_this) {
			_SearchScope(_child);
		}
		p_obj = sibling(p_obj);
	}
];

[ _ResetScope;
	scope_pov = -1;
];

[ _UpdateScope p_actor _start_pos;
	! scope is already calculated
	if(p_actor == 0) p_actor = player;
	if(scope_pov == p_actor) return;

	scope_pov = p_actor;
	_start_pos = ScopeCeiling(p_actor);

#IfDef DEBUG_SCOPE;
	print "updating scope for ",(object) _start_pos,"^";
#EndIf;

	! the directions are always in scope
	scope-->0 = Directions;
	scope_objects = 1;

	! if we are in a container, add it to scope
	if(_start_pos ~= location) {
		scope-->(scope_objects++) = _start_pos;
	}

	! give the starting pos a chance to add objects to scope
	if(_start_pos.add_to_scope) _start_pos.add_to_scope();

	if(darkness) {
		! only the player's possessions are in scope
		_SearchScope(child(player));
	} else {
		! Add all in player location (which may be inside an object)
		_SearchScope(child(_start_pos));
	}
];

[ ScopeCeiling p_actor _parent;
	! this routine is in I6 stdlib, but not in DM
	!
    for(:: p_actor = _parent) {
        _parent = parent(p_actor);
        !   print "Examining ", p_actor, "(", (object) p_actor, ") whose parent is ", _parent, "(", (object) _parent, ")...^";
        if(_parent == 0 || (p_actor has container && p_actor hasnt transparent or open)) {
            return p_actor;
        }
    }
];

[ LoopOverScope p_routine p_actor _i;
	! DM: LoopOverScope(R,actor)
	! Calls routine p_routine(obj) for each object obj in scope for the
	! given actor. If no actor is given, the actor defaults to be the player.
	! No return value

	_UpdateScope(p_actor);
	for(_i = 0: _i < scope_objects: _i++) p_routine(scope-->_i);
];

[ PlaceInScope p_obj _i;
	! DM: PlaceInScope(obj)
	! Used in “scope routines” (only) when scope_stage is set to 2 (only).
	! Places obj in scope for the token currently being parsed. No other
	! objects are placed in scope as a result of this, unlike the case of
	! ScopeWithin. No return value

	! skip if already added
	for(_i = 0: _i < scope_objects: _i++) {
		if(scope-->_i == p_obj) return;
	}
	! add it
	if(scope_objects >= MAX_SCOPE) {
		RunTimeError(ERR_SCOPE_FULL);
		return;
	}			
	scope-->(scope_objects++) = p_obj;
];

[ ScopeWithin p_obj _i;
	! DM: ScopeWithin(obj)
	! Used in “scope routines” (only) when scope_stage is set to 2 (only).
	! Places the contents of obj in scope for the token currently being
	! parsed, and applies the rules of scope recursively so that contents of
	! see-through objects are also in scope, as is anything added to scope.
	! No return value

	! is there a child?
	p_obj = child(p_obj);
	if(p_obj == nothing) return;

	! skip if already added
	for(_i = 0: _i < scope_objects: _i++) {
		if(scope-->_i == p_obj) return;
	}

	! add all children
	_SearchScope(p_obj);
];

[ TestScope p_obj p_actor _i;
	! DM: TestScope(obj,actor)
	! Tests whether the object obj is in scope to the given actor. If no
	! actor is given, the actor is assumed to be the player.
	! The routine returns true or false.
	!print "TestScope ", (object) p_obj, "^";

	! special case for debugging verbs; everything is in scope
	if(action_debug) rtrue;

	_UpdateScope(p_actor);
	for(_i = 0: _i < scope_objects: _i++) {
		if(scope-->_i == p_obj) rtrue;
	}
	rfalse;
];

[ ObjectIsInvisible p_item p_flag;
	return ObjectIsUntouchable(p_item, p_flag, true);
];

[ ObjectIsUntouchable p_item p_flag p_checkvisible _ancestor _i;
	! DM: ObjectIsUntouchable(obj,flag)
	! Determines whether any solid barrier, that is, any container that is
	! not open, lies between the player and obj. If flag is true, this
	! routine never prints anything; otherwise it prints a message like
	! “You can't, because ! … is in the way.” if any barrier is found.
	! The routine returns true if a barrier is found, false if not.

	_ancestor = CommonAncestor(player, p_item);
	if(_ancestor == 0) {
		_ancestor = p_item;
		while (_ancestor && (_i = ObjectScopedBySomething(_ancestor)) == 0)
			_ancestor = parent(_ancestor);
		if(_i ~= 0) {
			if(ObjectIsUntouchable(_i, p_flag)) return; ! An item immediately added to scope
		}
	} else if(player ~= _ancestor) {
		! First, a barrier between the player and the ancestor.  The player
		! can only be in a sequence of enterable objects, and only closed
		! containers form a barrier.
		_i = parent(player);
		while (_i ~= _ancestor) {
			if(_i has container && _i hasnt open &&
				(p_checkvisible == false || _i hasnt transparent)) {
				if(p_flag == false) PrintMsg(MSG_TOUCHABLE_FOUND_CLOSED, _i);
				rtrue;
			}
			_i = parent(_i);
		}
    }

	! Second, a barrier between the item and the ancestor.  The item can
	! be carried by someone, part of a piece of machinery, in or on top
	! of something and so on.
	if (p_item ~= _ancestor) {
		_i = parent(p_item);
		while (_i ~= _ancestor) {
			if(_i has container && _i hasnt open &&
				(p_checkvisible == false || _i hasnt transparent)) {
				if(p_flag == false) PrintMsg(MSG_TOUCHABLE_FOUND_CLOSED, _i);
				rtrue;
			}
			_i = parent(_i);
		}
	}
    rfalse;
];

