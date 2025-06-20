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
} enemy;

void walk_up(coordinates *local, char **world)
{
    local->direction = UP;
    if(*(*(world + local->y - 1) + local->x) == FREE)
    {
        local->y -= 1;
    }
}

void walk_right(coordinates *local, char **world)
{
    local->direction = RIGHT;
    if(*(*(world + local->y) + local->x + 1) == FREE)
    {
        local->x += 1;
    }
}

void walk_down(coordinates *local, char **world)
{
    local->direction = DOWN;
    if(*(*(world + local->y + 1) + local->x) == FREE)
    {
        local->y += 1;
    }
}

void walk_left(coordinates *local, char **world)
{
    local->direction = LEFT;
    if(*(*(world + local->y) + local->x - 1) == FREE)
    {
        local->x -= 1;
    }
}

void put_bomb(player *bomberman, char *information/*, char **world*/)
{
    if(bomberman->bombs != '0')
    {
        bomberman->bombs -= 1;
        *(information + 8) = bomberman->bombs;
    }
}

bool new_game(player *bomberman, char **world, char *information)
{
    FILE *file = fopen("world.txt", "r");
    if(!file)
    {
        puts("ERRO - O nivel nao pode ser aberto");
        return false;
    }

    bomberman->lifes = '3';
    bomberman->bombs = '3';
    bomberman->points = 0;
    bomberman->local.direction = DOWN;
    coordinates squares = {0, 0, 0};
    char object = fgetc(file);
    while(!feof(file))
    {
        switch(object)
        {
            case '\n':
            squares.x -= 1;
            break;

            case PLAYER:
            bomberman->local.x = squares.x;
            bomberman->local.y = squares.y;
            *(*(world + squares.y) + squares.x) = FREE;
            break;

            default:
            *(*(world + squares.y) + squares.x) = object;
        }
        squares.x += 1;
        if(squares.x/WIDTH)
        {
            squares.x = 0;
            squares.y += 1;
        }
        object = fgetc(file);
    }
    *(information + 8) = bomberman->bombs;
    *(information + 21) = bomberman->lifes;
    *(information + 35) = '0';
    fclose(file);
    return true;
}


bool load_game(player *bomberman, char **world, char *information)
{
    FILE *file = fopen("save.dat", "rb");
    if(!file)
    {
        puts("Nao ha nenhum jogo salvo");
        return false;
    }
    else
    {
        puts("Jogo carregado com sucesso");
    }

    coordinates squares = {0, 0, 0};
    char object;
    fread(&object, sizeof(char), 1, file);
    while(object != '\n')
    {
        *(*(world + squares.y) + squares.x) = object;
        squares.x += 1;
        if(squares.x/WIDTH)
        {
            squares.x = 0;
            squares.y += 1;
        }
        fread(&object, sizeof(char), 1, file);
    }
    fread(bomberman, sizeof(player), 1, file);

    *(information + 8) = bomberman->bombs;
    *(information + 21) = bomberman->lifes;
    *(information + 35) = '0';

    fclose(file);
    return true;
}

bool save_game(player bomberman, char **world)
{
    FILE *file = fopen("save.dat", "wb");
    if(!file)
    {
        puts("ERRO - O jogo nao pode ser salvo");
        return false;
    }
    else
    {
        puts("Jogo salvo com sucesso");
    }

    for(int y = 0; y < STATURE; y++)
    {
        for(int x = 0; x < WIDTH; x++)
        {
            fwrite(*(world + y) + x, sizeof(char), 1, file);
        }
    }
    char split = '\n';
    fwrite(&split, sizeof(char), 1, file);
    fwrite(&bomberman, sizeof(player), 1, file);

    fclose(file);
    return true;
}

