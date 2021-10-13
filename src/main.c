//------------------------------------------------------------------------------
// LIBRARIES
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h> // termios, TCSANOW, ECHO, ICANON
#include <unistd.h>  // STDIN_FILENO
#include <errno.h>
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
static struct termios oldt, newt;

//------------------------------------------------------------------------------
// FUNCTIONS
//------------------------------------------------------------------------------

char *getContentFromFile(const char *const filename)
{
    char *buffer = NULL;
    // Get file size
    struct stat st;
    if (!stat(filename, &st))
    {
        const size_t fileSize = st.st_size;
        // Open file
        FILE *file = fopen(filename, "rb");
        if (file)
        {
            // Allocate memory to store the entire file
            buffer = (char *)malloc((fileSize + 1) * sizeof(char));
            if (buffer)
            {
                // Copy the contents of the file to the buffer
                const size_t result = fread(buffer, sizeof(char), fileSize, file);
                buffer[fileSize] = '\0';
                if (ferror(file) || result != fileSize)
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

void restoreTerminalSettings(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void changeTerminalSettings(void)
{
    // gets the parameters of the current terminal
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    /* ICANON normally takes care that one line at a time will be processed
       that means it will return if it sees a "\n" or an EOF or an EOL */
    newt.c_lflag &= ~(ICANON);
    /* Those new settings will be set to STDIN
       TCSANOW tells tcsetattr to change attributes immediately. */
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    atexit(restoreTerminalSettings);
}

//------------------------------------------------------------------------------
// MAIN
//------------------------------------------------------------------------------

int main(const int argc, const char *const argv[])
{
    const char *prog;
    changeTerminalSettings();
    // parse command line arguments
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
        fprintf(stderr, "\n[Error]: Couldn't read the file %s: %s\n", fileName, strerror(errno));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------------------
// END
//------------------------------------------------------------------------------
