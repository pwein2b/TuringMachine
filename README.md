# TuringMachine
A simple implementation of a deterministic turing machine in c++.
This is meant to be an educational project, showing how a turing machine moves around and writes on its tape.

To use the TuringMachine, you need to do the following steps:

- Create an instance of the TuringMchine

```
TuringMachine tm;
```

- Add rules
```
tm.addRule("S1", '_', '1', Direction::RIGHT, "F2");
```
This tells the machine what to do if it happens to be in state S1 and reads '_'. It will then write down '1', go to the right and change its state to F2. addRule() will automatically add the states if they have not been used before.

- Set a starting state
```
tm.setStart("S1");
```

- (optionally) Set a final state. You can also add multiple final states.
```
tm.setFinalState("F3");
```

- Create a tape with the input to the program
```
Tape tape("1111");
```
This will put four 1s  onto the tape. The turing machine will then start at the first 1. Optionally you can set a starting position to start to the right of it.

- Run the machine.
```
tm.step(&tape);
```
This will show the tape, the head and the state of the machine in the terminal everytime the machine moves. You get the next step by hitting enter. If you would like to run the machine automatically, use
```
tm.run(&tape);
```
Both methods will return a bool (or nothing, I can not solve the halting problem), showing wether a final state has been reached or not. This can be used to accept a formal language.

You can find a complete example in main.cpp.