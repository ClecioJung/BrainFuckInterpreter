# ----------------------------------------
# Project definitions
# ----------------------------------------

# Name of the project
EXEC = BrainFuckInterpreter

# Folders
ODIR = .obj
DDIR = .deps
SDIR = src
IDIR = src

# .c files
SRCS = $(wildcard $(SDIR)/*.c $(SDIR)/*.cpp)

# .h files
INCS = $(wildcard $(IDIR)/*.h $(IDIR)/*.hpp)

# Dependency files (auto generated)
DEPS = $(patsubst %,%.d,$(basename $(subst $(SDIR),$(DDIR),$(SRCS))))

# Object files
OBJS = $(patsubst %,%.o,$(basename $(subst $(SDIR),$(ODIR),$(SRCS))))

# ----------------------------------------
# Compiler and linker definitions
# ----------------------------------------

 # Compiler and linker
#CC = gcc
#CXX = g++

# Libraries
INCLUDES =

# Flags for compiler
CFLAGS = -W -Wall -Wextra -pedantic -std=c99 -O2
CXXFLAGS = -W -Wall -Wextra -pedantic -O2
DEPFLAGS = -MT $@ -MMD -MP -MF $(DDIR)/$*.Td

# Compiler macros
COMPILE.CC = $(CC) $(CFLAGS)
COMPILE.CXX = $(CXX) $(CXXFLAGS)
POSTCOMPILE = mv -f $(DDIR)/$*.Td $(DDIR)/$*.d && touch $@

# ----------------------------------------
# Fomating macros
# ----------------------------------------

BOLD = \033[1m
NORMAL = \033[0m
RED = \033[0;31m
GREEN = \033[0;32m

# ----------------------------------------
# Compilation and linking rules
# ----------------------------------------

all: $(EXEC)

$(EXEC): $(OBJS)
	@ echo "${GREEN}Building binary: ${BOLD}$@${GREEN} using dependencies: ${BOLD}$^${NORMAL}"
	$(COMPILE.CC) $(filter %.s %.o,$^) -o $@ $(INCLUDES)
	@ touch $@

$(ODIR)/%.o : $(SDIR)/%.c
$(ODIR)/%.o : $(SDIR)/%.c $(DDIR)/%.d | $(DDIR) $(ODIR)
	@ echo "${GREEN}Building target: ${BOLD}$@${GREEN}, using dependencies: ${BOLD}$^${NORMAL}"
	$(COMPILE.CC) $(DEPFLAGS) -c $(filter %.c %.s %.o,$^) -o $@
	@ $(POSTCOMPILE)

$(ODIR)/%.o : $(SDIR)/%.cpp
$(ODIR)/%.o : $(SDIR)/%.cpp $(DDIR)/%.d | $(DDIR) $(ODIR)
		@ echo "${GREEN}Building target: ${BOLD}$@${GREEN}, using dependencies: ${BOLD}$^${NORMAL}"
		$(COMPILE.CXX) $(DEPFLAGS) -c $(filter %.cpp %.s %.o,$^) -o $@
		@ $(POSTCOMPILE)

$(DDIR)/%.d: ;
.PRECIOUS: $(DDIR)/%.d

-include $(DEPS)

# ----------------------------------------
# Script rules
# ----------------------------------------

$(DDIR):
	mkdir -p $@

$(ODIR):
	mkdir -p $@

clean:
	rm -fr $(ODIR)/ $(DDIR)/ $(EXEC) $(SDIR)/*.gch *~ env.mk

remade: clean all

.PHONY: all clean remade

# ----------------------------------------
