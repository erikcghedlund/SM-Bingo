#include <stdio.h>
#include <cjson/cJSON.h>
#include <stdlib.h>
#include <rxi/log.h>

#ifdef _WIN32
#include <Fileapi.h>
#else
#include <sys/stat.h>
#endif

#include "game_structures.h"

card json_card_to_struct(cJSON * j_card, int id) {
    return (card) {id, cJSON_GetObjectItem(j_card, "text")->valuestring, cJSON_GetObjectItem(j_card, "desc")->valuestring, cJSON_GetObjectItem(j_card, "author")->valuestring};
}


cJSON * json_from_file(char * filepath) {
#ifdef _WIN32
    //TODO
#else
    struct stat st;
    if (stat(filepath, &st)) {
        log_error("Failed to stat file %s", filepath);
        return NULL;
    }
    unsigned long size = st.st_size;
#endif
    FILE * f = fopen(filepath, "r");
    if (f == NULL) {
        log_fatal("Failed to open file \"%s\"");
        return NULL;
    }
    char * filebuf = (char *)malloc(sizeof(char) * size);
    if (filebuf == NULL) {
        log_fatal("Failed to allocate file buffer \"%s\"");
        return NULL;
    }
    fread(filebuf, sizeof(char), size, f);
    cJSON * ret_val =  cJSON_ParseWithLength(filebuf, size);
    free(filebuf);
    return ret_val;
}
