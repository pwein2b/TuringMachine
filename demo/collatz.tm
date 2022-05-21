# Calculate the Collatz index of a number, if it exists

# Inititialize the tape: start the unary counter on the right after a '$' sign
TapeInit: {0,1},R -> TapeInit
TapeInit: _,R -> TapeInit1
TapeInit1: _,$,L -> MoveLeft # Back to the beginning

# Start at the left-most digit and check if the number is already 1
CheckFinished: 0,S -> Collatz # it is not
CheckFinished: 1,L -> CheckFinished1
CheckFinished1: _,R -> Done # Yes
CheckFinished1: {0,1},R -> Collatz # it's not

# Starting at the right-most digit, apply the Collatz Operation.
# Option 1: Divide by 2
Collatz: 0,_,S -> IncrementCounter

# Option 2: 3n+1
Times3: import "times3.tm" at U1 -> IncrementCounter
Collatz: 1,S -> Times3

# Move right to where the counter is and increment
UnaryIncrement: import "unaryincrement.tm" -> MoveLeft
IncrementCounter: {_,0,1},R -> IncrementCounter
IncrementCounter: $,R -> UnaryIncrement

# Move left from the index to the (right end of the) number and check if we're done
MoveLeft: {0,1},L -> MoveLeft
MoveLeft: {$,_},L -> MoveLeft1
MoveLeft1: _,L -> MoveLeft1
MoveLeft1: {0,1},S -> CheckFinished

# Done: Move right to where the index is
Done: {0,1_},R -> Done
Done: _,R -> Done
Done: $,R -> Done1
Done1 final;
