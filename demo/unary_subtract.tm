# Given two unary numbers separated by '#', calculate their difference.

alphabet {1,#,_};

Q1: 1,R -> Q1
Q1: #,R -> Q2
Q2: #,R -> Q2
Q2: _,#,jump L until _ -> F
F final;
Q2: 1,#,jump L until 1 -> Q3
Q3: #,L -> Q3
Q3: 1,#,jump L until _ -> Q1
