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
#define BLOCK 'W'
#define WALL 'D'
#define KEY_BOX 'B'
#define EMPTY_BOX 'K'
#define ENEMY 'E'
#define KEY 'F'
#define BOMB 'O'
#define EXPLODING_WALL 'X'//To tendo que adionar isso aq pq lá em baixo eu preciso checar se a parede foi destruida sem transformar em "vazio" para poder limitar até onde desenhar a merda do sprite da explosão

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

//aqui são os defines do inimigo, vou comentar tudo e que se dane, eu fico perdidinho mane
#define VEL_ENEMY 1
#define MAX_ENEMYS 5

#define MAX_BOMBS 3
#define TIMER_BOMB 3.0

#define TIME_ANIMATION_BOMB 0.1f

typedef struct
{
    int x;
    int y;
    int offsetX;
    int offsetY;
    int direction;
} coordinates;

typedef struct 
{
    coordinates local;
    double planttime;
    bool active;
    int radio;
    double explosion_timer;//para animação
    int explosion_frame;//para animação
    bool Exploding; //para animação
} bomb;

typedef struct
{
    coordinates local;
    char lifes;
    int points;
    bomb bombs[MAX_BOMBS];
} player;

typedef struct
{
    coordinates local;
    double walkTimer;
    bool ta_vivo;
} enemy;

typedef struct
{
    char name[11];
    int points;
} record;

bool walk_up(coordinates *local, char **world)
{
    local->direction = UP;
    if(local->offsetY != 0)
    {
        if(local->offsetY != -10) local->offsetY -= 1;
            else{
                local->offsetY = 9;
                local->y -= 1;
            }
            return true;
    }

    if(*(*(world + local->y - 1) + local->x) == FREE)
    {
        //Se ele estiver à esquerda do meio
        if(local->offsetX < 0){
            if(*(*(world + local->y - 1) + local->x - 1) == FREE)
            {
                if(local->offsetY != -10) local->offsetY -= 1;
                else{
                    local->offsetY = 9;
                    local->y -= 1;
                }
                return true;
            }
            return false;
        }

        //Se ele estiver à direita do meio
        if(local->offsetX > 0){
            if(*(*(world + local->y - 1) + local->x + 1) == FREE)
            {
                if(local->offsetY != -10) local->offsetY -= 1;
                else{
                    local->offsetY = 9;
                    local->y -= 1;
                }
                return true;
            }
            return false;
        }

        //Se ele estiver exatamente no meio:
        if(local->offsetY != -10) local->offsetY -= 1;
        else{
            local->offsetY = 9;
            local->y -= 1;
        }
        return true;
    }
    return false;
}

bool walk_right(coordinates *local, char **world)
{
    local->direction = RIGHT;
    if(local->offsetX != 0)
    {
        if(local->offsetX != 10) local->offsetX += 1;
            else{
                local->offsetX = -9;
                local->x += 1;
            }
            return true;
    }

    if(*(*(world + local->y) + local->x + 1) == FREE)
    {
        //Se ele estiver acima do meio
        if(local->offsetY < 0){
            if(*(*(world + local->y - 1) + local->x + 1) == FREE)
            {
                if(local->offsetX != 10) local->offsetX += 1;
                else{
                    local->offsetX = -9;
                    local->x += 1;
                }
                return true;
            }
            return false;
        }

        //Se ele estiver abaixo do meio
        if(local->offsetY > 0){
            if(*(*(world + local->y + 1) + local->x + 1) == FREE)
            {
                if(local->offsetX != 10) local->offsetX += 1;
                else{
                    local->offsetX = -9;
                    local->x += 1;
                }
                return true;
            }
            return false;
        }

        //Se ele estiver exatamente no meio:
        if(local->offsetX != 10) local->offsetX += 1;
        else{
            local->offsetX = -9;
            local->x += 1;
        }
        return true;
    }
    return false;
}

