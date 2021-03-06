//------------------------------------------------------------------------------
// LIBRARIES
//------------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "arguments.h"

//------------------------------------------------------------------------------
// DEFINITIONS
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// USER TYPES
//------------------------------------------------------------------------------

// Table used to concentrate all the information related to the comand line arguments
struct ArgCmd {
  const char *cmd;
  size_t cmdLength;
  const char *alias;
  size_t aliasLength;
  int parameter;
  ArgFunction function;
  const char *usage;
};

//------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GLOBAL VARIABLES
//------------------------------------------------------------------------------

static struct ArgCmd *argList = NULL;
static unsigned int argNum = 0;
static ArgFunction usageFunction = NULL, defaultFunction = NULL;
static const char *software = NULL;

//------------------------------------------------------------------------------
// FUNCTIONS
//------------------------------------------------------------------------------

int initArguments(const ArgFunction usageFunc, const ArgFunction defaultFunc) {
  atexit(endArguments);
  usageFunction = usageFunc;
  defaultFunction = defaultFunc;
  return addArgument("--help", "-h", argumentsUsage, "Display this help message.");
}

void endArguments(void) {
  free((struct ArgCmd *) argList);
  argList = NULL;
  argNum = 0;
  usageFunction = NULL;
  defaultFunction = NULL;
  software = NULL;
}

int addArgument(const char *const cmd, const char *const alias, ArgFunction function, const char *const usage) {
  if (cmd && function) {
    struct ArgCmd *newList = (struct ArgCmd *) realloc((void *) argList, (argNum+1)*sizeof(struct ArgCmd));
    if (newList) {
      argList = newList;
      argList[argNum] = (struct ArgCmd) {
        .cmd = cmd,
        .cmdLength = strlen(cmd),
        .alias = alias,
        .aliasLength = strlen(alias),
        .parameter = EXIT_FAILURE,
        .function = function,
        .usage = usage,
      };
      const char *param = strchr(cmd, '=');
      if (param) {
        argList[argNum].parameter = EXIT_SUCCESS;
        argList[argNum].cmdLength = param - cmd;
        param = strchr(alias, '=');
        if (param) {
          argList[argNum].aliasLength = param - alias;
        }
      }
      argNum++;
      return EXIT_SUCCESS;
    }
  }
  return EXIT_FAILURE;
}

static int checkCommand(const char *const arg, const struct ArgCmd *const command) {
  size_t length = strlen(arg);
  if (command->parameter) {
    if (!strncmp(arg, command->cmd, command->cmdLength)) {
      return EXIT_SUCCESS;
    } else if (command->alias && !strncmp(arg, command->alias, command->aliasLength)) {
      return EXIT_SUCCESS;
    }
  } else {
    if (length == command->cmdLength || length == command->aliasLength) {
      if (!strcmp(arg, command->cmd) || (command->alias && !strcmp(arg, command->alias))) {
        return EXIT_SUCCESS;
      }
    }
  }
  return EXIT_FAILURE;
}

static int parseArgument(const char *const arg) {
  char buf[100];
  // make sequential search looking for the argument in the table
  for (unsigned int argCmd = 0; argCmd < argNum; argCmd++) {
    if (!checkCommand(arg, &argList[argCmd])) {
      // found the argument in the list
      return argList[argCmd].function(arg);
    }
  }
  // no argument found, try default action
  if (defaultFunction) {
    if (!defaultFunction(arg)) {
      return EXIT_SUCCESS;
    }
  }
  snprintf (buf, sizeof(buf), "Unknown argument: %s", arg);
  argumentsUsage(buf);
  return EXIT_FAILURE;
}

int parseArguments(const int argc, const char *const argv[]) {
  software = argv[0];
  for (int argIdx = 1; argIdx < argc; argIdx++) {
    if (parseArgument(argv[argIdx])) {
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

static int max(const int x, const int y) {
  return (x > y ? x : y);
}

int argumentsUsage(const char *const msg) {
  unsigned int argCmd;
  int cmdMaxLength = 0, aliasMaxLength = 0;
  if (msg && strlen(msg)) {
    // if is not an help command show message
    if (strcmp(msg, argList[0].cmd) && strcmp(msg, argList[0].alias)) {
      printf("%s\n", msg);
    }
  }
  if (usageFunction) {
    if (usageFunction(software)) {
      return EXIT_FAILURE;
    }
  }

  for (argCmd = 0; argCmd < argNum; argCmd++) {
    cmdMaxLength = max(cmdMaxLength, strlen(argList[argCmd].cmd));
    if (argList[argCmd].alias) {
      aliasMaxLength = max(aliasMaxLength, strlen(argList[argCmd].alias));
    }
  }

  printf("[Options]:\n");
  for (argCmd = 0; argCmd < argNum; argCmd++) {
    printf("\t%-*s", cmdMaxLength, argList[argCmd].cmd);
    if (argList[argCmd].alias) {
      printf(" or %-*s", aliasMaxLength, argList[argCmd].alias);
    }
    if (argList[argCmd].usage) {
      printf(": %s", argList[argCmd].usage);
    }
    printf("\n");
  }
  return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
// END
//------------------------------------------------------------------------------
