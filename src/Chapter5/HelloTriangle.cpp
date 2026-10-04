#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

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

void initializeBuffer() {
    // 테스트 정점 데이터
    float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
    };

    unsigned int VBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void processShader() {
    // Vertex Shader 생성
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
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
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // 성공 여부 확인
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
	glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
	std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 쉐이더 프로그램 실행
    unsigned int compiledShaders[] = {vertexShader, fragmentShader};
    createShaderProgram(compiledShaders);
}

void createShaderProgram(unsigned int *compiledShaders) {
    // 프로그램 생성
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // 이전에 컴파일한 셰이더들 프로그램 객체에 붙이기
    int shadersCnt = sizeof(compiledShaders) / sizeof(unsigned int);
    for (int i = 0; i < shadersCnt; ++i) {
	glAttachShader(shaderProgram, compiledShaders[i]);
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

    // 셰이더 프로그램 사용
    glUseProgram(shaderProgram);

    // 링크한 셰이더들 삭제
    for (int i = 0; i < shadersCnt; ++i) {
	glDeleteShader(compiledShaders[i]);
    }
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

    // 버퍼 설정
    initializeBuffer();

    // 쉐이더 생성 및 쉐이더 프로그램 실행
    processShader();

    // render loop
    while (!glfwWindowShouldClose(window)) {
	// input
	processInput(window); // 입력 감지

	// render
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // 컬러 버퍼 비우기

	glfwSwapBuffers(window);
	glfwPollEvents();
    }

    glfwTerminate(); // GLFW 리소스 정리
    return 0;
}