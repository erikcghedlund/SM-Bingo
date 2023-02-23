#ifndef GAME_STRUCTURES_H
#define GAME_STRUCTURES_H

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

#endif
