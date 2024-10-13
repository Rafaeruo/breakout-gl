/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 13/08/2024
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include<cmath>

using namespace std;

// GLAD
#include "../deps/include/glad/glad.h"

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// STB_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include "../deps/stb_image.h"

#include "Sprite.h"
#include "MovingSprite.h"

using namespace glm;

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
int loadTexture(string filePath, int &imgWidth, int &imgHeight);
void checkBoundsCollision(MovingSprite &ball);
bool checkCollision(Sprite &rect1, Sprite &rect2);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 600, HEIGHT = 800;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = "#version 400\n"
								   "layout (location = 0) in vec3 position;\n"
								   "layout (location = 1) in vec2 texc;\n"
								   "uniform mat4 projection;\n"
								   "uniform mat4 model;\n"
								   "out vec2 texCoord;\n"
								   "void main()\n"
								   "{\n"
								   //...pode ter mais linhas de código aqui!
								   "gl_Position = projection * model * vec4(position.x, position.y, position.z, 1.0);\n"
								   "texCoord = vec2(texc.s, 1.0 - texc.t);\n"
								   "}\0";

// Códifo fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = "#version 400\n"
									 "in vec2 texCoord;\n"
									 "uniform sampler2D texBuffer;\n"
									 "uniform vec2 offsetTex;\n"
									 "out vec4 color;\n"
									 "void main()\n"
									 "{\n"
									 "color = texture(texBuffer, texCoord + offsetTex);\n"
									 "}\n\0";

float vel(bool continuousMovement, float currentVel) {
	const float rate = 1.03;
	if (continuousMovement) {
		float newVel = pow(currentVel, rate);
		if (newVel > 7) {
			return 7;
		} 
		return newVel;
	}

	return 2;
}

float curVel = 2;
bool previousMovementDirection = true;

bool keys[1024] = {false};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "BreakoutGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informações de versão
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	// Sprite do fundo da cena
	// Carregando uma textura (recebendo seu ID)

	// Inicializando a sprite do background
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

	// Inicializando a sprite do personagem
	texID = loadTexture("../Texturas/personagem.png", imgWidth, imgHeight);
	Sprite character = Sprite(texID, vec3(WIDTH / 2, 150, 0.0), vec3(64, 111, 0), 0, 1, 1);
	character.setup();

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


	// create three layers (y axis) of rectangles
	// each layer should have 15 recangles of size 600/15
	const int numLayers = 3;
	const int numRectanglesPerLayer = 15;
	std::vector<Sprite> rectangles;
	rectangles.reserve(numLayers * numRectanglesPerLayer);

	texID = loadTexture("../Texturas/degrau1.png", imgWidth, imgHeight);

	for (size_t i = 0; i < numLayers; i++)
	{
		for (size_t j = 0; j < numRectanglesPerLayer; j++)
		{

			int rectangleWidth = 40;
			int rectangleHeight = 15;
			// Create a rectangle sprite
			Sprite rectangle = Sprite(
				texID, 
				vec3((j * rectangleWidth) + (j + 20), 700.0 + (i * rectangleHeight), 0.0), 
				vec3(rectangleWidth, rectangleHeight, 0.0), 
				0, 1, 1);
			rectangle.setup();
			
			// Store the rectangle in the array
			rectangles.push_back(rectangle);
		}
	}
	
	float ballWidth = 25.0f;  // Width of the sprite
	float ballHeight = 25.0f; // Height of the sprite

	// Calculate the center position
	float ballX = WIDTH / 2.0f;
	float ballY = HEIGHT / 2.0f;

	texID = loadTexture("../Texturas/bola2.png", imgWidth, imgHeight);
	MovingSprite ball = MovingSprite(
		texID, 
		vec3(ballX, ballY, 0.0f), 
		vec3(ballWidth, ballHeight, 0.0f), 
		0, 1, 1);
	ball.setup();
	ball.setVelocity(vec3(1, 1, 0));

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		vec2 offsetTex = vec2(0.0, 0.0);
		glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), offsetTex.s, offsetTex.t);

		float diffX = character.getPosition().x - WIDTH / 2;
		background.setPosition(vec3((WIDTH / 2) - diffX * 0.1, HEIGHT / 2, 0.0));
		background.render(shaderID);

		backgroundLonge.setPosition(vec3((WIDTH / 2) - diffX * 0.1, HEIGHT / 2, 0.0));
		backgroundLonge.render(shaderID);

		backgroundMedio.setPosition(vec3((WIDTH / 2) - diffX * 0.2, HEIGHT / 2, 0.0));
		backgroundMedio.render(shaderID);

		backgroundPerto.setPosition(vec3((WIDTH / 2) - diffX * 0.3, HEIGHT / 2, 0.0));
		backgroundPerto.render(shaderID);

		bool moved = false;
		if (keys[GLFW_KEY_LEFT] || keys[GLFW_KEY_A]) {
			curVel = vel(!previousMovementDirection, curVel);
			previousMovementDirection = false;
			moved = true;
			character.getPosition().x -= curVel;
		}
		if (keys[GLFW_KEY_RIGHT] || keys[GLFW_KEY_D]) {
			curVel = vel(previousMovementDirection, curVel);
			previousMovementDirection = true;
			moved = true;
			character.getPosition().x += curVel;
		}

		if (!moved) {
			curVel = vel(false, curVel);
		}
			
		// Incremento circular (em loop) do índice do frame

		float now = glfwGetTime();
		character.update(now);
		offsetTex.s = character.getTextureOffsetS();
		offsetTex.t = 0.0;
		// glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), offsetTex.s, offsetTex.t);
		character.render(shaderID);
		offsetTex.s = 0;
		offsetTex.t = 0;
		// glUniform2f(glGetUniformLocation(shaderID, "offsetTex"), offsetTex.s, offsetTex.t);


		bool collision = false;
		for (auto& rectangle : rectangles) {
			if (!collision && checkCollision(rectangle, ball)) {
				collision = true;
				ball.setVelocity(vec3(ball.getVelocity().x, -ball.getVelocity().y, 0));
				rectangle.getPosition().y = -1000.0f;
			}
			rectangle.render(shaderID); // Call render on each sprite
		}
	
		if (!collision && checkCollision(character, ball)) {
			collision = true;
			ball.setVelocity(vec3(ball.getVelocity().x, -ball.getVelocity().y, 0));
		}

		// Player didnt catch ball
		if (!collision && ball.getPosition().y < character.getPosition().y - character.getDimensions().y) {
			ball.setVelocity(vec3(0.0f, 0.0f, 0.0f));
		}

		// TODO stop player from moving
		// if (checkBoundsCollision(character)) {

		// }

		ball.update(now);
		ball.render(shaderID);
		checkBoundsCollision(ball);

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	// glDeleteVertexArrays(1, background.VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
	{
		keys[key] = true;
	}
	if (action == GLFW_RELEASE)
	{
		keys[key] = false;
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

int loadTexture(string filePath, int &imgWidth, int &imgHeight)
{
	GLuint texID;

	// Gera o identificador da textura na memória
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Configurando o wrapping da textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Configurando o filtering de minificação e magnificação da textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Carregamento da imagem da textura
	int nrChannels;
	unsigned char *data = stbi_load(filePath.c_str(), &imgWidth, &imgHeight, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) // jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else // png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << filePath << std::endl;
	}

	return texID;
}

