#include <cjson/cJSON.h>
#include <errno.h>
#include <rxi/log.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game_structures.h"
#include "json_conversion.h"

#define BUFFERSIZE 32
#define DESCSIZE 128
#define DEFAULT_CARD_FILE "./settings/cards.json"

player **players;

int max(int x, int y)
{
    return (((x > y) * x) + ((x <= y) * y));
}

int min(int x, int y)
{
    return (((x < y) * x) + ((x >= y) * y));
}

void swap(int *x, int *y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

int *random_numbers(int range, int nums)
{
    int *choises = (int *)malloc(sizeof(int) * range), *ret_val = (int *)malloc(sizeof(int) * nums);
    for (int i = range - 1; i > -1; i--)
        choises[i] = i;
    for (int i = 0; i < nums; i++)
    {
        int index = rand() % (range - i);
        ret_val[i] = choises[index];
        swap(choises + index, choises + range - i - 1);
    }
    free(choises);
    return ret_val;
}
// Tags not implented
card add_card(int id, char *text, char *desc, char *author, ...)
{
    int textn = strlen(text), descn = strlen(desc), authorn = strlen(author);
#define str_alloc(n) (char *)malloc(sizeof(char) * (n + 1))
    card ret_val = {id, str_alloc(textn), str_alloc(descn), str_alloc(authorn), NULL};
#undef str_alloc
    strncpy(ret_val.text, text, textn + 1);
    strncpy(ret_val.desc, desc, descn + 1);
    strncpy(ret_val.author, author, authorn + 1);
    log_trace("Loaded card [%d]%s: %s by %s", ret_val.id, ret_val.text, ret_val.desc, ret_val.author);
    return ret_val;
}

void del_card(const card *card)
{
    log_trace("Deleting card at %p", card);
    free(card->text);
    free(card->desc);
    free(card->author);
}

available_cards retrieve_cards(cJSON *j_cards)
{
    log_trace("Parsing cards...");
    if (!cJSON_IsArray(j_cards))
        return (available_cards){0, NULL};
    int arr_size = cJSON_GetArraySize(j_cards);
    available_cards ret_val = {arr_size, (card *)malloc(sizeof(card) * arr_size)};
    for (int i = 0; i < arr_size; i++)
    {
        cJSON *j_card = cJSON_GetArrayItem(j_cards, i);
        ret_val.cards[i] = add_card(i, cJSON_GetObjectItem(j_card, "text")->valuestring,
                                    cJSON_GetObjectItem(j_card, "desc")->valuestring,
                                    cJSON_GetObjectItem(j_card, "author")->valuestring);
    }
    return ret_val;
}

available_cards read_card_file(char *filepath)
{
    log_trace("Loading cards from file %s", filepath);
    cJSON *json = json_from_file(filepath);
    available_cards ret_val = retrieve_cards(cJSON_GetObjectItem(json, "cards"));
    cJSON_Delete(json);
    return ret_val;
}

board add_board(int gridsize, const available_cards *cards)
{
    log_trace("Creating board with gridsize %d", gridsize);
    board ret_val = {gridsize, (player_card **)malloc(sizeof(player_card *) * gridsize)};
    int *cards_given = random_numbers(cards->size, gridsize * gridsize);
    for (int i = 0; i < gridsize; i++)
    {
        ret_val.cards[i] = (player_card *)malloc(sizeof(player_card) * gridsize);
        for (int j = 0; j < gridsize; j++)
        {
            log_trace("Adding card id %d, text %s to board", cards_given[i * gridsize + j],
                      cards->cards[cards_given[i * gridsize + j]].text);
            ret_val.cards[i][j] = (player_card){cards->cards + cards_given[i * gridsize + j], 0};
        }
    }
    free(cards_given);
    return ret_val;
}

void del_board(const board *board)
{
    log_trace("Deleting board at %p", board);
    for (int i = 0; i < board->size; i++)
    {
        free(board->cards[i]);
    }
    free(board->cards);
}

player add_player(int id, char *name, int gridsize, const available_cards *cards)
{
    log_trace("Creating player [%d]:%s", id, name);
    int name_len = strlen(name);
    player ret_val = {id, (char *)malloc(name_len + 1), add_board(gridsize, cards)};
    strncpy(ret_val.name, name, name_len + 1);
    return ret_val;
}

void del_player(const player *player)
{
    log_trace("Deleting player %s", player->name);
    free(player->name);
    del_board(&player->board);
}

// Not implemented
int parse_args(int argc, char **argv)
{
    log_trace("Parsing args...");
    srand(time(NULL));
    return 0;
}

int main(int argc, char **argv)
{
    parse_args(argc, argv);
    available_cards cards = read_card_file(DEFAULT_CARD_FILE);
    if (cards.size == 0)
        log_error("Failed retrieving cards from file %s", DEFAULT_CARD_FILE);
    else
    {
        log_info("Successfully loaded %d cards", cards.size);
    }
    player test = add_player(0, "Test", 5, &cards);
    log_debug("player %s has cards:", test.name);
    for (int i = 0; i < test.board.size; i++)
        for (int j = 0; j < test.board.size; j++)
            puts(test.board.cards[i][j].card->text);
    del_player(&test);
    for (int i = 0; i < cards.size; i++)
        del_card(cards.cards + i);
    free(cards.cards);
    return 0;
}
