CC = g++

COMPILER_FLAGS = -std=c++17 -Wall

INCLUDES = -Iinclude -I.

LINKER_FLAGS = -L.

OUTPUT = bigbeat 

.PHONY: all
all: $(OUTPUT)

SRC = src/*.cpp lexer.yy.c parser.tab.cc

lexer.yy.c: src/lexer.l parser.tab.cc
	flex -o $@ $<

parser.tab.cc: src/parser.yy
	bison -d $<

$(OUTPUT): $(SRC)
	$(CC) $(COMPILER_FLAGS) $(SRC) -o $@  $(INCLUDES) $(LINKER_FLAGS)

.PHONY: clean
clean:
	rm -f parser.tab.cc *.hh lexer.yy.c parser.output $(OUTPUT)
