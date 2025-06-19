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
#define STATURE 25
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
    int direction;
} coordinates;

typedef struct
{
    coordinates local;
    char lifes;
    char bombs;
    int points;
} player;

typedef struct
{
    coordinates local;
    int direction;
} enemy;

void walk_up(coordinates *local, int **world)
{
    local->direction = UP;
    if(!*(*(world + local->y - 1) + local->x))
    {
        local->y -= 1;
    }
}

void walk_right(coordinates *local, int **world)
{
    local->direction = RIGHT;
    if(!*(*(world + local->y) + local->x + 1))
    {
        local->x += 1;
    }
}

void walk_down(coordinates *local, int **world)
{
    local->direction = DOWN;
    if(!*(*(world + local->y + 1) + local->x))
    {
        local->y += 1;
    }
}

void walk_left(coordinates *local, int **world)
{
    local->direction = LEFT;
    if(!*(*(world + local->y) + local->x - 1))
    {
        local->x -= 1;
    }
}

void put_bomb(player *bomberman, char *information/*, int **world*/)
{
    if(bomberman->bombs != '0')
    {
        bomberman->bombs -= 1;
        information[8] = bomberman->bombs;
    }
}

int **start_game(char *level, player *bomberman, char *information)
{
    FILE *file = fopen(level, "r+");
    if(!file)
    {
        puts("ERRO - O nivel nao pode ser aberto");
        return NULL;
    }

    int **world = (int **)malloc(STATURE*sizeof(int *));
    if(!world)
    {
        puts("ERRO - A memoria nao pode ser alocada");
        return NULL;
    }
    for(int y = 0; y < STATURE; y++)
    {
        *(world + y) = (int *)malloc(WIDTH*sizeof(int));
        if(!*(world + y))
        {
            puts("ERRO - A memoria nao pode ser alocada");
            return NULL;
        }
    }

    bomberman->lifes = '3';
    bomberman->bombs = '3';
    bomberman->points = 0;
    bomberman->local.direction = RIGHT;
    strcpy(information, "Bombas: 3     Vidas: 3     Pontos: 0");

    coordinates squares = {0, 0, 0};
    char object = fgetc(file);
    while(!feof(file))
    {
        switch(object)
        {
            case FREE:
            *(*(world + squares.y) + squares.x) = 0;
            break;

            case WALL:
            *(*(world + squares.y) + squares.x) = 1;
            break;

            case EXPLOSIBLE:
            *(*(world + squares.y) + squares.x) = 2;
            break;

            case EMPTY_BOX:
            *(*(world + squares.y) + squares.x) = 3;
            break;

            case KEY_BOX:
            *(*(world + squares.y) + squares.x) = 4;
            break;

            case '\n':
            squares.x -= 1;
            break;

            case PLAYER:
            bomberman->local.x = squares.x;
            bomberman->local.y = squares.y;
            *(*(world + squares.y) + squares.x) = 0;
        }
        squares.x += 1;
        if(squares.x/WIDTH)
        {
            squares.x = 0;
            squares.y += 1;
        }
        object = fgetc(file);
    }
    fclose(file);
    return world;
}

void save_game();

void pause(char *level, player *bomberman, char information[])
{
    while(!IsKeyPressed(KEY_V))
    {
        if(IsKeyPressed(KEY_N))
        {
            start_game(level, bomberman, information);
            break;
        }
        //if(IsKeyPressed(KEY_C));
        //if(IsKeyPressed(KEY_S));
        if(IsKeyPressed(KEY_Q)) CloseWindow();
        BeginDrawing();
            DrawText("PAUSE", 130, 220, 150, RED);
        EndDrawing();
    }
}

void lose_life(player *bomberman ,char *information)
{
    bomberman->lifes -= 1;
    information[21] = bomberman->lifes;
    if(bomberman->lifes == '0')
    {
        while(!IsKeyPressed(KEY_N))
        {
            if(IsKeyPressed(KEY_Q)) CloseWindow();
            //if(IsKeyPressed(KEY_C));
            DrawRectangle(620, 520, 60, 60, RAYWHITE);
            DrawText("0", 640, 520, 60, GREEN);
            BeginDrawing();
                DrawText("GAME OVER", 130, 220, 150, RED);
            EndDrawing();
        }
        start_game("world.txt", bomberman, information);
    }
}

