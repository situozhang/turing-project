; This program checks if the input string belongs to L2
; Input: a string of an equation consists of 1's, x and =, e.g. '11x111=111111'

; the finite set of states
#Q = {0,q1,q2,q3,cmp,mh,mh2,accept,accept2,accept3,accept4,halt_accept,reject,reject2,reject3,reject4,reject5,halt_reject}

; the finite set of input symbols
#S = {1,x,=}

; the complete set of tape symbols
#G = {1,x,=,_,t,r,u,e,f,a,l,s}

; the start state
#q0 = 0

; the blank symbol
#B = _

; the set of final states
#F = {halt_accept}

; the number of tapes
#N = 3

; the transition functions

; State 0: start state
0 1__ 1__ *** q1
0 x__ ___ r** reject
0 =__ ___ r** reject
0 ___ ___ *** reject ; empty input

; State q1: cut the first factor to the 2nd tape
q1 1__ _1_ rr* q1
q1 x__ ___ r** q2
q1 =__ ___ r** reject
q1 ___ ___ *** reject

; State q2: make sure the second factor has at least one '1'
q2 1__ 1__ *** q3
q2 x__ ___ r** reject
q2 =__ ___ r** reject
q2 ___ ___ *** reject

; State q3: cut the second factor to the 3rd tape
q3 1__ __1 r*r q3
q3 =__ =__ *ll mh
q3 x__ ___ r** reject
q3 ___ ___ *** reject

; State mh: move 2nd and 3rd heads to the left
mh =11 =11 *ll mh
mh =_1 =_1 **l mh
mh =1_ =1_ *l* mh
mh =__ ___ rrr cmp

; State cmp: verify the equation
cmp 111 _11 rr* cmp
cmp 1_1 1__ *lr mh2
cmp __1 ___ *lr mh2
cmp x11 x11 *** reject
cmp x_1 x_1 *** reject
cmp =11 =11 *** reject
cmp =_1 =_1 *** reject
cmp _11 _11 *** reject

; State mh2: move 2nd head to the left
mh2 111 111 *l* mh2
mh2 1_1 1_1 *r* cmp
mh2 11_ 11_ *** reject
mh2 _11 _11 *** reject
mh2 _1_ _1_ *r* accept

; State accept*: write 'true' on 1st tape
accept ___ t__ r** accept2
accept2 ___ r__ r** accept3
accept3 ___ u__ r** accept4
accept4 ___ e__ *** halt_accept

; State reject*: write 'false' on 1st tape
reject x__ ___ r** reject
reject =__ ___ r** reject
reject 1__ ___ r** reject
reject 111 ___ rrr reject
reject x11 ___ rrr reject
reject =11 ___ rrr reject
reject _11 ___ *rr reject
reject x_1 ___ r*r reject
reject 1_1 ___ r*r reject
reject =_1 ___ r*r reject
reject __1 ___ **r reject
reject 11_ ___ rr* reject
reject x1_ ___ rr* reject
reject =1_ ___ r_* reject
reject _1_ ___ *r* reject
reject ___ f__ r** reject2
reject2 ___ a__ r** reject3
reject3 ___ l__ r** reject4
reject4 ___ s__ r** reject5
reject5 ___ e__ *** halt_reject