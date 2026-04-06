# CSC332Midterm

1. Create a repository of the git and work within your section
    Clone the main GitHub repository using GitHub Desktop.

    Create a New Branch named after your feature ( feature_supercoolthingy). Do not work directly on the 'main' branch.

    Locate your File: Go to your .c file named after the task assigned in the discord.

    Edit your File: I have already included the template to follow the getopt() and argc/argv structure for input, and included the necessary headers as per explained in the next bullet point.

    Function Uniformity: All main entry functions must follow the int main(int argc, char *argv[]) format w getopt(). I have provided an outline structure to put your code into already, you can change the variables needed, add or remove more as necessary.

                Here are the basic paramters I have listed for each of the functions:
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

If you need help, just let me know, you can look at my loganylzer.c in the root for help/formatting if it gets confusing.

Useful links if you need to learn how to use any form of Git/github/github Desktop. I personally use github desktop because I'm a noob who is scared of the command line, but just use what's comfortable for you. 

https://www.youtube.com/watch?v=8Dd7KRpKeaE

https://www.youtube.com/watch?v=AYJQi6TyPyU
