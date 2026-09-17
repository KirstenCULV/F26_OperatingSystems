Kirsten Chavis 11984972

This program reads one line of input at a time. Commands can run concurrently. Each line is split into separate commands using the semicolon and whitespace.

For each command a child process is created using fork(), this process uses execvp() to execute the command, and the parent keeps track of how many child processes were created and uses wait() to wait for child processes to finish before moving on t0 the next line.

The built in quit command is handled directly instead of being passed to execvp(). When quit is found, any other commands from that line can be executed before the program finishes and then exits.

The batch mode has a few bugs and will have to be updated. 
