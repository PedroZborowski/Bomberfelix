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
#define TRAPDOOR 'N'
#define EXPLODING_WALL 'X'

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define PLAYER_ANIM_SPEED 0.15
#define PLAYER_ANIM_SPEED_IDLE 1
#define ENEMY_ANIM_SPEED 0.15
#define ANIM_SPEED_TRANS 0.15
#define ANIM_TROFEU_SPEED 0.4

#define MAX_BOMBS 3
#define MAX_RECORDS 10
#define TIMER_BOMB 3.0
#define TIME_ANIMATION_BOMB 0.1
#define TIME_ANIMATION_BOMB_NOT_EXPLOSION 0.5
#define emoteCooldown 3.5
#define IFRAMES 3
#define NUM_EMOTES 11

//Coordenadas de um objeto qualquer
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
    int frame_atual_walk; // para animação
    double animation_timer_enemy; //para animação
} enemy;

//Lista encadeada de inimigos que sao zumbis, chamaremos de horda
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
    int level;
    double lastDmg;
    double lastEmoteTime;
    int state;
    int Frame_atual;
    double animation_timer;
    double invincibletimer;
    int invinciblestate;
} player;

typedef struct
{
    char name[13];
    int points;
} record;

//Inicializa a lista encadeada de zumbis
void bootList(LIST* horde)
{
    horde->start = NULL;
}

//Verifica o tamanho da horda
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

//Insere um zumbi na horda
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

//Remove um zumbi da horda
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

//Reinicializa a lista encadeada de zumbis
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

//Verifica se dois objetos colidem
bool checkHitbox(Rectangle plrhb, Rectangle otherhb){
    return CheckCollisionRecs(plrhb, otherhb);
}

//Resumo das funções de movimento: ele precisa checar se o player está exatamente no meio e, ao andar,
//só precisa checar o quadrado na direção dele, ou se ele está entre dois quadrados e ele precisaria checar esses dois.

