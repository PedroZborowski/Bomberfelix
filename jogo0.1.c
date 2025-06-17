/*Gabriel Felix Hilleshein - 124047790
Gabriel Henzel Diniz Costa - 124171450
Laísa Tatiana Oliveira de Medeiros - 124225429
Luiz Vitor Vieira de Mattos- 124280314
Pedro de Oliveira Bokel Zborowski - 124176573
Thiago Barbosa da Silva - 124247625*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 1200
#define HEIGHT 600
#define PLAYER 'J'
#define FREE ' '
#define WALL 'W'
#define EXPLOSIBLE 'D'
#define KEY_BOX 'B'
#define KEY 'C'
#define EMPTY_BOX 'K'
#define ENEMY 'E'
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3


typedef struct
{
    int x;
    int y;
} coordinates;

typedef struct
{
    coordinates local;
    char lifes;
    char bombs;
    int points;
    int direction;
} player;

typedef struct
{
    coordinates local;
    int direction;
} enemy;

void walk_right(coordinates *local, int *direction, int **teste)
{
    *direction = 1;
    if(!*(*(teste + local->y) + local->x + 1))
    {
        local -> x += 1;
    }
}

void walk_left(coordinates *local, int *direction, int **teste)
{
    *direction = 3;
    if(!*(*(teste + local->y) + local->x - 1))
    {
        local -> x -= 1;
    }
}

void walk_up(coordinates *local, int *direction, int **teste)
{
    *direction = 0;
    if(!*(*(teste + local->y - 1) + local->x))
    {
        local -> y -= 1;
    }
}

void walk_down(coordinates *local, int *direction, int **teste)
{
    *direction = 2;
    if(!*(*(teste + local->y + 1) + local->x))
    {
        local -> y += 1;
    }
}

void soltarbomba(int direction, coordinates local, char *bombs, char *information, int **teste)
{
    if(*bombs != '0')
    {
        *bombs -= 1;
        information[8] = *bombs;
    }
}

void perdevida(char *lifes ,char *information)
{
    *lifes -= 1;
    information[21] = *lifes;
    if(*lifes == '0')
    {
        puts("YOU LOSE\n");
    }
}

int main()
{
    FILE *world = fopen("world.txt", "r+");
    if(!world)
    {
        puts("ERRO - O mundo nao pode ser aberto");
        return 1;
    }

    int **teste = (int **)malloc(25*sizeof(int *));
    if(!teste)
    {
        puts("ERRO");
        return -1;
    }
    for(int i = 0; i < 25; i++)
    {
        *(teste + i) = (int *)malloc(60*sizeof(int));
        if(!*(teste + i))
        {
            puts("ERRO");
            return -1;
        }
    }


    player bomberman;
    bomberman.lifes = '3';
    bomberman.bombs = '3';
    bomberman.points = 0;
    bomberman.direction = 1;

    coordinates squares = {0, 0};
    char object = fgetc(world);
    while(!feof(world))
    {
        switch(object)
        {
            case WALL:
            *(*(teste + squares.y) + squares.x) = 1;
            break;

            case EXPLOSIBLE:
            *(*(teste + squares.y) + squares.x) = 2;
            break;

            case EMPTY_BOX:
            *(*(teste + squares.y) + squares.x) = 3;
            break;

            case KEY_BOX:
            *(*(teste + squares.y) + squares.x) = 4;
            break;

            case KEY:
            *(*(teste + squares.y) + squares.x) = 5;
            break;

            case FREE:
            *(*(teste + squares.y) + squares.x) = 0;
            break;

            case '\n':
            squares.x -= 1;
            break;

            case PLAYER:
            bomberman.local.x = squares.x;
            bomberman.local.y = squares.y;
            *(*(teste + squares.y) + squares.x) = 0;
        }
        squares.x += 1;
        if(squares.x/60)
        {
            squares.x = 0;
            squares.y += 1;
        }
        object = fgetc(world);
    }
    fclose(world);


    char information[50] = "Bombas: 3     Vidas: 3     Pontos: 0";
    int testando = -1;
    while(testando != 0)
    {
        if(testando == 1) walk_right(&bomberman.local, &bomberman.direction, teste);
        if(testando == 2) walk_left(&bomberman.local, &bomberman.direction, teste);
        if(testando == 3) walk_up(&bomberman.local, &bomberman.direction, teste);
        if(testando == 4) walk_down(&bomberman.local, &bomberman.direction, teste);
        if(testando == 5) soltarbomba(bomberman.direction, bomberman.local, &bomberman.bombs, information, teste);
        if(testando == 6) perdevida(&bomberman.lifes , information);

            for(int i = 0; i < 25; i++)
            {
                for(int j = 0; j < 60; j++)
                {
                    /*switch(*(*(teste + i) + j))
                    {
                        case 0:
                        printf("%c", ' ');
                        break;

                        case 1:
                        printf("%c", 'W');
                        break;

                        case 2:
                        printf("%c", 'D');
                        break;

                        case 3:
                        printf("%c", 'K');
                        break;

                        case 4:
                        printf("%c", 'B');
                        break;

                        case 5:
                        printf("%c", 'C');
                    }*/
                   printf("%d", *(*(teste + i) + j));
                }
                printf("\n");
            }
            /*switch(bomberman.direction)
            {
                case 0:
                break;

                case 1:
                break;

                case 2:
                break;

                case 3:
            }*/
            //DrawTexture(player_spr, bomberman.local.x, bomberman.local.y, WHITE);
            scanf("%d", &testando);
    }
    return 0;
}