bool walk_down(coordinates *local, char **world)
{
    local->direction = DOWN;
    if(local->offsetY != 0)
    {
        if(local->offsetY != 10) local->offsetY += 1;
            else{
                local->offsetY = -9;
                local->y += 1;
            }
            return true;
    }
    
    if(*(*(world + local->y + 1) + local->x) == FREE)
    {
        //Se ele estiver à esquerda do meio
        if(local->offsetX < 0){
            if(*(*(world + local->y + 1) + local->x - 1) == FREE)
            {
                if(local->offsetY != 10) local->offsetY += 1;
                else{
                    local->offsetY = -9;
                    local->y += 1;
                }
                return true;
            }
            return false;
        }

        //Se ele estiver à direita do meio
        if(local->offsetX > 0){
            if(*(*(world + local->y + 1) + local->x + 1) == FREE)
            {
                if(local->offsetY != 10) local->offsetY += 1;
                else{
                    local->offsetY = -9;
                    local->y += 1;
                }
                return true;
            }
            return false;
        }

        //Se ele estiver exatamente no meio:
        if(local->offsetY != 10) local->offsetY += 1;
        else{
            local->offsetY = -9;
            local->y += 1;
        }
        return true;
    }
    return false;
}

bool walk_left(coordinates *local, char **world)
{
    local->direction = LEFT;
    if(local->offsetX != 0)
    {
        if(local->offsetX != -10) local->offsetX -= 1;
            else{
                local->offsetX = 9;
                local->x -= 1;
            }
            return true;
    }

    if(*(*(world + local->y) + local->x - 1) == FREE)
    {
        //Se ele estiver acima do meio
        if(local->offsetY < 0){
            if(*(*(world + local->y - 1) + local->x - 1) == FREE)
            {
                if(local->offsetX != -10) local->offsetX -= 1;
                else{
                    local->offsetX = 9;
                    local->x -= 1;
                }
                return true;
            }
            return false;
        }

        //Se ele estiver abaixo do meio
        if(local->offsetY > 0){
            if(*(*(world + local->y + 1) + local->x - 1) == FREE)
            {
                if(local->offsetX != -10) local->offsetX -= 1;
                else{
                    local->offsetX = 9;
                    local->x -= 1;
                }
                return true;
            }
            return false;
        }

        //Se ele estiver exatamente no meio:
        if(local->offsetX != -10) local->offsetX -= 1;
        else{
            local->offsetX = 9;
            local->x -= 1;
        }
        return true;
    }
    return false;
}

bool new_game(player *bomberman, char **world, char *information, enemy letter_student[MAX_ENEMYS], int *num_enemys_on)
{
    FILE *file = fopen("world.txt", "r");
    if(!file)
    {
        puts("ERRO - O nivel nao pode ser aberto");
        return false;
    }

    *num_enemys_on = 0;

    bomberman->lifes = '3';
    bomberman->points = 0;
    bomberman->local.direction = DOWN;
    bomberman->local.offsetX = 0;
    bomberman->local.offsetY = 0;
    for(int i = 0; i < MAX_BOMBS; i++){
        bomberman->bombs[i].active = false;
        bomberman->bombs[i].radio = 2;
    }
    coordinates squares; //leitor do arquivo, vai assim: W | E  W
    squares.x = 0;
    squares.y = 0;
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


            case ENEMY:

                enemy *inimigo_atual = &letter_student[*num_enemys_on];
                inimigo_atual->local.x = squares.x;
                inimigo_atual->local.y = squares.y;
                inimigo_atual->local.offsetX = 0;
                inimigo_atual->local.offsetY = 0;
                inimigo_atual->ta_vivo = true;
                    
                // iniciando o timer de cada inimigo. EXTREMAMENTE IMPORTANTE, MEXA COM CUIDADO AQ
                letter_student->local.direction = GetRandomValue(0, 3);
                inimigo_atual->walkTimer = 0.0; 

                *(*(world + squares.y) + squares.x) = FREE;
                    
                (*num_enemys_on)++;
            
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
    *(information + 8) = '3';
    *(information + 21) = '3';
    *(information + 35) = '0';
    fclose(file);
    return true;
}

bool load_game(player *bomberman, char **world, char *information, enemy letter_student[MAX_ENEMYS], int *num_enemys_on)
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

    coordinates squares;
    squares.x = 0;
    squares.y = 0;
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

    char active_bombs = '0';
    for(int i = 0; i < MAX_BOMBS; i++)
    {
        if(!bomberman->bombs[i].active) active_bombs++;
    }
    *(information + 8) = active_bombs;
    *(information + 21) = bomberman->lifes;
    *(information + 35) = '0';

    fclose(file);
    return true;
}

