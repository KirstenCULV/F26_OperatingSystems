/*
This is version 3 of KShell
A simpler version of a standard UNIX shell that can execute multiple commands concurrently
*/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unistd.h>

// Built in quit command

int kshell_quit(char **args);

const char* builtin_str[] = {
    "quit"
};

int (*builtin_func[]) (char **) = {
    &kshell_quit
};
int kshell_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}
int kshell_quit(char **args)
{
    return 0;
}

// fork execute wait
int kshell_few(char **args)
{
    pid_t pid;
    int cstatus;
    
    pid = fork();
    if (pid == 0) {
        // make baby
        if (execvp(args[0], args) == -1) {
      perror("kshell: command failed");
    }
    exit(EXIT_FAILURE);
    
  } else if (pid < 0) {
    // forking error
    perror("kshell: fork failed");
    
  } else {
    // parenting
    
    do {
      waitpid(pid, &cstatus, WUNTRACED);
    } while (!WIFEXITED(cstatus) && !WIFSIGNALED(cstatus));
  }

  return 1;
    }

// execution !!!!
int kshell_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // is empty
    return 1;
  }

  for (i = 0; i < kshell_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return kshell_few(args);
}

// reading
char *kshell_read_line(void)
{
#ifdef KSHELL_USE_STD_GETLINE
    char *line = NULL;
    ssize_t bufsize =0; // buffering
    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS); // end of file
        }
        else {
            perror("kshell: getline\n");
            exit(EXIT_FAILURE);
        }
    }
    return line;
#else
#define KSHELL_RL_BUFSIZE 512
    int bufsize = KSHELL_RL_BUFSIZE;
    int position = 0;
    char* buffer = (char*)malloc(512);
    int c;
    
    if (!buffer) {
        fprintf(stderr, "kshell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    
    //reading
    while (1) {
        c = getchar();
        
        if (c == EOF) {
            exit(EXIT_SUCCESS);
        }
            else if (c == '\n') {
                buffer[position] = '\0';
                return buffer;
            }
            else {
                buffer[position] = c;
            }
            position++;
    
    // bigger than buffer so reallocate
    if (position >= bufsize) {
        bufsize += KSHELL_RL_BUFSIZE;
        buffer = (char*)realloc(buffer, bufsize);
        if (!buffer) {
            fprintf(stderr, "kshell: allocation error\n");
            exit(EXIT_FAILURE);
        }
       }
    }
 #endif
}   

//parsing        
#define KSHELL_TOK_BUFSIZE 64
#define KSHELL_TOK_DELIM " \t\r\n\a"

char **kshell_split_line(char *line)
{
    int bufsize = KSHELL_TOK_BUFSIZE, position = 0;
    char **tokens = (char**)malloc(bufsize * sizeof(char*));
    char *token, **tokens_backup;
    
    if (!tokens) {
        fprintf(stderr, "kshell: allocation error\n");
        exit(EXIT_FAILURE);
        
    }
    
    token = strtok(line, KSHELL_TOK_DELIM);
    
    while (token != NULL) {
        tokens[position] = token;
        position++;
        
        if (position >= bufsize) {
            bufsize += KSHELL_TOK_BUFSIZE;
            tokens_backup = tokens;
            tokens = (char**)realloc(tokens, bufsize * sizeof(char*));
            
            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "kshell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }  
            token = strtok(NULL, KSHELL_TOK_DELIM);
    }
    
    tokens[position] = NULL;
    return tokens;
}

//looper
void kshell_loop(void)
{
    char *line;
    char **args;
    int status;
    bool batchMode = false;

    std::ifstream batchFile;

      if (**args == 2){
        batchMode = true;
        batchFile.open(args[1]);
        
        if(!batchFile) {
            fprintf(stderr, "kshell: could not open batch file\n");
            exit(EXIT_FAILURE);
            }
        }
    std::string line;
    
    std::istream* input = &std::cin;
    
    if (batchMode) {
        input = &batchFile;
    }

    if (!batchMode) {
        std::cout << "tinysh> ";
    }
    do {
        line = kshell_read_line();
        args = kshell_split_line(line);
        status = kshell_execute(args);
        
        free(line);
        free(args);
    }
    while (status);
}

//main stuff finally jeez
int main(int argc, char **argv)
{
    
    // command loop
    kshell_loop();
    
    return EXIT_SUCCESS;
}
            
            
            
            


