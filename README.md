# CSC332Midterm

1. Create a repository of the git and work within your section
    Clone the main GitHub repository using GitHub Desktop.

    Create a New Branch named after your feature. Do not work directly on the 'main' branch.

    Locate your File: Go to your .c file named after the task assigned in the discord.

    Edit your File: I have already included the template to follow the getopt() and argc/argv structure for input, and included the necessary headers as per explained in the next bullet point.

    Function Uniformity: All main entry functions must follow the int main(int argc, char *argv[]) format w getopt(). I have provided an outline structure to put your code into already, you can change the variables needed, add or remove more as necessary.

                Here are the basic paramters I assume that are needed for each of the functions to operate:
                   Loganalyzer: -f [filepath]

                    Filecrypt: -f [filepath] -k [key]

                    Timedexec: -t [seconds] -c [command]

                    Filediff: -a [file1] -b [file2]

                    Processgroup: -g [groupid] -s [signal]

    Commit & Push: Push your branch to GitHub and open a Pull Request for [Your Name] to review.
   
2. When finished, run test cases and try and break/test your code and create a doc/pdf of your results. you can create a simple main.c file for yourselves to test the logic and breakability, but just delete that file when you're finished. Please put your tests with screenshots and/or table of your expected output and actual output in a pdf.

    2.1 Compiling: To compile your feature and the shell at once, simply open your terminal in this folder and type make. I made a makefile that will generate the necessary .exe files for testing

                For full compilation, you'll want to:
                Save your code.
                Open your terminal (Cygwin/Bash).
                Type make to compile everything automatically.
                Run the shell with ./myshell to test your feature.

3. Documentation (Man Pages)
Every feature must include a text file following the standard man page format.

            Title it: [feature_name].man

            Include: Name, Synopsis, Description, Options, and at least one Example.

3.1 Example

            NAME
                myshell - a custom command-line interpreter and process manager

            SYNOPSIS
                ./myshell

            DESCRIPTION
                myshell is a custom Unix-like shell environment designed to manage 
                and execute a suite of specialized system utilities. It acts as the 
                parent process, reading user input, parsing command-line arguments, 
                and using fork() and execvp() to spawn child processes for specific 
                tasks.
    
                The shell continuously prompts the user for input until the 'exit' 
                command is issued. It serves as a unified interface for the 
                CSC332 Midterm toolset.
    
            BUILT-IN COMMANDS
                exit
                    Terminates the shell and returns control to the host operating 
                    system.
    
            SUPPORTED UTILITIES
                The shell supports executing the following utilities (assuming they 
                are compiled in the same directory):
        
                - loganalyzer  : Analyzes log files via memory mapping.
                - filecrypt    : Encrypts/decrypts files using a key.
                - filediff     : Compares two files.
                - timedexec    : Executes a command with a timeout.
                - processgroup : Manages signals for a specific process group.
    
            EXAMPLES
                Starting the shell:
                    $ ./myshell
                    myshell> 
                    
                Running a utility from within the shell:
                    myshell> loganalyzer -f test.log
    
            AUTHOR
                Dakota Paxton - Project Lead

4. Final Deliverables (By the 18th latest):
   Please submit a PDF in the /docs folder above that contains the following:
   Page 1: The Man Page
   
       Example as seen above

    Page 2: Test Cases (The Proof)

        A table showing Test # | Input | Expected Output | Actual Output | Pass/Fail.
    
        Also include screenshots: Cropped screenshots of terminal showing the command being run and the result.

    
If you need help, just let me know, you can look at my loganalyzer.c in the root for help/formatting and as reference.

Useful links if you need to learn how to use any form of Git/github/github Desktop. I personally use github desktop because I'm a noob who is scared of the command line, but just use what's comfortable for you. 

https://www.youtube.com/watch?v=8Dd7KRpKeaE

https://www.youtube.com/watch?v=AYJQi6TyPyU
