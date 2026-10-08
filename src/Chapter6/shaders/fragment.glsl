#version 330 core
out vec4 FragColor;
// in vec4 vertexColor; // vertex shader와 자료형, 이름 똑같이 맞춰서 연결시킴

// uniform 변수는 전역으로 사용할 수 있어서 다른 셰이더의 입출력으로 값을 받을 필요 없이
// CPU 코드에서 glGetUniformLocation으로 위치 가져온 뒤 glUniform4f 등으로 값 설정 가능
// uniform vec4 ourColor;

in vec3 ourColor;

// Practice 3
in vec3 ourPosition;

void main()
{
	//FragColor = vertexColor;
	//FragColor = vec4(ourColor, 1.0);

	// Practice 3
	FragColor = vec4(ourPosition, 1.0);
}