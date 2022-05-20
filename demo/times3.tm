# Compute the product of three with a given binary number.

# Move to the right end of the tape
S: 0,0,R -> S
S: 1,1,R -> S
S: _,_,L -> U0

# The states U0, U1, U2 represent the carry (Übertrag)
U0: 1,1,L -> U1
U0: _,_,R -> E
U1: 0,1,L -> U0
U1: 1,0,L -> U2
U1: _,1,L -> E
U2: 0,0,L -> U1
U2: _,0,L -> U1
U2: 1,1,L -> U2

# End state: Move right to find the first digit of the result and terminate when it has been found
_E: _,_,R -> E
