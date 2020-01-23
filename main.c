#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define VERSION     "1.0.0"
#define MEMORY_SIZE 30000

int mem[MEMORY_SIZE];
int mIndex = 0;
int maxIndex = 0;
int programLoop = -1;
int debugMode = 0;

void printUsage(const char *const software)
{
    printf("[Usage] %s [Options] [script.b]\n", software);
    printf("\n[Options]:\n");
    printf("\t-d\tActivate the debug mode (allows the use of the commands # and @)\n");
	printf("\t-h\tDisplays software usage instructions\n");
	printf("\t-v\tDisplays software version\n");
	printf("\t-l\tDisplays language instructions\n");
}

void printVersion(void)
{
    printf("[Version] " VERSION "\n");
}

void printLanguageInstructions(void)
{
    printf("[Language Instructions]\n");
    printf("\t>\tIncrement the data pointer\n");
    printf("\t<\tDecrement the data pointer\n");
    printf("\t+\tIncrement the byte at the data pointer\n");
    printf("\t-\tDecrement the byte at the data pointer\n");
	printf("\t.\tOutput the byte at the data pointer\n");
	printf("\t,\tGet one byte from the input and stores it at the data pointer\n");
	printf("\t[\tIf the byte at the data pointer is non zero, then loop the instructions inside the brackets [ ]\n");
	printf("\t]\tEnd of loop\n");
	printf("\t#\tPrint current cell and its value (Debug mode only)\n");
	printf("\t@\tPrint all used cells and its values (Debug mode only)\n");
}

char *getContentFromFile(const char *const name)
{
    char *buffer = NULL;

    // Get file size
    struct stat st;
    if(!stat(name, &st)) {
        const size_t fileSize = st.st_size;

        // Open file
        FILE *file = fopen(name, "rb");
        if (file) {
            // Allocate memory to store the entire file
            buffer = (char *) malloc((fileSize+1)*sizeof(char));
            if (buffer) {
                // Copy the contents of the file to the buffer
                const size_t result = fread(buffer, sizeof(char), fileSize, file);
                buffer[fileSize] = '\0';
                if (result != fileSize) {
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

void codeError(const char *prog, char *msg, unsigned int line, unsigned int col)
{
    fprintf(stderr, "\n[Error in line %d, column %d]: %s\n", line, col, msg);
    for (unsigned int columns = col; columns > 1; columns--) {
        prog--;
    }
    int lineLength = strchr(prog, '\n') - prog;
    fprintf(stderr, "%.*s\n", lineLength, prog);
    for (unsigned int columns = col; columns > 1; columns--) {
        putc(' ', stderr);
    }
    fprintf(stderr, "^\n");
}

int brainFuck(const char *prog, unsigned int line, unsigned int col)
{
    programLoop++;

    for (; *prog; prog++, col++) {
        switch (*prog) {
        case '>':
            mIndex++;
            maxIndex = maxIndex > mIndex? maxIndex : mIndex;
            if (mIndex >= MEMORY_SIZE) {
                codeError(prog, "Invalid pointer address", line, col);
                return EXIT_FAILURE;
            }
            break;
        case '<':
            mIndex--;
            if (mIndex < 0) {
                codeError(prog, "Invalid pointer address", line, col);
                return EXIT_FAILURE;
            }
            break;
        case '+':
            mem[mIndex]++;
            break;
        case '-':
            mem[mIndex]--;
            break;
        case '.':
            putchar(mem[mIndex]);
            break;
        case ',':
            printf("\nInsert a key:\n");
            mem[mIndex] = getchar();
            break;
        case '[':
            prog++;
            col++;
            while (mem[mIndex]) {
                if (brainFuck(prog, line, col) == EXIT_FAILURE) {
                    return EXIT_FAILURE;
                }
            }
            // Find the end of the loop
            for (int loop = 1; *prog; prog++, col++) {
                if (*prog == '[') {
                    loop++;
                } else if (*prog == ']') {
                    loop--;
                    if (!loop) {
                        break;
                    }
                } else if (*prog == '\n') {
                    line++;
                    col = 0;
                }
            }
            // Didn't find the end of the loop
            if (!*prog) {
                codeError(prog, "Missing end of the loop ']'", line, col);
                return EXIT_FAILURE;
            }
            break;
        case ']':
            programLoop--;
            if (programLoop < 0) {
                codeError(prog, "Incorrect loop declaration", line, col);
                return EXIT_FAILURE;
            }
            // Break the loop
            return EXIT_SUCCESS;
            break;
        case '\n':
            line++;
            col = 0;
            break;
        case '#':
            if (debugMode) {
                printf("\ncell %d: %d\n", mIndex, mem[mIndex]);
            }
            break;
        case '@':
            if (debugMode) {
                printf("\n");
                for (int index = 0; index <= maxIndex; index++) {
                    printf("cell %d: %d\n", index, mem[index]);
                }
            }
            break;
        }
    }
    programLoop--;
    return EXIT_SUCCESS;
}

int main(const int argc, const char *const argv[])
{
    const char *prog;
    const char *fileName = NULL;
    int returnValue;

    // Clears memory
    memset(mem, 0, MEMORY_SIZE*sizeof(int));

    for (int argIndex = 1; argIndex < argc; argIndex++) {
        if (argv[argIndex][0] == '-') {
            switch (argv[argIndex][1]) {
            case 'd':
                printf("Debug mode on\n");
                debugMode = 1;
                break;
            case 'h':
                printUsage(argv[0]);
                return EXIT_SUCCESS;
            case 'v':
                printVersion();
                return EXIT_SUCCESS;
            case 'l':
                printLanguageInstructions();
                return EXIT_SUCCESS;
            }
        } else {
            if (fileName) { // File name alreafy informed
                fileName = NULL;
                break;
            } else {
                fileName = argv[argIndex];
            }
        }
    }

    if (!fileName) {
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    prog = getContentFromFile(fileName);
    if (prog) {
        returnValue = brainFuck(prog, 1, 1);
        free((char *) prog);
    } else {
        fprintf(stderr, "\n[Error]: Couldn't open the file %s\n", fileName);
        return EXIT_FAILURE;
    }
    return returnValue;
}
