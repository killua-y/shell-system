
#include <stdio.h>
#include <assert.h>
// for strcmp
#include <string.h>
// for exit
#include <stdlib.h>
// execvp
#include <unistd.h>

#include "tokens.h"

#include <sys/types.h>
#include <sys/wait.h>

#define MAXSTRING 20

extern char* strdup(const char*);

void execute(char** tokens);
int howManyString(char **tokens);
int countSemiColon(char** tokens);
int stringContain(char **tokens, char * contain);

// to run the command with seimicolon inside;
void executeSemicolon(char** tokens) {
  int semi = countSemiColon(tokens);
  char ** newtokens = malloc(MAXSTRING * sizeof(char *));
  // int a;
  // for (a = 0; a < MAXSTRING; ++a) {
  //   newtokens[a] = (char *)malloc(256);
  //   }
  int i;
  for (i=0; i < semi; ++i) {
    newtokens[i] = malloc(sizeof(char) * strlen(tokens[i]) + 1);
    strcpy(newtokens[i], tokens[i]);
  }
  execute(newtokens);
  free_tokens(newtokens);

  int len = howManyString(tokens);
  char ** resttokens = malloc(MAXSTRING * sizeof(char *));
  // int b;
  // for (b = 0; b < MAXSTRING; ++b) {
  //   resttokens[b] = (char *)malloc(256);
  //   }
  int j = 0;
  int count = 0;
  for (j = semi + 1; j < len; ++j) {
    resttokens[count] = malloc(sizeof(char) * strlen(tokens[j]) + 1);
    strcpy(resttokens[count], tokens[j]);
    count++;
  }
  execute(resttokens);
  if (!stringContain(tokens, ";")) {
    free_tokens(resttokens);
  } 
}

// count where is the semicolon.
int countSemiColon(char** tokens) {
  int len = howManyString(tokens);
  int i;
  int result = -1;
  for (i=0; i < len; ++i) {
    result += 1;
    if (strcmp(tokens[i], ";") == 0) {
      return result;
    }
  }
  return result;
}

// Input redirection <
void executeInputRe(char ** tokens) {
  int len = howManyString(tokens);
  int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);				
	} else {
		int i; // tokens index
    int j = 0; // args index
		char* args[len + 1];
    for (i = 0; i <= len; ++i) {
      if (i < len) {
        char* v = tokens[i];
        if (strncmp(v, "<", 1) != 0) {
          args[j] = v;
          j++;
          }
        } else {
          args[j] = 0;
          }
      }
      execvp(tokens[0], args);
	}
}

void
check_rv(int rv)
{
	if (rv == -1) {
		perror("fail");
		exit(1);
	}
}

// to run the cd command.
void
cd(char** tokens)
{
	char* place = tokens[1];
	int ret;
	ret = chdir(place);
	if (ret == -1) {
		printf("cd %s: command not found\n", place);
	}
}

// to run the normal command.
void executefinal(char **tokens) {
  if (strcmp(tokens[0], "cd") == 0) {
    cd(tokens);
		return;
  }
  if (strcmp(tokens[0], "exit") == 0) {
    printf("Bye bye.\n");
    exit(0);
  }
  if (strcmp(tokens[0], "help") == 0) {
    printf("cd (change directory)\n");
    printf("source (Execute a script_\n");
    printf("prev (Prints the previous command line and executes it again.)\n");
    exit(0);
  }

  int cpid;
  if ((cpid = fork())) {
    int status;
		waitpid(cpid, &status, 0);
	} else {
    int execvpRun = execvp(tokens[0], tokens);
		if (execvpRun == -1) {
      printf("%s: command not found\n", tokens[0]);
      exit(0);
    }
	}
}

// to check if there are any special symbol in the command and assign diiferent method to run them.
void execute(char **tokens) {
  if (stringContain(tokens, ";")) {
    // printf("semicolon exist\n");
    executeSemicolon(tokens);
  } else if (stringContain(tokens, "<")) {
    executeInputRe(tokens);
  } else if (stringContain(tokens, ">")) {
    executefinal(tokens);
  } else {
    executefinal(tokens);
  }
}

// a helper that checks an array of strings has the given string or not;
int stringContain(char **tokens, char * contain) {
  int len = howManyString(tokens);
  int i;
  for (i = 0; i < len; ++i) {
    if (strcmp(tokens[i], contain) == 0) {
      return 1;
    }
  }
  return 0;
}

// a helper that check how many strins is inside.
int howManyString(char **tokens) {
  char **iter = tokens;
  int result = 0;
  while (*iter != NULL) {
    ++iter;
    result += 1;
  }
  return result;
}

int main(int argc, char **argv) {

  char input[256];

  if (argc == 1) {
    printf("Welcome to mini-shell.\n");
    printf("shell $ ");
    while(fgets(input, 256, stdin) != NULL) {
      if (input[0] != '\n') {
        char **tokens = get_tokens(input);
        execute(tokens);
        free_tokens(tokens);
      }
      printf("shell $ ");
    }
  }
  // else {
	// 	FILE* pFile;
	// 	pFile = fopen(argv[1], "r");
	// 	if (pFile != NULL) {
	// 		while(fgets(input, 256, pFile) != NULL) {
  //       fflush(stdout);
	// 			char **tokens = get_tokens(input);
	// 			execute(tokens);
	// 			free_tokens(tokens);
  //       }
  //     }
	// 	fclose(pFile);
  //   }

  return 0;
}
