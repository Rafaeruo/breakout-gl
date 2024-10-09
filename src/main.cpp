// main.cpp
#include "../deps/include/glad/glad.h"
#include <GLFW/glfw3.h> // Necessário ter instalado no sistema

#include <iostream>

// Inclua as bibliotecas GLM para matemática
#include <glm/glm.hpp> // Necessário ter instalado no sistema
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Defina STB_IMAGE_IMPLEMENTATION antes de incluir stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "../deps/stb_image.h"

// Dimensões da janela
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// Código do Vertex Shader
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;

    uniform mat4 model;
    uniform mat4 projection;

    out vec2 TexCoord;

    void main()
    {
        gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)glsl";

// Código do Fragment Shader
const char* fragmentShaderSource = R"glsl(
    #version 330 core
    out vec4 FragColor;

    in vec2 TexCoord;

    uniform sampler2D texture1;

    void main()
    {
        FragColor = texture(texture1, TexCoord);
    }
)glsl";

// Funções de callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// Função para carregar textura
unsigned int loadTexture(char const * path);

int main()
{
    // Inicialização do GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criação da janela GLFW
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pinball", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Falha ao criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Registro de callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Inicialização do GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Falha ao inicializar GLAD" << std::endl;
        return -1;
    }

    // Compilação dos shaders
    // Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Verificação de erros
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Erro na compilação do Vertex Shader:\n" << infoLog << std::endl;
    }

    // Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Verificação de erros
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Erro na compilação do Fragment Shader:\n" << infoLog << std::endl;
    }

    // Linkagem dos shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // Verificação de erros
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Erro na linkagem do Shader Program:\n" << infoLog << std::endl;
    }
    // Deletar shaders já que não são mais necessários
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Definição dos dados dos vértices
    float vertices[] = {
        // Posição     // Coordenadas de textura
         50.0f,  50.0f,  1.0f, 1.0f, // Top Right
         50.0f, -50.0f,  1.0f, 0.0f, // Bottom Right
        -50.0f, -50.0f,  0.0f, 0.0f, // Bottom Left
        -50.0f,  50.0f,  0.0f, 1.0f  // Top Left
    };

    unsigned int indices[] = {  // Opcional, se usar EBO
        0, 1, 3,
        1, 2, 3
    };

    // Configuração do VAO, VBO e EBO
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // glGenBuffers(1, &EBO); // Se usar EBO

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Se usar EBO
    /*
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    */

    // Atributo de posição
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Atributo de coordenadas de textura
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Carregamento da textura
    unsigned int texture = loadTexture("path_to_texture.png");

    // Configuração da projeção ortográfica
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH),
                                      static_cast<float>(SCREEN_HEIGHT), 0.0f, -1.0f, 1.0f);

    // Variáveis para controle de tempo
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // Posição inicial do jogador
    float playerPosX = SCREEN_WIDTH / 2.0f;
    float playerPosY = SCREEN_HEIGHT - 100.0f;

    // Loop principal
    while (!glfwWindowShouldClose(window))
    {
        // Calcular deltaTime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Entrada
        processInput(window);

        // Renderização
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Ativação do shader
        glUseProgram(shaderProgram);

        // Definição das matrizes
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(playerPosX, playerPosY, 0.0f));

        // Enviar matrizes para o shader
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Vincular textura
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        // Desenhar objeto
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        // Se usar EBO:
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Troca de buffers e eventos
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Atualizar posição do jogador (exemplo simples)
        float velocidade = 200.0f * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            playerPosX -= velocidade;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            playerPosX += velocidade;

        // Limitar posição do jogador dentro da janela
        if (playerPosX < 50.0f)
            playerPosX = 50.0f;
        if (playerPosX > SCREEN_WIDTH - 50.0f)
            playerPosX = SCREEN_WIDTH - 50.0f;
    }

    // Desalocação de recursos
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

// Função de callback para redimensionamento
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Processamento de entrada (adapte conforme necessário)
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Função para carregar textura
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int largura, altura, nrComponents;
    unsigned char *data = stbi_load(path, &largura, &altura, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, largura, altura, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Falha ao carregar textura: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
