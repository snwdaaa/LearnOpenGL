#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_s.h>
#include <array>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

// VBO, EBO 설정
void initializeVBO(unsigned int& VBO, const std::array<float, 36>& vertices) {
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

void initializeEBO(unsigned int& EBO) {
    unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3 //  second triangle
    };

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

// VAO 설정
void initializeVertexArray(unsigned int& VAO) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
}

// 텍스처 설정
void loadTexture(
    unsigned int& texture,
    unsigned int unit,
    const int& width, 
    const int& height, 
    const int& nrChannels, 
    unsigned char* data,
    bool isPNG) 
{
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit); // 텍스처 유닛 설정
    glBindTexture(GL_TEXTURE_2D, texture); // 텍스처 바인딩

    // 텍스처 wrapping/filtering 옵션 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data) {
	if (isPNG) {
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // 텍스처 생성
	}
	else {
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // 텍스처 생성
	}
	glGenerateMipmap(GL_TEXTURE_2D); // 밉맵 생성
    }
    else {
	std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
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
    unsigned int VAO, VBO, EBO;

    // 가운데 삼각형
    std::array<float, 36> vertices = {
	// positions	    // colors		// texture coords
	0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,	1.0f, 1.0f, // top right
	0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,	1.0f, 0.0f, // bottom right
	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // bottom left
	-0.5f, 0.5f, 0.0f,  1.0f, 1.0f, 0.0f,	0.0f, 1.0f  // top left

    };

    initializeVertexArray(VAO);
    initializeVBO(VBO, vertices);
    initializeEBO(EBO);

    // 위치 attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // stride: 6 * sizeof(float)
    glEnableVertexAttribArray(0);

    // 색상 attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // 처음 + 3 * sizeof(float)에서 시작
    glEnableVertexAttribArray(1);

    // 텍스처 attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // 처음 + 6 * sizeof(float)에서 시작
    glEnableVertexAttribArray(2);

    // 셰이더
    Shader ourShader("src/Chapter7/shaders/vertex.glsl", "src/Chapter7/shaders/fragment.glsl");

    // 텍스처 불러오기
    int width, height, nrChannels;
    unsigned char* data1 = stbi_load("src/Chapter7/container.jpg", &width, &height, &nrChannels, 0);
    unsigned int texture1, texture2;
    loadTexture(texture1, 0, width, height, nrChannels, data1, false);

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data2 = stbi_load("src/Chapter7/awesomeface.png", &width, &height, &nrChannels, 0);
    loadTexture(texture2, 1, width, height, nrChannels, data2, true);

    ourShader.use();

    // texture
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);

    // render loop
    while (!glfwWindowShouldClose(window)) {
	// input
	processInput(window); // 입력 감지

	// render
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT); // 컬러 버퍼 비우기

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