bool save_game(player bomberman, char **world, enemy letter_student[MAX_ENEMYS], int num_enemys_on)
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

void saveRecord(player bomberman)
{
    record saving;
    saving.points = bomberman.points;
    FILE *file = fopen("record.dat", "rb+");
    if(!file)
    {
        puts("O arquivo nao existia ainda mas agora foi criado");
        file = fopen("record.dat", "wb");
        if(!file)
        {
            puts("ERRO - O record nao pode ser salvo");
        }
        puts("Digite seu nome: ");
        scanf("%s", saving.name);
        fgetc(stdin);
        fwrite(&saving, sizeof(record), 1, file);
        strcpy(saving.name, "----------");
        saving.points = 0;
        for(int i = 0; i < 9; i++)
        {
            fwrite(&saving, sizeof(record), 1, file);
        }
        puts("Record salvo com sucesso");
    }
    else
    {
        puts("Arquivo ja existe, verificando onde a pontuacao ficara");
        record records[10];
        int check = 10;
        for(int i = 0; i < 10; i++)
        {
            fread(records+i, sizeof(record), 1, file);
            if(check == 10) if((records+i)->points < saving.points) check = i;
        }
        fseek(file, check*sizeof(record), SEEK_SET);
        if(check != 10)
        {
            printf("Pontuacao na posicao %d\n", check+1);
            puts("Digite seu nome: ");
            scanf("%s", saving.name);
            fgetc(stdin);
            fwrite(&saving, sizeof(record), 1, file);
            for(int j = check; j < 9; j++)
            {
                fwrite(records+j, sizeof(record), 1, file);
            }
            puts("Record salvo com sucesso");
        }
        else
        {
            puts("Nao obteve uma pontuacao suficiente para ser salva");
        }
    }
    fclose(file);
}

