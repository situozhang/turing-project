; This program checks if the input string belongs to L1
; Input: a string of a's and b's, e.g. 'aabaab'

; the finite set of states
#Q = {0,ct1,ct2,cmp,mh,accept,accept2,accept3,accept4,halt_accept,reject,reject2,reject3,reject4,reject5,halt_reject}

; the finite set of input symbols
#S = {a,b}

; the complete set of tape symbols
#G = {a,b,_,t,r,u,e,f,a,l,s}

; the start state
#q0 = 0

; the blank symbol
#B = _

; the set of final states
#F = {halt_accept}

; the number of tapes
#N = 2

; the transition functions

; State 0: start state
0 a_ a_ ** ct1
0 b_ __ r* reject
0 __ __ ** reject ; empty input

; State ct1: cut starting a's to the 2nd tape
ct1 a_ _a rr ct1
ct1 b_ b_ ** ct2
ct1 __ __ ** reject

; State ct2: cut starting b's to the 2nd tape
ct2 b_ _b rr ct2
ct2 a_ a_ *l mh
ct2 __ __ ** reject

; State mh: move 2nd head to the left
mh ab ab *l mh
mh aa aa *l mh
mh a_ a_ *r cmp

; State cmp: compare two strings
cmp aa __ rr cmp
cmp bb __ rr cmp
cmp a_ __ r* reject
cmp ab __ rr reject
cmp _a __ *r reject
cmp _b __ *r reject
cmp b_ __ r* reject
cmp ba __ rr reject
cmp __ __ ** accept

; State accept*: write 'true' on 1st tape
accept __ t_ r* accept2
accept2 __ r_ r* accept3
accept3 __ u_ r* accept4
accept4 __ e_ ** halt_accept

; State reject*: write 'false' on 1st tape
reject a_ __ r* reject
reject ab __ rr reject
reject b_ __ r* reject
reject ba __ rr reject
reject _a __ *r reject
reject _b __ *r reject
reject __ f_ r* reject2
reject2 __ a_ r* reject3
reject3 __ l_ r* reject4
reject4 __ s_ r* reject5
reject5 __ e_ ** halt_reject