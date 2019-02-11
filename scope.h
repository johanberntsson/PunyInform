! Routines to update what is in scope

Constant MAX_SCOPE = 32;

Array scope-->MAX_SCOPE;

[ SearchScope p_obj _child;
	while(p_obj) {
!		print "Adding ",(object) p_obj," to scope.^";
		scope-->(scope_objects++) = p_obj;
		if(p_obj.add_to_scope) p_obj.add_to_scope();
		_child = child(p_obj);
		if(_child ~= 0 && (p_obj hasnt container || p_obj has open || p_obj has transparent)) {
			SearchScope(_child);
		}
		p_obj = sibling(p_obj);
	}
];

[ UpdateScope p_start_pos;
	scope_objects = 0;
    ! start recursion
    SearchScope(child(p_start_pos));
];

