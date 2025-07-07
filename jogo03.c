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
#define KEY_BOX 'K'
#define EMPTY_BOX 'B'
#define ENEMY 'E'
#define KEY 'F'
#define BOMB 'O'
#define EXPLODING_WALL 'X'//To tendo que adionar isso aq pq lá em baixo eu preciso checar se a parede foi destruida sem transformar em "vazio" para poder limitar até onde desenhar a merda do sprite da explosão

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define PLAYER_ANIM_SPEED 0.15
#define PLAYER_ANIM_SPEED_IDLE 1

#define MAX_BOMBS 3
#define MAX_RECORDS 10
#define TIMER_BOMB 3.0
#define TIME_ANIMATION_BOMB 0.1
#define TIME_ANIMATION_BOMB_NOT_EXPLOSION 0.5
#define IFRAMES 1


/*typedef int TIPOCHAVE;

typedef struct
{
    TIPOCHAVE chave;
} REGISTRO;

typedef struct aux
{
    REGISTRO reg;
    struct aux* next;
} ELEMENT_QUEUE;

typedef ELEMENT_QUEUE* POINTER_QUEUE;

typedef struct
{
    POINTER_QUEUE start;
    POINTER_QUEUE end;
} QUEUE;*/


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
    double walkTimer;
} enemy;

typedef struct aux
{
    enemy zombie;
    struct aux* next;
} ELEMENT_LIST;

typedef ELEMENT_LIST* POINTER_LIST;

typedef struct
{
    POINTER_LIST start;
} LIST;

typedef struct 
{
    coordinates local;
    double planttime;
    bool active;
    int radio;
    double explosion_timer;//para animação
    int explosion_frame;//para animação
    bool Exploding; //para animação
    int frame_atual_bomb;
    double animation_timer_bomb;
} bomb;

typedef struct
{
    coordinates local;
    char lifes;
    int points;
    bomb bombs[MAX_BOMBS];
    char keys;
    double lastDmg;

    int state;
    int Frame_atual;
    double animation_timer;
} player;

typedef struct
{
    char name[13];
    int points;
} record;

void bootList(LIST* horde)
{
    horde->start = NULL;
}

int sizeList(LIST* horde)
{
    POINTER_LIST find = horde->start;
    int size = 0;
    while(find != NULL)
    {
        size++;
        find = find->next;
    }
    return size;
}

void insertInList(LIST* horde, enemy zombie)
{
    POINTER_LIST new = (POINTER_LIST)malloc(sizeof(ELEMENT_LIST));
    if(!new)
    {
        puts("ERRO");
    }
    new->zombie = zombie;
    POINTER_LIST find;
    int number = sizeList(horde);
    if(number == 0)
    {
        new->next = horde->start;
        horde->start = new;
    }
    else
    {
        find = horde->start;
        for(int i = 0; i < number-1; i++) find = find->next;
        new->next = find->next;
        find->next = new;
    }
}

void takeFromList(LIST* horde, int number)
{
    POINTER_LIST find;
    POINTER_LIST erase;
    if(number == 0)
    {
        erase = horde->start;
        horde->start = erase->next;
    }
    else
    {
        find = horde->start;
        for(int i = 0; i < number-1; i++) find = find->next;
        erase = find->next;
        find->next = erase->next;
    }
    free(erase);
}

void rebootList(LIST* horde)
{
    POINTER_LIST find = horde->start;
    while(find != NULL)
    {
        POINTER_LIST erase = find;
        find = find->next;
        free(erase);
    }
    horde->start = NULL;
}

