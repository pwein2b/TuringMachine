# Increment a unary number at the right side
# '_' and '$' are considered as tape delimiters

# Find the end of the number
S: 1,R -> S
S: {_,$},S -> T

# Increment
T: {_,$},1,L -> U

# Move to the beginning of the number
U: 1,L -> U
U: {_,$},R -> V
V final;
