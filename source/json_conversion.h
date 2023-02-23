#ifndef JSON_CONVERSIONH
#define JSON_CONVERSIONH
#include <cjson/cJSON.h>
#include "game_structures.h"

card json_card_to_struct(cJSON * j_card, int id);

cJSON * json_from_file(char * filepath);

#endif
