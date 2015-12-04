default: all

#CC = gcc -lm -W -Wall -pedantic -Wmissing-prototypes \
#	-Wredundant-decls -Wnested-externs -Wdisabled-optimization \
#	-ansi -g -fexceptions -Wno-parentheses -Wshadow -Wcast-align \
#	-Winline -Wstrict-prototypes -Wno-unknown-pragmas

# for blas:
# CC = gcc -lm -O3 -lpthread

# for speed:
CC = gcc -lm -O3

OBJ = asa_cg.o

all: driver

$(OBJ): $(INCLUDE)

driver: $(OBJ) driver.c
	$(CC) -o driver driver.c $(OBJ) $(LINK)

clean:
	rm *.o

purge:
	rm *.o driver