bool walkUp(coordinates *local, char **world)
{
    if(local->offsetY > -2)
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
        if(local->offsetX < -2){
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
        if(local->offsetX > 2){
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

bool checkHitbox(Rectangle plrhb, Rectangle otherhb){
    return CheckCollisionRecs(plrhb, otherhb);
}

bool walkRight(coordinates *local, char **world)
{
    if(local->offsetX < 2)
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
        if(local->offsetY < -2){
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
        if(local->offsetY > 2){
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

bool walkDown(coordinates *local, char **world)
{
    if(local->offsetY < 2)
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
        if(local->offsetX < -2){
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
        if(local->offsetX > 2){
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

bool walkLeft(coordinates *local, char **world)
{
    if(local->offsetX > -2)
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
        if(local->offsetY < -2){
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
        if(local->offsetY > 2){
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

bool newGame(player *bomberman, char **world, char *information, LIST *horde)
{
    FILE *file = fopen("world1.txt", "r");
    //FILE *file = fopen("world2.txt", "r");
    if(!file)
    {
        puts("ERRO - O nivel nao pode ser aberto");
        return false;
    }

    bomberman->lifes = '3';
    bomberman->points = 0;
    bomberman->keys = 0;
    bomberman->local.direction = DOWN;
    bomberman->local.offsetX = 0;
    bomberman->local.offsetY = 0;

    bomberman->state = 0; //inicia parado
    bomberman->Frame_atual = 0;
    bomberman->animation_timer = 0.0;

    for(int i = 0; i < MAX_BOMBS; i++){
        bomberman->bombs[i].active = false;
        bomberman->bombs[i].radio = 2;
    }

    enemy zombie;
    zombie.local.offsetX = 0;
    zombie.local.offsetY = 0;
    zombie.walkTimer = 0.0;
    rebootList(horde);

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
            zombie.local.x = squares.x;
            zombie.local.y = squares.y;
            zombie.local.direction = GetRandomValue(0, 3);
            insertInList(horde, zombie);
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
    *(information + 8) = '3';
    *(information + 21) = bomberman->lifes;
    sprintf(information + 35, "%d", bomberman->points);
    fclose(file);
    return true;
}

bool loadGame(player *bomberman, char **world, char *information, LIST *horde)
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

    rebootList(horde);
    enemy zombie;
    fread(&zombie, sizeof(enemy), 1, file);
    while(!feof(file))
    {
        insertInList(horde, zombie);
        fread(&zombie, sizeof(enemy), 1, file);
    }

    char active_bombs = '0';
    for(int i = 0; i < MAX_BOMBS; i++)
    {
        if(!bomberman->bombs[i].active) active_bombs++;
    }
    *(information + 8) = active_bombs;
    *(information + 21) = bomberman->lifes;
    sprintf(information + 35, "%d", bomberman->points);

    fclose(file);
    return true;
}

bool saveGame(player bomberman, char **world, LIST *horde)
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

    POINTER_LIST find = horde->start;
    while(find != NULL)
    {
        fwrite(&find->zombie, sizeof(enemy), 1, file);
        find = find->next;
    }

    fclose(file);
    return true;
}

void write(char *name)
{
    int letter = 0;
    while(!WindowShouldClose())
    {
        if(IsKeyPressed(KEY_A)) {*(name+letter) = 'a'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_B)) {*(name+letter) = 'b'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_C)) {*(name+letter) = 'c'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_D)) {*(name+letter) = 'd'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_E)) {*(name+letter) = 'e'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_F)) {*(name+letter) = 'f'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_G)) {*(name+letter) = 'g'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_H)) {*(name+letter) = 'h'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_I)) {*(name+letter) = 'i'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_J)) {*(name+letter) = 'j'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_K)) {*(name+letter) = 'k'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_L)) {*(name+letter) = 'l'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_M)) {*(name+letter) = 'm'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_N)) {*(name+letter) = 'n'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_O)) {*(name+letter) = 'o'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_P)) {*(name+letter) = 'p'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_Q)) {*(name+letter) = 'q'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_R)) {*(name+letter) = 'r'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_S)) {*(name+letter) = 's'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_T)) {*(name+letter) = 't'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_U)) {*(name+letter) = 'u'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_V)) {*(name+letter) = 'v'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_W)) {*(name+letter) = 'w'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_X)) {*(name+letter) = 'x'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_Y)) {*(name+letter) = 'y'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_Z)) {*(name+letter) = 'z'; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_SPACE)) {*(name+letter) = ' '; *(name+letter+1) = '|'; letter++;}
        if(IsKeyPressed(KEY_BACKSPACE)) if(letter > 0) {*(name+letter-1) = '|'; *(name+letter) = ' '; letter--;}

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("DIGITE SEU NOME", 400, 100, 50, BLACK);
            DrawText(name, 40, 100, 50, BLACK);
        EndDrawing();

        if(IsKeyPressed(KEY_ENTER) || letter == 10)
        {
            while(!IsKeyPressed(KEY_S))
            {
                BeginDrawing();
                    DrawText("O NOME ESTA CORRETO?", 40, 300, 50, BLACK);
                    DrawText("Pressione (S) para sim ou (N) para não", 40, 500, 50, BLACK);
                EndDrawing();
                if(IsKeyPressed(KEY_N))
                {
                    *(name+letter) = ' ';
                    letter = 0;
                    *(name+letter) = '|';
                    break;
                }
            }
            if(letter)
            {
                *(name+10) = ' ';
                *(name+11) = '|';
                break;
            }
        }
    }
}

