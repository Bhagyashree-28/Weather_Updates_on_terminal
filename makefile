# you can put include files in this directory
IDIR =./include
CC=gcc
CFLAGS=-I$(IDIR)

# directory where object files will be created
ODIR=obj

# create string with ../include/<header filename> in our case ../include/myheader.h
_DEPS = myheader.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# create string with obj/<object filename> in our case obj/response.o and parse.o  (obj folder will be created under current directory)
_LIBOBJ = response.o parse.o
LIBOBJ = $(patsubst %,$(ODIR)/%,$(_LIBOBJ))
LIBSRC = response.c parse.c
_APPOBJ = main.o
APPOBJ = $(patsubst %,$(ODIR)/%,$(_APPOBJ))

all: connect output.out

#create executable file output.out
output.out: $(APPOBJ) $(LIBOBJ)
	$(CC) -Wall -o $@ $^ $(CFLAGS) -L/usr/lib -lssl -lcrypto -ljson-c
#create object files
$(APPOBJ): main.c $(DEPS)
	$(CC) -c -Wall -o $@ $< $(CFLAGS) -L/usr/lib -lssl -lcrypto -ljson-c

$(ODIR)/%.o: %.c
	$(CC) -c -Wall -o $@ $< -L/usr/lib -lssl -lcrypto -ljson-c
#install libraries in order to run program
connect:
	sudo apt-get install libssl-dev
	openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout mycert.pem -out mycert.pem
	sudo apt install libjson-c-dev
	
	
.PHONY: clean

# removes object files and temporary files created with ~ at the end of file name in current include and object directory
clean: 
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm -f output.out
	