void pause_game(player *bomberman, char *information, char **world)
{
    while(!IsKeyPressed(KEY_V))
    {
        if(IsKeyPressed(KEY_N))
        {
            if(new_game(bomberman, world, information)) break;
        }
        if(IsKeyPressed(KEY_C))
        {
            if(load_game(bomberman, world, information)) break;
        }
        if(IsKeyPressed(KEY_S)) save_game(*bomberman, world);
        if(IsKeyPressed(KEY_Q) || WindowShouldClose()) CloseWindow();
        BeginDrawing();
            DrawText("PAUSE", 130, 220, 150, RED);
        EndDrawing();
    }
}

void lose_life(player *bomberman ,char *information, char **world)
{
    bomberman->lifes -= 1;
    *(information + 21) = bomberman->lifes;
    if(bomberman->lifes == '0')
    {
        while(!IsKeyPressed(KEY_J))//tecla temporaria
        {
            if(IsKeyPressed(KEY_Q) || WindowShouldClose()) CloseWindow();
            if(IsKeyPressed(KEY_N))
            {
                if(new_game(bomberman, world, information)) break;
            }
            if(IsKeyPressed(KEY_C))
            {
                if(load_game(bomberman, world, information)) break;
            }
            DrawRectangle(620, 520, 60, 60, RAYWHITE);
            DrawText("0", 640, 520, 60, GREEN);
            BeginDrawing();
                DrawText("GAME OVER", 130, 220, 150, RED);
            EndDrawing();
        }
    }
}

void show_devs()
{
    while(!IsKeyPressed(KEY_V))
    {
        if(WindowShouldClose()) CloseWindow();
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("DESENVOLVEDORES", 250, 0, 50, RED);
            DrawText("Gabriel Felix Hilleshein - 124047790", 65, 100, 40, RED);
            DrawText("Gabriel Henzel Diniz Costa - 124171450", 65, 170, 40, RED);
            DrawText("Laísa Tatiana Oliveira de Medeiros - 124225429", 65, 240, 40, RED);
            DrawText("Luiz Vitor Vieira de Mattos- 124280314", 65, 310, 40, RED);
            DrawText("Pedro de Oliveira Bokel Zborowski - 124176573", 65, 380, 40, RED);
            DrawText("Thiago Barbosa da Silva - 124247625", 65, 450, 40, RED);
        EndDrawing();
    }
}

void records()
{
    while(!IsKeyPressed(KEY_V))
    {
        if(WindowShouldClose()) CloseWindow();
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("RECORDS", 50, 10, 50, RED);
            DrawText("Nome do jogador | pontuação", 350, 15, 40, RED);
            DrawText("1º ---------- | 0", 40, 100, 50, BLACK);
            DrawText("2º ---------- | 0", 40, 200, 50, BLACK);
            DrawText("3º ---------- | 0", 40, 300, 50, BLACK);
            DrawText("4º ---------- | 0", 40, 400, 50, BLACK);
            DrawText("5º ---------- | 0", 40, 500, 50, BLACK);
            DrawText("6º ---------- | 0", 600, 100, 50, BLACK);
            DrawText("7º ---------- | 0", 600, 200, 50, BLACK);
            DrawText("8º ---------- | 0", 600, 300, 50, BLACK);
            DrawText("9º ---------- | 0", 600, 400, 50, BLACK);
            DrawText("10º ---------- | 0", 600, 500, 50, BLACK);
        EndDrawing();
    }
}

void controls()
{
    while(!IsKeyPressed(KEY_V))
    {
        if(WindowShouldClose()) CloseWindow();
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("CONTROLES", 50, 10, 50, RED);
            DrawText("ASWD OU SETINHAS - MOVIMENTAÇÃO", 40, 100, 50, BLACK);
            DrawText("B - SOLTA BOMBA", 40, 200, 50, BLACK);
            DrawText("TAB - PAUSA", 40, 300, 50, BLACK);
            DrawText("Q - SAIR DO JOGO", 40, 400, 50, BLACK);
            DrawText("N - NOVO JOGO", 40, 500, 50, BLACK);
            DrawText("S - SALVAR", 600, 200, 50, BLACK);
            DrawText("C - CARREGAR JOGO", 600, 300, 50, BLACK);
            DrawText("V - VOLTAR", 600, 400, 50, BLACK);
        EndDrawing();
    }
}

