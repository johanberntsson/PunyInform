! routines to update scope rules

Constant MAX_SCOPE = 32;

Array scope-->MAX_SCOPE;

[ search_scope p_obj _child;
!	if(p_obj == nothing) return;
	while(p_obj ~= 0) {
!		print "Adding ",(object) p_obj," to scope.^";
		scope-->scope_objects = p_obj;
		scope_objects++;
		_child = child(p_obj);
		if(_child ~= 0 && (p_obj hasnt container || p_obj has open || p_obj has transparent)) {
			search_scope(_child);
		}
		p_obj = sibling(p_obj);
	}
];

[ update_scope p_start_pos;
	scope_objects = 0;
    ! start recursion
    search_scope(child(p_start_pos));
];

