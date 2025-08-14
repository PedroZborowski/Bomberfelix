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
## Funcionalidades desenvolvidas

<p>1. <strong>Menu Principal e Navegação:</strong> O jogo apresenta um menu com as opções: Novo Jogo, Jogo Salvo, Recordes, Controles, Criadores e Sair. A navegação é realizada pelas setas direcionais e a seleção pela tecla enter, um sprite de uma bomba indica qual é a opção selecionada atual.</p>

<p>2. <strong>Mecânica de Jogo:</strong></p>

<ol type="a">
<li><strong>Movimentação e Animação:</strong> O personagem se move nos quatro eixos de maneira constante, com animações para cada direção e para o estado "parado".</li>
<li><strong>Sistema de Bombas:</strong> O jogador pode plantar bombas que explodem após 3 segundos, destruindo caixas, paredes destrutíveis e inimigos em uma área em cruz, atingindo 100 pixels no total. O jogador possui 3 bombas no total, isto é, pode colocar 3 bombas por vez. As bombas retornam ao inventário do jogador após explodidas. As bombas possuem animação para indicar que vão explodir e as explosão também possuem animações.</li>
<li><strong>Inimigos e IA Simples:</strong> Zumbis se movem pelo mapa e trocam de direção de tempos em tempos de maneira aleatória ou quando encostam em uma parede. Os zumbis também possuem animações para todas as direções.</li>
<li><strong>Coletáveis e Progressão:</strong> O objetivo é coletar 5 chaves, escondidas em caixas que você consegue destruir com suas bombas, coletando as 5 você abre um alçapão para avançar de nível.</li>
<li><strong>Vidas e Pontuação:</strong> O jogador inicia com 3 vidas e perde uma sempre que encosta na explosão da bomba ou encosta no inimigo. O jogador ganha pontos ao destruir inimigos e caixas, ou perde pontos ao sofrer dano.</li>
</ol>

<p>3. <strong>Salvar e Carregar Jogo:</strong> Implementada a funcionalidade de salvar o estado completo do jogo em um arquivo binário, que pode ser carregado posteriormente pelo menu. Se não existir ainda, ele é criado.</p>

<p>4. <strong>Sistema de Recordes:</strong> As 10 melhores pontuações são armazenadas. Ao fim de uma partida, o jogador é notificado caso tenha alcançado um novo recorde.</p>

<p>5. <strong>Recursos Autorais: </strong>O projeto possui uma trilha sonora e efeitos sonoros próprios, todos os sprites, sons e música do jogo foram feitos pelos desenvolvedores (curiosidade: foi usado violão, flauta, piano e até mesmo moveis para os som), além de um sistema de emotes que adiciona personalidade ao personagem principal, pois ao clicar o "t", o player solta frases de efeito do personagem Felix.</p>