int main()
{
    player bomberman;
    char *information = (char *)malloc(50*sizeof(char));
    if(!information)
    {
        puts("ERRO - A memoria nao pode ser alocada");
        return -1;
    }
    strcpy(information, "Bombas: X     Vidas: X     Pontos: X");
    char **world = (char **)malloc(STATURE*sizeof(char *));
    if(!world)
    {
        puts("ERRO - A memoria nao pode ser alocada");
        return -1;
    }
    for(int y = 0; y < STATURE; y++)
    {
        *(world + y) = (char *)malloc(WIDTH*sizeof(char));
        if(!*(world + y))
        {
            puts("ERRO - A memoria nao pode ser alocada");
            return -1;
        }
    }

    InitWindow(WIDTH*METERS, HEIGHT*METERS, "Felix Bomberman");
    SetTargetFPS(60);
    for(coordinates arrow = {0, 0, 0}; !WindowShouldClose();/* espaço reservado para um possivel background animado*/)
    {
        if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) arrow.y = (arrow.y+2)%3;
        if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) arrow.y = (arrow.y+1)%3;
        if(IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) arrow.x = (arrow.x+1)%2;
        if(IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) arrow.x = (arrow.x+1)%2;
        if(IsKeyPressed(KEY_ENTER))
        {
            if(arrow.y == 0 && arrow.x == 0)
            {
                if(new_game(&bomberman, world, information)) break;
            }
            if(arrow.y == 1 && arrow.x == 0)
            {
                if(load_game(&bomberman, world, information)) break;
                else
                {
                    /*BeginDrawing();
                        DrawText("Nao ha nenhum jogo salvo", 130, 500, 70, BLACK); inserir mensagem temporaria com pilha? aqui e no pause/gameover screen
                    EndDrawing();*/
                }
            }
            if(arrow.y == 2 && arrow.x == 0) records();
            if(arrow.y == 0 && arrow.x == 1) controls();
            if(arrow.y == 1 && arrow.x == 1) show_devs();
            if(arrow.y == 2 && arrow.x == 1)
            {
                CloseWindow();
                return 0;
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("BOMBERMAN FELIX", 130, 100, 100, RED);
            DrawText("Novo jogo", 130, 250, 70, BLACK);
            DrawText("Jogo salvo", 130, 350, 70, BLACK);
            DrawText("Records", 130, 450, 70, BLACK);
            DrawText("Controles", 630, 250, 70, BLACK);
            DrawText("Criadores", 630, 350, 70, BLACK);
            DrawText("Sair", 630, 450, 70, BLACK);
            DrawRectangle(70 + (500*arrow.x), (100*arrow.y) + 250, 50, 50, RED);
        EndDrawing();
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
    while(!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) walk_right(&bomberman.local, world);
        if(IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) walk_left(&bomberman.local, world);
        if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) walk_up(&bomberman.local, world);
        if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) walk_down(&bomberman.local, world);
        if(IsKeyPressed(KEY_B)) put_bomb(&bomberman, information/*, world*/);
        if(IsKeyPressed(KEY_K)) lose_life(&bomberman , information, world);
        if(IsKeyPressed(KEY_TAB)) pause_game(&bomberman, information, world);

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
                        case WALL:
                        DrawTexture(wall_spr, x*METERS, y*METERS, WHITE);
                        break;

                        case EXPLOSIBLE:
                        DrawTexture(explosible_spr, x*METERS, y*METERS, WHITE);
                        break;

                        case EMPTY_BOX:
                        DrawTexture(box_spr, x*METERS, y*METERS, WHITE);
                        break;

                        case KEY_BOX:
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
    free(information);
    for(int y = 0; y < STATURE; y++)
    {
        free(*(world + y));
    }
    free(world);
    CloseWindow();
    return 0;
}