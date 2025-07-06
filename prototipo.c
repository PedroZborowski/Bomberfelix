#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    char lifes;
    char bombs;
    int points;
} player;

typedef struct
{
    char nome[11];
    int pontos;
} record;

/*void teste(player bomberman)
{
    record salvando;
    salvando.pontos = bomberman.points;
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
        scanf("%s", salvando.nome);
        fgetc(stdin);
        fwrite(&salvando, sizeof(record), 1, file);
        strcpy(salvando.nome, "nada");
        salvando.pontos = 0;
        for(int i = 0; i < 9; i++)
        {
            fwrite(&salvando, sizeof(record), 1, file);
        }
        puts("Record salvo com sucesso");
    }
    else
    {
        puts("Arquivo ja existe, verificando onde a pontuacao ficara"); //ta dando erro qd tento salvar na 1 posicao
        for(int j = 0; j < 10; j++)
        {
            record lendo;
            fread(&lendo, sizeof(record), 1, file);
            if(lendo.pontos < bomberman.points)
            {
                printf("pontuacao na posicao %d\n", j+1);
                fseek(file, (long int)-sizeof(record), SEEK_CUR);
                puts("Digite seu nome: ");
                scanf("%s", salvando.nome);
                fgetc(stdin);
                fwrite(&salvando, sizeof(record), 1, file);
                record tentando[50];
                int i = j;
                while(i < 9)
                {
                    tentando[i] = lendo;
                    fread(&lendo, sizeof(record), 1, file);
                    i++;
                }
                tentando[i] = lendo;
                fseek(file, (j+1)*sizeof(record), SEEK_SET);
                j++;
                while(j < 10)
                {
                    fwrite(tentando+j, sizeof(record), 1, file);
                    j++;
                }
            }
        }
    }
    fclose(file);
}*/

void teste(player bomberman)
{
    record salvando;
    salvando.pontos = bomberman.points;
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
        scanf("%s", salvando.nome);
        fgetc(stdin);
        fwrite(&salvando, sizeof(record), 1, file);
        strcpy(salvando.nome, "nada");
        salvando.pontos = 0;
        for(int i = 0; i < 9; i++)
        {
            fwrite(&salvando, sizeof(record), 1, file);
        }
        puts("Record salvo com sucesso");
    }
    else
    {
        puts("Arquivo ja existe, verificando onde a pontuacao ficara");
        record tentando[10];
        int check = 10;
        for(int i = 0; i < 10; i++)
        {
            fread(tentando+i, sizeof(record), 1, file);
            if(check == 10) if((tentando+i)->pontos < bomberman.points) check = i;
        }
        fseek(file, check*sizeof(record), SEEK_SET); //(long int)
        if(check != 10)
        {
            printf("Pontuacao na posicao %d\n", check+1);
            puts("Digite seu nome: "); //if check != 10
            scanf("%s", salvando.nome);
            fgetc(stdin);
            fwrite(&salvando, sizeof(record), 1, file);
            for(int j = check; j < 9; j++)
            {
                fwrite(tentando+j, sizeof(record), 1, file);
            }
            puts("Record salvo com sucesso");
        }
        else
        {
            puts("Nao obteve uma pontuacao suficiente");
        }
    }
    fclose(file);
}

int main()
{
    player bomberman;
    bomberman.points = 0;
    int num;
    while(num != 0)
    {
        puts("Digite um numero para somar na pontuacao ou 0 para encerrar");
        scanf("%d", &num);
        bomberman.points += num;
    }
    fgetc(stdin);
    printf("Pontuacao a ser salva: %d\n", bomberman.points);

    teste(bomberman);

    FILE *file = fopen("record.dat", "rb");
    if(!file)
    {
        puts("ERRO");
        return -1;
    }
    puts("Arquivo aberto para leitura: ");
    record salvando;
    fread(&salvando, sizeof(record), 1, file);
    while(!feof(file))
    {
        printf("%s: %d\n", salvando.nome, salvando.pontos);
        fread(&salvando, sizeof(record), 1, file);
    }
    fclose(file);
    return 0;
}