void records()
{
    FILE *file = fopen("record.dat", "rb");
    if(!file)
    {
        while(!IsKeyPressed(KEY_V))
        {
            if(WindowShouldClose()) CloseWindow();
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("RECORDS", 50, 10, 50, RED);
                DrawText("Nome do jogador | pontuação", 350, 15, 40, RED);
                DrawText(" 1º ---------- | 0", 40, 100, 50, BLACK);
                DrawText(" 2º ---------- | 0", 40, 200, 50, BLACK);
                DrawText(" 3º ---------- | 0", 40, 300, 50, BLACK);
                DrawText(" 4º ---------- | 0", 40, 400, 50, BLACK);
                DrawText(" 5º ---------- | 0", 40, 500, 50, BLACK);
                DrawText(" 6º ---------- | 0", 600, 100, 50, BLACK);
                DrawText(" 7º ---------- | 0", 600, 200, 50, BLACK);
                DrawText(" 8º ---------- | 0", 600, 300, 50, BLACK);
                DrawText(" 9º ---------- | 0", 600, 400, 50, BLACK);
                DrawText("10º ---------- | 0", 600, 500, 50, BLACK);
            EndDrawing();
        }
    }
    else
    {
        record records[10];
        char pontos[10][50];
        for(int i = 0; i < 10; i++)
        {
            fread(records+i, sizeof(record), 1, file);
            sprintf(pontos[i], "%d", records[i].points);
        }
        while(!IsKeyPressed(KEY_V)) //ta feio q doi isso aq mas depois eu arrumo
        {
            if(WindowShouldClose()) CloseWindow();
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("RECORDS", 50, 10, 50, RED);
                DrawText("Nome do jogador | pontuação", 350, 15, 40, RED);
                DrawText(" 1º             |", 50, 100, 50, BLACK);
                DrawText(records[0].name , 110, 100, 50, BLACK);
                DrawText(pontos[0] , 380, 100, 50, BLACK);
                DrawText(" 2º             |", 40, 200, 50, BLACK);
                DrawText(records[1].name, 110, 200, 50, BLACK);
                DrawText(pontos[1], 380, 200, 50, BLACK);
                DrawText(" 3º             |", 40, 300, 50, BLACK);
                DrawText(records[2].name , 110, 300, 50, BLACK);
                DrawText(pontos[2] , 380, 300, 50, BLACK);
                DrawText(" 4º             |", 40, 400, 50, BLACK);
                DrawText(records[3].name , 110, 400, 50, BLACK);
                DrawText(pontos[3] , 380, 400, 50, BLACK);
                DrawText(" 5º             |", 40, 500, 50, BLACK);
                DrawText(records[4].name , 110, 500, 50, BLACK);
                DrawText(pontos[4] , 380, 500, 50, BLACK);
                DrawText(" 6º             |", 600, 100, 50, BLACK);
                DrawText(records[5].name , 670, 100, 50, BLACK);
                DrawText(pontos[5] , 940, 100, 50, BLACK);
                DrawText(" 7º             |", 600, 200, 50, BLACK);
                DrawText(records[6].name , 670, 200, 50, BLACK);
                DrawText(pontos[6] , 940, 200, 50, BLACK);
                DrawText(" 8º             |", 600, 300, 50, BLACK);
                DrawText(records[7].name , 670, 300, 50, BLACK);
                DrawText(pontos[7] , 940, 300, 50, BLACK);
                DrawText(" 9º             |", 600, 400, 50, BLACK);
                DrawText(records[8].name , 670, 400, 50, BLACK);
                DrawText(pontos[8] , 940, 400, 50, BLACK);
                DrawText("10º             |", 600, 500, 50, BLACK);
                DrawText(records[9].name , 670, 500, 50, BLACK);
                DrawText(pontos[9] , 940, 500, 50, BLACK);
            EndDrawing();
        }
    }
    fclose(file);
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

void pause_game(player *bomberman, char *information, char **world, enemy letter_student[MAX_ENEMYS], int *num_enemys_on)
{
    while(!IsKeyPressed(KEY_V))
    {
        if(IsKeyPressed(KEY_N))
        {
            if(new_game(bomberman, world, information, letter_student, num_enemys_on)) break;
        }
        if(IsKeyPressed(KEY_C))
        {
            if(load_game(bomberman, world, information, letter_student, num_enemys_on)) break;
        }
        if(IsKeyPressed(KEY_S)) save_game(*bomberman, world, letter_student, *num_enemys_on);
        if(IsKeyPressed(KEY_R)) records();
        if(IsKeyPressed(KEY_K)) controls();
        if(IsKeyPressed(KEY_Q) || WindowShouldClose()) CloseWindow();
        BeginDrawing();
            DrawText("PAUSE", 130, 220, 150, RED);
        EndDrawing();
    }
}

void lose_life(player *bomberman, char *information, char **world, enemy letter_student[MAX_ENEMYS], int *num_enemys_on)
{
    if(bomberman->points >=100) bomberman->points -= 100;//falta mudar a informacao dos pontos
    else bomberman->points = 0;
    sprintf(information + 35 , "%d", bomberman->points);
    bomberman->lifes -= 1;
    *(information + 21) -= 1;
    if(bomberman->lifes <= '0')
    {
        while(!IsKeyPressed(KEY_J))//tecla temporaria
        {
            if(IsKeyPressed(KEY_Q) || WindowShouldClose()) CloseWindow();
            if(IsKeyPressed(KEY_N))
            {
                if (new_game(bomberman, world, information, letter_student, num_enemys_on)) break;
            }
            if(IsKeyPressed(KEY_C))
            {
                if(load_game(bomberman, world, information, letter_student, num_enemys_on)) break;
            }
            DrawRectangle(620, 520, 60, 60, RAYWHITE);
            DrawText("0", 640, 520, 60, GREEN);
            BeginDrawing();
            DrawText("GAME OVER", 130, 220, 150, RED);
            EndDrawing();
        }
        saveRecord(*bomberman);
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

void put_bomb(player *bomberman, char **world, char *information)
{
    for(int i = 0; i < MAX_BOMBS; i++)
    {
        if(!bomberman->bombs[i].active)
        {
            switch(bomberman->local.direction){
            case UP:
            bomberman->bombs[i].local.y = bomberman->local.y - 1;
            bomberman->bombs[i].local.x = bomberman->local.x;
            break;

            case DOWN:
            bomberman->bombs[i].local.y = bomberman->local.y + 1;
            bomberman->bombs[i].local.x = bomberman->local.x;
            break;

            case LEFT:
            bomberman->bombs[i].local.y = bomberman->local.y;
            bomberman->bombs[i].local.x = bomberman->local.x - 1;
            break;

            case RIGHT:
            bomberman->bombs[i].local.y = bomberman->local.y;
            bomberman->bombs[i].local.x = bomberman->local.x + 1;
            }
            if(*(*(world + bomberman->bombs[i].local.y) + bomberman->bombs[i].local.x) == FREE){
            *(*(world + bomberman->bombs[i].local.y) + bomberman->bombs[i].local.x) = BOMB; //aritmética de ponteiros, estou acessando a linha *(world + bombY) e logo em seguida indo para a coluna certa *(world + bombY) + bombX depois disso eu só desreferebcui

            bomberman->bombs[i].planttime = GetTime();
            bomberman->bombs[i].active = true;
            bomberman->bombs[i].Exploding = false;//para animação
            bomberman->bombs[i].explosion_frame = 0; //para animação
            bomberman->bombs[i].explosion_timer = 0.0; //para animação
            *(information + 8) -= 1;
            }
        }
    }
}

void Explosion_impact(int x, int y, player *bomberman, char *information, char **world, enemy in_life_enemys[MAX_ENEMYS], int *num_on_enemys){
    if(bomberman->local.x == x && bomberman->local.y == y){
        lose_life(bomberman, information, world, in_life_enemys, num_on_enemys);
    }
    //bem simples também, apenas passo pelo número de inimigos ativo e desativo eles. Aproveito e aumento os points dele, para facilitar o trabalho do Henzel (minha vez de roubar trampo muahahahahhahah)
    for(int i = 0; i < *num_on_enemys; i++){
        if(in_life_enemys[i].ta_vivo &&in_life_enemys[i].local.x == x && in_life_enemys[i].local.y == y){
            in_life_enemys[i].ta_vivo = false; // No céu tem pão? morri - Diddi
            bomberman->points +=20;
            sprintf(information + 35 , "%d", bomberman->points);
        }
    }

    char *cell = *(world + y) + x;
    if(*cell == WALL || *cell == EMPTY_BOX){
        *cell = EXPLODING_WALL;
        bomberman->points += 10;
        sprintf(information + 35 , "%d", bomberman->points);
    }
    else if(*cell == KEY_BOX){
        *cell = KEY;
        bomberman->points += 10;
        sprintf(information + 35 , "%d", bomberman->points);
    }
}

void enemy_move(enemy *letter_student, char **world)
{
    if(!letter_student->ta_vivo) return;
    bool arrived = false;
    //Henzel amassou com essas funções aqui ta
    switch (letter_student->local.direction)
    {
        case UP:
            arrived = !walk_up(&letter_student->local, world);
            break;
         case RIGHT:
            arrived = !walk_right(&letter_student->local, world);
            break;
          case DOWN:
            arrived = !walk_down(&letter_student->local, world);
            break;
        case LEFT:
            arrived = !walk_left(&letter_student->local, world);
    }
    bool walkRandomly = false;
    if(letter_student->local.offsetX == 0 && letter_student->local.offsetY == 0){
        if(GetRandomValue(0,2) == 0) walkRandomly = true;
    }
    if (GetTime() - letter_student->walkTimer >= 3 || arrived || walkRandomly)
    {
        letter_student->local.direction = GetRandomValue(0, 3);
        letter_student->walkTimer = GetTime();
    }
}

int main()
{
    player bomberman;
    enemy letter_student[MAX_ENEMYS];
    int num_enemys_on = 0;

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

    InitWindow(WIDTH*METERS, HEIGHT*METERS, "BomberFelix");
    SetTargetFPS(60);
    for(coordinates arrow = {0, 0, 0, 0, 0}; !WindowShouldClose();/* espaço reservado para um possivel background animado*/)
    {
        if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) arrow.y = (arrow.y+2)%3;
        if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) arrow.y = (arrow.y+1)%3;
        if(IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) arrow.x = (arrow.x+1)%2;
        if(IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) arrow.x = (arrow.x+1)%2;
        if(IsKeyPressed(KEY_ENTER))
        {
            if(arrow.y == 0 && arrow.x == 0)
            {
                if(new_game(&bomberman, world, information, letter_student, &num_enemys_on)) break;
            }
            if(arrow.y == 1 && arrow.x == 0)
            {
                if(load_game(&bomberman, world, information, letter_student, &num_enemys_on)) break;
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
            DrawText("BOMBERFELIX ", 130, 100, 100, RED);
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
    Texture2D bomb_spr = LoadTexture("Sprites/interagiveis/bomb.png");
    Texture2D explosion_spr[3];
    explosion_spr[0] = LoadTexture("Sprites/explosao/explosao1.png");
    explosion_spr[1] = LoadTexture("Sprites/explosao/explosao2.png");
    explosion_spr[2] = LoadTexture("Sprites/explosao/explosao3.png");

    //segundo o site oficial, checa se a textura e valida e esta carregada na GPU, retorna TRUE 
    if(!IsTextureValid(bomb_spr)){
        puts("ERRO - Nao foi possivel achar o sprite da bomba.");
        CloseWindow();
        return -1;
    }
    for (int i = 0; i < 3; i++) {
        if (!IsTextureValid(explosion_spr[i])) {
            printf("ERRO - Nao foi possivel achar algum dos sprites da explosao.");
            CloseWindow();
            return 1;
        }
    }
        
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
        bool hasWalked = false;
        if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) hasWalked = walk_right(&bomberman.local, world);
        if(!hasWalked){
            if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) hasWalked = walk_left(&bomberman.local, world);
            if(!hasWalked){
                if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) hasWalked = walk_up(&bomberman.local, world);
                if(!hasWalked){
                    if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) walk_down(&bomberman.local, world);
                }
            }
        }
        if(IsKeyPressed(KEY_B)) put_bomb(&bomberman, world, information);
        if(IsKeyPressed(KEY_P)) {bomberman.points += 100; printf("Pontos: %d\n", bomberman.points);}//trapaça pra testar record
        if(IsKeyPressed(KEY_K)) lose_life(&bomberman , information, world, letter_student, &num_enemys_on);
        if(IsKeyPressed(KEY_TAB)) pause_game(&bomberman, information, world, letter_student, &num_enemys_on);


        
        for(int i = 0; i < num_enemys_on; i++)
        {
            enemy_move(&letter_student[i], world);
        }

        
        for(int i = 0; i < MAX_BOMBS; i++)
        {
            if(bomberman.bombs[i].active)
            {
                //BOMBA EXPLODIU CUIDADOOOOOOOOOOO
                if(!bomberman.bombs[i].Exploding)
                { //se está plantada e sem explodir ainda
                    if(GetTime() - bomberman.bombs[i].planttime >= TIMER_BOMB)
                    {
                        bomberman.bombs[i].Exploding = true;
                        bomberman.bombs[i].explosion_timer = GetTime();
                        bomberman.bombs[i].explosion_frame = 0;
                        //liberando o espaço onde a bemdita estava pae
                        *(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x) = FREE;
                        *(information + 8) += 1;

                        //explosão onde a bomba estava
                        Explosion_impact(bomberman.bombs[i].local.x, bomberman.bombs[i].local.y, &bomberman, information, world, letter_student, &num_enemys_on);

                        //cara, ta dando um monte de erro, não aguento mais namoral, to ha 7 horas fazendo isso. 
                        //precisa dessas variaveis para controlar se a explosão de cima, baixo, esquereda ou direita bateram em algo
                        bool parou_cima = false;
                        bool parou_baixo = false;
                        bool parou_esquerda = false;
                        bool parou_direita = false;  

                        //Aqui é o raio da bomba (raio de distancia, não de raio), o professor especifica que tem que ser 100x100, ou seja, 5 blocos, pq cada um é 20 né
                        for(int j = 1; j <= bomberman.bombs[i].radio; j++)
                        {

                            //direção para cima 
                            ///////////////////////////////////////////////////////////////
                            if(!parou_cima)
                            {
                                if(*(*(world + bomberman.bombs[i].local.y - j) + bomberman.bombs[i].local.x) == BLOCK){
                                    parou_cima = true;
                                }else{
                                //se o bloco q sofreu impacto não é vazio...
                                if(*(*(world + bomberman.bombs[i].local.y - j) + bomberman.bombs[i].local.x) != FREE) parou_cima = true;
                                Explosion_impact(bomberman.bombs[i].local.x, bomberman.bombs[i].local.y - j, &bomberman, information, world, letter_student, &num_enemys_on);
                                }
                            }
                            //////////////////////////////////////////////////////////////
                            //Direção para baixo
                            ///////////////////////////////////////////////////////////////
                            if(!parou_baixo)
                            {
                                if(*(*(world + bomberman.bombs[i].local.y + j) + bomberman.bombs[i].local.x) == BLOCK){
                                    parou_baixo = true;
                                }else{
                                //se o bloco q sofreu impacto não é vazio...
                                if(*(*(world + bomberman.bombs[i].local.y + j)+ bomberman.bombs[i].local.x)!=FREE) parou_baixo = true;
                                Explosion_impact(bomberman.bombs[i].local.x, bomberman.bombs[i].local.y + j, &bomberman, information, world, letter_student, &num_enemys_on);
                                }
                            }
                            //////////////////////////////////////////////////////////////
                            //Direção para a esquerda
                            ///////////////////////////////////////////////////////////////
                            if(!parou_esquerda){
                                if(*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x - j) == BLOCK){
                                    parou_esquerda = true;
                                }else{
                                //se o bloco q sofreu impacto não é vazio...
                                if(*(*(world + bomberman.bombs[i].local.y)+ bomberman.bombs[i].local.x - j)!=FREE) parou_esquerda = true;
                                Explosion_impact(bomberman.bombs[i].local.x - j, bomberman.bombs[i].local.y, &bomberman, information, world, letter_student, &num_enemys_on);
                                }
                            }
                            //////////////////////////////////////////////////////////////
                            //Direção para a direita
                            ///////////////////////////////////////////////////////////////
                            if(!parou_direita){
                                if(*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x + j) == BLOCK){
                                    parou_direita = true;
                                }else{
                                //se o bloco q sofreu impacto não é vazio...
                                if(*(*(world + bomberman.bombs[i].local.y)+ bomberman.bombs[i].local.x + j)!=FREE) parou_direita = true;
                                Explosion_impact(bomberman.bombs[i].local.x + j, bomberman.bombs[i].local.y, &bomberman, information, world, letter_student, &num_enemys_on);
                                }
                            }
                            //////////////////////////////////////////////////////////////
                        }
                    }
                }
                else
                {
                    float frames_duration = TIME_ANIMATION_BOMB;
                    int num_frames = 3;

                    if(GetTime() - bomberman.bombs[i].explosion_timer >= frames_duration){
                        bomberman.bombs[i].explosion_frame++;
                        bomberman.bombs[i].explosion_timer = GetTime();
                    }
                    if(bomberman.bombs[i].explosion_frame >= num_frames){
                        bomberman.bombs[i].active = false;
                    
                        for(int y = 0; y < STATURE; y++){ //essa parte aqui nao parece estar otimizada
                            for(int x = 0; x < WIDTH; x++){
                                if(*(*(world + y)+x)==EXPLODING_WALL){
                                    *(*(world+y)+x) = FREE;
                                }
                            }
                        }
                    }
                }
            }
        }
        //PRECISA EXIBIR AS PONTUAÇÕES, O NUMERO DE BOMBAS, O DE VIDA E O de PONTOS, n sei como o henzel fez, mas precisa fazer dnv :D

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
                        case BLOCK:
                        DrawTexture(wall_spr, x*METERS, y*METERS, WHITE);
                        break;

                        case WALL:
                        DrawTexture(explosible_spr, x*METERS, y*METERS, WHITE);
                        break;

                        case EMPTY_BOX:
                        DrawTexture(box_spr, x*METERS, y*METERS, WHITE);
                        break;

                        case KEY_BOX:
                        DrawTexture(box_spr, x*METERS, y*METERS, WHITE);
                        break;

                        case KEY:
                        DrawTexture(key_spr, x*METERS, y*METERS, WHITE);
                        break;
                        
                        case EXPLODING_WALL:
                        DrawTexture(explosion_spr[0], x*METERS, y*METERS, WHITE);
                    }
                }
            }

            for(int i = 0; i< MAX_BOMBS; i++){
                if(bomberman.bombs[i].active){//se ta ativa
                    if(!bomberman.bombs[i].Exploding){//se ta plantada
                        DrawTexture(bomb_spr, bomberman.bombs[i].local.x *METERS, bomberman.bombs[i].local.y * METERS, WHITE);
                    }
                    else{//se ta fazendo kabum
                        int frame = bomberman.bombs[i].explosion_frame;
                        if(frame < 3){

                            DrawTexture(explosion_spr[frame], bomberman.bombs[i].local.x * METERS, bomberman.bombs[i].local.y * METERS, WHITE);

                            bool para_cima_desenho = false;
                            bool para_baixo_desenho = false;
                            bool para_esquerda_desenho = false;
                            bool para_direita_desenho = false;
                            for(int j = 1; j <= bomberman.bombs[i].radio; j++){
                        //////////////////////////////////////////////////////////
                        //Para cima
                        //////////////////////////////////////////////////////////    
                            if (!para_cima_desenho && bomberman.bombs[i].local.y - j >= 0) {
                            if (*(*(world + bomberman.bombs[i].local.y - j) + bomberman.bombs[i].local.x) == BLOCK) {
                                para_cima_desenho = true;
                            } else {
                                DrawTexture(explosion_spr[frame], bomberman.bombs[i].local.x * METERS, (bomberman.bombs[i].local.y - j) * METERS, WHITE);
                                // Se o bloco no mapa não for vazio, o desenho para também.
                                if (*(*(world + bomberman.bombs[i].local.y - j) + bomberman.bombs[i].local.x) != FREE) para_cima_desenho = true;
                            }
                        }
                        //////////////////////////////////////////////////////////
                        //Para baixo
                        //////////////////////////////////////////////////////////    
                            if (!para_baixo_desenho && bomberman.bombs[i].local.y + j < STATURE) {
                            if (*(*(world + bomberman.bombs[i].local.y + j) + bomberman.bombs[i].local.x) == BLOCK) {
                                para_baixo_desenho = true;
                            } else {
                                DrawTexture(explosion_spr[frame], bomberman.bombs[i].local.x * METERS, (bomberman.bombs[i].local.y + j) * METERS, WHITE);
                                // Se o bloco no mapa não for vazio, o desenho para também.
                                if (*(*(world + bomberman.bombs[i].local.y + j) + bomberman.bombs[i].local.x) != FREE) para_baixo_desenho = true;
                            }
                        }
                        //////////////////////////////////////////////////////////
                        //Para esquerda
                        //////////////////////////////////////////////////////////    
                            if (!para_esquerda_desenho && bomberman.bombs[i].local.x - j >= 0) {
                            if (*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x - j) == BLOCK) {
                                para_esquerda_desenho = true;
                            } else {
                                DrawTexture(explosion_spr[frame], (bomberman.bombs[i].local.x - j) * METERS, bomberman.bombs[i].local.y * METERS, WHITE);
                                // Se o bloco no mapa não for vazio, o desenho para também.
                                if (*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x - j) != FREE) para_esquerda_desenho = true;
                            }
                        }
                        //////////////////////////////////////////////////////////
                        //Para direita
                        //////////////////////////////////////////////////////////    
                            if (!para_direita_desenho && bomberman.bombs[i].local.x + j < WIDTH) {
                            if (*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x + j) == BLOCK) {
                                para_direita_desenho = true;
                            } else {
                                DrawTexture(explosion_spr[frame], (bomberman.bombs[i].local.x + j) * METERS, bomberman.bombs[i].local.y * METERS, WHITE);
                                // Se o bloco no mapa não for vazio, o desenho para também.
                                if (*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x + j) != FREE) para_direita_desenho = true;
                            }
                        }
                        //////////////////////////////////////////////////////////
                        }
                    }
                    }
                }
            }

            switch(bomberman.local.direction)
            {
                case UP:
                DrawTexture(player_up_spr, bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
                break;

                case RIGHT:
                DrawTexture(player_right_spr, bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
                break;

                case DOWN:
                DrawTexture(player_down_spr, bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
                break;

                case LEFT:
                DrawTexture(player_left_spr, bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
            }
            for (int i = 0; i < num_enemys_on; i++)
            {
                if(letter_student[i].ta_vivo){
                DrawTexture(player_down_spr, letter_student[i].local.x * METERS + letter_student[i].local.offsetX, letter_student[i].local.y * METERS + letter_student[i].local.offsetY, RED);
                }
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
    UnloadTexture(bomb_spr);
    free(information);
    for (int i = 0; i < 3; i++) {
        UnloadTexture(explosion_spr[i]);
    }
    for(int y = 0; y < STATURE; y++)
    {
        free(*(world + y));
    }
    free(world);
    CloseWindow();
    return 0;
}