// Check for collision with screen bounds
void checkBoundsCollision(MovingSprite &ball) {
    float ballLeft = ball.getPosition().x - ball.getDimensions().x / 2.0f;
    float ballRight = ball.getPosition().x + ball.getDimensions().x / 2.0f;
    float ballTop = ball.getPosition().y + ball.getDimensions().y / 2.0f;
    float ballBottom = ball.getPosition().y - ball.getDimensions().y / 2.0f;

    // Check left and right bounds
    if (ballLeft < 0.0f) {
        ball.setVelocity(vec3(-ball.getVelocity().x, ball.getVelocity().y, 0)); // Reverse horizontal velocity
        ball.getPosition().x = ball.getDimensions().x / 2.0f; // Reset position to avoid getting stuck
    } 
    else if (ballRight > WIDTH) {
        ball.setVelocity(vec3(-ball.getVelocity().x, ball.getVelocity().y, 0)); // Reverse horizontal velocity
        ball.getPosition().x = WIDTH - ball.getDimensions().x / 2.0f; // Reset position to avoid getting stuck
    }

    // Check top and bottom bounds
    if (ballTop > HEIGHT) {
        ball.setVelocity(vec3(ball.getVelocity().x, -ball.getVelocity().y, 0)); // Reverse vertical velocity
        ball.getPosition().y = HEIGHT - ball.getDimensions().y / 2.0f; // Reset position
    } 
    else if (ballBottom < 0.0f) {
        ball.setVelocity(vec3(ball.getVelocity().x, -ball.getVelocity().y, 0)); // Reverse vertical velocity
        ball.getPosition().y = ball.getDimensions().y / 2.0f; // Reset position
    }
}

// Check for collision between two rectangles
bool checkCollision(Sprite &rect1, Sprite &rect2) {
    // Get the positions and dimensions of the rectangles
    float rect1Left = rect1.getPosition().x - rect1.getDimensions().x / 2.0f;
    float rect1Right = rect1.getPosition().x + rect1.getDimensions().x / 2.0f;
    float rect1Top = rect1.getPosition().y + rect1.getDimensions().y / 2.0f;
    float rect1Bottom = rect1.getPosition().y - rect1.getDimensions().y / 2.0f;

    float rect2Left = rect2.getPosition().x - rect2.getDimensions().x / 2.0f;
    float rect2Right = rect2.getPosition().x + rect2.getDimensions().x / 2.0f;
    float rect2Top = rect2.getPosition().y + rect2.getDimensions().y / 2.0f;
    float rect2Bottom = rect2.getPosition().y - rect2.getDimensions().y / 2.0f;

    // Check for overlap
    return (rect1Left < rect2Right && rect1Right > rect2Left && 
            rect1Top > rect2Bottom && rect1Bottom < rect2Top);
}