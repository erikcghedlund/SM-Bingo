CC = gcc
CJSON = third_party/lib/cJSON/cJSON.o
STD = --std=c18
DEBUG = -g

DIRECTORIES = debug third_party debug/bin debug/lib third_party/lib third_party/include third_party/source third_party/lib/cJSON
LIBRARIES = debug/lib/log.o 
EXECUTABLES = debug/bin/server
HEADERS = third_party/include/cJSON/cJSON.h
TARGETS = $(EXECUTABLES) $(HEADERS)

CJSON_VERSION = 1.7.15
CJSON_URL = https://github.com/DaveGamble/cJSON/archive/refs/tags/v$(CJSON_VERSION).tar.gz

all:	$(TARGETS)

clean:
	rm -rf $(DIRECTORIES)

debug/bin: debug
	mkdir $@

debug/lib: debug
	mkdir $@

debug:
	mkdir $@

debug/bin/server: debug/bin source/server.c debug/lib/log.o $(CJSON)
	$(CC) -o $@ source/server.c $(CJSON) $(STD) $(DEBUG) debug/lib/log.o

debug/lib/log.o: debug/lib source/log.c
	$(CC) -c -o $@ source/log.c $(STD) $(DEBUG)

third_party/lib: third_party
	mkdir $@

third_party/lib/cJSON: third_party/lib
	mkdir $@

third_party/include: third_party
	mkdir $@

third_party/source: third_party
	mkdir $@

third_party:
	mkdir $@

third_party/cJSON.tar.gz: third_party
	wget $(CJSON_URL) -O $@

third_party/source/cJSON/cJSON.c: third_party/cJSON.tar.gz third_party/source
	tar -xf  third_party/cJSON.tar.gz cJSON-$(CJSON_VERSION)/cJSON.c
	mv cJSON-$(CJSON_VERSION)/ third_party/source/cJSON

third_party/include/cJSON/cJSON.h: third_party/cJSON.tar.gz third_party/include
	tar -xf  third_party/cJSON.tar.gz cJSON-$(CJSON_VERSION)/cJSON.h
	mv cJSON-$(CJSON_VERSION)/ third_party/include/cJSON

third_party/lib/cJSON/cJSON.o: third_party/lib/cJSON third_party/include/cJSON/cJSON.h third_party/source/cJSON/cJSON.c
	$(CC) -c -o $@ third_party/source/cJSON/cJSON.c $(STD) $(DEBUG) -Ithird_party/include/cJSON/