void saveRecord(player bomberman)
{
    record saving;
    saving.points = bomberman.points;
    strcpy(saving.name,"|          ");
    FILE *file = fopen("record.dat", "rb+");
    if(!file)
    {
        puts("O arquivo nao existia ainda mas agora foi criado");
        file = fopen("record.dat", "wb");
        if(!file)
        {
            puts("ERRO - O record nao pode ser salvo");
        }
        while(!IsKeyPressed(KEY_ENTER))
        {
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("VOCÊ É O PRIMEIRO A SALVAR SUA PONTUAÇÃO", 300, 300, 50, BLACK);
                DrawText("Pressione ENTER para continuar", 300, 500, 50, BLACK);
            EndDrawing();
        }
        write(saving.name);
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
        record *reading = (record *)malloc(MAX_RECORDS*sizeof(record));
        if(!reading)
        {
            puts("ERRO");
        }
        int position = 10;
        for(int i = 0; i < MAX_RECORDS; i++)
        {
            fread(reading+i, sizeof(record), 1, file);
            if(position == 10) if((reading+i)->points < saving.points) position = i;
        }
        if(position != 10)
        {
            fseek(file, position*sizeof(record), SEEK_SET);
            while(!IsKeyPressed(KEY_ENTER))
            {
                BeginDrawing();
                    ClearBackground(RAYWHITE);
                    if(!position) DrawText("PARABENS SUA PONTUAÇÃO É A MELHOR", 300, 300, 50, BLACK);
                    else DrawText(TextFormat("PARABENS SUA PONTUAÇÃO É A %dº MELHOR", position+1), 300, 300, 50, BLACK);
                    DrawText("Pressione ENTER para continuar", 300, 500, 50, BLACK);
                EndDrawing();
            }
            write(saving.name);
            fwrite(&saving, sizeof(record), 1, file);
            for(int j = position; j < 9; j++)
            {
                fwrite(reading+j, sizeof(record), 1, file);
            }
            puts("Record salvo com sucesso");
        }
        else
        {
            puts("Nao obteve uma pontuacao suficiente para ser salva");
        }
        free(reading);
    }
    fclose(file);
}

