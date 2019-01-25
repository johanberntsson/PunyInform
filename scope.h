! routines to update scope rules

Constant MAX_SCOPE = 32;

Array scope-->MAX_SCOPE;

[ search_scope obj i;
	if(obj == nothing) return;
	scope-->i = obj;
	!print "Adding ",(name) obj," to scope";
	search_scope(sibling(obj), i + 1);
];

[ update_scope start_pos  i;
    ! clear the scope
	for(i = 0: i < scope->0: i++) {
		scope-->i =  nothing;
	}

    ! start recursion
    search_scope(child(start_pos), 0);
];

