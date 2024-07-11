#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<stdlib.h>
#include<time.h>
#include<vector>
#include<iostream>
#define WIDTH_SCREEN 600
#define HEIGHT_SCREEN 1000
using namespace std;
using namespace sf;

// Funcao que detecta colisao entre blocos
bool collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2) {
	if (x1 + w1 >= x2 && x1 <= x2 + w2 && y1 + h1 >= y2 && y1 <= y2 + h2) {
		return true;
	}
	return false;
}

int main() {
	// Cria uma janela e as determinadas configuracoes
	RenderWindow window(VideoMode(HEIGHT_SCREEN, WIDTH_SCREEN), "Flappy Bird (Zaralha's Version)");
	window.setFramerateLimit(60);
	window.setKeyRepeatEnabled(false);
	srand(time(0));

	// Todos os sons do jogo a seguir
	struct Sounds {
		SoundBuffer chingBuffer;
		SoundBuffer hopBuffer;
		SoundBuffer dishkBuffer;
		Sound ching;
		Sound hop;
		Sound dishk;
	} sounds;

	// Carregar os sons
	sounds.chingBuffer.loadFromFile("./Resources/audio/score.wav");
	sounds.hopBuffer.loadFromFile("./Resources/audio/flap.wav");
	sounds.dishkBuffer.loadFromFile("./Resources/audio/crash.wav");
	sounds.ching.setBuffer(sounds.chingBuffer);
	sounds.hop.setBuffer(sounds.hopBuffer);
	sounds.dishk.setBuffer(sounds.dishkBuffer);

	// Agora todas as texturas (o Flappy, como bate as asas, possui 3 texturas)
	struct Textures {
		Texture flappy[3]; // Passaro
		Texture pipe; // Cano
		Texture background; // Plano de fundo
		Texture gameover; // Perdeu PlayBoy
		Texture title; // Titulo do game
	} textures;

	// Carregar as texturas
	textures.background.loadFromFile("./Resources/images/background.png");
	textures.title.loadFromFile("./Resources/images/title.png");
	textures.pipe.loadFromFile("./Resources/images/pipe.png");
	textures.gameover.loadFromFile("./Resources/images/gameover.png");
	textures.flappy[0].loadFromFile("./Resources/images/zaralha1.png");
	textures.flappy[1].loadFromFile("./Resources/images/zaralha2.png");
	textures.flappy[2].loadFromFile("./Resources/images/zaralha3.png");

	// Estrutura do Flappy | v = velocidade vertical | frame = qual textura?
	struct Flappy {
		double v = 0;
		int frame = 0;
			Sprite sprite;
	} flappy;

	// Posicao inicial, dimensoes
	flappy.sprite.setPosition(250, 300);
	flappy.sprite.setScale(2, 2);

	// Um vetor que pode ter seu tamanho manipulado para podermos representar os canos
	vector<Sprite> pipes;

	// waiting = Jogo nao iniciado | started = vamooo | gameover = perdeu playboy
	enum GameState {waiting, started, gameover};

	// Configuracao do jogo utilizando struct
	// frames conta o total de frames passados ??desde o inicio
	struct Game {
		int score = 0, highscore = 0, frames =0;
		GameState gameState = waiting;
		Sprite background[3];
		Sprite gameover;
		Sprite title;
		Text pressSpace;
		Text pressC;
		Text scoreText;
		Text highscoreText;
		Font font;

	} game;
	
	// Carregar a fonte, configurar posicoes e escalas
	game.font.loadFromFile("../Resources/fonts/flappybird.ttf");
	game.background[0].setTexture(textures.background);
	game.background[1].setTexture(textures.background);
	game.background[2].setTexture(textures.background);
	game.background[0].setScale(1.15625, 1.171875);
	game.background[1].setScale(1.15625, 1.171875);
	game.background[2].setScale(1.15625, 1.171875);
	game.background[1].setPosition(333, 0); 
	game.background[2].setPosition(666, 0);
	game.title.setTexture(textures.title);
	game.title.setOrigin(192 / 2, 42 / 2);
	game.title.setPosition(500, 125);
	game.title.setScale(2, 2);
	game.gameover.setTexture(textures.gameover); 
	game.gameover.setOrigin(192 / 2, 42 / 2);
	game.gameover.setPosition(500, 125);
	game.gameover.setScale(2, 2);
	game.pressSpace.setString("Pressione SPACE para comecar"); // O tal do texto
	game.pressSpace.setFont(game.font);
	game.pressSpace.setFillColor(Color::Black); // Cor das letras
	game.pressSpace.setCharacterSize(50); // Tamanho das letras
	game.pressSpace.setOrigin(game.pressC.getLocalBounds().width / 2, 0);
	game.pressSpace.setPosition(100, 250);
	game.pressC.setString("Pressione C para continuar"); 
	game.pressC.setFont(game.font); 
	game.pressC.setFillColor(Color::Black); 
	game.pressC.setCharacterSize(50); 
	game.pressC.setOrigin(game.pressC.getLocalBounds().width / 2, 0); 
	game.pressC.setPosition(500, 250); 
	game.scoreText.setFont(game.font); 
	game.scoreText.setFillColor(Color::White); 
	game.scoreText.setCharacterSize(75);
	game.scoreText.move(30, 0);
	game.highscoreText.setFont(game.font);
	game.highscoreText.setFillColor(Color::White);
	game.highscoreText.move(30, 80);

	// Agora o loop (muah ha ha ha)
	while (window.isOpen()) { // Basicamente um laco que ocorre se o jogo estiver aberto
		// Atualizar a pontuacao (score)
		flappy.sprite.setTexture(textures.flappy[1]);
		game.scoreText.setString(to_string(game.score));
		game.highscoreText.setString("MAX " + to_string(game.highscore));
		
		// Atualizar o flappy (flappy mesmo)
		float fx = flappy.sprite.getPosition().x;
		float fy = flappy.sprite.getPosition().y;
		float fw = 34 * flappy.sprite.getScale().x;
		float fh = 24 * flappy.sprite.getScale().y;

		// Asas do flappy enquanto joga
		if (game.gameState == waiting || game.gameState == started) {
			// Mudar a textura uma vez em 6 frames
			if (game.frames % 6 == 0) {
				flappy.frame += 1;
			}
			if (flappy.frame == 3) {
				flappy.frame = 0;
			}
		}
		flappy.sprite.setTexture(textures.flappy[flappy.frame]);

		// Mover o flappy
		if (game.gameState == started) {
			flappy.sprite.move(0, flappy.v);
			flappy.v += 0.5;
		}

		// O teto eh o limite e se sair da tela eh gameover
		if (game.gameState == started) {
			if (fy < 0) {
				flappy.sprite.setPosition(250, 0);
				flappy.v = 0;
			} else if (fy > 600) {
				flappy.v = 0;
				game.gameState = gameover;
				sounds.dishk.play(); // Toca aquele sonzinho tiltante de VOCE BATEU AI CARA
			}
		}
		// Contar a sua pontuacao
		for (vector<Sprite>::iterator itr = pipes.begin(); itr != pipes.end(); itr++) {
			if (game.gameState == started && (*itr).getPosition().x == 250) {
				game.score++;
				sounds.ching.play(); // Toca aquele plin quando voce passa pelos canos
				if (game.score > game.highscore) {
					game.highscore = game.score; 
					// Se conseguir um score melhor ficara salvo!
				}
				break;
			}
		}
		// Hora de gerar os canos
		if (game.gameState == started && game.frames % 150 == 0) {
			int r = rand() % 275 + 75; // Gera um numero aleatorio, ajudara no setPosition dos canos.
			int gap = 150; // A distancia entre o cano de cima e o de baixo!

			// Cano de baixo:
			Sprite lowerPipe;
			lowerPipe.setTexture(textures.pipe);
			lowerPipe.setPosition(1000, r + gap);
			lowerPipe.setScale(2, 2);

			// Cano de cima:
			Sprite upperPipe;
			upperPipe.setTexture(textures.pipe);
			upperPipe.setPosition(1000, r);
			upperPipe.setScale(2, -2);

			// Puxar para a lista
			pipes.push_back(lowerPipe);
			pipes.push_back(upperPipe);
		}
		// Movendo os canos
		if (game.gameState == started) {
			for (vector<Sprite>::iterator itr = pipes.begin(); itr != pipes.end(); itr++) {
				(*itr).move(-3, 0);
			}
		}
		// Removendo os canos que nao estao mais na tela;
		if (game.frames % 100 == 0) {
			if (game.frames % 100 == 0) {
				for (auto itr = pipes.begin(); itr != pipes.end();) {
					if ((*itr).getPosition().x < -104) {
						itr = pipes.erase(itr); // Remove o cano e avança o iterador
					}
					else {
						++itr; // Avança para o próximo cano
					}
				}
			}

		}
		// Agora, precisamos de algo que detecte as colisoes
		if (game.gameState == started) {
			for (vector<Sprite>::iterator itr = pipes.begin(); itr != pipes.end(); itr++) {
				float px, py, pw, ph; // Sao variaveis relacionadas ao cano
				if ((*itr).getScale().y > 0) {
					px = (*itr).getPosition().x;
					py = (*itr).getPosition().y;
					pw = 52 * (*itr).getScale().x;
					ph = 320 * (*itr).getScale().y;
				} else {
					pw = 52 * (*itr).getScale().x;
					ph = -320 * (*itr).getScale().y;
					px = (*itr).getPosition().x;
					py = (*itr).getPosition().y - ph;
				}
				if (collision(fx, fy, fw, fh, px, py, pw, ph)) {
					game.gameState = gameover;
					sounds.dishk.play();
				}
			}
		}

		// Eventos
		Event event;
		while (window.pollEvent(event)) {
			// tchau!
			if (event.type == Event::Closed) {
				window.close();
			// Flap
			} else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Space) {
				if (game.gameState == waiting) {
					game.gameState = started; // Se você apertar o espaco o jogo muda do status 'waiting' para 'started'. Ou seja, comecou!
				}
				if (game.gameState == started) {
					flappy.v = -8;
					sounds.hop.play();
				}
			// Recomecando...
			}
			else if (event.type == Event::KeyPressed && event.key.code == Keyboard::C && game.gameState == gameover) {
				game.gameState = waiting;
				flappy.sprite.setPosition(250, 300);
				game.score = 0;
				pipes.clear();
			}
		}
		// Limpar a tela | Imagens na tela
		window.clear();
		window.draw(game.background[0]);
		window.draw(game.background[1]);
		window.draw(game.background[2]);
		window.draw(flappy.sprite);

		// Imagem dos canos
		for (vector<Sprite>::iterator itr = pipes.begin(); itr != pipes.end(); itr++) {
			window.draw(*itr);
		}

		// Imagem da pontuacao e pontuacao maxima
		window.draw(game.scoreText);
		window.draw(game.highscoreText);

		// Titulo do jogo, pressione SPACE para começar!
		if (game.gameState == waiting) {
			window.draw(game.title);
			if (game.frames % 60 < 30) {
				window.draw(game.pressSpace);
			}
		}
		// Game Over, pressione C para continuar!
		if (game.gameState == gameover) {
			window.draw(game.gameover);
			if (game.frames % 60 < 30) {
				window.draw(game.pressC);
			}
		}
		window.display();

		// Atualizando todos os frames...
		game.frames++;
	}
	return 0;
}
