#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_s.h>
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
void initializeVBO(unsigned int& VBO, const std::array<float, 18>& vertices) {
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

// VAO 설정
void initializeVertexArray(unsigned int& VAO) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
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
    std::array<float, 18> vertices = {
	// positions		// colors
	-0.5f, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f,   // bottom right
	 0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,   // bottom left
	 0.0f,  0.5f, 0.0f,	0.0f, 0.0f, 1.0f    // top
    };

    initializeVertexArray(VAO);
    initializeVBO(VBO, vertices);

    // 위치 attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // stride: 6 * sizeof(float)
    glEnableVertexAttribArray(0);

    // 색상 attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // 처음 + 3 * sizeof(float)에서 시작
    glEnableVertexAttribArray(1);

    // 셰이더
    Shader ourShader("src/Chapter6/shaders/vertex.glsl", "src/Chapter6/shaders/fragment.glsl");

    // render loop
    while (!glfwWindowShouldClose(window)) {
	// input
	processInput(window); // 입력 감지

	// render
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // 컬러 버퍼 비우기

	ourShader.use();
	// ourShader.setFloat("xOffset", 0.5); // Practice 2

	//// 시간 지남에 따라 색상 점차 변하게 만들기
	//float timeValue = glfwGetTime();
	//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
	//// uniform을 사용하려면 셰이더 안에서 uniform 속성의 인덱스/위치를 찾아야 함
	//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
	//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	glfwSwapBuffers(window);
	glfwPollEvents();
    }

    // 리소스 정리
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate(); // GLFW 리소스 정리
    return 0;
}
