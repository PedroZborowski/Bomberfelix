/*Gabriel Felix Hilleshein - 124047790
Gabriel Henzel Diniz Costa - 124171450
Laísa Tatiana Oliveira de Medeiros - 124225429
Luiz Vitor Vieira de Mattos- 124280314
Pedro de Oliveira Bokel Zborowski - 124176573
Thiago Barbosa da Silva - 124247625*/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 60
#define HEIGHT 30
#define METERS 20

#define PLAYER 'J'
#define FREE ' '
#define WALL 'W'
#define EXPLOSIBLE 'D'
#define KEY_BOX 'B'
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

void walk_up(coordinates *local, int *direction, int **teste)
{
    *direction = UP;
    if(!*(*(teste + local->y - 1) + local->x))
    {
        local->y -= 1;
    }
}

void walk_right(coordinates *local, int *direction, int **teste)
{
    *direction = RIGHT;
    if(!*(*(teste + local->y) + local->x + 1))
    {
        local->x += 1;
    }
}

void walk_down(coordinates *local, int *direction, int **teste)
{
    *direction = DOWN;
    if(!*(*(teste + local->y + 1) + local->x))
    {
        local->y += 1;
    }
}

void walk_left(coordinates *local, int *direction, int **teste)
{
    *direction = LEFT;
    if(!*(*(teste + local->y) + local->x - 1))
    {
        local->x -= 1;
    }
}

void put_bomb(int direction, coordinates local, char *bombs, char *information, int **teste)
{
    if(*bombs != '0')
    {
        *bombs -= 1;
        information[8] = *bombs;
    }
}

void lose_life(char *lifes ,char *information)
{
    *lifes -= 1;
    information[21] = *lifes;
    if(*lifes == '0')
    {
        while(!WindowShouldClose())
        {
            DrawRectangle(620, 520, 60, 60, RAYWHITE);
            DrawText("0", 640, 520, 60, GREEN);
            BeginDrawing();
                DrawText("GAME OVER", 130, 220, 150, RED);
            EndDrawing();
        }
        CloseWindow();
    }
}

