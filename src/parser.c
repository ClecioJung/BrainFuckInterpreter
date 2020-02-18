//------------------------------------------------------------------------------
// LIBRARIES
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

//------------------------------------------------------------------------------
// DEFINITIONS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// USER TYPES
//------------------------------------------------------------------------------

struct Environment {
  const char *prog;
  unsigned int line;
  unsigned int col;
  unsigned int mIndex;
  unsigned int maxIndex;
  int loop;
  int *mem;
};

typedef int (*InstFunction)(struct Environment *);

struct Instruction {
  char cmd;
  InstFunction function;
  const char *description;
};

//------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GLOBAL VARIABLES
//------------------------------------------------------------------------------

static struct Instruction *instList = NULL;
static unsigned int instNum = 0;

unsigned int memorySize = 30000;
int debugMode = 0;

//------------------------------------------------------------------------------
// FUNCTIONS
//------------------------------------------------------------------------------

static void nextProg(struct Environment *const env) {
  env->prog++;
  env->col++;
  if (*env->prog == '\n') {
    env->line++;
    env->col = 0;
  }
}

static void previousProg(struct Environment *const env) {
  env->prog--;
  env->col--;
  if (*env->prog == '\n') {
    env->line--;
    env->col = 0;
  }
}

static void codeError(struct Environment *const env, const char *const msg) {
  fprintf(stderr, "\n[Error in line %d, column %d]: %s\n", env->line, env->col, msg);
  for (unsigned int columns = env->col; columns > 1; columns--) {
    env->prog--;
  }
  int lineLength = strchr(env->prog, '\n') - env->prog;
  fprintf(stderr, "%.*s\n", lineLength, env->prog);
  for (unsigned int columns = env->col; columns > 1; columns--) {
    putc(' ', stderr);
  }
  fprintf(stderr, "^\n");
}

static int incrementIndex(struct Environment *env) {
  env->mIndex++;
  if (env->mIndex >= memorySize) {
    codeError(env, "Invalid pointer address");
    return EXIT_FAILURE;
  }
  env->maxIndex = env->maxIndex > env->mIndex ? env->maxIndex : env->mIndex;
  return EXIT_SUCCESS;
}

static int decrementIndex(struct Environment *env) {
  if (env->mIndex == 0) {
    codeError(env, "Invalid pointer address");
    return EXIT_FAILURE;
  }
  env->mIndex--;
  return EXIT_SUCCESS;
}

static int incrementByte(struct Environment *env) {
  env->mem[env->mIndex]++;
  return EXIT_SUCCESS;
}

static int decrementByte(struct Environment *env) {
  env->mem[env->mIndex]--;
  return EXIT_SUCCESS;
}

static int outputByte(struct Environment *env) {
  putchar(env->mem[env->mIndex]);
  return EXIT_SUCCESS;
}

static int getByte(struct Environment *env) {
  printf("\nInsert a key:\n");
  env->mem[env->mIndex] = getchar();
  return EXIT_SUCCESS;
}

static int beginLoop(struct Environment *env) {
  if (env->mem[env->mIndex]) {
    env->loop++;
  } else {
    // find end of loop
    int loop = 1;
    do {
      nextProg(env);
      switch (*env->prog) {
        case '[':
        loop++;
        break;
        case ']':
        loop--;
        break;
        case '\0':
        env->prog--;
        codeError(env, "Missing end of the loop ']'");
        return EXIT_FAILURE;
      }
    } while (loop);
  }
  return EXIT_SUCCESS;
}

static int endLoop(struct Environment *env) {
  if (env->mem[env->mIndex]) {
    // return to the begin of the loop
    int loop = 1;
    do {
      previousProg(env);
      switch (*env->prog) {
        case '[':
        loop--;
        break;
        case ']':
        loop++;
        break;
      }
    } while (loop);
  } else {
    env->loop--;
    if (env->loop < 0) {
      codeError(env, "Incorrect loop declaration");
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

static int printCell(struct Environment *env) {
  if (debugMode) {
    printf("\ncell %d: %d\n", env->mIndex, env->mem[env->mIndex]);
  }
  return EXIT_SUCCESS;
}

static int printAllCells(struct Environment *env) {
  if (debugMode) {
    printf("\n");
    for (unsigned int index = 0; index <= env->maxIndex; index++) {
      printf("cell %d: %d\n", index, env->mem[index]);
    }
  }
  return EXIT_SUCCESS;
}

static int addInstruction(const char cmd, InstFunction function, const char *const description) {
  if (cmd && function) {
    struct Instruction *newList = (struct Instruction *) realloc((void *) instList, (instNum+1)*sizeof(struct Instruction));
    if (newList) {
      instList = newList;
      instList[instNum] = (struct Instruction) {
        .cmd = cmd,
        .function = function,
        .description = description,
      };
      instNum++;
      return EXIT_SUCCESS;
    }
  }
  return EXIT_FAILURE;
}

void endBrainFuck(void) {
  free((struct Instruction *) instList);
  instList = NULL;
  instNum = 0;
}

void initBrainFuck(void) {
  atexit(endBrainFuck);
  addInstruction('>', incrementIndex, "Increment the data pointer.");
  addInstruction('<', decrementIndex, "Decrement the data pointer.");
  addInstruction('+', incrementByte, "Increment the byte at the data pointer.");
  addInstruction('-', decrementByte, "Decrement the byte at the data pointer.");
  addInstruction('.', outputByte, "Output the byte at the data pointer.");
  addInstruction(',', getByte, "Get one byte from the input and stores it at the data pointer.");
  addInstruction('[', beginLoop, "If the byte at the data pointer is non zero, then loop the instructions inside the brackets [ ].");
  addInstruction(']', endLoop, "End of loop.");
  addInstruction('#', printCell, "Print current cell and its value (Debug mode only).");
  addInstruction('@', printAllCells, "Print all used cells and its values (Debug mode only).");
}

void printInstructions(void) {
  printf("[Language Instructions]\n");
  for (unsigned int instIdx = 0; instIdx < instNum; instIdx++) {
    printf("   %c   %s\n", instList[instIdx].cmd, instList[instIdx].description);
  }
}

void brainFuck(const char *const code) {
  // Initiates program variables
  struct Environment *env = (struct Environment *) malloc(sizeof(struct Environment));

  *env = (struct Environment) {
    .mIndex = 0,
    .maxIndex = 0,
    .loop = 0,
    .line = 1,
    .col = 1,
    .prog = code,
    .mem = NULL,
  };

  // Initiates program memory
  env->mem = (int *) malloc(memorySize*sizeof(int));
  memset(env->mem, 0, memorySize*sizeof(int));

  for (; *env->prog; nextProg(env)) {
    // sequential search teh instruction table
    for (unsigned int instIdx = 0; instIdx < instNum; instIdx++) {
      if (*env->prog == instList[instIdx].cmd) {
        if (!instList[instIdx].function(env)) {
          break;
        } else {
          // something wrong appened
          goto exitBrainFuck;
        }
      }
    }
    // didn't found valid instruction in the table
    // so ignores
  }

  if (env->loop) {
    codeError(env, "Incorrect loop declaration");
  }

  exitBrainFuck:
  free((void *) env->mem);
  free((void *) env);
}

//------------------------------------------------------------------------------
// END
//------------------------------------------------------------------------------
