# The compiler to use
CC = gcc

# Compiler flags (Enable all warnings)
CFLAGS = -Wall -Wextra

# Libraries to link for the shell (specifically for readline)
LIBS = -lreadline -lncurses

# The list of executables to create
TARGETS = myshell loganalyzer filecrypt filediffadvanced processgroup timedexec

# The default 'all' rule
all: $(TARGETS)

# Rules for each specific executable
# Added $(LIBS) to link the readline library
myshell: mainInterface.c
	$(CC) $(CFLAGS) mainInterface.c -o myshell $(LIBS)

loganalyzer: loganalyzer.c
	$(CC) $(CFLAGS) loganalyzer.c -o loganalyzer

filecrypt: filecrypt.c
	$(CC) $(CFLAGS) filecrypt.c -o filecrypt

filediffadvanced: filediffadvanced.c
	$(CC) $(CFLAGS) filediffadvanced.c -o filediffadvanced

processgroup: processgroup.c
	$(CC) $(CFLAGS) processgroup.c -o processgroup

timedexec: timedexec.c
	$(CC) $(CFLAGS) timedexec.c -o timedexec

# Rule to clean up executables
clean:
	rm -f $(TARGETS) *.exe