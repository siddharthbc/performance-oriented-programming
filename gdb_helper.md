
# Instructions to Disassemble Python Bytecode

## Disassemble the Instructions

# run gdb on the python interpreter
gdb /path/to/python

# set a breakpoint at `_PyEval_EvalFrameDefault`:
break _PyEval_EvalFrameDefault

# Run the Python script `basic_add.py`:
run basic_add.py


# Disassemble the instructions:
disassemble




## Check How Many Instructions a Particular Function Has Executed

# run gdb on a binary
gdb /path/to/binary/

# Set a breakpoint at `SingleScalar`:
break SingleScalar


# Run the program:
run

# Record the full execution:
record full

# Step to the next instruction:
next

# Get information about the recorded instructions:
Info record