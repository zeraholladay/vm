.PHONY: all src clean

# Compilers and flags
CC := gcc
CFLAGS := -Iinclude -Wall
LEX := flex
YACC := bison
GPERF := gperf

LEX_SRC = src/lexer.l
YACC_SRC = src/parser.y
GPERF_SCR = src/prim_op.gperf

LEX_OUT = src/lexer.c
YACC_OUT = src/parser.c
YACC_HEADER = include/parser.h
GPERF_OUT = src/prim_op.c

REPL = bin/repl

OS := $(shell uname)

ifeq ($(OS), Darwin)
	LIBS := $(shell pkg-config --libs glib-2.0) -ll
else
    LIBS = -lfl
endif

all: $(REPL)

$(REPL): $(YACC_OUT) $(LEX_OUT) $(GPERF_OUT) src
	$(CC) $(CFLAGS) -o $(REPL) bin/*.o $(FLEX_LIB) $(LIBS)

$(LEX_OUT): $(LEX_SRC) $(YACC_HEADER)
	$(LEX) -o $(LEX_OUT) $(LEX_SRC)

$(YACC_OUT): $(YACC_SRC)
	$(YACC) -o $(YACC_OUT) --defines=$(YACC_HEADER) $(YACC_SRC)

$(GPERF_OUT): $(GPERF_SCR) $(YACC_HEADER)
	$(GPERF) $(GPERF_SCR) --output-file=$(GPERF_OUT)

src:
	@$(MAKE) -C src/

lint: clean
	clang-format -i src/*.c include/*.h
	cppcheck src/*.c include/*.h

clean:
	@rm -f $(REPL) $(LEX_OUT) $(YACC_OUT) $(YACC_HEADER) $(GPERF_OUT) $(OBJS)
	@$(MAKE) -C src/ clean
