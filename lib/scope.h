! Routines to update what is in scope

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

[ InScope; ! TODO
];

[ LoopOverScope; ! TODO
];

[ PlaceInScope; ! TODO
];

[ ScopeWithin; ! TODO
];

[ TestScope; ! TODO
];

[ ObjectIsUntouchable item flag1 flag2 ancestor i;
    ! Determine if there's any barrier preventing the player from moving
    ! things to "item".  Return false if no barrier; otherwise print a
    ! suitable message and return true.
    ! If flag1 is set, do not print any message.
    ! If flag2 is set, also apply Take/Remove restrictions.

    ! If the item has been added to scope by something, it's first necessary
    ! for that something to be touchable.
    !"TODO: ObjectIsUntouchable";
    rfalse;
];

