#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char *lsh_read_line(void)
{
  char *line = NULL;  // so the variable doesn't store any garbage value
  size_t bufsize = 0; // have getline allocate a buffer for us

  if (getline(&line, &bufsize, stdin) == -1)
  {
    if (feof(stdin)) 
    {
      exit(EXIT_SUCCESS);  // We recieved an EOF
    } 
    else  
    {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  return line;
}

char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));  //initialising a certain size
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();

  if (pid == 0) 
  {
    // Child process
    if (execvp(args[0], args) == -1) //error
    {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } 
  else if (pid < 0) 
  {
    // Error forking
    perror("lsh");
  } 
  else 
  {
    // Parent process
    do 
    { //wait till child process is over
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

void lsh_loop(void)
{
    char * line;
    char **args;
    int status;

    do 
    {
        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);  // without this a memory leak will occur
        free(args);
    } while(status);
}

int main(int argc, char **argv)
{
  (void) argc;
  (void) argv;
    // Load config files, if any.

    // Run command loop.
    lsh_loop();

    // Perform any shutdown/cleanup.

    return EXIT_SUCCESS;
}