//Movimenta o objeto para cima
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

    if(*(*(world + local->y - 1) + local->x) == FREE || *(*(world + local->y - 1) + local->x) == KEY)
    {  
        //Se ele estiver à esquerda do meio
        if(local->offsetX < -2){
            if(*(*(world + local->y - 1) + local->x - 1) == FREE || *(*(world + local->y - 1) + local->x - 1) == KEY)
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
            if(*(*(world + local->y - 1) + local->x + 1) == FREE || *(*(world + local->y - 1) + local->x + 1) == KEY)
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

//Movimenta o objeto para a direita
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

    if(*(*(world + local->y) + local->x + 1) == FREE || *(*(world + local->y) + local->x + 1) == KEY)
    {
        //Se ele estiver acima do meio
        if(local->offsetY < -2){
            if(*(*(world + local->y - 1) + local->x + 1) == FREE || *(*(world + local->y - 1) + local->x + 1) == KEY)
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
            if(*(*(world + local->y + 1) + local->x + 1) == FREE || *(*(world + local->y + 1) + local->x + 1) == KEY)
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

//Movimenta o objeto para baixo
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
    
    if(*(*(world + local->y + 1) + local->x) == FREE || *(*(world + local->y + 1) + local->x) == KEY)
    {
        //Se ele estiver à esquerda do meio
        if(local->offsetX < -2){
            if(*(*(world + local->y + 1) + local->x - 1) == FREE || *(*(world + local->y + 1) + local->x - 1) == KEY)
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
            if(*(*(world + local->y + 1) + local->x + 1) == FREE || *(*(world + local->y + 1) + local->x + 1) == KEY)
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

//Movimenta o objeto para a esquerda
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

    if(*(*(world + local->y) + local->x - 1) == FREE || *(*(world + local->y) + local->x - 1) == KEY)
    {
        //Se ele estiver acima do meio
        if(local->offsetY < -2){
            if(*(*(world + local->y - 1) + local->x - 1) == FREE || *(*(world + local->y - 1) + local->x - 1) == KEY)
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
            if(*(*(world + local->y + 1) + local->x - 1) == FREE || *(*(world + local->y + 1) + local->x - 1) == KEY)
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

//Le o arquivo do nivel 1 gerando o mesmo, reseta todas as outras variaveis
bool newGame(player *bomberman, char **world, char *information, LIST *horde, coordinates *trapdoor, char *argv[], coordinates *stairs)
{
    //Abrir arquivo e tratamento de erros
    FILE *file = fopen(argv[0], "r");
    if(!file)
    {
        puts("ERRO - O nivel nao pode ser aberto");
        return false;
    }
    //inicializações
    bomberman->lifes = '3';
    bomberman->points = 0;
    bomberman->keys = 0;
    bomberman->level = 1;
    bomberman->local.direction = DOWN;
    bomberman->local.offsetX = 0;
    bomberman->local.offsetY = 0;
    bomberman->lastDmg = -10;
    bomberman->lastEmoteTime = GetTime();
    bomberman->state = 0; //inicia parado
    bomberman->Frame_atual = 0;
    bomberman->animation_timer = GetTime();
    bomberman->invinciblestate = 1;
    bomberman->invincibletimer = GetTime();
    for(int i = 0; i < MAX_BOMBS; i++){
        bomberman->bombs[i].active = false;
        bomberman->bombs[i].radio = 2;
    }

    trapdoor->offsetX = 0;
    trapdoor->offsetY = 0;
    trapdoor->direction = DOWN;

    stairs->offsetX = 0;
    stairs->offsetY = 0;
    stairs->direction = DOWN;
    stairs->x = -1;
    stairs->y = -1;

    enemy zombie;
    zombie.local.offsetX = 0;
    zombie.local.offsetY = 0;
    zombie.walkTimer = 0.0;
    zombie.animation_timer_enemy = GetTime();
    zombie.frame_atual_walk = 0;
    rebootList(horde);

    coordinates squares; //leitor do arquivo
    squares.x = 0;
    squares.y = 0;
    char object = fgetc(file);
    //Loop pra carregar o mapa
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

            case TRAPDOOR:
            trapdoor->x = squares.x;
            trapdoor->y = squares.y;
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

//Salva as informacoes atuais do jogo em um arquivo binario
bool saveGame(player bomberman, char **world, LIST *horde, coordinates trapdoor)
{
    //Abre o arquivo pra salvar o jogo
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

    //Escreve cada informação crua do "world" binário
    for(int y = 0; y < STATURE; y++)
    {
        for(int x = 0; x < WIDTH; x++)
        {
            fwrite(*(world + y) + x, sizeof(char), 1, file);
        }
    }

    //Coloca um \n como separador para recapturar na hora da leitura, e escreve as informações dos players, do alçapão e dos inimigos.
    char split = '\n';
    fwrite(&split, sizeof(char), 1, file);
    fwrite(&bomberman, sizeof(player), 1, file);
    fwrite(&trapdoor, sizeof(coordinates), 1, file);

    POINTER_LIST find = horde->start;
    while(find != NULL)
    {
        fwrite(&find->zombie, sizeof(enemy), 1, file);
        find = find->next;
    }

    fclose(file);
    return true;
}

//Carrega as informacoes salvas anteriormente em um arquivo binario
bool loadGame(player *bomberman, char **world, char *information, LIST *horde, coordinates *trapdoor)
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
    //Daqui para baixo, carrega de acordo com o que foi salvo pela função acima (vai até o \n carregando o mapa depois pega o resto das informações em ordem)
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
    fread(trapdoor, sizeof(coordinates), 1, file);
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

//Recebe do usuario um nome de ate 10 digitos exibindo na tela o mesmo enquanto é escrito
void writeName(char *name, Texture2D digiteseunome)
{
    int letter = 0;
    //Detecta as letras pressionadas
    while(letter >= 0)
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
        if(WindowShouldClose()) CloseWindow();

        //Insere o nome
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(digiteseunome, 0, 0, WHITE);
            DrawText(name, 500, 200, 50, WHITE);
        EndDrawing();

        if(IsKeyPressed(KEY_ENTER) || letter == 10)
        {
            while(!IsKeyPressed(KEY_S))
            {
                BeginDrawing();
                    DrawText("O NOME ESTA CORRETO?", 40, 300, 50, WHITE);
                    DrawText("Pressione (S) para sim ou (N) para não", 40, 500, 50, WHITE);
                EndDrawing();
                if(IsKeyPressed(KEY_N))
                {
                    strcpy(name,"|          ");
                    letter = 0;
                    break;
                }
            }
            if(letter)
            {
                *(name+letter) = ' ';
                *(name+10) = ' ';
                *(name+11) = '|';
                break;
            }
        }
    }
}

//Verifica se a pontuacao do usuario devera entrar nos records
//Caso deva salva o record em um arquivo binario
void saveRecord(player bomberman, Sound recordsound, Texture2D *felixtrofeu, Texture2D digiteseunome)
{
    PlaySound(recordsound);
    bool gifdone = false;
    double timer_trans = GetTime();
    int frameatual = 0;

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
        while(!gifdone){
        if(GetTime() - timer_trans > ANIM_TROFEU_SPEED){
            frameatual++;
            timer_trans = GetTime();
        }
        if (frameatual >= 3){
            gifdone = true;
        }
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(felixtrofeu[frameatual], 0, 0, WHITE);
        EndDrawing();
        }
        while (!IsKeyPressed(KEY_ENTER)){
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(felixtrofeu[2], 0, 0, WHITE);
        EndDrawing();
        }
        float cont = GetTime();
        while(GetTime() - cont < 0.167){
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTexture(felixtrofeu[2], 0, 0, WHITE);
            EndDrawing();
        }
        writeName(saving.name, digiteseunome);
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
            PlaySound(recordsound);
            while(!gifdone){
        if(GetTime() - timer_trans > ANIM_TROFEU_SPEED){
            frameatual++;
            timer_trans = GetTime();
        }
        if (frameatual >= 3){
            gifdone = true;
        }
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(felixtrofeu[frameatual], 0, 0, WHITE);
        EndDrawing();
        }
        while (!IsKeyPressed(KEY_ENTER)){
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexture(felixtrofeu[2], 0, 0, WHITE);
        EndDrawing();
        }
        float cont = GetTime();
        while(GetTime() - cont < 0.167){
            BeginDrawing();
                ClearBackground(BLACK);
                DrawTexture(felixtrofeu[2], 0, 0, WHITE);
            EndDrawing();
        }
            writeName(saving.name, digiteseunome);
            fwrite(&saving, sizeof(record), 1, file);
            for(int j = position; j < 9; j++)
            {
                fwrite(reading+j, sizeof(record), 1, file);
            }
        }
        free(reading);
    }
    fclose(file);
}

//Le o arquivo binario records para exibir as informações armazenadas
void showRecords(Music musicmenu, Texture2D telarecordes)
{
    FILE *file = fopen("record.dat", "rb");
    if(!file)
    {
        while(!IsKeyPressed(KEY_V))
        {
            UpdateMusicStream(musicmenu);
            if(WindowShouldClose() || IsKeyPressed(KEY_Q)) CloseWindow();
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawTexture(telarecordes, 0, 0, WHITE);
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
            if(WindowShouldClose() || IsKeyPressed(KEY_Q)) CloseWindow();
            BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawTexture(telarecordes, 0, 0, WHITE);
                int position = 0;
                for(int j = 1; position < MAX_RECORDS/2; j++)
                {
                    DrawText(TextFormat("%s %d",(reading+position)->name, (reading+position)->points), 190, 210 +(j-1)*60, 40, WHITE);
                    position++;
                }
                for(int j = 1; position < MAX_RECORDS; j++)
                {
                    DrawText(TextFormat("%s %d",(reading+position)->name, (reading+position)->points), 700, 210 +(j-1)*60, 40, WHITE);
                    position++;
                }
            EndDrawing();
        }
        free(reading);
    }
    fclose(file);
}

//Mostra ao usuario os controles
void showControls(Music musicmenu, Texture2D telacontroles)
{
    //Prende na tela dos controles
    while(!IsKeyPressed(KEY_V))
    {
        UpdateMusicStream(musicmenu);
        if(WindowShouldClose() || IsKeyPressed(KEY_Q)) CloseWindow();
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(telacontroles, 0, 0, WHITE);
        EndDrawing();
    }
}

//Mostra ao usuario os desenvolvedores
void showDevs(Music musicmenu, Texture2D telacriadores)
{
    //Prende na tela dos desenvolvedores
    while(!IsKeyPressed(KEY_V))
    {
        UpdateMusicStream(musicmenu);
        if(WindowShouldClose() || IsKeyPressed(KEY_Q)) CloseWindow();
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(telacriadores, 0, 0, WHITE);
        EndDrawing();
    }
}

//Coloca a bomba
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
            *(*(world + bomberman->bombs[i].local.y) + bomberman->bombs[i].local.x) = BOMB;

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

//Movimenta o inimigo pseudo aleatoriamente
void enemyMove(enemy *zombie, char **world)
{
    //Usa uma variável de controle pra saber se ele bate na parede.
    //Anda na direção que ele está no momento, e, se ele bater, resorteia a direção.
    //Outro fator é o timer. Se passar de 3 segundos, ele resorteia também.
    //Por último, ao chegar nos meios dos quadrados, ele também tem uma chance de tomar uma nova decisão.
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

//Exibe o menu permitindo que o usuario selecione o que fazer
void menu(player *bomberman, char **world, char *information, LIST *horde, Music musicmenu, Texture2D capa, Sound interacao, Texture2D capamenu, coordinates *trapdoor, char *argv[], Texture2D bombmenu, Texture2D telarecordes, Texture2D telacontroles, Texture2D telacriadores, coordinates *stairs)
{
    //Prende na capa "pressione enter para continuar"
    while (!IsKeyPressed(KEY_ENTER)){
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(capa, 0, 0, WHITE);
            UpdateMusicStream(musicmenu);
        EndDrawing();
    }
    //Precisa aguardar um frame para o enter não registrar logo em seguida e apertar "newgame" também.
    float cont = GetTime();
    while(GetTime() - cont < 0.167){
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(capa, 0, 0, WHITE);
            UpdateMusicStream(musicmenu);
        EndDrawing();
    }
    coordinates arrow = {0, 0, 0, 0, 0};
    //Movimenta o cursor de acordo com os botões apertados. Sai do loop quando aperta enter, e então, entra em outro loop baseado na opção escolhida.
    while(!arrow.direction)
    {
        UpdateMusicStream(musicmenu);
        if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){arrow.y = (arrow.y+2)%3; PlaySound(interacao);}
        if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){arrow.y = (arrow.y+1)%3; PlaySound(interacao);}
        if(IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)){arrow.x = (arrow.x+1)%2; PlaySound(interacao);}
        if(IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)){arrow.x = (arrow.x+1)%2; PlaySound(interacao);}
        if(IsKeyPressed(KEY_ENTER))
        {
            if(arrow.y == 0 && arrow.x == 0)
            {
                if(newGame(bomberman, world, information, horde, trapdoor, argv, stairs)) break;
            }
            if(arrow.y == 1 && arrow.x == 0)
            {
                if(loadGame(bomberman, world, information, horde, trapdoor)) break;
                else
                {
                    BeginDrawing();
                        DrawText("Nao ha nenhum jogo salvo", 130, 500, 70, BLACK);
                    EndDrawing();
                }
            }
            if(arrow.y == 2 && arrow.x == 0) showRecords(musicmenu, telarecordes);
            if(arrow.y == 0 && arrow.x == 1) showControls(musicmenu, telacontroles);
            if(arrow.y == 1 && arrow.x == 1) showDevs(musicmenu, telacriadores);
            if(arrow.y == 2 && arrow.x == 1)
            {
                CloseWindow();
            }
        }
        if(WindowShouldClose() || IsKeyPressed(KEY_Q)) CloseWindow();
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawTexture(capamenu, 0, 0, WHITE);
            DrawTexture(bombmenu, 200 + (420*arrow.x), (100*arrow.y) + 140, WHITE);
        EndDrawing();
    }
}

//Mostra ao usuario a tela de derrota
void gameOver(Texture2D gameoverimg){
    //Usa a mesma lógica da função anterior para exibir uma tela, usando a função fade para dar um efeito de transparencia com a tela de game over.
    while (!IsKeyPressed(KEY_ENTER)){
        BeginDrawing();
            DrawRectangle(0, 0, WIDTH*METERS, HEIGHT*METERS, Fade(RED, 0.3f));
            DrawTexture(gameoverimg, 0, 0, WHITE);
        EndDrawing();
    }
    float cont = GetTime();
    while(GetTime() - cont < 0.167){
        BeginDrawing();
            DrawRectangle(0, 0, WIDTH*METERS, HEIGHT*METERS, Fade(RED, 0.3f));
            DrawTexture(gameoverimg, 0, 0, WHITE);
        EndDrawing();
    }
}

//Diminui a vida do jogador
void loseLife(player *bomberman, char *information, char **world, LIST *horde, Sound loss, Sound lostlife, Music musicmenu, Texture2D capa, Sound intmenu, Texture2D capamenu, Sound recordsound, coordinates *trapdoor, char *argv[], Texture2D bombmenu, Texture2D telarecordes, Texture2D telacontroles, Texture2D telacriadores, Texture2D gameoverimg, Texture2D *felixtrofeu, coordinates *stairs, Texture2D digiteseunome)
{
    //Se o bomberman não estiver nos frames de imortalidade, ele toma dano.
    if(GetTime() - bomberman->lastDmg >= IFRAMES){
        bomberman->lastDmg = GetTime();
        bomberman->invincibletimer = GetTime();
        bomberman->invinciblestate = 1;
        if(bomberman->points >=100) bomberman->points -= 100;
        else bomberman->points = 0;
        sprintf(information + 35 , "%d", bomberman->points);
        bomberman->lifes -= 1;
        *(information + 21) -= 1;
        if(bomberman->lifes <= '0')
        {
            PlaySound(loss);
            gameOver(gameoverimg);
            saveRecord(*bomberman, recordsound, felixtrofeu, digiteseunome);
            menu(bomberman, world, information, horde, musicmenu, capa, intmenu, capamenu, trapdoor, argv, bombmenu, telarecordes, telacontroles, telacriadores, stairs);
        }
        else PlaySound(lostlife);
    }
}

//Gera os efeitos da bomba naquilo que atingir
void Explosion_impact(int x, int y, player *bomberman, char *information, char **world, LIST *horde){
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

//Pausa o jogo permitindo que o usuario selecione o que fazer
void pauseGame(player *bomberman, char *information, char **world, LIST *horde, Music musicmenu, Texture2D capa, Sound intmenu, Texture2D capamenu, coordinates *trapdoor, char *argv[], Texture2D bombmenu, Texture2D telarecordes, Texture2D telacontroles, Texture2D telacriadores, Texture2D pauseicon, coordinates *stairs)
{
    //Trava o jogo nesse loop até que aperte V (voltar). Te dá varias opções de botões.
    while(!IsKeyPressed(KEY_V))
    {
        if(IsKeyPressed(KEY_N))
        {
            if(newGame(bomberman, world, information, horde, trapdoor, argv, stairs)) break;
        }
        if(IsKeyPressed(KEY_C))
        {
            if(loadGame(bomberman, world, information, horde, trapdoor)) break;
        }
        if(IsKeyPressed(KEY_M))
        {
            menu(bomberman, world, information, horde, musicmenu, capa, intmenu, capamenu, trapdoor, argv, bombmenu, telarecordes, telacontroles, telacriadores, stairs);
            break;
        }
        if(IsKeyPressed(KEY_S)) saveGame(*bomberman, world, horde, *trapdoor);
        if(IsKeyPressed(KEY_R))
        {
            showRecords(musicmenu, telarecordes);
            break;
        }
        if(IsKeyPressed(KEY_K))
        {
            showControls(musicmenu, telacontroles);
            break;
        }
        if(IsKeyPressed(KEY_Q) || WindowShouldClose()) CloseWindow();
        BeginDrawing();
            DrawTexture(pauseicon, 0, -50, WHITE);    
        EndDrawing();
    }
}

//Faz o Felix soltar uma frase de efeito
void doEmote(
    Sound s1, Sound s2, Sound s3, Sound s4, Sound s5, Sound s6,
    Sound s7, Sound s8, Sound s9, Sound s10, Sound s11, player *bomberman
) {
    //Realiza o emote de acordo com um número sorteado pseudoaleatoriamente, bloqueado pelo cooldown também.
    float agora = GetTime();
    if (agora - bomberman->lastEmoteTime >= emoteCooldown) {
        switch (GetRandomValue(1,NUM_EMOTES)) {
            case 1: PlaySound(s1); break;
            case 2: PlaySound(s2); break;
            case 3: PlaySound(s3); break;
            case 4: PlaySound(s4); break;
            case 5: PlaySound(s5); break;
            case 6: PlaySound(s6); break;
            case 7: PlaySound(s7); break;
            case 8: PlaySound(s8); break;
            case 9: PlaySound(s9); break;
            case 10: PlaySound(s10); break;
            case 11: PlaySound(s11);
        }
        bomberman->lastEmoteTime = agora;
    }
}

//Conta o numero de niveis nos arquivos do jogo
int countlevels()
{
    char argc = '0';
    int levels = 0;
    char *argv = (char *)malloc(13*sizeof(char));
    if(!argv)
    {
        puts("ERRO - a memoria nao pode ser alocada");
        return -1;
    }
    strcpy(argv, "world0.txt");
    while(argc != ':')
    {
        *(argv+5) = argc+1;
        FILE *file = fopen(argv, "r");
        if(!file)
        {
            free(argv);
            fclose(file);
            return levels;
        }
        fclose(file);
        argc++;
        levels++;
    }
    free(argv);
    return -1;
}

//Move o jogador para a proxima fase
bool nextLevel(player *bomberman, char **world, char *information, LIST *horde, coordinates *trapdoor, char *argv, Music musicmapa2, Sound doorsound, Texture2D *felix_alcapao, coordinates *stairs)
{
    //Loop para exibir o gif do nível avançando.
    PlaySound(doorsound);
    *stairs = *trapdoor;
    bool gifdone = false;
    double timer_trans = GetTime();
    int frameatual = 0;
    while(!gifdone){
        if(GetTime() - timer_trans > ANIM_SPEED_TRANS){
            frameatual++;
            timer_trans = GetTime();
        }
        if (frameatual >= 12){
            gifdone = true;
        }
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(felix_alcapao[frameatual], 0, 0, WHITE);
        EndDrawing();
    }
    FILE *file = fopen(argv, "r");
    if(!file)
    {
        puts("ERRO");
        return false;
    }
    //Reinicializa tudo, menos as informações que não devem reiniciar.
    bomberman->keys = 0;
    bomberman->local.direction = DOWN;
    bomberman->local.offsetX = 0;
    bomberman->local.offsetY = 0;
    bomberman->lastDmg = -10;
    bomberman->lastEmoteTime = GetTime();
    bomberman->state = 0;
    bomberman->Frame_atual = 0;
    bomberman->animation_timer = GetTime();
    bomberman->invinciblestate = 1;
    bomberman->invincibletimer = GetTime();
    PlayMusicStream(musicmapa2);

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

            case TRAPDOOR:
            trapdoor->x = squares.x;
            trapdoor->y = squares.y;
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

int main()
{
    //Aloca memoria para o nome dos niveis depois de contar quantos sao
    int levels = countlevels();
    if(levels == -1) return -1;
    if(!levels)
    {
        puts("ERRO - nenhum nivel foi encontrado");
        return 1;
    }
    char **argv = (char **)malloc(levels*sizeof(char*));
    if(!argv)
    {
        puts("ERRO - a memoria nao pode ser alocada");
        return -1;
    }
    char temp = '1';
    for(int i = 0; i < levels; i++)
    {
        argv[i] = (char *)malloc(13*sizeof(char));
        if(!argv[i])
        {
            puts("ERRO - a memoria nao pode ser alocada");
            return -1;
        }
        strcpy(argv[i], "world0.txt");
        *(argv[i]+5) = temp;
        temp++;
    }

    //Cria as variaveis fundamentais do jogo, alocando memoria quando necessario
    coordinates trapdoor;
    coordinates stairs;
    player bomberman;
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

    //Carrega os audios e retorna erro caso nao consiga
    InitAudioDevice();
    Music musicmap1 = LoadMusicStream("Musicas/Songs/Mapa1.ogg");
    Music musicmap2 = LoadMusicStream("Musicas/Songs/Mapa2.ogg");
    Music musicmenu = LoadMusicStream("Musicas/Songs/Menu.ogg");
    Sound losssound = LoadSound("Musicas/SoundFX/Derrota.ogg");
    Sound lostlifesound = LoadSound("Musicas/SoundFX/LoseLife.ogg");
    Sound obtainedkeysound = LoadSound("Musicas/SoundFX/ObtainedKey.ogg");
    Sound victorysound = LoadSound("Musicas/SoundFX/Vitoria.ogg");
    Sound alcapao = LoadSound("Musicas/SoundFX/alcapao.ogg");
    Sound intMenu = LoadSound("Musicas/SoundFX/intMenu.ogg");
    Sound newRecord = LoadSound("Musicas/SoundFX/newRecord.ogg");
    Sound pause = LoadSound("Musicas/SoundFX/pause.ogg");
    Sound unpause = LoadSound("Musicas/SoundFX/unpause.ogg");
    Sound lastkey = LoadSound("Musicas/SoundFX/lastKey.mp3");
    Sound emote1, emote2, emote3, emote4, emote5, emote6;
    Sound emote7, emote8, emote9, emote10, emote11;
    emote1  = LoadSound("Musicas/Emotes/s1.ogg");
    emote2  = LoadSound("Musicas/Emotes/s2.ogg");
    emote3  = LoadSound("Musicas/Emotes/s3.ogg");
    emote4  = LoadSound("Musicas/Emotes/s4.ogg");
    emote5  = LoadSound("Musicas/Emotes/s5.ogg");
    emote6  = LoadSound("Musicas/Emotes/s6.ogg");
    emote7  = LoadSound("Musicas/Emotes/s7.ogg");
    emote8  = LoadSound("Musicas/Emotes/s8.ogg");
    emote9  = LoadSound("Musicas/Emotes/s9.ogg");
    emote10 = LoadSound("Musicas/Emotes/s10.ogg");
    emote11 = LoadSound("Musicas/Emotes/s11.ogg");
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
    if (emote1.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica s1.ogg");
        CloseAudioDevice();
        return -1;
    }
    if (emote2.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica s2.ogg");
        CloseAudioDevice();
        return -1;
    }
    if (emote3.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica s3.ogg");
        CloseAudioDevice();
        return -1;
    }
    if (emote4.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica s4.ogg");
        CloseAudioDevice();
        return -1;
    }
    if (emote5.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica s5.ogg");
        CloseAudioDevice();
        return -1;
    }
    if (emote6.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica s6.ogg");
        CloseAudioDevice();
        return -1;
    }
    if (emote7.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica s7.ogg");
        CloseAudioDevice();
        return -1;
    }
    if (emote8.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica s8.ogg");
        CloseAudioDevice();
        return -1;
    }
    if (emote9.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica s9.ogg");
        CloseAudioDevice();
        return -1;
    }
    if (emote10.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica s10.ogg");
        CloseAudioDevice();
        return -1;
    }
    if (emote11.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica s11.ogg");
        CloseAudioDevice();
        return -1;
    }
    if (alcapao.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
    if (intMenu.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
    if (newRecord.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
    if (pause.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
    if (unpause.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar a musica");
        CloseAudioDevice();
        return -1;
    }
    if (lastkey.stream.buffer == NULL) {
        puts("ERRO - Nao foi possivel carregar o som da ultima chave.");
        CloseAudioDevice();
        return -1;
    }
    musicmenu.looping = true;
    musicmap1.looping = true;
    musicmap2.looping = true;
    PlayMusicStream(musicmenu);

    //Carrega os sprites e telas do menu retornando erro caso nao consiga
    Texture2D capa = LoadTexture("Sprites/telas/capa.png");
    if(!IsTextureValid(capa))
    {
        puts("ERRO - Nao foi possivel achar o sprite da capa.");
        CloseWindow();
        return 1;    
    }
    Texture2D capamenu = LoadTexture("Sprites/telas/menu.png");
    if(!IsTextureValid(capamenu))
    {
        puts("ERRO - Nao foi possivel achar o sprite da capa do menu.");
        CloseWindow();
        return 1;    
    }
    Texture2D bombmenu = LoadTexture("Sprites/telas/bombmenu.png");
    if (!IsTextureValid(bombmenu)) {
        puts("ERRO - Nao foi possivel achar o sprite da bomba do cursor");
        CloseWindow();
        return 1;
    }
    Texture2D telarecordes = LoadTexture("Sprites/telas/recordes.png");
    if (!IsTextureValid(telarecordes))
    {
    puts("ERRO - Nao foi possivel achar o sprite da tela de recordes.");
    CloseWindow();
    return 1;
    }
    Texture2D telacriadores = LoadTexture("Sprites/telas/criadores.png");
    if (!IsTextureValid(telacriadores))
    {
    puts("ERRO - Nao foi possivel achar o sprite da tela de criadores.");
    CloseWindow();
    return 1;
    }
    Texture2D telacontroles = LoadTexture("Sprites/telas/controles.png");
    if (!IsTextureValid(telacontroles))
    {
    puts("ERRO - Nao foi possivel achar o sprite da tela de controles.");
    CloseWindow();
    return 1;
    }
    menu(&bomberman, world, information, &horde, musicmenu, capa, intMenu, capamenu, &trapdoor, argv, bombmenu, telarecordes, telacontroles, telacriadores, &stairs);
    PlayMusicStream(musicmap1);

    //Carrega os sprites e telas do jogo retornando erro caso nao consiga
    Texture2D wall_spr = LoadTexture("Sprites/cenario/parede.png");
    Texture2D box_spr = LoadTexture("Sprites/cenario/caixa.png");
    Texture2D explosible_spr = LoadTexture("Sprites/cenario/parede_destrutivel.png");
    Texture2D key_spr = LoadTexture("Sprites/interagiveis/chave.png");
    Texture2D floor_spr = LoadTexture("Sprites/cenario/chao.png");
    Texture2D in = LoadTexture("Sprites/cenario/in.png");
    Texture2D bomb_animation[2];
    Texture2D alcapaofechado = LoadTexture("Sprites/cenario/alcapaofechado.png");
    Texture2D alcapaoaberto = LoadTexture("Sprites/cenario/alcapaoaberto.png");
    Texture2D pauseicon = LoadTexture("Sprites/telas/pausado.png");
    Texture2D gameoverimg = LoadTexture("Sprites/telas/gameover.png");
    Texture2D caixam2 = LoadTexture("Sprites/cenariom2/caixa.png");
    Texture2D chaom2 = LoadTexture("Sprites/cenariom2/chao.png");
    Texture2D paredem2 = LoadTexture("Sprites/cenariom2/parede.png");
    Texture2D parededm2 = LoadTexture("Sprites/cenariom2/parede_destrutivel.png");
    Texture2D escada = LoadTexture("Sprites/cenariom2/escada.png");
    Texture2D digiteseunome = LoadTexture("Sprites/telas/digiteseunome.png");
    Texture2D trofeu[3];
    trofeu[0] = LoadTexture("Sprites/telas/novorecorde/trofeu1.png");
    trofeu[1] = LoadTexture("Sprites/telas/novorecorde/trofeu2.png");
    trofeu[2] = LoadTexture("Sprites/telas/novorecorde/trofeu3.png");
    Texture2D felix_alcapao[12];
    felix_alcapao[0] = LoadTexture("Sprites/telas/passounivel/felix_alcapao1.png");
    felix_alcapao[1] = LoadTexture("Sprites/telas/passounivel/felix_alcapao2.png");
    felix_alcapao[2] = LoadTexture("Sprites/telas/passounivel/felix_alcapao3.png");
    felix_alcapao[3] = LoadTexture("Sprites/telas/passounivel/felix_alcapao4.png");
    felix_alcapao[4] = LoadTexture("Sprites/telas/passounivel/felix_alcapao5.png");
    felix_alcapao[5] = LoadTexture("Sprites/telas/passounivel/felix_alcapao6.png");
    felix_alcapao[6] = LoadTexture("Sprites/telas/passounivel/felix_alcapao7.png");
    felix_alcapao[7] = LoadTexture("Sprites/telas/passounivel/felix_alcapao8.png");
    felix_alcapao[8] = LoadTexture("Sprites/telas/passounivel/felix_alcapao9.png");
    felix_alcapao[9] = LoadTexture("Sprites/telas/passounivel/felix_alcapao10.png");
    felix_alcapao[10] = LoadTexture("Sprites/telas/passounivel/felix_alcapao11.png");
    felix_alcapao[11] = LoadTexture("Sprites/telas/passounivel/felix_alcapao12.png");
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
    Texture2D zombie_walk_down[4];
    zombie_walk_down[0] = LoadTexture("Sprites/inimigo/Zombie-walk-down1.png");
    zombie_walk_down[1] = LoadTexture("Sprites/inimigo/Zombie-walk-down2.png");
    zombie_walk_down[2] = LoadTexture("Sprites/inimigo/Zombie-walk-down3.png");
    zombie_walk_down[3] = LoadTexture("Sprites/inimigo/Zombie-walk-down4.png");
    Texture2D zombie_walk_up[4];
    zombie_walk_up[0] = LoadTexture("Sprites/inimigo/Zombie-walk-up1.png");
    zombie_walk_up[1] = LoadTexture("Sprites/inimigo/Zombie-walk-up2.png");
    zombie_walk_up[2] = LoadTexture("Sprites/inimigo/Zombie-walk-up3.png");
    zombie_walk_up[3] = LoadTexture("Sprites/inimigo/Zombie-walk-up4.png");
    Texture2D zombie_walk_left[4];
    zombie_walk_left[0] = LoadTexture("Sprites/inimigo/Zombie-walk-left1.png");
    zombie_walk_left[1] = LoadTexture("Sprites/inimigo/Zombie-walk-left2.png");
    zombie_walk_left[2] = LoadTexture("Sprites/inimigo/Zombie-walk-left3.png");
    zombie_walk_left[3] = LoadTexture("Sprites/inimigo/Zombie-walk-left4.png");
    Texture2D zombie_walk_rigth[4];
    zombie_walk_rigth[0] = LoadTexture("Sprites/inimigo/Zombie-walk-right1.png");
    zombie_walk_rigth[1] = LoadTexture("Sprites/inimigo/Zombie-walk-right2.png");
    zombie_walk_rigth[2] = LoadTexture("Sprites/inimigo/Zombie-walk-right3.png");
    zombie_walk_rigth[3] = LoadTexture("Sprites/inimigo/Zombie-walk-right4.png");
    
    if (!IsTextureValid(digiteseunome)) {
    puts("ERRO - Nao foi possivel achar o sprite da tela de digitar seu nome.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(escada)) {
    puts("ERRO - Nao foi possivel achar o sprite da escada.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[11])) {
    puts("ERRO - Nao foi possivel achar o sprite felix_alcapao12.png.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[10])) {
    puts("ERRO - Nao foi possivel achar o sprite felix_alcapao11.png.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[9])) {
    puts("ERRO - Nao foi possivel achar o sprite felix_alcapao10.png.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[8])) {
    puts("ERRO - Nao foi possivel achar o sprite felix_alcapao9.png.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[7])) {
    puts("ERRO - Nao foi possivel achar o sprite felix_alcapao8.png.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[6])) {
    puts("ERRO - Nao foi possivel achar o sprite felix_alcapao7.png.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[5])) {
    puts("ERRO - Nao foi possivel achar o sprite felix_alcapao6.png.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[4])) {
    puts("ERRO - Nao foi possivel achar o sprite felix_alcapao5.png.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[3])) {
    puts("ERRO - Nao foi possivel achar o sprite felix_alcapao4.png.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[2])) {
    puts("ERRO - Nao foi possivel achar o sprite felix_alcapao3.png.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[1])) {
    puts("ERRO - Nao foi possivel achar o sprite felix_alcapao2.png.");
    CloseWindow();
    return 1;
    }
    if (!IsTextureValid(felix_alcapao[0])) {
        puts("ERRO - Nao foi possivel achar o sprite da parede do primeiro png do gif de passar nível.");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(trofeu[0])) {
        puts("ERRO - Nao foi possivel achar o sprite da parede do troféu 1");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(trofeu[1])) {
        puts("ERRO - Nao foi possivel achar o sprite da parede do troféu 2");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(trofeu[2])) {
        puts("ERRO - Nao foi possivel achar o sprite da parede do troféu 3");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(paredem2)) {
        puts("ERRO - Nao foi possivel achar o sprite da parede do mapa 2");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(parededm2)) {
        puts("ERRO - Nao foi possivel achar o sprite da parede destrutível do mapa 2");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(chaom2)) {
        puts("ERRO - Nao foi possivel achar o sprite do chao do mapa 2");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(caixam2)) {
        puts("ERRO - Nao foi possivel achar o sprite da caixa do mapa 2");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(in)) {
        puts("ERRO - Nao foi possivel achar o sprite da porta");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(gameoverimg)) {
        puts("ERRO - Nao foi possivel achar o sprite do ícone de game over.");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(pauseicon)) {
        puts("ERRO - Nao foi possivel achar o sprite do ícone de pause.");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(alcapaofechado)) {
        puts("ERRO - Nao foi possivel achar o sprite do alcapao fechado.");
        CloseWindow();
        return 1;
    }
    if (!IsTextureValid(alcapaoaberto)) {
        puts("ERRO - Nao foi possivel achar o sprite do alcapao aberto.");
        CloseWindow();
        return 1;
    }
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
            puts("ERRO - Nao foi possivel achar algum dos sprites da explosao.");
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
    if(!IsTextureValid(in))
    {
        puts("ERRO - Nao foi possivel achar o sprite da porta da entrada.");
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

    Vector2 origin = {0.0, 0.0};
    while(!WindowShouldClose())
    {   
        if(bomberman.level == 1) UpdateMusicStream(musicmap1);
        else UpdateMusicStream(musicmap2);
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
            if(bomberman.state != 1){
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
            PlaySound(pause);
            for(int i = 0; i < MAX_BOMBS; i++) *(times+i) = TIMER_BOMB - (GetTime() - bomberman.bombs[i].planttime);
            pauseGame(&bomberman, information, world, &horde, musicmenu, capa, intMenu, capamenu, &trapdoor, argv, bombmenu, telarecordes, telacontroles, telacriadores, pauseicon, &stairs);
            for(int i = 0; i < MAX_BOMBS; i++) bomberman.bombs[i].planttime = GetTime() - (TIMER_BOMB - *(times+i));
            PlaySound(unpause);
            free(times);
        }
        if (IsKeyPressed(KEY_T))
        doEmote(
            emote1, emote2, emote3, emote4, emote5, emote6,
            emote7, emote8, emote9, emote10, emote11, &bomberman
        );
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

                        //precisa dessas variaveis para controlar se a explosão de cima, baixo, esquereda ou direita bateram em algo
                        bool parou_cima = false;
                        bool parou_baixo = false;
                        bool parou_esquerda = false;
                        bool parou_direita = false;  

                        for(int j = 1; j <= bomberman.bombs[i].radio; j++)
                        {

                            //direção para cima
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
                            //Direção para baixo
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
                            //Direção para a esquerda
                            if(!parou_esquerda){
                                if(*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x - j) == BLOCK){
                                    parou_esquerda = true;
                                }else{
                                //se o bloco q sofreu impacto não é vazio...
                                if(*(*(world + bomberman.bombs[i].local.y)+ bomberman.bombs[i].local.x - j)!=FREE) parou_esquerda = true;
                                Explosion_impact(bomberman.bombs[i].local.x - j, bomberman.bombs[i].local.y, &bomberman, information, world, &horde);
                                }
                            }
                            //Direção para a direita
                            if(!parou_direita){
                                if(*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x + j) == BLOCK){
                                    parou_direita = true;
                                }else{
                                //se o bloco q sofreu impacto não é vazio...
                                if(*(*(world + bomberman.bombs[i].local.y)+ bomberman.bombs[i].local.x + j)!=FREE) parou_direita = true;
                                Explosion_impact(bomberman.bombs[i].local.x + j, bomberman.bombs[i].local.y, &bomberman, information, world, &horde);
                                }
                            }
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
                    
                        for(int y = 0; y < STATURE; y++){
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
            //define as texturas que vão ser carregadas baseada no mapa no qual o jogador se encontra.
            Texture2D parede, paredequebravel, chao, caixa;
            if(bomberman.level == 1){
                parede = wall_spr;
                paredequebravel = explosible_spr;
                chao = floor_spr;
                caixa = box_spr;
            }
            else{
                parede = paredem2;
                paredequebravel = parededm2;
                chao = chaom2;
                caixa = caixam2;
            }
            //adicionando textura no chao inteiro
            //primeiro eu crio um triangulo do sizeList da tela
            Rectangle sourceRec = {0.0, 0.0, WIDTH*METERS, 500};
            Rectangle playerhb = {bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, 16, 16};
            //esse código desenha o sprite no retangulo criado. Como o sprite(tam: 20x20) é bem menor que o retangulo (tam: tela), ele vai preenchendo até cobrir tudo
            DrawTextureRec(chao, sourceRec, origin, WHITE);

            for(int y = 0; y < STATURE; y++)
            {
                for(int x = 0; x < WIDTH; x++)
                {
                    switch(*(*(world + y) + x))
                    {
                        case BLOCK:
                        DrawTexture(parede, x*METERS, y*METERS, WHITE);
                        break;

                        case WALL:
                        DrawTexture(paredequebravel, x*METERS, y*METERS, WHITE);
                        break;

                        case EMPTY_BOX:
                        DrawTexture(caixa, x*METERS, y*METERS, WHITE);
                        break;

                        case KEY_BOX:
                        DrawTexture(caixa, x*METERS, y*METERS, WHITE);
                        break;

                        case KEY:
                        Rectangle keyhb = {x*METERS, y*METERS, 20, 20};
                        if(checkHitbox(playerhb, keyhb)){
                            bomberman.keys++;
                            if(bomberman.keys < 5) PlaySound(obtainedkeysound);
                            else PlaySound(lastkey);
                            *(*(world + y) + x) = FREE;
                        }
                        else{
                            DrawTexture(key_spr, x*METERS, y*METERS, WHITE);
                        }
                        break;
                        
                        case EXPLODING_WALL:
                        DrawTexture(explosion_spr[0], x*METERS, y*METERS, WHITE);
                        break;

                        case 'I':
                        DrawTexture(in, x*METERS, y*METERS, WHITE);
                    }
                }
            }
            if(bomberman.keys > 4)
            {
                DrawTexture(alcapaoaberto, trapdoor.x*METERS, trapdoor.y*METERS, WHITE);
                if(bomberman.local.x == trapdoor.x && bomberman.local.y == trapdoor.y)
                {
                    if(bomberman.level == levels)
                    {
                        saveRecord(bomberman, newRecord, trofeu, digiteseunome);
                        menu(&bomberman, world, information, &horde, musicmenu, capa, intMenu, capamenu, &trapdoor, argv, bombmenu, telarecordes, telacontroles, telacriadores, &stairs);
                    }
                    else
                    {
                        if(!nextLevel(&bomberman, world, information, &horde, &trapdoor, argv[bomberman.level], musicmap2, alcapao, felix_alcapao, &stairs))
                        {
                            return 1;
                        }
                        bomberman.level++;
                    }
                }
            }
            else DrawTexture(alcapaofechado, trapdoor.x*METERS, trapdoor.y*METERS, WHITE);
            DrawTexture(escada, stairs.x*METERS, stairs.y*METERS, WHITE);
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
                            Rectangle exphb = {bomberman.bombs[i].local.x*METERS, bomberman.bombs[i].local.y*METERS, 20,20};
                            if(checkHitbox(playerhb, exphb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu, capa, intMenu, capamenu, newRecord, &trapdoor, argv, bombmenu, telarecordes, telacontroles, telacriadores, gameoverimg, trofeu, &stairs, digiteseunome);
                            bool para_cima_desenho = false;
                            bool para_baixo_desenho = false;
                            bool para_esquerda_desenho = false;
                            bool para_direita_desenho = false;
                            for(int j = 1; j <= bomberman.bombs[i].radio; j++){
                        //Para cima
                            if (!para_cima_desenho && bomberman.bombs[i].local.y - j >= 0) {
                            if (*(*(world + bomberman.bombs[i].local.y - j) + bomberman.bombs[i].local.x) == BLOCK) {
                                para_cima_desenho = true;
                            } else {
                                DrawTexture(explosion_spr[frame], bomberman.bombs[i].local.x*METERS, (bomberman.bombs[i].local.y - j)*METERS, WHITE);
                                Rectangle exphb = {bomberman.bombs[i].local.x*METERS, (bomberman.bombs[i].local.y - j)*METERS, 20,20};
                                if(checkHitbox(playerhb, exphb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu, capa, intMenu, capamenu, newRecord, &trapdoor, argv, bombmenu, telarecordes, telacontroles, telacriadores, gameoverimg, trofeu, &stairs, digiteseunome);
                                // Se o bloco no mapa não for vazio, o desenho para também.
                                if (*(*(world + bomberman.bombs[i].local.y - j) + bomberman.bombs[i].local.x) != FREE) para_cima_desenho = true;
                            }
                        }
                        //Para baixo   
                            if (!para_baixo_desenho && bomberman.bombs[i].local.y + j < STATURE) {
                            if (*(*(world + bomberman.bombs[i].local.y + j) + bomberman.bombs[i].local.x) == BLOCK) {
                                para_baixo_desenho = true;
                            } else {
                                DrawTexture(explosion_spr[frame], bomberman.bombs[i].local.x*METERS, (bomberman.bombs[i].local.y + j)*METERS, WHITE);
                                Rectangle exphb = {bomberman.bombs[i].local.x*METERS, (bomberman.bombs[i].local.y + j)*METERS, 20,20};
                                if(checkHitbox(playerhb, exphb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu, capa, intMenu, capamenu, newRecord, &trapdoor, argv, bombmenu, telarecordes, telacontroles, telacriadores, gameoverimg, trofeu, &stairs, digiteseunome);
                                // Se o bloco no mapa não for vazio, o desenho para também.
                                if (*(*(world + bomberman.bombs[i].local.y + j) + bomberman.bombs[i].local.x) != FREE) para_baixo_desenho = true;
                            }
                        }
                        //Para esquerda    
                            if (!para_esquerda_desenho && bomberman.bombs[i].local.x - j >= 0) {
                            if (*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x - j) == BLOCK) {
                                para_esquerda_desenho = true;
                            } else {
                                DrawTexture(explosion_spr[frame], (bomberman.bombs[i].local.x - j)*METERS, bomberman.bombs[i].local.y*METERS, WHITE);
                                Rectangle exphb = {(bomberman.bombs[i].local.x - j)*METERS, bomberman.bombs[i].local.y*METERS, 20,20};
                                if(checkHitbox(playerhb, exphb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu, capa, intMenu, capamenu, newRecord, &trapdoor, argv, bombmenu, telarecordes, telacontroles, telacriadores, gameoverimg, trofeu, &stairs, digiteseunome);
                                // Se o bloco no mapa não for vazio, o desenho para também.
                                if (*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x - j) != FREE) para_esquerda_desenho = true;
                            }
                        }
                        //Para direita    
                            if (!para_direita_desenho && bomberman.bombs[i].local.x + j < WIDTH) {
                            if (*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x + j) == BLOCK) {
                                para_direita_desenho = true;
                            } else {
                                DrawTexture(explosion_spr[frame], (bomberman.bombs[i].local.x + j)*METERS, bomberman.bombs[i].local.y*METERS, WHITE);
                                Rectangle exphb = {(bomberman.bombs[i].local.x + j)*METERS, bomberman.bombs[i].local.y*METERS, 20,20};
                                if(checkHitbox(playerhb, exphb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu, capa, intMenu, capamenu, newRecord, &trapdoor, argv, bombmenu, telarecordes, telacontroles, telacriadores, gameoverimg, trofeu, &stairs, digiteseunome);
                                // Se o bloco no mapa não for vazio, o desenho para também.
                                if (*(*(world + bomberman.bombs[i].local.y) + bomberman.bombs[i].local.x + j) != FREE) para_direita_desenho = true;
                            }
                        }
                        }
                    }
                    }
                }
            }
            bool invincible = (GetTime() - bomberman.lastDmg < IFRAMES);
            if(invincible){
                if(!bomberman.invinciblestate){
                    switch(bomberman.local.direction)
                    {
                        case UP:
                        if(bomberman.state == 1){
                        DrawTexture(player_walk_up[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                        }
                        else{
                            DrawTexture(player_idle_up[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                        }
                        break;

                        case RIGHT:
                        if(bomberman.state == 1){
                        DrawTexture(player_walk_rigth[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                        }
                        else{
                            DrawTexture(player_idle_right[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                        }
                        break;

                        case DOWN:
                        if(bomberman.state == 1){
                        DrawTexture(player_walk_down[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                        }
                        else{
                            DrawTexture(player_idle_down[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                        }
                        break;

                        case LEFT:
                        if(bomberman.state == 1){
                        DrawTexture(player_walk_left[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                        }
                        else{
                            DrawTexture(player_idle_left[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                        }
                    }
                if(GetTime() - bomberman.invincibletimer >= 0.4){bomberman.invinciblestate = !bomberman.invinciblestate; bomberman.invincibletimer = GetTime();}
                }
                else{
                if(GetTime() - bomberman.invincibletimer >= 0.1){bomberman.invinciblestate = !bomberman.invinciblestate; bomberman.invincibletimer = GetTime();}
                }
            }
            else{
                switch(bomberman.local.direction)
                {
                    case UP:
                    if(bomberman.state == 1){
                    DrawTexture(player_walk_up[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                    }
                    else{
                        DrawTexture(player_idle_up[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                    }
                    break;

                    case RIGHT:
                    if(bomberman.state == 1){
                    DrawTexture(player_walk_rigth[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                    }
                    else{
                        DrawTexture(player_idle_right[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                    }
                    break;

                    case DOWN:
                    if(bomberman.state == 1){
                    DrawTexture(player_walk_down[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                    }
                    else{
                        DrawTexture(player_idle_down[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                    }
                    break;

                    case LEFT:
                    if(bomberman.state == 1){
                    DrawTexture(player_walk_left[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                    }
                    else{
                        DrawTexture(player_idle_left[bomberman.Frame_atual], bomberman.local.x*METERS + bomberman.local.offsetX+2, bomberman.local.y*METERS + bomberman.local.offsetY+2, WHITE);
                    }
                }
            }

            POINTER_LIST alive = horde.start;
            while(alive != NULL)
            {
                if (GetTime() - alive->zombie.animation_timer_enemy > ENEMY_ANIM_SPEED) {
                alive->zombie.frame_atual_walk++;
                alive->zombie.animation_timer_enemy = GetTime();
                }
                if (alive->zombie.frame_atual_walk >= 4) {
                alive->zombie.frame_atual_walk = 0;
                }
                enemyMove(&alive->zombie, world);
                if(alive->zombie.local.direction == UP){
                    DrawTexture(zombie_walk_up[alive->zombie.frame_atual_walk], alive->zombie.local.x*METERS + alive->zombie.local.offsetX+2, alive->zombie.local.y*METERS + alive->zombie.local.offsetY+2, WHITE);
                }
                if(alive->zombie.local.direction == RIGHT){
                    DrawTexture(zombie_walk_rigth[alive->zombie.frame_atual_walk], alive->zombie.local.x*METERS + alive->zombie.local.offsetX+2, alive->zombie.local.y*METERS + alive->zombie.local.offsetY+2, WHITE);
                }
                if(alive->zombie.local.direction == DOWN){
                    DrawTexture(zombie_walk_down[alive->zombie.frame_atual_walk], alive->zombie.local.x*METERS + alive->zombie.local.offsetX+2, alive->zombie.local.y*METERS + alive->zombie.local.offsetY+2, WHITE);
                }
                if(alive->zombie.local.direction == LEFT){
                    DrawTexture(zombie_walk_left[alive->zombie.frame_atual_walk], alive->zombie.local.x*METERS + alive->zombie.local.offsetX+2, alive->zombie.local.y*METERS + alive->zombie.local.offsetY+2, WHITE);
                }
                Rectangle enemyhb = {alive->zombie.local.x*METERS + alive->zombie.local.offsetX + 2, alive->zombie.local.y*METERS + alive->zombie.local.offsetY + 2, 16, 16};
                if(checkHitbox(playerhb, enemyhb)) loseLife(&bomberman, information, world, &horde, losssound, lostlifesound, musicmenu, capa, intMenu, capamenu, newRecord, &trapdoor, argv, bombmenu, telarecordes, telacontroles, telacriadores, gameoverimg, trofeu, &stairs, digiteseunome);
                alive = alive->next;
            }
            DrawText(information, 20, 530, 50, BLACK);
            //DrawText(TextFormat("%.1lf", GetTime()), 1140, 580, 20, BLACK);
            for(int i = 0; i < bomberman.keys; i++){
                DrawTexture(key_spr, 1000+i*35, 540, WHITE);
            }
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
    for (int i = 0; i < 4; i++) {
        UnloadTexture(zombie_walk_down[i]);
        UnloadTexture(zombie_walk_up[i]);
        UnloadTexture(zombie_walk_left[i]);
        UnloadTexture(zombie_walk_rigth[i]);
    }
    for (int i = 0; i < 3; i++) {
        UnloadTexture(explosion_spr[i]);
    }
    for(int i = 0; i < 12; i++) UnloadTexture(felix_alcapao[i]);
    for(int i = 0; i < 3; i++) UnloadTexture(trofeu[i]);
    UnloadTexture(bomb_animation[0]);
    UnloadTexture(bomb_animation[1]);
    UnloadTexture(wall_spr);
    UnloadTexture(explosible_spr);
    UnloadTexture(box_spr);
    UnloadTexture(key_spr);
    UnloadTexture(floor_spr);
    UnloadTexture(bombmenu);
    UnloadTexture(telarecordes);
    UnloadTexture(telacriadores);
    UnloadTexture(telacontroles);
    UnloadTexture(pauseicon);
    UnloadTexture(gameoverimg);
    UnloadTexture(in);
    UnloadTexture(chaom2);
    UnloadTexture(paredem2);
    UnloadTexture(parededm2);
    UnloadTexture(caixam2);
    UnloadTexture(escada);
    UnloadTexture(digiteseunome);
    UnloadSound(emote1);
    UnloadSound(emote2);
    UnloadSound(emote3);
    UnloadSound(emote4);
    UnloadSound(emote5);
    UnloadSound(emote6);
    UnloadSound(emote7);
    UnloadSound(emote8);
    UnloadSound(emote9);
    UnloadSound(emote10);
    UnloadSound(emote11);
    UnloadSound(losssound);
    UnloadSound(lostlifesound);
    UnloadSound(obtainedkeysound);
    UnloadSound(victorysound);
    UnloadSound(alcapao);
    UnloadSound(intMenu);
    UnloadSound(newRecord);
    UnloadSound(pause);
    UnloadSound(unpause);
    UnloadSound(lastkey);
    UnloadMusicStream(musicmap1);
    UnloadMusicStream(musicmap2);
    UnloadMusicStream(musicmenu);
    CloseAudioDevice();
    CloseWindow();
    free(information);
    for(int y = 0; y < STATURE; y++)
    {
        free(*(world + y));
    }
    rebootList(&horde);
    for(int i = 0; i < levels; i++)
    {
        free(argv[i]);
    }
    free(argv);
    free(world);
    return 0;
}