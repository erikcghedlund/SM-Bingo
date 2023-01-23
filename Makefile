CC = gcc
CJSON = third_party/lib/cjson/cJSON.o
STD = --std=c18
DEBUG = -g -wall

DIRECTORIES = debug third_party
LIBRARIES = debug/lib/log.o 
EXECUTABLES = debug/bin/server
HEADERS = third_party/include/cjson/cJSON.h
TARGETS = $(EXECUTABLES)

CJSON_VERSION = 1.7.15
CJSON_URL = https://github.com/DaveGamble/cJSON/archive/refs/tags/v$(CJSON_VERSION).tar.gz

all:	$(TARGETS)

clean:
	rm -rf $(DIRECTORIES)

debug/bin:
	mkdir $@ --parent

debug/lib:
	mkdir $@ --parent

debug/bin/server: debug/bin source/server.c debug/lib/log.o $(CJSON)
	$(CC) -o $@ source/server.c $(CJSON) $(STD) $(DEBUG) debug/lib/log.o -Ithird_party/include

debug/lib/log.o: debug/lib source/log.c
	$(CC) -c -o $@ source/log.c $(STD) $(DEBUG)

third_party/lib/cjson:
	mkdir $@ --parent

third_party/include: third_party
	mkdir $@ --parent

third_party/source: third_party
	mkdir $@ --parent

third_party:
	mkdir $@ --parent

third_party/cJSON.tar.gz: third_party
	wget $(CJSON_URL) --output-document $@ --no-clobber --quiet || :

third_party/source/cjson/cJSON.c: third_party/cJSON.tar.gz third_party/source
	tar -xf  third_party/cJSON.tar.gz cJSON-$(CJSON_VERSION)/cJSON.c
	mv cJSON-$(CJSON_VERSION)/ -T third_party/source/cjson 2> /dev/null || rm cJSON-$(CJSON_VERSION)/ -rf

third_party/include/cjson/cJSON.h: third_party/cJSON.tar.gz third_party/include
	tar -xf  third_party/cJSON.tar.gz cJSON-$(CJSON_VERSION)/cJSON.h
	mv cJSON-$(CJSON_VERSION)/ -T third_party/include/cjson 2> /dev/null || rm cJSON-$(CJSON_VERSION)/ -rf

third_party/lib/cjson/cJSON.o: third_party/lib/cjson third_party/include/cjson/cJSON.h third_party/source/cjson/cJSON.c
	$(CC) -c -o $@ third_party/source/cjson/cJSON.c $(STD) $(DEBUG) -Ithird_party/include/cjson
