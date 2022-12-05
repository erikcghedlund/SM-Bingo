#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <cjson/cJSON.h>

#ifdef _WIN32
#include <Fileapi.h>
#else
#include <sys/stat.h>
#endif

#define BUFFERSIZE 32
#define DESCSIZE 128
#define DEFAULT_CARD_FILE "./settings/cards.json"

typedef struct card {
    int id;
    char * text, * desc, * author, **tags;
} card;

typedef struct player_card {
    card * card;
    char flipped;
} player_card;

typedef struct available_cards {
    int size;
    card * cards;
} available_cards;

typedef struct board {
    int size;
    player_card ** cards;
} board;

typedef struct player {
    int id;
    char * name;
    board board;
} player;

typedef struct game {
    int id;
    player * players;
} game;

int max(int x, int y) {
    return (((x > y) * x) + ((x <= y) * y));
}

void swap(int * x, int * y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

int * random_numbers(int range, int nums) {
    int * choises = (int *)malloc(sizeof(int)*range), * ret_val = (int *)malloc(sizeof(int)*nums);
    for (int i = range - 1; i > -1; i--)
        choises[i] = i;
    for (int i = 0; i < nums; i++) {
        int index = rand()%range;
        ret_val[i] = choises[index];
        swap(choises + index, choises + range - i - 1);
    }
    free(choises);
    return ret_val;
}

char * timestr(char * buf) {
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);
    snprintf(buf, BUFFERSIZE, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    return buf;
}

int log_error(const char * fmt, ...) {
    char buf[BUFFERSIZE];
    int n = fprintf(stderr,  "[%s]: ", timestr(buf));
    va_list argptr;
    va_start(argptr, fmt);
    n += vfprintf(stderr, fmt, argptr);
    va_end(argptr);
    return n;
}

// Tags not implented
card add_card(int id, char * text, char * desc, char * author, ...) {
    int textn = strlen(text),
        descn = strlen(desc),
        authorn = strlen(author);
#define str_alloc(n) (char *)malloc(sizeof(char) * (n + 1))
    card ret_val = {id, str_alloc(textn), str_alloc(descn), str_alloc(authorn), NULL};
#undef str_alloc
    strncpy(ret_val.text, text, textn + 1);
    strncpy(ret_val.desc, desc, descn + 1);
    strncpy(ret_val.author, author, authorn + 1);
    return ret_val;
}

void destroy_card(const card * card) {
    free(card->text);
    free(card->desc);
    free(card->author);
}

card json_card_to_struct(cJSON * j_card, int id) {
    return (card) {id, cJSON_GetObjectItem(j_card, "text")->valuestring, cJSON_GetObjectItem(j_card, "desc")->valuestring, cJSON_GetObjectItem(j_card, "author")->valuestring};
}

available_cards retrieve_cards(cJSON * j_cards) {
    if (! cJSON_IsArray(j_cards)) return (available_cards) {0, NULL};
    int arr_size = cJSON_GetArraySize(j_cards);
    available_cards ret_val = {arr_size, (card *)malloc(sizeof(card)*arr_size)};
    for (int i = 0; i < arr_size; i++)
        json_card_to_struct(cJSON_GetArrayItem(j_cards, i), i);
    return ret_val;
}

cJSON * json_from_file(char * filepath) {
#ifdef _WIN32
    //TODO
#else
    struct stat st;
    if (stat(filepath, &st)) {
        log_error("Failed to stat file %s\n", filepath);
        return NULL;
    }
    unsigned long size = st.st_size;
#endif
    FILE * f = fopen(filepath, "r");
    if (f == NULL) {
        log_error("Failed to open file \"%s\"\n");
        return NULL;
    }
    char * filebuf = (char *)malloc(sizeof(char) * size);
    if (filebuf == NULL) {
        log_error("Failed to allocate file buffer \"%s\"\n");
        return NULL;
    }
    fread(filebuf, sizeof(char), size, f);
    cJSON * ret_val =  cJSON_ParseWithLength(filebuf, size);
    free(filebuf);
    return ret_val;
}

available_cards read_card_file(char * filepath) {
    cJSON * json = json_from_file(filepath);
    available_cards ret_val = retrieve_cards(cJSON_GetObjectItem(json, "cards"));
    cJSON_Delete(json);
    return ret_val;
}

board add_board(int gridsize, const available_cards * cards){
    board ret_val = {gridsize, (player_card **)malloc(sizeof(player_card *) * gridsize)};
    int * cards_given = random_numbers(cards->size, gridsize * gridsize);
    for (int i = 0; i < gridsize; i++) {
        ret_val.cards[i] = (player_card *)malloc(sizeof(player_card) * gridsize);
        for (int j = 0; j < gridsize; j++)
            ret_val.cards[i][j] = (player_card) {cards->cards + cards_given[i*gridsize + j], 0};
    }
    free(cards_given);
}

void del_board(const board * board) {
    for (int i = 0; i < board->size; i++) {
        free(board->cards[i]);
    }
    //free(board->cards); // Currently a memory leak
}

player add_player(int id, char * name, int gridsize, const available_cards * cards) {
    int name_len = strlen(name);
    player ret_val = {id, (char *)malloc(name_len + 1), add_board(gridsize, cards)};
    strncpy(ret_val.name, name, name_len + 1);
    return ret_val;
}

void del_player(const player * player) {
    free(player->name);
    del_board(&player->board);
}

int main(void) {
    available_cards cards = read_card_file(DEFAULT_CARD_FILE);
    if (cards.size == 0)
        log_error("Failed retrieving cards from file %s\n", DEFAULT_CARD_FILE);
    else {
        log_error("Successfully loaded %d cards\n", cards.size);
    }
    player test = add_player(0, "Test", 5, &cards);
    del_player(&test);
    free(cards.cards);
    return 0;
}
