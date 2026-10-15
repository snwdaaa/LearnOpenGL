#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_s.h>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f; // 현재 프레임과 이전 프레임 사이 시간
float lastFrame = 0.0f; // 이전 프레임 시간

// 창 크기 변경할 때마다 호출되는 콜백 함수
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 입력 감지 콜백
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
	glfwSetWindowShouldClose(window, true);
    }

    // Camera
    const float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
	cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
	cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
	cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
	cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
    }
}

// VBO, EBO 설정
void initializeVBO(unsigned int& VBO, const std::array<float, 180>& vertices) {
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
    glEnable(GL_DEPTH_TEST); // Depth Test 활성화

    // 윈도우-뷰포트 크기 동기화 콜백
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // VAO 생성 & 바인딩 -> VBO -> glVertexAttribPointer 순서
    unsigned int VAO, VBO, EBO;

    // 큐브 (36개 정점)
    std::array<float, 180> vertices = {
	// positions	      // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    initializeVertexArray(VAO);
    initializeVBO(VBO, vertices);
    initializeEBO(EBO);

    // 위치 attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // stride: 5 * sizeof(float)
    glEnableVertexAttribArray(0);

    // 텍스처 attribute (색상 attribute 없음)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // 처음 + 3 * sizeof(float)에서 시작
    glEnableVertexAttribArray(2);

    // 셰이더
    Shader ourShader(LOCAL_PATH("shaders/vertex.glsl"), LOCAL_PATH("shaders/fragment.glsl"));

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

    glm::vec3 cubePositions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -2.5f),
	glm::vec3(1.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    // render loop
    while (!glfwWindowShouldClose(window)) {
	// input
	processInput(window); // 입력 감지

	// render
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 컬러 버퍼 비우기

	// delta time 계산
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// 모델 행렬
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	// 뷰 행렬
	glm::mat4 view;
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	// 투영 행렬
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(80.0f), 2560.0f / 1440.0f, 0.1f, 100.0f);
	//int modelLoc = glGetUniformLocation(ourShader.ID, "model");
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	int viewLoc = glGetUniformLocation(ourShader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(VAO);
	for (unsigned int i = 0; i < 10; i++) {
	    glm::mat4 model = glm::mat4(1.0f);
	    model = glm::translate(model, cubePositions[i]);
	    float angle = 20.0f * i;

	    if (i % 3 == 0) {
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));
	    }
	    else {
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
	    }
	    
	    ourShader.setMat4("model", model);

	    glDrawArrays(GL_TRIANGLES, 0, 36);
	}
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
