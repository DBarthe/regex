
# compilation
CXX ?= clang++
CXXFLAGS = $(STD) $(INCLUDES) $(OFLAGS) $(DBGFLAGS) $(DEFINES) $(CXXSPECIAL)

STD = -std=c++11
OFLAGS =
DBGFLAGS =
DEFINES =
CXXSPECIAL =

LDFLAGS =
LDFLAGS_SHARED = -shared $(LDFLAGS)

DEBUG ?= yes
OPTI ?= yes
ifeq ($(DEBUG),yes)
	DBGFLAGS = -ggdb3
	DEFINES += -DDEBUG
else
	DEFINES += -DNDEBUG
	ifeq ($(OPTI),yes)
		OFLAGS=-O3
	endif
endif

# targets
LIB_SHARED_TARGET = libregex.so
LIB_STATIC_TARGET = libregex.a
LIB_TARGET = $(LIB_STATIC_TARGET) $(LIB_SHARED_TARGET)

PROGRAM_TARGET = regex

TEST_TARGET = test-regex

ALL_TARGET = $(LIB_TARGET) $(PROGRAM_TARGET) $(TEST_TARGET)

# sources
LIB_SRCDIR = src/lib
PROGRAM_SRCDIR = src/program
TEST_SRCDIR = src/test

LIB_SRC = $(wildcard $(LIB_SRCDIR)/*.cpp)
PROGRAM_SRC = $(wildcard $(PROGRAM_SRCDIR)/*.cpp)
TEST_SRC = $(wildcard $(TEST_SRCDIR)/*.cpp)

# includes
INCDIRS = $(LIB_SRCDIR)
INCLUDES = $(foreach dir,$(INCDIRS),-I $(INCDIRS))

# object files
LIB_OBJ = $(LIB_SRC:.cpp=.o)
_PROGRAM_OBJ = $(PROGRAM_SRC:.cpp=.o)
PROGRAM_OBJ = $(LIB_OBJ) $(_PROGRAM_OBJ)
_TEST_OBJ = $(TEST_SRC:.cpp=.o)
TEST_OBJ = $(LIB_OBJ) $(_TEST_OBJ)
ALL_OBJ = $(LIB_OBJ) $(_PROGRAM_OBJ) $(_TEST_OBJ)

# commandes
AR = ar -rc


# rules
all: lib program test

lib: CXXSPECIAL = -fPIC
lib: $(LIB_TARGET)

$(LIB_SHARED_TARGET): $(LIB_OBJ)
	$(CXX) $(CXXFLAGS) -o $(LIB_SHARED_TARGET) $(LIB_OBJ) $(LDFLAGS_SHARED)

$(LIB_STATIC_TARGET): $(LIB_OBJ)
	$(AR) $(LIB_STATIC_TARGET) $(LIB_OBJ)

program: $(PROGRAM_TARGET)

$(PROGRAM_TARGET): CXXSPECIAL=	
$(PROGRAM_TARGET): $(PROGRAM_OBJ)
	$(CXX) $(CXXFLAGS) -o $(PROGRAM_TARGET) $(PROGRAM_OBJ) $(LDFLAGS)

test: $(TEST_TARGET)

$(TEST_TARGET): CXXSPECIAL=	
$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJ) $(LDFLAGS)

clean:
	$(RM) $(ALL_OBJ)
	$(RM) $(ALL_TARGET)

rebuild: clean all
