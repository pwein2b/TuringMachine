# TuringMachine
A simple implementation of a deterministic turing machine in c++.
This is meant to be an educational project, showing how a turing machine moves around and writes on its tape.

## Compilation
Apart from the C++ standard library and the meson build system, no other dependencies are required. To compile, open the main directory in terminal and type:

```
meson builddir
```

to initialize the build directory and
```
cd builddir
ninja
```

to compile.


## Usage
The preferred way of usage is creating an external text file that describes a Turing Machine, but it is also possible to compile a TM statically.

### Static usage
To use a statically compiled TuringMachine, you need to do the following steps:

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

You can find a complete example in demo/StaticMachine.cpp

### Turing Machine descriptions in external files
Turing Machines to work with may be loaded from file in the following format:
Each line represents a rule. For any states S, T and a character x, the rule that says "If the machine is in S and character x is read, then write y, go to T and move the r/w device right" is represented by the following line:
```
S: x,y,R -> T
```
States can have names that contain letters a-z, A-Z, numbers and underscores. States that don't exist upon reading such a line will be created. The first rule line determines the state to start in.
If the character read from the tape should not be modified (or equivalently, written back to the tape), one can simply omit the `,y` part of the line.

If S should become a final state, it should appear in a specific line
```
S final;
```

If there are several rules with same direction of movement and target state, they can be abbreviated.
For example, the rule set
```
A: 0,1,R -> B
A: 0,R -> C
B: 0,1,R -> B
B: 1,R -> C
```
can be simplified to
```
A,B: 0,1,R -> B
A,B: {0,1},R -> C
```

Comments can appear after a `#` symbol.
You can find an example in demo/times3.tm.
