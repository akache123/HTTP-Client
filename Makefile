C = clang
#CFLAGS = -Wall -Wextra -Werror -Wpedantic
TARGTDIR = bin/
SRCDIR = src/

all: $(TARGTDIR)myweb

$(TARGTDIR)myweb: $(TARGTDIR)myweb.o
	$(CC) -o $(TARGTDIR)myweb $(TARGTDIR)myweb.o

$(TARGTDIR)myweb.o: $(SRCDIR)myweb.c
	$(CC) $(CFLAGS) -c $(SRCDIR)myweb.c -o $(TARGTDIR)myweb.o

clean:
	rm -f $(TARGTDIR)output.dat $(TARGTDIR)myweb $(TARGTDIR)myweb.o 


