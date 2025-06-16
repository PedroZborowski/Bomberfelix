#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LARGURA 1200
#define ALTURA 600
#define JOGADOR 'J'
#define LIVRE ' '
#define PAREDE 'W'
#define DESTRUTIVEL 'D'
#define BAU 'B'
#define CHAVE 'C'
#define CAIXA 'K'
#define INIMIGO 'E'

typedef struct
{
    int x;
    int y;
} coordenadas;

typedef struct
{
    coordenadas local;
    char vidas;
    char bombas;
    int pontos;
} jogador;

typedef struct
{
    coordenadas local;
} inimigo;

void andardireita(coordenadas *local, FILE *mapa)
{
    char objeto = fgetc(mapa);
    if(fgetc(mapa) == LIVRE)
    {
        fseek(mapa, -2*sizeof(char), SEEK_CUR);
        fputc(LIVRE, mapa);
        fputc(objeto, mapa);
        fseek(mapa, -sizeof(char), SEEK_CUR);
        local -> x += 20;
    }
    fseek(mapa, -2*sizeof(char), SEEK_CUR);
}

void andaresquerda(coordenadas *local, FILE *mapa)
{
    fseek(mapa, -sizeof(char), SEEK_CUR);
    if(fgetc(mapa) == LIVRE)
    {
        char objeto = fgetc(mapa);
        fseek(mapa, -2*sizeof(char), SEEK_CUR);
        fputc(objeto, mapa);
        fputc(LIVRE, mapa);
        fseek(mapa, -2*sizeof(char), SEEK_CUR);
        local -> x -= 20;
    }
}

void andarcima(coordenadas *local, FILE *mapa)
{
    char objeto = fgetc(mapa);
    fseek(mapa, -63*sizeof(char), SEEK_CUR);
    if(fgetc(mapa) == LIVRE)
    {
        fseek(mapa, 61*sizeof(char), SEEK_CUR);
        fputc(LIVRE, mapa);
        fseek(mapa, -63*sizeof(char), SEEK_CUR);
        fputc(objeto, mapa);
        fseek(mapa, -sizeof(char), SEEK_CUR);
        local -> y -= 20;
    }
    fseek(mapa, 61*sizeof(char), SEEK_CUR);
}

void andarbaixo(coordenadas *local, FILE *mapa)
{
    char objeto = fgetc(mapa);
    fseek(mapa, 61*sizeof(char), SEEK_CUR);
    if(fgetc(mapa) == LIVRE)
    {
        fseek(mapa, -63*sizeof(char), SEEK_CUR);
        fputc(LIVRE, mapa);
        fseek(mapa, 61*sizeof(char), SEEK_CUR);
        fputc(objeto, mapa);
        fseek(mapa, -sizeof(char), SEEK_CUR);
        local -> y += 20;
    }
    fseek(mapa, -63*sizeof(char), SEEK_CUR);
}

void soltarbomba(coordenadas local, char *bombas, char *informacoes, FILE *mapa)
{
    if(*bombas != '0')
    {
        *bombas -= 1;
        informacoes[8] = *bombas;
    }
}

