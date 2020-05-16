! Routines to update what is in scope
! http://www.inform-fiction.org/manual/html/s32.html#p244
!
! add_to_scoe
!
! scope_stage:
! 1: allow multiple objects or not (true, false)
! 2: use ScopeWithin an PlaceInScope to add objects to scope
! 3: write an error message

Constant MAX_SCOPE = 32;

Array scope-->MAX_SCOPE;

[ _SearchScope p_obj _child;
	while(p_obj) {
!		print "Adding ",(object) p_obj," to scope.^";
		if(scope_objects >= MAX_SCOPE) {
			RunTimeError(ERR_SCOPE_FULL);
			return;
		}			
		scope-->(scope_objects++) = p_obj;
		if(p_obj.add_to_scope) p_obj.add_to_scope();
		_child = child(p_obj);
		if(_child ~= 0 && (p_obj hasnt container || p_obj has open || p_obj has transparent)) {
			_SearchScope(_child);
		}
		p_obj = sibling(p_obj);
	}
];

[ _UpdateScope p_start_pos;
	scope-->0 = Directions;
	scope_objects = 1;
	if(p_start_pos ~= location) {
		scope-->(scope_objects++) = p_start_pos;
	}
	if(p_start_pos.add_to_scope) p_start_pos.add_to_scope();
	! Add all in player location (which may be inside an object)
	_SearchScope(child(p_start_pos));
];


[ SearchScope p_domain1 p_domain2 p_context _i;
	! SearchScope  domain1 domain2 context
	! Works out what objects are in scope (possibly asking an outside
	! routine), but does not look at anything the player has typed.
];

[ AddToScope p_obj;
	! according to DM this can be called from add_to_scope
];

[ InScope p_obj;
	! Used in “scope routines” (only) when scope_stage is set to 2 (only).
	! Places obj in scope for the token currently being parsed. No other
	! objects are placed in scope as a result of this, unlike the case of
	! ScopeWithin. No return value
];

[ LoopOverScope p_routine p_actor _i;
	! Calls routine p_routine(obj) for each object obj in scope for the
	! given actor. If no actor is given, the actor defaults to be the player.
	! No return value

	! TODO: use p_actor
	for(_i = 0: _i < scope_objects: _i++) p_routine(scope-->_i);
];

[ PlaceInScope p_obj;
	! Used in “scope routines” (only) when scope_stage is set to 2 (only).
	! Places obj in scope for the token currently being parsed. No other
	! objects are placed in scope as a result of this, unlike the case of
	! ScopeWithin. No return value
];

[ ScopeWithin p_obj;
	! Used in “scope routines” (only) when scope_stage is set to 2 (only).
	! Places the contents of obj in scope for the token currently being
	! parsed, and applies the rules of scope recursively so that contents of
	! see-through objects are also in scope, as is anything added to scope.
	! No return value
];

[ TestScope p_obj p_actor;
	! Tests whether the object obj is in scope to the given actor. If no
	! actor is given, the actor is assumed to be the player.
	! The routine returns true or false.
];

! Determines whether any solid barrier, that is, any container that is not
! open, lies between the player and obj. If flag is true, this routine never
! prints anything; otherwise it prints a message like “You can't, because
! … is in the way.” if any barrier is found.
! The routine returns   true if a barrier is found, false if not.
!
! (not DM) If flag2 is set, also apply Take/Remove restrictions.
[ ObjectIsUntouchable p_item p_flag p_flag2 _ancestor _i;
    rfalse;
];

