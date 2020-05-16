! Routines to update what is in scope
! http://www.inform-fiction.org/manual/html/s32.html#p244
!
! add_to_scoe
!
! scope_stage:
! 1: allow multiple objects or not (true, false)
! 2: use ScopeWithin an PlaceInScope to add objects to scope
! 3: write an error message

! TODO
! - everything should be in scope for debugging verbs such as Scope
! - you should be able to get the scope from an NPC's point of view
! - 
Constant MAX_SCOPE = 32;

Global scope_pov;        ! Whose POV the scope is from (usually the player)
Array scope-->MAX_SCOPE; ! objects visible from the current POV

[ _SearchScope p_obj _child _add_this;
	while(p_obj) {
#IfDef DEBUG_SCOPE;
		print "Adding ",(object) p_obj," to scope. Action = ", action, "^";
#EndIf;
		if(scope_objects >= MAX_SCOPE) {
			RunTimeError(ERR_SCOPE_FULL);
			return;
		}			
		scope-->(scope_objects++) = p_obj;
		if(p_obj.add_to_scope) p_obj.add_to_scope();
		_child = child(p_obj);
		_add_this = _child ~= 0 && (p_obj hasnt container || p_obj has open || p_obj has transparent);
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
	! Add all in player location (which may be inside an object)
	_SearchScope(child(_start_pos));
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

[ AddToScope p_obj;
	! Mentioned in DM
	print "[<AddToScope> not implemented... YET!]^";
];

[ LoopOverScope p_routine p_actor _i;
	! DM: LoopOverScope(R,actor)
	! Calls routine p_routine(obj) for each object obj in scope for the
	! given actor. If no actor is given, the actor defaults to be the player.
	! No return value

	if(p_actor == 0) p_actor = player;
	_UpdateScope(p_actor);
	for(_i = 0: _i < scope_objects: _i++) p_routine(scope-->_i);
];

[ PlaceInScope p_obj;
	! DM: PlaceInScope(obj)
	! Used in “scope routines” (only) when scope_stage is set to 2 (only).
	! Places obj in scope for the token currently being parsed. No other
	! objects are placed in scope as a result of this, unlike the case of
	! ScopeWithin. No return value
	print "[<PlaceInScope> not implemented... YET!]^";
];

[ ScopeWithin p_obj;
	! DM: ScopeWithin(obj)
	! Used in “scope routines” (only) when scope_stage is set to 2 (only).
	! Places the contents of obj in scope for the token currently being
	! parsed, and applies the rules of scope recursively so that contents of
	! see-through objects are also in scope, as is anything added to scope.
	! No return value
	print "[<ScopeWithin> not implemented... YET!]^";
];

[ TestScope p_obj p_actor _i;
	! DM: TestScope(obj,actor)
	! Tests whether the object obj is in scope to the given actor. If no
	! actor is given, the actor is assumed to be the player.
	! The routine returns true or false.
	!print "TestScope ", (object) p_obj, "^";

	! special case for debugging verbs; everything is in scope
	if(action == ##Scope) rtrue;

	for(_i = 0: _i < scope_objects: _i++) {
		if(scope-->_i == p_obj) rtrue;
	}
	!print "TestScope failed^";
	rfalse;
];

[ ObjectIsUntouchable p_item p_flag _ancestor _i;
	! DM: ObjectIsUntouchable(obj,flag)
	! Determines whether any solid barrier, that is, any container that is
	! not open, lies between the player and obj. If flag is true, this
	! routine never prints anything; otherwise it prints a message like
	! “You can't, because ! … is in the way.” if any barrier is found.
	! The routine returns true if a barrier is found, false if not.
    rfalse;
];

