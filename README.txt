Alunos:
Marco Antonio Ribeiro
Jose Ricardo Goncalves

Compilacao:
make

Execucao:
make test

Execucao pra estagio especifico:
bin/tp1 <estagio>

bibliotecas utilizadas:
- OpenGL:
	* Funcoes basicas como habilitar recursos e trabalhar matrizes
- GLFW:
	* Gerencimanto de janelas e eventos
- GLU:
	* Utilizacao pela gluPerspective
- GLEW
	* Normaliza as funcoes de shader
- SDL2 / SDL2_Mixer
	* Audio

Funcoes implementadas:
- O jogo comeca no modo pausa;
- A tecla R volta o jogo ao estado inicial, paddle no meio da tela e bola parada na posicao inicial;
- A tecla Q termina o programa;
- O botao esquerdo do mouse pausa ou retorna ao jogo;
- O botao da direita imprime os atributos de todos os objetos, e coloca o jogo em modo pausa, pressionado novamente, executa um ciclo do jogo, e novamente imprime todos os atributos;
- Tijolos:
	* normais: possuem de 1 a 3 vidas ou sao indestrutiveis, representada pela opacidade de uma borda branca que os envolve (transparente em 1 vida e muito opaca quando indestrutivel)
	* bonus: tambem possuem de 1 a 3 vidas e apresentam borda, mas nao podem ser indestrutiveis. Tipos de bonus:
		> aumentar temporariamente o tamanho do paddle;
		> aumentar temporariamente o tamanho da bola;
	 	> aumentar e/ou diminuir a velocidade da bola;
		> rotacionar a tela;
	 	> gerar ondulacoes no jogo
	* abstratos: possuem apenas uma vida ou sao indestrutiveis, no caso desses a bola passa direto por dentro, sem reflexao
- Controle de volume da musica pelo teclado:
	* botao '=' aumenta volume
		> com shift: volume maximo
	* botao '-' diminui volume
		> com shift: mudo
- Musicas que variam por fases, sons que variam em cada bonus e efeitos sonoros de colisao;
- A velocidade do paddle e' usada para calcular uma variacao no angulo refletido da bola (80% da velocidade do paddler e' acrescido 'a velocidade da bola);
- O deslocamento vertical do mouse pode indicar um acrescimo na forca a ser usada na rebatida da bola (multiplicador 1.5 no topo e 0.5 no chao);
- A posicao que a bola acerta no paddle tambem e' utilizada para modificar a velocidade da bola (multiplicador 1.2 nas bordas e 0.8 no centro);
- Utilizacao de shaders:
	* bonus de rotacionar tela
	* bonus de ondular a tela
- Facilidade na criacao de novas fases
	* foi criado um formato de arquivo para criacao de uma fase
	* existem sete niveis de dificuldades criados, em cada nivel existe uma fase diferente
- Numero de vidas na tela;
- Tela de vitoria e derrota atraves de texturas;

Equacao final de velocidade da bola:
(velocidade_atual + (velocidade_paddler * 0.8)) * posicao_relativa_paddler * posicao_y_mouse
