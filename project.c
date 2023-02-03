#include "project.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

struct game
{
    char *name;
    float price;
    float revenue;
};


// adds new game in database
struct game *add(struct game *games, char* name, float price, int size) {
    // allocate updated games
    struct game* new_games = realloc(games, (size+1) * sizeof(struct game));
    struct game new_game;
    // allocate the name and copy the values
    new_game.name = malloc((strlen(name)+1)*sizeof(char));
    strcpy(new_game.name, name);
    new_game.price = price;
    new_game.revenue = 0.0;
    new_games[size] = new_game;
    return new_games;    
}

// checks if game in database returns either 0 (if not in database) or the index+1 of the game
int name_match(struct game *games, char *name, int len) {
    int match = 0;
    for (int i = 0; i < len; i++)
        {
            if (strcmp(name,games[i].name)==0) match = (i+1);
        }
    return match;
}

// buys a new game, returns 1 if success, else 0
int buy(struct game *games, char *name, int len, int amount) {
    int index = name_match(games, name, len);
    if (index) {
        games[index-1].revenue += (games[index-1].price*amount);
        return 1;
    } else return 0;
}


// comparing function
int cmp(const void* a, const void* b) {
	struct game *gameA = (struct game *)a;
	struct game *gameB = (struct game *)b;

    return gameB->revenue - gameA->revenue;
}



// lists the names of the games in desceding order, returns 1 is success else 0
int list(struct game *games, int len) {
    if (games != NULL) {
        qsort(games, len, sizeof(struct game), cmp);
        for (int i = 0; i < len; i++)
        {
            printf("%s %.2f %.2f\n", games[i].name, games[i].price, games[i].revenue);
        }
        return 1;
    } else {
        return 0;
    }
}

// writes games in to a file
int write(char *file_name, struct game *games, int len) {
    FILE *file = fopen(file_name, "w");
    if(!file) return 0;
    else {
        for (int i = 0; i < len; i++)
        {
            fprintf(file, "%s %.2f %.2f\n", games[i].name, games[i].price, games[i].revenue);
        }
        fclose(file);
        return 1;
    }
}

// reads new list from file
struct game *read(char *file_name, struct game *games, int *len) {
    FILE *file = fopen(file_name, "r");
    //If can't open, return the original list
    if (!file) {
        printf("Cannot open file %s for reading.\n", file_name);
        return games;
    }

    struct game *new_games = NULL;
    char name[1000];
    float price;
    float revenue;
    int i = 0;

    while(fscanf(file, "%s %f %f\n", name, &price, &revenue) != EOF) {
        new_games = add(new_games, name, price, i);
        new_games[i].revenue = revenue;
        i++;
    }
    if (feof(file)) {
        fclose(file);
        //If success frees the original list
        for (int i = 0; i < *len; i++)
        {
            free(games[i].name);
        }
        free(games);
        printf("SUCCESS\n");
        *len=i;
        return new_games;
    }
    fclose(file);
    return games;
}



int main(void) {
    
    int quit = 0;

    struct game* game_array = NULL;
    int len = 0;

    while (!quit)
    {
        char buf[1000];
        //Asking the string from user and store it in buf
        fgets(buf, 1000, stdin);

        //ADD A NEW GAME
        if (buf[0] == 'A') {
            char cmd;
            char name[1000];
            float price;
            //analyze the parameters
            int x = sscanf(buf, "%c %s %f", &cmd, name, &price);
            if (x != 3) printf("A should be followed by exactly 2 arguments.\n");
            else if (price<=0) printf("Game price cannot be negative or zero.\n");
            else {
                if (name_match(game_array, name, len)) printf("Game %s is already in the database.\n", name);
                else {
                    game_array = add(game_array, name, price, len);
                    len++;
                    printf("SUCCESS\n");
                }
            }
        
        //BUY A GAME
        } else if (buf[0] == 'B') {
            char cmd;
            char name[1000];
            int amount;
            //analyze the parameters
            int x = sscanf(buf, "%c %s %d", &cmd, name, &amount);
            if (x != 3) printf("B should be followed by exactly 2 arguments.\n");
            else if (amount<1) printf("Number of bought items cannot be less than 1.\n");
            else {
                if (buy(game_array, name, len, amount)) {
                    printf("SUCCESS\n");
                } else {
                    printf("Game not in database\n");
                }
            }

        //LIST THE GAMES
        } else if (buf[0] == 'L') {
            if (list(game_array, len)) printf("SUCCESS\n");
            else printf("No games\n");

        //WRITE THE GAMES IN A FILE
        } else if (buf[0] == 'W') {
            char cmd;
            char name[1000];
            //analyze the parameters
            int x = sscanf(buf, "%c %s", &cmd, name);
            if (x != 2) printf("Given parameters are wrong\n");
            else {
                if (write(name, game_array,len)) printf("SUCCESS\n");
                else printf("Failed\n");
            }
        //LOAD GAMES FROM A FILE
        } else if (buf[0] == 'O') {
            char cmd;
            char name[1000];
            //analyze the parameters
            int x = sscanf(buf, "%c %s", &cmd, name);
            if (x != 2) printf("Given parameters are wrong\n");
            else {
                game_array = read(name, game_array, &len);
            }
        } else if (buf[0] == 'Q') {
            printf("SUCCESS\n");
            for (int i = 0; i < len; i++)
            {
                free(game_array[i].name);
            }
            free(game_array);
            quit = 1;
        } else {
            printf("Invalid command %c\n", buf[0]);
        }
    }
    

}