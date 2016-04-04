CFLAGS="-fPIC -shared"
LDFLAGS="-ldl -shared"

all: sshdoor.so

sshdoor.so: sshdoor.c
	$(CC) -Wall -shared -fPIC -o $@ $^ -ldl
