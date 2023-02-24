#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#include <cjson/cJSON.h>
#include <rxi/log.h>

#ifdef _WIN32
#include <Fileapi.h>
#else
#include <sys/stat.h>
#endif

#include "game_structures.h"
#include "json_conversion.h"

#define BUFFERSIZE 32
#define DESCSIZE 128
#define DEFAULT_CARD_FILE "./settings/cards.json"

int max(int x, int y) {
    return (((x > y) * x) + ((x <= y) * y));
}

int min(int x, int y) {
    return (((x < y) * x) + ((x >= y) * y));
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

available_cards retrieve_cards(cJSON * j_cards) {
    if (! cJSON_IsArray(j_cards)) return (available_cards) {0, NULL};
    int arr_size = cJSON_GetArraySize(j_cards);
    available_cards ret_val = {arr_size, (card *)malloc(sizeof(card)*arr_size)};
    for (int i = 0; i < arr_size; i++)
        json_card_to_struct(cJSON_GetArrayItem(j_cards, i), i);
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
    return ret_val;
}

void del_board(const board * board) {
    for (int i = 0; i < board->size; i++) {
        free(board->cards[i]);
    }
    free(board->cards); // Currently a memory leak
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

// Not implemented
int parse_args(int argc, char ** argv) {
    return 0;
}

int main(int argc, char ** argv) {
    parse_args(argc, argv);
    available_cards cards = read_card_file(DEFAULT_CARD_FILE);
    if (cards.size == 0)
        log_error("Failed retrieving cards from file %s", DEFAULT_CARD_FILE);
    else {
        log_info("Successfully loaded %d cards", cards.size);
    }
    player test = add_player(0, "Test", 5, &cards);
    del_player(&test);
    free(cards.cards);
    return 0;
}
