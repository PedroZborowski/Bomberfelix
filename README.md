# Bomberfelix
<div>
<p>O objetivo deste trabalho foi consolidar os conhecimentos adquiridos na disciplina de Programação de Computadores II através da implementação de uma versão personalizada do clássico jogo Bomberman. Utilizando a linguagem C e a biblioteca gráfica Raylib, desenvolvemos o BombermanFelix.</p>
<p>A temática do jogo é centrada no personagem Felix, um estudante de Ciência da Computação da UFRJ, que se vê em meio a um apocalipse zumbi na Faculdade de Letras. O jogo é composto, basicamente, por duas fases, sendo uma delas os corredores de Letras e, a segunda fase os "calabouços"de Letras.</p>
<p>No jogo você controla o Felix e tem o objetivo inicial de encontrar as 5 chaves perdidas pela instalação, para entrar no calabouço. Pelos corredores de Letras, você deve colocar bombas para destruir paredes que estão em seu caminho, evitar inimigos aterrorizantes, quebrar caixas em busca da chave e, o mais importante: se divertir. Cada ação sua te dão pontos, esses serão seus records, maximize eles!</p>

<p>O jogo buscou atingir todos requisitos mínimos propostos, mas além dos requisitos mínimos, o projeto conta com diferenciais como trilha sonora e efeitos sonoros de autoria própria, e um sistema de "emotes"com falas aleatórias.</p>
</div>

## Como compilar

<p>Para compilar e executar o jogo é necessário ter a raylib e o arquivo descompactado do
jogo. <strong>Para baixar a raylib:</strong></p>

<ol>
<li>1. Vá para a página de releases da raylib: https://github.com/raysan5/raylib/releases</li>
<li>2. Baixe a versão pré-compilado adequada para o seu sistema operacional</li>
<li>3. Descompacte o arquivo em alguma pasta de fácil acesso.</li>
</ol>

<p>Com a raylib instalada, dê git clone neste repositório e prossiga:</p>

## Compilação

<p>Dentro da pasta principal, compile o jogo com o código abaixo.</p>
<p>Lembre-se de substituir o "SeuUsuario"e o local do arquivo.</p>

```bash
gcc jogo03.c -o jogo3 -I /usuarios/SeuUsuario/localdaRaylib/raylib/raylib-5.5_linux_amd64/include -L /usuarios/SeuUsuario/localdaRaylib/raylib/raylib-5.5_linux_amd64/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

<p>Após ter compilado, execute o jogo com:</p>
```bash
./jogo3
```

Caso de erro e não ache a biblioteca, use o seguinte comando:
```bash
LD_LIBRARY_PATH=/usuarios/SeuUsuario/localdaRaylib/raylib/raylib-5.5_linux_amd64/lib ./jogo03
```
