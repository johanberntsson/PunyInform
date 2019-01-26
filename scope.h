! routines to update scope rules

Constant MAX_SCOPE = 32;

Array scope-->MAX_SCOPE;

[ search_scope obj i;
	if(obj == nothing) return;
	scope-->i = obj;
	scope_objects++;
	!print "Adding ",(name) obj," to scope";
	search_scope(sibling(obj), i + 1);
];

[ update_scope start_pos;
    ! clear the scope
! 	for(i = 0: i < scope->0: i++) {
! 		scope-->i =  nothing;
! 	}

	scope_objects = 0;
    ! start recursion
    search_scope(child(start_pos), 0);
];

