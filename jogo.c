#include "raylib.h"
#include <string.h>
#define LARGURA 1200
#define ALTURA 600

typedef struct
{
    int x;
    int y;
} coordenadas;

typedef struct
{
    coordenadas local;
    int vidas;
    int bombas;
} player;

typedef struct
{
};



int main(void){
int x = 80;
int y = 80;


//Inicializações
InitWindow(LARGURA, ALTURA, "Colisao"); //Inicializa janela (tamanho e título)
SetTargetFPS(60);// Ajusta a execução do jogo para 60 frames por segundo
//Laço principal do jogo
while (!WindowShouldClose()) // Detecta botão de fechamento da janela ou ESC
{
// Trata entrada do usuário e atualiza estado do jogo
if(IsKeyPressed(KEY_RIGHT)) x += 20;
if(IsKeyPressed(KEY_LEFT)) x -= 20;
if(IsKeyPressed(KEY_UP)) y -= 20;
if(IsKeyPressed(KEY_DOWN)) y += 20;
// Atualiza a representação visual a partir do estado do jogo
BeginDrawing(); //Inicia o ambiente de desenho na tela
ClearBackground(RAYWHITE); //Limpa a tela e define cor de fundo
for(int i = 0; i < 8; i++)
{
    for(int j = 0; j < 8; j++)
    {
        if(1) DrawRectangle(i*20, j*20, 20, 20, BLACK);
    }
}
DrawRectangle(x, y, 20, 20, RED); //Desenha texto, posição, tamanho e cor
EndDrawing(); //Finaliza o ambiente de desenho na tela
}
CloseWindow(); // Fecha a janela e o contexto OpenGL
return 0;
}