void showRecords(Music musicmenu)
{
    FILE *file = fopen("record.dat", "rb");
    if(!file)
    {
        while(!IsKeyPressed(KEY_V))
        {
            UpdateMusicStream(musicmenu);
            if(WindowShouldClose()) CloseWindow();
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("RECORDS", 50, 10, 50, RED);
                DrawText("Nome do jogador | pontuação", 350, 15, 40, RED);
                DrawText(" 1º ---------- | 0", 50, 100, 50, BLACK);
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
        record *reading = (record *)malloc(MAX_RECORDS*sizeof(record));
        if(!reading)
        {
            puts("ERRO");
        }
        for(int i = 0; i < MAX_RECORDS; i++)
        {
            fread(reading+i, sizeof(record), 1, file);
        }
        while(!IsKeyPressed(KEY_V))
        {
            UpdateMusicStream(musicmenu);
            if(WindowShouldClose()) CloseWindow();
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("RECORDS", 50, 10, 50, RED);
                DrawText("Nome do jogador | pontuação", 350, 15, 40, RED);
                int position = 0;
                for(int j = 1; position < MAX_RECORDS/2; j++)
                {
                    DrawText(TextFormat("%dº %s %d", position+1, (reading+position)->name, (reading+position)->points), 40, 100*j, 50, BLACK);
                    position++;
                }
                for(int j = 1; position < MAX_RECORDS; j++)
                {
                    DrawText(TextFormat("%dº %s %d", position+1, (reading+position)->name, (reading+position)->points), 600, 100*j, 50, BLACK);
                    position++;
                }
            EndDrawing();
        }
        free(reading);
    }
    fclose(file);
}

void showControls(Music musicmenu)
{
    while(!IsKeyPressed(KEY_V))
    {
        UpdateMusicStream(musicmenu);
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

void showDevs(Music musicmenu)
{
    while(!IsKeyPressed(KEY_V))
    {
        UpdateMusicStream(musicmenu);
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

void putBomb(player *bomberman, char **world, char *information)
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
            bomberman->bombs[i].frame_atual_bomb = 0;      // para animação
            bomberman->bombs[i].animation_timer_bomb = GetTime(); // para aniamção
            bomberman->bombs[i].Exploding = false;//para animação
            bomberman->bombs[i].explosion_frame = 0; //para animação
            bomberman->bombs[i].explosion_timer = 0.0; //para animação
            *(information + 8) -= 1;
            }
        }
    }
}

void enemyMove(enemy *zombie, char **world)
{
    bool arrived = false;
    switch(zombie->local.direction)
    {
        case UP:
            arrived = !walkUp(&zombie->local, world);
            break;
        case RIGHT:
            arrived = !walkRight(&zombie->local, world);
            break;
        case DOWN:
            arrived = !walkDown(&zombie->local, world);
            break;
        case LEFT:
            arrived = !walkLeft(&zombie->local, world);
    }
    bool walkRandomly = false;
    if(zombie->local.offsetX == 0 || zombie->local.offsetY == 0){
        if(GetRandomValue(0,2) == 0) walkRandomly = true;
    }
    if (GetTime() - zombie->walkTimer >= 3 || arrived || walkRandomly)
    {
        zombie->local.direction = GetRandomValue(0, 3);
        zombie->walkTimer = GetTime();
    }
}

void menu(player *bomberman, char **world, char *information, LIST *horde, Music musicmenu)
{
    coordinates arrow = {0, 0, 0, 0, 0};
    while(!arrow.direction)
    {
        UpdateMusicStream(musicmenu);
        if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) arrow.y = (arrow.y+2)%3;
        if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) arrow.y = (arrow.y+1)%3;
        if(IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) arrow.x = (arrow.x+1)%2;
        if(IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) arrow.x = (arrow.x+1)%2;
        if(IsKeyPressed(KEY_ENTER))
        {
            if(arrow.y == 0 && arrow.x == 0)
            {
                if(newGame(bomberman, world, information, horde)) break;
            }
            if(arrow.y == 1 && arrow.x == 0)
            {
                if(loadGame(bomberman, world, information, horde)) break;
                else
                {
                    /*BeginDrawing();
                        DrawText("Nao ha nenhum jogo salvo", 130, 500, 70, BLACK); inserir mensagem temporaria com pilha? aqui e no pause/gameover screen
                    EndDrawing();*/
                }
            }
            if(arrow.y == 2 && arrow.x == 0) showRecords(musicmenu);
            if(arrow.y == 0 && arrow.x == 1) showControls(musicmenu);
            if(arrow.y == 1 && arrow.x == 1) showDevs(musicmenu);
            if(arrow.y == 2 && arrow.x == 1)
            {
                CloseWindow();
            }
        }
        if(WindowShouldClose() || IsKeyPressed(KEY_Q)) CloseWindow();
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
}

void loseLife(player *bomberman, char *information, char **world, LIST *horde, Sound loss, Sound lostlife, Music musicmenu)
{
    if(GetTime() - bomberman->lastDmg >= IFRAMES){
        bomberman->lastDmg = GetTime();
        if(bomberman->points >=100) bomberman->points -= 100;
        else bomberman->points = 0;
        sprintf(information + 35 , "%d", bomberman->points);
        bomberman->lifes -= 1;
        *(information + 21) -= 1;
        if(bomberman->lifes <= '0')
        {
            PlaySound(loss);
            while(!IsKeyPressed(KEY_B))
            {
                if(IsKeyPressed(KEY_Q) || WindowShouldClose()) CloseWindow();
                BeginDrawing();
                    DrawText("GAME OVER", 130, 220, 150, RED);
                    DrawText("Precione B para continuar", 100, 320, 80, RED);
                    DrawRectangle(0, STATURE*METERS, WIDTH*METERS, 5*METERS,RAYWHITE);
                    DrawText(information, 20, 520, 60, GREEN);
                EndDrawing();
            }
            saveRecord(*bomberman);
            menu(bomberman, world, information, horde, musicmenu);
        }
        else PlaySound(lostlife);
    }
}

