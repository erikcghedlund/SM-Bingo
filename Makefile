CC = gcc
CJSON = /lib/libcjson.so.1.7.15
STD = --std=c18
DEBUG = -g

TARGETS = bin bin/server

all:	$(TARGETS)

bin:
	mkdir bin

bin/server: bin source/server.c
	$(CC) -o $@ source/server.c $(CJSON) $(STD) $(DEBUG)