int main()
{
    FILE *world = fopen("world.txt", "r+");
    if(!world)
    {
        puts("ERRO - O mundo nao pode ser aberto");
        return -1;
    }

    int **teste = (int **)malloc(25*sizeof(int *));
    if(!teste)
    {
        puts("ERRO - A memoria nao pode ser alocada");
        return -1;
    }
    for(int y = 0; y < 25; y++)
    {
        *(teste + y) = (int *)malloc(WIDTH*sizeof(int));
        if(!*(teste + y))
        {
            puts("ERRO - A memoria nao pode ser alocada");
            return -1;
        }
    }

    player bomberman;
    bomberman.lifes = '3';
    bomberman.bombs = '3';
    bomberman.points = 0;
    bomberman.direction = RIGHT;
    char information[50] = "Bombas: 3     Vidas: 3     Pontos: 0";

    coordinates squares = {0, 0};
    char object = fgetc(world);
    while(!feof(world))
    {
        switch(object)
        {
            case FREE:
            *(*(teste + squares.y) + squares.x) = 0;
            break;

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

            case '\n':
            squares.x -= 1;
            break;

            case PLAYER:
            bomberman.local.x = squares.x;
            bomberman.local.y = squares.y;
            *(*(teste + squares.y) + squares.x) = 0;
        }
        squares.x += 1;
        if(squares.x/WIDTH)
        {
            squares.x = 0;
            squares.y += 1;
        }
        object = fgetc(world);
    }
    fclose(world);

    InitWindow(WIDTH*METERS, HEIGHT*METERS, "Felix Bomberman");

    //código abaixo para carregar as sprites. Extremamente importante esse código estar depois do initwindow
    Texture2D player_spr = LoadTexture("Sprites/Player/felix.png");
    Texture2D wall_spr = LoadTexture("Sprites/cenario/parede.png");
    Texture2D box_spr = LoadTexture("Sprites/cenario/caixa.png");
    Texture2D explosible_spr = LoadTexture("Sprites/cenario/parede_destrutivel.png");
    Texture2D key_spr = LoadTexture("Sprites/interagiveis/chave.png");
    Texture2D floor_spr = LoadTexture("Sprites/cenario/chao.png");

    //segundo o site oficial, checa se a textura e valida e esta carregada na GPU, retorna TRUE 
    if(!IsTextureValid(player_spr))
    {
        puts("ERRO - Nao foi possivel achar o sprite do felix. PROCURE O FELIX!");
        CloseWindow();
        return 1;    
    }
    if(!IsTextureValid(wall_spr))
    {
        puts("ERRO - Nao foi possivel achar o sprite da parede.");
        CloseWindow();
        return 1;    
    }
    if(!IsTextureValid(explosible_spr))
    {
        puts("ERRO - Nao foi possivel achar o sprite da parede destrutivel.");
        CloseWindow();
        return 1;    
    }
    if(!IsTextureValid(box_spr))
    {
        puts("ERRO - Nao foi possivel achar o sprite da caixa.");
        CloseWindow();
        return 1;    
    }
    if(!IsTextureValid(key_spr))
    {
        puts("ERRO - Nao foi possivel achar o sprite da chave.");
        CloseWindow();
        return 1;    
    }
    if(!IsTextureValid(floor_spr))
    {
        puts("ERRO - Nao foi possivel achar o sprite do chao.");
        CloseWindow();
        return 1;    
    }

    //posição inicial
    Vector2 origin = {0.0, 0.0};
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_RIGHT)) walk_right(&bomberman.local, &bomberman.direction, teste);
        if(IsKeyPressed(KEY_LEFT)) walk_left(&bomberman.local, &bomberman.direction, teste);
        if(IsKeyPressed(KEY_UP)) walk_up(&bomberman.local, &bomberman.direction, teste);
        if(IsKeyPressed(KEY_DOWN)) walk_down(&bomberman.local, &bomberman.direction, teste);
        if(IsKeyPressed(KEY_B)) put_bomb(bomberman.direction, bomberman.local, &bomberman.bombs, information, teste);
        if(IsKeyPressed(KEY_K)) lose_life(&bomberman.lifes , information);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            //adicionando textura no chao inteiro
            //primeiro eu crio um triangulo do tamanho da tela
            Rectangle sourceRec = {0.0, 0.0, WIDTH*METERS, 500}; // o motivo de ser 500 e não HEIGHT e pq ali em baixo fica a hud ne, se quiser entender melhor troca isso para "HEIGHT"
            //esse código desenha o sprite no retangulo criado. Como o sprite(tam: 20x20) é bem menor que o retangulo (tam: tela), ele vai preenchendo até cobrir tudo
            DrawTextureRec(floor_spr, sourceRec, origin, WHITE);
            for(int y = 0; y < 25; y++)
            {
                for(int x = 0; x < WIDTH; x++)
                {
                    switch(*(*(teste + y) + x))
                    {
                        case 1:
                        DrawTexture(wall_spr, x*METERS, y*METERS, WHITE);
                        break;

                        case 2:
                        DrawTexture(explosible_spr, x*METERS, y*METERS, WHITE);
                        break;

                        case 3:
                        DrawTexture(box_spr, x*METERS, y*METERS, WHITE);
                        break;

                        case 4:
                        DrawTexture(box_spr, x*METERS, y*METERS, WHITE);
                        break;

                        case 5:
                        DrawTexture(key_spr, x*METERS, y*METERS, WHITE);
                    }
                }
            }
            /*switch(bomberman.direction)
            {
                case UP:
                break;

                case RIGHT:
                break;

                case DOWN:
                break;

                case LEFT:
            }*/
            DrawTexture(player_spr, bomberman.local.x*METERS, bomberman.local.y*METERS, WHITE);
            DrawText(information, 20, 520, 60, GREEN);
        EndDrawing();
    }

    //Necessário para descarregar as texturas
    UnloadTexture(player_spr);
    UnloadTexture(wall_spr);
    UnloadTexture(explosible_spr);
    UnloadTexture(box_spr);
    UnloadTexture(key_spr);
    UnloadTexture(floor_spr);
    free(teste);
    CloseWindow();
    return 0;
}