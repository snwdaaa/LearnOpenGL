#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shaders/ShaderReader.h"

#include <array>

// 창 크기 변경할 때마다 호출되는 콜백 함수
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 입력 감지 콜백
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
	glfwSetWindowShouldClose(window, true);
    }
}

// VBO 설정
void initializeVBO(unsigned int& VBO, const std::array<float, 9>& vertices) {
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

// VAO 설정
void initializeVertexArray(unsigned int& VAO) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
}

unsigned int createShaderProgram(const std::array<unsigned int, 2>& compiledShaders) {
    // 프로그램 생성
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // 이전에 컴파일한 셰이더들 프로그램 객체에 붙이기
    for (unsigned int shader : compiledShaders) {
	glAttachShader(shaderProgram, shader);
    }
    glLinkProgram(shaderProgram); // 셰이더 프로그램 링크

    // 성공 여부 확인
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
	glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }

    return shaderProgram;
}

void processShader(
    const char* vertexShaderSrc,
    const char* fragmentShaderSrc,
    std::array<unsigned int, 2>& compiledShaders)
{
    // Vertex Shader 생성
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
    glCompileShader(vertexShader);

    // 성공 여부 확인
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
	glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader 생성
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);
    glCompileShader(fragmentShader);

    // 성공 여부 확인
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    compiledShaders = { vertexShader, fragmentShader };
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // GLFW 창 객체 만들기
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
	std::cout << "Failed to create GLFW window" << std::endl;
	glfwTerminate();
	return -1;
    }

    glfwMakeContextCurrent(window);

    // GLAD -> OpenGL 함수 포인터 관리
    // GLAD 초기화
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
	std::cout << "Failed to initialize GLAD" << std::endl;
	return -1;
    }

    // 뷰포트 설정
    glViewport(0, 0, 800, 600); // 왼쪽 아래 모서리 위치, 너비, 높이

    // 윈도우-뷰포트 크기 동기화 콜백
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // VAO 생성 & 바인딩 -> VBO -> glVertexAttribPointer 순서
    unsigned int VAO, VBO;

    // 가운데 삼각형
    std::array<float, 9> vertices = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
    };

    initializeVertexArray(VAO);
    initializeVBO(VBO, vertices);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 쉐이더 생성 및 쉐이더 프로그램 링크
    std::array<unsigned int, 2> compiledShaders;
    std::string vertexCode = readShaderFile("src/Chapter6/shaders/vertex.glsl");
    std::string fragmentCode = readShaderFile("src/Chapter6/shaders/fragment.glsl");
    processShader(vertexCode.c_str(), fragmentCode.c_str(), compiledShaders);
    unsigned int shaderProgram = createShaderProgram(compiledShaders);

    // 링크가 끝난 셰이더 객체는 더 이상 필요 없음
    for (unsigned int shader : compiledShaders) {
	glDeleteShader(shader);
    }

    // render loop
    while (!glfwWindowShouldClose(window)) {
	// input
	processInput(window); // 입력 감지

	// render
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // 컬러 버퍼 비우기

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	glfwSwapBuffers(window);
	glfwPollEvents();
    }

    // 리소스 정리
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate(); // GLFW 리소스 정리
    return 0;
}
