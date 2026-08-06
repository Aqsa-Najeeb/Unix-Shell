#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"


/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {  //array of strings likw char**
  "cd",
  "help",
  "exit"
};

/*
builtin_func[0] = lsh_cd
builtin_func[1] = lsh_help
builtin_func[2] = lsh_exit
*/

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/
int lsh_cd(char **args)
{
  if (args[1] == NULL) 
  {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } 
  else
   {
    if (chdir(args[1]) != 0)  //-1 is error
    {

      perror("lsh");
    }
  }
  return 1;
}

int lsh_help(char **args)
{
  int i;
  printf("Aqsa Najeeb's Unix Shell\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) 
  {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int lsh_exit(char **args)
{
  return 0;
}

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

int lsh_execute(char **args)
{

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (int i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
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