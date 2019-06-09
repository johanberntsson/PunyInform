Constant MSG_YOUHAVEWON "You have won";
Constant MSG_YOUHAVEDIED "You have died";
Constant MSG_RESTARTRESTOREORQUIT "^Would you like to RESTART, RESTORE or QUIT? ";

[ PrintMsg msg;
    if(msg ofclass String) print_ret (string) msg;

    ! Not a string, check

];