int main()
{
    InitWindow(WIDTH*METERS, HEIGHT*METERS, "Felix Bomberman");

    for(int i = 0;!WindowShouldClose();)
    {
        if(IsKeyPressed(KEY_UP)) i = (i+1)%3;
        if(IsKeyPressed(KEY_DOWN)) i = (i+2)%3;
        if(IsKeyPressed(KEY_ENTER))
        {
            if(i == 2) break;
            if(i == 1) puts("carregar jogo ainda n ta pronto D:");
            if(i == 0) CloseWindow();
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("BOMBERMAN FELIX", 130, 100, 100, RED);
            DrawText("Novo jogo", 130, 200, 70, BLACK);
            DrawText("Carregar jogo", 130, 300, 70, BLACK);
            DrawText("Sair", 130, 400, 70, BLACK);
            DrawRectangle(70, (-100*i) + 400, 50, 50, RED);
        EndDrawing();
    }

    player bomberman;
    char information[50];
    int **world = start_game("world.txt", &bomberman, information);
    if(!world)
    {
        return -1;
    }

    //código abaixo para carregar as sprites. Extremamente importante esse código estar depois do initwindow
    Texture2D player_down_spr = LoadTexture("Sprites/Player/Felix_down.png");
    Texture2D player_up_spr = LoadTexture("Sprites/Player/Felix_up.png");
    Texture2D player_left_spr = LoadTexture("Sprites/Player/Felix_left.png");
    Texture2D player_right_spr = LoadTexture("Sprites/Player/Felix_right.png");

    Texture2D wall_spr = LoadTexture("Sprites/cenario/parede.png");
    Texture2D box_spr = LoadTexture("Sprites/cenario/caixa.png");
    Texture2D explosible_spr = LoadTexture("Sprites/cenario/parede_destrutivel.png");
    Texture2D key_spr = LoadTexture("Sprites/interagiveis/chave.png");
    Texture2D floor_spr = LoadTexture("Sprites/cenario/chao.png");

    //segundo o site oficial, checa se a textura e valida e esta carregada na GPU, retorna TRUE 
    if(!IsTextureValid(player_down_spr) )
    {
        puts("ERRO - Nao foi possivel achar o sprite do Felix_down.");
        CloseWindow();
        return 1;    
    }
        if(!IsTextureValid(player_up_spr) )
    {
        puts("ERRO - Nao foi possivel achar o sprite do Felix_up.");
        CloseWindow();
        return 1;    
    }
        if(!IsTextureValid(player_right_spr) )
    {
        puts("ERRO - Nao foi possivel achar o sprite do Felix_right.");
        CloseWindow();
        return 1;    
    }
        if(!IsTextureValid(player_left_spr) )
    {
        puts("ERRO - Nao foi possivel achar o sprite do Felix_left.");
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
        if(IsKeyPressed(KEY_RIGHT)) walk_right(&bomberman.local, world);
        if(IsKeyPressed(KEY_LEFT)) walk_left(&bomberman.local, world);
        if(IsKeyPressed(KEY_UP)) walk_up(&bomberman.local, world);
        if(IsKeyPressed(KEY_DOWN)) walk_down(&bomberman.local, world);
        if(IsKeyPressed(KEY_B)) put_bomb(&bomberman, information/*, world*/);
        if(IsKeyPressed(KEY_K)) lose_life(&bomberman , information);
        if(IsKeyPressed(KEY_TAB)) pause("world.txt", &bomberman, information);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            //adicionando textura no chao inteiro
            //primeiro eu crio um triangulo do tamanho da tela
            Rectangle sourceRec = {0.0, 0.0, WIDTH*METERS, 500}; // o motivo de ser 500 e não HEIGHT e pq ali em baixo fica a hud ne, se quiser entender melhor troca isso para "HEIGHT"
            //esse código desenha o sprite no retangulo criado. Como o sprite(tam: 20x20) é bem menor que o retangulo (tam: tela), ele vai preenchendo até cobrir tudo
            DrawTextureRec(floor_spr, sourceRec, origin, WHITE);
            for(int y = 0; y < STATURE; y++)
            {
                for(int x = 0; x < WIDTH; x++)
                {
                    switch(*(*(world + y) + x))
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
            switch(bomberman.local.direction)
            {
                case UP:
                DrawTexture(player_up_spr, bomberman.local.x*METERS, bomberman.local.y*METERS, WHITE);
                break;

                case RIGHT:
                DrawTexture(player_right_spr, bomberman.local.x*METERS, bomberman.local.y*METERS, WHITE);
                break;

                case DOWN:
                DrawTexture(player_down_spr, bomberman.local.x*METERS, bomberman.local.y*METERS, WHITE);
                break;

                case LEFT:
                DrawTexture(player_left_spr, bomberman.local.x*METERS, bomberman.local.y*METERS, WHITE);
            }

            DrawText(information, 20, 520, 60, GREEN);
        EndDrawing();
    }

    //Necessário para descarregar as texturas
    UnloadTexture(player_down_spr);
    UnloadTexture(player_up_spr);
    UnloadTexture(player_right_spr);
    UnloadTexture(player_left_spr);
    UnloadTexture(wall_spr);
    UnloadTexture(explosible_spr);
    UnloadTexture(box_spr);
    UnloadTexture(key_spr);
    UnloadTexture(floor_spr);
    for(int y = 0; y < STATURE; y++)
    {
        free(*(world + y));
    }
    free(world);
    CloseWindow();
    return 0;
}