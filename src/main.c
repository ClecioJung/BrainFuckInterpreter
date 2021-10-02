//------------------------------------------------------------------------------
// LIBRARIES
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "arguments.h"
#include "parser.h"

//------------------------------------------------------------------------------
// USER TYPES
//------------------------------------------------------------------------------

enum ActionToBeTaken
{
    acNone = 0,
    acParser,
};

//------------------------------------------------------------------------------
// GLOBAL VARIABLES
//------------------------------------------------------------------------------

static const char *const version = "1.0.0";
static const char *fileName = NULL;
static enum ActionToBeTaken action = acParser;

//------------------------------------------------------------------------------
// FUNCTIONS
//------------------------------------------------------------------------------

char *getContentFromFile(const char *const name)
{
    char *buffer = NULL;
    // Get file size
    struct stat st;
    if (!stat(name, &st))
    {
        const size_t fileSize = st.st_size;
        // Open file
        FILE *file = fopen(name, "rb");
        if (file)
        {
            // Allocate memory to store the entire file
            buffer = (char *)malloc((fileSize + 1) * sizeof(char));
            if (buffer)
            {
                // Copy the contents of the file to the buffer
                const size_t result = fread(buffer, sizeof(char), fileSize, file);
                buffer[fileSize] = '\0';
                if (result != fileSize)
                {
                    // Reading file error, free dinamically allocated memory
                    free(buffer);
                    buffer = NULL;
                }
            }
            fclose(file);
        }
    }
    return buffer;
}

static int printUsage(const char *const software)
{
    printf("[Usage] %s [script.b] [Options]\n", software);
    action = acNone;
    return EXIT_SUCCESS;
}

static int getFileName(const char *const arg)
{
    if (fileName)
    {
        return EXIT_FAILURE;
    }
    else
    {
        fileName = arg;
    }
    return EXIT_SUCCESS;
}

static int printVersion(const char *const arg)
{
    (void)arg;
    printf("[Version] %s\n", version);
    action = acNone;
    return EXIT_SUCCESS;
}

static int debugModeOn(const char *const arg)
{
    (void)arg;
    printf("Debug mode on.\n");
    debugMode = 1;
    return EXIT_SUCCESS;
}

static int printLanguageInstructions(const char *const arg)
{
    (void)arg;
    printInstructions();
    action = acNone;
    return EXIT_SUCCESS;
}

static int changeMemorySize(const char *const arg)
{
    unsigned int size;
    const char *ptr = strchr(arg, '=');
    size = atoi(++ptr);
    if (size > 10)
    {
        printf("Program buffer size changed to %u elements.\n", size);
        memorySize = size;
    }
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
// MAIN
//------------------------------------------------------------------------------

int main(const int argc, const char *const argv[])
{
    const char *prog;
    initBrainFuck();
    initArguments(printUsage, getFileName);
    addArgument("--version", "-v", printVersion, "Display the software version.");
    addArgument("--debug", "-d", debugModeOn, "Activate the debug mode (allows the use of the commands # and @).");
    addArgument("--language", "-l", printLanguageInstructions, "Displays language instructions.");
    addArgument("--memory=%d", "-m=%d", changeMemorySize, "Change program buffer size.");
    parseArguments(argc, argv);

    if (action == acNone)
    {
        return EXIT_SUCCESS;
    }
    if (!fileName)
    {
        argumentsUsage("No file specified");
        return EXIT_FAILURE;
    }
    prog = getContentFromFile(fileName);
    if (prog)
    {
        brainFuck(prog);
        free((char *)prog);
    }
    else
    {
        fprintf(stderr, "\n[Error]: Couldn't open the file %s\n", fileName);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
// END
//------------------------------------------------------------------------------
