! routines to update scope rules

Constant MAX_SCOPE = 32;

Array scope-->MAX_SCOPE;

[ search_scope p_obj p_i;
	if(p_obj == nothing) return;
	scope-->p_i = p_obj;
	scope_objects++;
	!print "Adding ",(name) p_obj," to scope";
	search_scope(sibling(p_obj), p_i + 1);
];

[ update_scope p_start_pos;
	scope_objects = 0;
    ! start recursion
    search_scope(child(p_start_pos), 0);
];