void Explosion_impact(int x, int y, player *bomberman, char *information, char **world, LIST *horde){
    //bem simples também, apenas passo pelo número de inimigos ativo e desativo eles. Aproveito e aumento os points dele, para facilitar o trabalho do Henzel (minha vez de roubar trampo muahahahahhahah)
    POINTER_LIST find = horde->start;
    for(int number = 0; find != NULL; number++){
        if(find->zombie.local.x == x && find->zombie.local.y == y){
            takeFromList(horde, number);
            bomberman->points +=20;
            sprintf(information + 35 , "%d", bomberman->points);
        }
        find = find->next;
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

void pauseGame(player *bomberman, char *information, char **world, LIST *horde, Music musicmenu)
{
    while(!IsKeyPressed(KEY_V))
    {
        if(IsKeyPressed(KEY_N))
        {
            if(newGame(bomberman, world, information, horde)) break;
        }
        if(IsKeyPressed(KEY_C))
        {
            if(loadGame(bomberman, world, information, horde)) break;
        }
        if(IsKeyPressed(KEY_M))
        {
            menu(bomberman, world, information, horde, musicmenu);
            break;
        }
        if(IsKeyPressed(KEY_S)) saveGame(*bomberman, world, horde);
        if(IsKeyPressed(KEY_R))
        {
            showRecords(musicmenu);
            break;
        }
        if(IsKeyPressed(KEY_K))
        {
            showControls(musicmenu);
            break;
        }
        if(IsKeyPressed(KEY_Q) || WindowShouldClose()) CloseWindow();
        BeginDrawing();
            DrawText("PAUSE", 130, 220, 150, RED);
        EndDrawing();
    }
}

int main()
{
    player bomberman;
    bomberman.lastDmg = 0;
    LIST horde;
    bootList(&horde);

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
    InitAudioDevice();
    Music musicmap1 = LoadMusicStream("Musicas/Songs/Mapa1.ogg");
    Music musicmap2 = LoadMusicStream("Musicas/Songs/Mapa2.ogg");
    Music musicmenu = LoadMusicStream("Musicas/Songs/Menu.ogg");
    Sound losssound = LoadSound("Musicas/SoundFX/Derrota.ogg");
    Sound lostlifesound = LoadSound("Musicas/SoundFX/LoseLife.ogg");
    Sound obtainedkeysound = LoadSound("Musicas/SoundFX/ObtainedKey.ogg");
    Sound victorysound = LoadSound("Musicas/SoundFX/Vitoria.ogg");
    if (musicmap1.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
    if (musicmap2.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
        if (musicmenu.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
    if (losssound.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
    if (lostlifesound.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
    if (obtainedkeysound.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
    if (victorysound.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
    musicmenu.looping = true;
    musicmap1.looping = true;
    musicmap2.looping = true;
    PlayMusicStream(musicmenu);
    menu(&bomberman, world, information, &horde, musicmenu);
    PlayMusicStream(musicmap1);
    //código abaixo para carregar as sprites. Extremamente importante esse código estar depois do initwindow
    Texture2D wall_spr = LoadTexture("Sprites/cenario/parede.png");
    Texture2D box_spr = LoadTexture("Sprites/cenario/caixa.png");
    Texture2D explosible_spr = LoadTexture("Sprites/cenario/parede_destrutivel.png");
    Texture2D key_spr = LoadTexture("Sprites/interagiveis/chave.png");
    Texture2D floor_spr = LoadTexture("Sprites/cenario/chao.png");
    Texture2D bomb_animation[2];
    bomb_animation[0] = LoadTexture("Sprites/timing/bomb_black.png");
    bomb_animation[1] = LoadTexture("Sprites/timing/bomb_red.png");


    Texture2D explosion_spr[3];
    explosion_spr[0] = LoadTexture("Sprites/explosao/explosao1.png");
    explosion_spr[1] = LoadTexture("Sprites/explosao/explosao2.png");
    explosion_spr[2] = LoadTexture("Sprites/explosao/explosao3.png");
    Texture2D player_walk_down[4];
    player_walk_down[0] = LoadTexture("Sprites/Player/Felix_walk_down1.png");
    player_walk_down[1] = LoadTexture("Sprites/Player/Felix_walk_down2.png");
    player_walk_down[2] = LoadTexture("Sprites/Player/Felix_walk_down3.png");
    player_walk_down[3] = LoadTexture("Sprites/Player/Felix_walk_down4.png");
    Texture2D player_walk_up[4];
    player_walk_up[0] = LoadTexture("Sprites/Player/Felix_walk_up1.png");
    player_walk_up[1] = LoadTexture("Sprites/Player/Felix_walk_up2.png");
    player_walk_up[2] = LoadTexture("Sprites/Player/Felix_walk_up3.png");
    player_walk_up[3] = LoadTexture("Sprites/Player/Felix_walk_up4.png");
    Texture2D player_walk_left[4];
    player_walk_left[0] = LoadTexture("Sprites/Player/Felix_walk_esq1.png");
    player_walk_left[1] = LoadTexture("Sprites/Player/Felix_walk_esq2.png");
    player_walk_left[2] = LoadTexture("Sprites/Player/Felix_walk_esq3.png");
    player_walk_left[3] = LoadTexture("Sprites/Player/Felix_walk_esq4.png");
    Texture2D player_walk_rigth[4];
    player_walk_rigth[0] = LoadTexture("Sprites/Player/Felix_walk_dir1.png");
    player_walk_rigth[1] = LoadTexture("Sprites/Player/Felix_walk_dir2.png");
    player_walk_rigth[2] = LoadTexture("Sprites/Player/Felix_walk_dir3.png");
    player_walk_rigth[3] = LoadTexture("Sprites/Player/Felix_walk_dir4.png");
    Texture2D player_idle_down[2];
    player_idle_down[0] = LoadTexture("Sprites/Player/Felix_idle_down1.png");
    player_idle_down[1] = LoadTexture("Sprites/Player/Felix_idle_down2.png");
    Texture2D player_idle_up[2];
    player_idle_up[0] = LoadTexture("Sprites/Player/Felix_idle_up1.png");
    player_idle_up[1] = LoadTexture("Sprites/Player/Felix_idle_up2.png");
    Texture2D player_idle_left[2];
    player_idle_left[0] = LoadTexture("Sprites/Player/Felix_idle_esq1.png");
    player_idle_left[1] = LoadTexture("Sprites/Player/Felix_idle_esq2.png");
    Texture2D player_idle_right[2];
    player_idle_right[0] = LoadTexture("Sprites/Player/Felix_idle_dir1.png");
    player_idle_right[1] = LoadTexture("Sprites/Player/Felix_idle_dir2.png");


    //segundo o site oficial, checa se a textura e valida e esta carregada na GPU, retorna TRUE 
    if(!IsTextureValid(bomb_animation[0])){
        puts("ERRO - Nao foi possivel achar o sprite 1 da bomba.");
        CloseWindow();
        return -1;
    }
        if(!IsTextureValid(bomb_animation[1])){
        puts("ERRO - Nao foi possivel achar o sprite 2 da bomba.");
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
     if (!IsTextureValid(player_walk_down[0])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_down1.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_down[1])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_down2.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_down[2])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_down3.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_down[3])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_down4.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_up[0])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_up1.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_up[1])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_up2.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_up[2])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_up3.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_up[3])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_up4.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_left[0])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_esq1.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_left[1])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_esq2.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_left[2])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_esq3.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_left[3])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_esq4.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_rigth[0])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_dir1.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_rigth[1])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_dir2.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_rigth[2])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_dir3.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_walk_rigth[3])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_walk_dir4.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_idle_down[0])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_idle_down1.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_idle_down[1])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_idle_down2.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_idle_up[0])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_idle_up1.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_idle_up[1])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_idle_up2.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_idle_left[0])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_idle_esq1.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_idle_left[1])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_idle_esq2.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_idle_right[0])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_idle_dir1.png");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(player_idle_right[1])) {
        puts("ERRO - Nao foi possivel achar o sprite Felix_idle_dir2.png");
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
        UpdateMusicStream(musicmap1);
        bool tamovendo = false;
            if (IsKeyPressed(KEY_F11)) ToggleFullscreen();
    
        if(IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)){
            if(walkRight(&bomberman.local, world)){
                 bomberman.local.direction = RIGHT;
                 tamovendo=true;
            }
        }
        if(IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)){
            if(walkLeft(&bomberman.local, world)){
                bomberman.local.direction = LEFT;
                tamovendo = true;
            }
        }
        if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)){
            if(walkUp(&bomberman.local, world)){
                bomberman.local.direction = UP;
                tamovendo = true;
            }
        }
        if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)){
            if(walkDown(&bomberman.local, world)){
                bomberman.local.direction = DOWN;
                tamovendo = true;
            }
        }
        if(tamovendo == true){
            if(bomberman.state != 1){ //to usando 0 para parado e 1 para andando
                bomberman.state = 1;
                bomberman.Frame_atual = 0; //reinicia o frame ao mudar o estado do meu mano
            }
        }
        else{
            if(bomberman.state != 0){
                bomberman.state = 0;
                bomberman.Frame_atual = 0;
            }
        }
    if (bomberman.state == 1) { 
        if (GetTime() - bomberman.animation_timer > PLAYER_ANIM_SPEED) {
            bomberman.Frame_atual++;
            bomberman.animation_timer = GetTime();
        }
        if (bomberman.Frame_atual >= 4) {
            bomberman.Frame_atual = 0;
        }
    } 
    else {
        if (GetTime() - bomberman.animation_timer > PLAYER_ANIM_SPEED_IDLE) {
            bomberman.Frame_atual++;
            bomberman.animation_timer = GetTime();
        }
        if (bomberman.Frame_atual >= 2) {
            bomberman.Frame_atual = 0;
        }
    }
        if(IsKeyPressed(KEY_B)) putBomb(&bomberman, world, information);
        if(IsKeyPressed(KEY_TAB)){
            double *times = (double*)malloc(sizeof(double)*MAX_BOMBS);
            for(int i = 0; i < MAX_BOMBS; i++) *(times+i) = TIMER_BOMB - (GetTime() - bomberman.bombs[i].planttime);
            pauseGame(&bomberman, information, world, &horde, musicmenu);
            for(int i = 0; i < MAX_BOMBS; i++) bomberman.bombs[i].planttime = GetTime() - (TIMER_BOMB - *(times+i));
            free(times);
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
                        Explosion_impact(bomberman.bombs[i].local.x, bomberman.bombs[i].local.y, &bomberman, information, world, &horde);

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
                                Explosion_impact(bomberman.bombs[i].local.x, bomberman.bombs[i].local.y - j, &bomberman, information, world, &horde);
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
                                Explosion_impact(bomberman.bombs[i].local.x, bomberman.bombs[i].local.y + j, &bomberman, information, world, &horde);
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
                                Explosion_impact(bomberman.bombs[i].local.x - j, bomberman.bombs[i].local.y, &bomberman, information, world, &horde);
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
                                Explosion_impact(bomberman.bombs[i].local.x + j, bomberman.bombs[i].local.y, &bomberman, information, world, &horde);
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

        BeginDrawing();
            ClearBackground(RAYWHITE);
            //adicionando textura no chao inteiro
            //primeiro eu crio um triangulo do sizeList da tela
            Rectangle sourceRec = {0.0, 0.0, WIDTH*METERS, 500}; // o motivo de ser 500 e não HEIGHT e pq ali em baixo fica a hud ne, se quiser entender melhor troca isso para "HEIGHT"
            //esse código desenha o sprite no retangulo criado. Como o sprite(tam: 20x20) é bem menor que o retangulo (tam: tela), ele vai preenchendo até cobrir tudo
            Rectangle playerhb = {bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, 16, 16};
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
                        Rectangle keyhb = {20, 20, x*METERS, y*METERS};
                        if(checkHitbox(playerhb, keyhb)){
                            PlaySound(obtainedkeysound);
                            bomberman.keys += 1;
                            *(*(world + y) + x) = FREE;
                        }
                        else{
                            DrawTexture(key_spr, x*METERS, y*METERS, WHITE);
                        }
                        break;
                        
                        case EXPLODING_WALL:
                        DrawTexture(explosion_spr[0], x*METERS, y*METERS, WHITE);
                    }
                }
            }

            for(int i = 0; i< MAX_BOMBS; i++){
                if(bomberman.bombs[i].active){//se ta ativa
                    if(!bomberman.bombs[i].Exploding){//se ta plantada
                        DrawTexture(bomb_animation[bomberman.bombs[i].frame_atual_bomb], bomberman.bombs[i].local.x*METERS, bomberman.bombs[i].local.y*METERS, WHITE);
                        if(GetTime() - bomberman.bombs[i].animation_timer_bomb > TIME_ANIMATION_BOMB_NOT_EXPLOSION){
                            bomberman.bombs[i].frame_atual_bomb ++;
                            bomberman.bombs[i].animation_timer_bomb = GetTime();
                        }
                        if(bomberman.bombs[i].frame_atual_bomb >= 2){
                            bomberman.bombs[i].frame_atual_bomb = 0;
                        }
                    }
                    else{//se ta fazendo kabum
                        int frame = bomberman.bombs[i].explosion_frame;
                        if(frame < 3){

                            DrawTexture(explosion_spr[frame], bomberman.bombs[i].local.x*METERS, bomberman.bombs[i].local.y*METERS, WHITE);
                            Rectangle exphb = {20,20, bomberman.bombs[i].local.x*METERS, bomberman.bombs[i].local.y*METERS};
                            if(checkHitbox(playerhb, exphb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu);
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
                                DrawTexture(explosion_spr[frame], bomberman.bombs[i].local.x*METERS, (bomberman.bombs[i].local.y - j)*METERS, WHITE);
                                Rectangle exphb = {20,20, bomberman.bombs[i].local.x*METERS, (bomberman.bombs[i].local.y - j)*METERS};
                                if(checkHitbox(playerhb, exphb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu);
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
                                DrawTexture(explosion_spr[frame], bomberman.bombs[i].local.x*METERS, (bomberman.bombs[i].local.y + j)*METERS, WHITE);
                                Rectangle exphb = {20,20, bomberman.bombs[i].local.x*METERS, (bomberman.bombs[i].local.y + j)*METERS};
                                if(checkHitbox(playerhb, exphb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu);
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
                                DrawTexture(explosion_spr[frame], (bomberman.bombs[i].local.x - j)*METERS, bomberman.bombs[i].local.y*METERS, WHITE);
                                Rectangle exphb = {20,20, (bomberman.bombs[i].local.x - j)*METERS, bomberman.bombs[i].local.y*METERS};
                                if(checkHitbox(playerhb, exphb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu);
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
                                DrawTexture(explosion_spr[frame], (bomberman.bombs[i].local.x + j)*METERS, bomberman.bombs[i].local.y*METERS, WHITE);
                                Rectangle exphb = {20,20, (bomberman.bombs[i].local.x + j)*METERS, bomberman.bombs[i].local.y*METERS};
                                if(checkHitbox(playerhb, exphb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu);
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
                if(bomberman.state == 1){
                DrawTexture(player_walk_up[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
                }
                else{
                    DrawTexture(player_idle_up[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
                }
                break;

                case RIGHT:
                if(bomberman.state == 1){
                DrawTexture(player_walk_rigth[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
                }
                else{
                    DrawTexture(player_idle_right[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
                }
                break;

                case DOWN:
                if(bomberman.state == 1){
                DrawTexture(player_walk_down[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
                }
                else{
                    DrawTexture(player_idle_down[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
                }
                break;

                case LEFT:
                if(bomberman.state == 1){
                DrawTexture(player_walk_left[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
                }
                else{
                     DrawTexture(player_idle_left[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX, bomberman.local.y*METERS + bomberman.local.offsetY, WHITE);
                }
            }
            POINTER_LIST alive = horde.start;
            while(alive != NULL)
            {
                enemyMove(&alive->zombie, world);
                DrawTexture(player_walk_rigth[0], alive->zombie.local.x*METERS + alive->zombie.local.offsetX, alive->zombie.local.y*METERS + alive->zombie.local.offsetY, RED);
                Rectangle enemyhb = {alive->zombie.local.x*METERS + alive->zombie.local.offsetX, alive->zombie.local.y*METERS + alive->zombie.local.offsetY, 16, 16};
                if(checkHitbox(playerhb, enemyhb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu);
                alive = alive->next;
            }
            DrawText(information, 20, 520, 60, GREEN);
        EndDrawing();
    }

    //Necessário para descarregar as texturas
    for (int i = 0; i < 4; i++) {
        UnloadTexture(player_walk_down[i]);
        UnloadTexture(player_walk_up[i]);
        UnloadTexture(player_walk_left[i]);
        UnloadTexture(player_walk_rigth[i]);
    }
    for (int i = 0; i < 2; i++) {
        UnloadTexture(player_idle_down[i]);
        UnloadTexture(player_idle_up[i]);
        UnloadTexture(player_idle_left[i]);
        UnloadTexture(player_idle_right[i]);
    }
    UnloadTexture(bomb_animation[0]);
    UnloadTexture(bomb_animation[1]);
    UnloadTexture(wall_spr);
    UnloadTexture(explosible_spr);
    UnloadTexture(box_spr);
    UnloadTexture(key_spr);
    UnloadTexture(floor_spr);
    UnloadMusicStream(musicmap1);
    UnloadMusicStream(musicmap2);
    UnloadMusicStream(musicmenu);
    CloseAudioDevice();
    free(information);
    for (int i = 0; i < 3; i++) {
        UnloadTexture(explosion_spr[i]);
    }
    for(int y = 0; y < STATURE; y++)
    {
        free(*(world + y));
    }
    rebootList(&horde);
    free(world);
    CloseWindow();
    return 0;
}