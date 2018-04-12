CC=gcc
CFLAGS=-Wall -D_GNU_SOURCE
DEPS =  stdbool.h divideFile.h macros.h patternSearch.h readArgs.h readDirectory.h signals.h
OBJ = divideFile.o main.o patternSearch.o readArgs.o readDirectory.o signals.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

simgrep: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)


.PHONY: clean
clean:
		rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
