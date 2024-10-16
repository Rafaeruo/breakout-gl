// BreakoutGL - Jogo de Breakout em 2D com OpenGL
// para a disciplina de Processamento Gráfico - Unisinos, 2024/2

// Bibliotecas padrão
#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <cmath>

// Bibliotecas terceiras
#include "../deps/include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Bibliotecas locais
#include "Sprite.h"
#include "MovingSprite.h"
#include "Collision.h"
#include "Texture.h"
#include "Helpers.cpp"

// Protótipos de funções no fim do arquivo
float calculateNextPlayerVelocity(bool continuousMovement, float currentVelocity);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
int setupShader();
std::vector<Sprite>  initializeRectangleSprites(); 
MovingSprite initializeBallSprite();
Sprite initializeCharacterSprite();

const GLuint WIDTH = 600, HEIGHT = 800;

const GLchar *vertexShaderSource = "#version 400\n"
								   "layout (location = 0) in vec3 position;\n"
								   "layout (location = 1) in vec2 texc;\n"
								   "uniform mat4 projection;\n"
								   "uniform mat4 model;\n"
								   "out vec2 texCoord;\n"
								   "void main()\n"
								   "{\n"
								   "gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);\n"
								   "texCoord = vec2(texc.s, 1.0 - texc.t);\n"
								   "}\0";

const GLchar *fragmentShaderSource = "#version 400\n"
									 "in vec2 texCoord;\n"
									 "uniform sampler2D texBuffer;\n"
									 "uniform vec2 offsetTex;\n"
									 "out vec4 color;\n"
									 "void main()\n"
									 "{\n"
									 "color = texture(texBuffer, texCoord + offsetTex);\n"
									 "}\n\0";

float currentCharacterVelocity = 2;
bool previousPlayerMovementDirection = true; // Direita ou esquerda
bool activeKeys[1024] = {false};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "BreakoutGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informações de versão
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported " << version << std::endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	glUseProgram(shaderID);

	// Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a variáveis do tipo uniform em GLSL para armazenar esse tipo de info
	// que não está nos buffers
	glUniform1i(glGetUniformLocation(shaderID, "texBuffer"), 0);

	// Matriz de projeção ortográfica
	mat4 projection = ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f);

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "projection"), 1, GL_FALSE, value_ptr(projection));

	// Ativando o primeiro buffer de textura da OpenGL
	glActiveTexture(GL_TEXTURE0);

	// Habilitar a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Habilitar o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	// Inicializar os sprites de fundos, personagem, retângulos e bola
	int imgWidth, imgHeight;
	int texID = loadTexture("../Texturas/bg.png", imgWidth, imgHeight);
	Sprite background = Sprite(texID, vec3(WIDTH / 2, HEIGHT / 2, 0.0), vec3(600 * 1.2, 800 * 1.2, 1.0), 0, 1, 1);
	background.setup();
	texID = loadTexture("../Texturas/estrelaLonge.png", imgWidth, imgHeight);
	Sprite backgroundLonge = Sprite(texID, vec3(WIDTH / 2, HEIGHT / 2, 0.0), vec3(600 * 1.2, 800 * 1.2, 1.0), 0, 1, 1);
	backgroundLonge.setup();
	texID = loadTexture("../Texturas/estrelaMedio.png", imgWidth, imgHeight);
	Sprite backgroundMedio = Sprite(texID, vec3(WIDTH / 2, HEIGHT / 2, 0.0), vec3(600 * 1.2, 800 * 1.2, 1.0), 0, 1, 1);
	backgroundMedio.setup();
	texID = loadTexture("../Texturas/estrelaPerto.png", imgWidth, imgHeight);
	Sprite backgroundPerto = Sprite(texID, vec3(WIDTH / 2, HEIGHT / 2, 0.0), vec3(600 * 1.2, 800 * 1.2, 1.0), 0, 1, 1);
	backgroundPerto.setup();

	Sprite character = initializeCharacterSprite();
	std::vector<Sprite> rectangles = initializeRectangleSprites();
	MovingSprite ball = initializeBallSprite();

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		vec2 offsetTex = vec2(0.0, 0.0); // Animações não serão utilizadas
		glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), offsetTex.s, offsetTex.t);

		// Renderizar fundos com parallax
		float diffX = character.getPosition().x - WIDTH / 2; // Diferença do personagem do centro da tela para aplicar parallax
		background.setPosition(vec3((WIDTH / 2) - diffX * 0.1, HEIGHT / 2, 0.0));
		background.render(shaderID);

		backgroundLonge.setPosition(vec3((WIDTH / 2) - diffX * 0.1, HEIGHT / 2, 0.0));
		backgroundLonge.render(shaderID);

		backgroundMedio.setPosition(vec3((WIDTH / 2) - diffX * 0.2, HEIGHT / 2, 0.0));
		backgroundMedio.render(shaderID);

		backgroundPerto.setPosition(vec3((WIDTH / 2) - diffX * 0.3, HEIGHT / 2, 0.0));
		backgroundPerto.render(shaderID);

		// Mover jogador
		bool moved = false;
		if (activeKeys[GLFW_KEY_LEFT]  && !checkLeftBound(character, WIDTH)) {
			currentCharacterVelocity = calculateNextPlayerVelocity(!previousPlayerMovementDirection, currentCharacterVelocity);
			previousPlayerMovementDirection = false;
			moved = true;
			character.getPosition().x -= currentCharacterVelocity;
		}
		else if (activeKeys[GLFW_KEY_RIGHT] && !checkRightBound(character, WIDTH)) {
			currentCharacterVelocity = calculateNextPlayerVelocity(previousPlayerMovementDirection, currentCharacterVelocity);
			previousPlayerMovementDirection = true;
			moved = true;
			character.getPosition().x += currentCharacterVelocity;
		}

		if (!moved) {
			currentCharacterVelocity = calculateNextPlayerVelocity(false, currentCharacterVelocity);
		}
		
		float now = glfwGetTime();
		character.update(now);
		character.render(shaderID);

		// Colisão da bola com os retângulos
		bool collision = false;
		for (auto& rectangle : rectangles) {
			if (!collision && checkCollision(rectangle, ball)) {
				collision = true;
				ball.setVelocity(vec3(ball.getVelocity().x, -ball.getVelocity().y, 0));
				rectangle.getPosition().y = -1000.0f; // Fazer o retângulo sumir
			}
			rectangle.render(shaderID);
		}

		// Colisão da bola com as paredes
		checkBallBoundsCollision(ball, WIDTH, HEIGHT);

		// Colisão entre jogador e bola
		if (!collision && checkCollision(character, ball)) {
			collision = true;
			changeBallVelocityAfterCollision(ball, character);
		}

		// Jogador não rebateu a bola
		if (!collision && ball.getPosition().y < character.getPosition().y - character.getDimensions().y) {
			ball.setVelocity(vec3(0.0f, 0.0f, 0.0f));
		}

		// Girar a bola
		ball.setAngle(ball.getAngle() + 5);
		ball.update(now);
		ball.render(shaderID);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