void perdevida(char *vidas ,char *informacoes)
{
    *vidas -= 1;
    informacoes[21] = *vidas;
    if(*vidas == '0')
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

void encontra(char objeto, coordenadas *local, FILE *mapa)
{
    rewind(mapa);
    int contador = 0;
    char letra = fgetc(mapa);
    while(letra != objeto)
    {
        contador++;
        letra = fgetc(mapa);
    }
    fseek(mapa , -sizeof(char), SEEK_CUR);
    local -> x = (contador%61)*20;
    local -> y = (contador/61)*20;
}

int posicao(coordenadas local)
{
    return (local.y/20)*62 + local.x/20;
}

void centraliza(coordenadas local, FILE *mapa)
{
    fseek(mapa, posicao(local)*sizeof(char), SEEK_SET);
}

int main(void)
{
    //posição inicial
    Vector2 position = { 0.0f, 0.0f };
    FILE *mapa = fopen("world.txt", "r+");
    if(!mapa)
    {
        puts("ERRO - mapa nao pode ser aberto");
        return 1;
    }

    jogador bomberman;
    bomberman.vidas = '3';
    bomberman.bombas = '3';
    bomberman.pontos = 0;
    encontra(JOGADOR, &bomberman.local, mapa);

    char informacoes[50] = "Bombas: 3     Vidas: 3     Pontos: 0";

    InitWindow(LARGURA, ALTURA, "Felix Bomberman");

    //código abaixo para carregar as sprites. Extremamente importante esse código estar depois do initwindow
    Texture2D player_spr = LoadTexture("Sprites/Player/felix.png");
    Texture2D parede_spr = LoadTexture("Sprites/cenario/parede.png");
    Texture2D caixa_spr = LoadTexture("Sprites/cenario/caixa.png");
    Texture2D parede_destrutivel_spr = LoadTexture("Sprites/cenario/parede_destrutivel.png");
    Texture2D bau_spr = LoadTexture("Sprites/interagiveis/bau.png");
    Texture2D chave_spr = LoadTexture("Sprites/interagiveis/chave.png");
    Texture2D chao_spr = LoadTexture("Sprites/cenario/chao.png");

    //segundo o site oficial, checa se a textura e valida e esta carregada na GPU, retorna TRUE 
    if (!IsTextureValid(player_spr))
    {
        printf("Não foi possível achar o sprite do felix. PROCURE O FELIX!\n");
        CloseWindow();
        return 1;    
    }
        if (!IsTextureValid(parede_spr))
    {
        printf("Não foi possível achar o sprite da parede.\n");
        CloseWindow();
        return 1;    
    }
            if (!IsTextureValid(parede_destrutivel_spr))
    {
        printf("Não foi possível achar o sprite da parede destrutivel.\n");
        CloseWindow();
        return 1;    
    }
        if (!IsTextureValid(caixa_spr))
    {
        printf("Não foi possível achar o sprite da caixa.\n");
        CloseWindow();
        return 1;    
    }
        if (!IsTextureValid(bau_spr))
    {
        printf("Não foi possível achar o sprite do bau.\n");
        CloseWindow();
        return 1;    
    }
        if (!IsTextureValid(chave_spr))
    {
        printf("Não foi possível achar o sprite da chave.\n");
        CloseWindow();
        return 1;    
    }
        if (!IsTextureValid(chao_spr))
    {
        printf("Não foi possível achar o sprite do chao.\n");
        CloseWindow();
        return 1;    
    }

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        centraliza(bomberman.local, mapa);
        if(IsKeyPressed(KEY_RIGHT)) andardireita(&bomberman.local, mapa);
        if(IsKeyPressed(KEY_LEFT)) andaresquerda(&bomberman.local, mapa);
        if(IsKeyPressed(KEY_UP)) andarcima(&bomberman.local, mapa);
        if(IsKeyPressed(KEY_DOWN)) andarbaixo(&bomberman.local, mapa);
        if(IsKeyPressed(KEY_B)) soltarbomba(bomberman.local, &bomberman.bombas, informacoes, mapa);
        if(IsKeyPressed(KEY_K)) perdevida(&bomberman.vidas , informacoes);
        rewind(mapa);
        coordenadas pixel = {0, 0};

        BeginDrawing();
            ClearBackground(RAYWHITE);
            //adicionando textura no chao inteiro
            //primeiro eu crio um triangulo do tamanho da tela
            Rectangle sourceRec = { 0.0f, 0.0f, LARGURA, 500 }; // o motivo de ser 500 e não ALTURA e pq ali em baixo fica a hud ne, se quiser entender melhor troca isso para "ALTURA"
            //esse código desenha o sprite no retangulo criado. Como o sprite(tam: 20x20) é bem menor que o retangulo (tam: tela), ele vai preenchendo até cobrir tudo
            DrawTextureRec(chao_spr, sourceRec, position, WHITE);


            char objeto = fgetc(mapa);
            while(!feof(mapa))
            {

                switch(objeto)
                {
                    case PAREDE:
                    DrawTexture(parede_spr, pixel.x, pixel.y, WHITE);
                    break;

                    case DESTRUTIVEL:
                    DrawTexture(parede_destrutivel_spr, pixel.x, pixel.y, WHITE);
                    break;

                    case CAIXA:
                    DrawTexture(caixa_spr, pixel.x, pixel.y, WHITE);
                    break;

                    case BAU:
                    DrawTexture(bau_spr, pixel.x, pixel.y, WHITE);
                    break;

                    case CHAVE:
                    DrawTexture(chave_spr, pixel.x, pixel.y, WHITE);
                }
                pixel.x += 20;
                if(pixel.x/1220)
                {
                    pixel.x = 0;
                    pixel.y += 20;
                }
                objeto = fgetc(mapa);
            }
        
            DrawTexture(player_spr, bomberman.local.x, bomberman.local.y, WHITE);
            // código antigo: DrawRectangle(bomberman.local.x, bomberman.local.y, 20, 20, RED);
            DrawText(informacoes, 20, 520, 60, GREEN);
        EndDrawing();
    }

    //Necessário para descarregar as texturas
    UnloadTexture(player_spr);
    UnloadTexture(parede_spr);
    UnloadTexture(parede_destrutivel_spr);
    UnloadTexture(caixa_spr);
    UnloadTexture(bau_spr);
    UnloadTexture(chave_spr);
    UnloadTexture(chao_spr);

    fclose(mapa);
    CloseWindow();
    printf("%s", informacoes);
    return 0;
}