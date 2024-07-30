# SO Shell - Phase 2

*Specification for the second phase of the project carried out as part of the Operating Systems course exercises.*

## Shell Implementation - Phase 2 (File Input)

Changes from Phase 1:

1. Print the prompt to `STDOUT` only if `STDIN` corresponds to a special character device (`man fstat`).
2. When reading a line from `STDIN`, you must account for the possibility that multiple lines will be read, and the last line may not be read completely. You need to handle the case where only part of a line is read in a single read operation.
3. You still need to execute only the first command from each of the read lines.
4. Remember that the last line of input may end with EOF rather than a newline.

Scripts and the first set of tests are located in the `test` directory. Instructions for running the tests are in the `test/README.md` file.

Each test from the first set is executed in two modes:
- In the first mode, the entire file is provided as input all at once.
- In the second mode, the input is given to the `splitter` program and piped to the shell. The `splitter` rewrites the input to the output, inserting pauses at pseudo-random moments.