Sprite initializeCharacterSprite() {
	int imgWidth, imgHeight;
	int texID = loadTexture("../Texturas/personagem.png", imgWidth, imgHeight);

	Sprite character = Sprite(texID, vec3(WIDTH / 2, 150, 0.0), vec3(64, 111, 0), 0, 1, 1);
	character.setup();
	return character;
}

MovingSprite initializeBallSprite() {
	int imgWidth, imgHeight;

	float ballWidth = 25.0f;
	float ballHeight = 25.0f;
	float ballX = WIDTH / 2.0f;
	float ballY = HEIGHT / 2.0f;

	int texID = loadTexture("../Texturas/bola.png", imgWidth, imgHeight);
	MovingSprite ball = MovingSprite(
		texID, 
		vec3(ballX, ballY, 0.0f), 
		vec3(ballWidth, ballHeight, 0.0f), 
		0, 1, 1);

	ball.setup();
	ball.setVelocity(generateRandomVelocity(6));
	return ball;
}

std::vector<Sprite>  initializeRectangleSprites() {
	const int numLayers = 4;
	const int numRectanglesPerLayer = 15;
	const int rectangleWidth = WIDTH / numRectanglesPerLayer;
	const int rectangleHeight = 15;

	std::vector<Sprite> rectangles;
	rectangles.reserve(numLayers * numRectanglesPerLayer);

	int layer = 1;
	int texID;
	int imgWidth, imgHeight;
	for (size_t i = 0; i < numLayers; i++)
	{
		texID = loadTexture("../Texturas/degrau" + std::to_string(layer) + ".png", imgWidth, imgHeight);
		for (size_t j = 1; j < numRectanglesPerLayer - 1; j++)
		{
			if (j == 4 || j == numRectanglesPerLayer - 5) {
				continue;
			}

			Sprite rectangle = Sprite(
				texID, 
				vec3((j * rectangleWidth) + (rectangleWidth / 2), 700.0 + (i * rectangleHeight), 0.0), 
				vec3(rectangleWidth, rectangleHeight, 0.0), 
				0, 1, 1);
			rectangle.setup();
			
			rectangles.push_back(rectangle);
		}

		layer++;
	}

	return rectangles;
}

float calculateNextPlayerVelocity(bool continuousMovement, float currentVelocity) {
	const float rate = 1.03;
	const int maxVelocity = 7;
	const int minVelocity = 2;

	if (continuousMovement) {
		float newVelocity = pow(currentVelocity, rate);

		if (newVelocity > maxVelocity) {
			return 7;
		} 

		return newVelocity;
	}

	return minVelocity;
}

// Funções baseadas no conteúdo de aula:

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
	{
		activeKeys[key] = true;
	}
	if (action == GLFW_RELEASE)
	{
		activeKeys[key] = false;
	}
}

// Esta função está basntante hardcoded - objetivo é compilar e "buildar" um programa de
//  shader simples e único neste exemplo de código
//  O código fonte do vertex e fragment shader está nos arrays vertexShaderSource e
//  fragmentShader source no iniçio deste arquivo
//  A função retorna o identificador do programa de shader
int setupShader()
{
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Checando erros de compilação (exibição via log no terminal)
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Checando erros de compilação (exibição via log no terminal)
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
				  << infoLog << std::endl;
	}
	// Linkando os shaders e criando o identificador do programa de shader
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Checando por erros de linkagem
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
				  << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}