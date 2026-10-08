#version 330 core
out vec4 FragColor;
in vec4 vertexColor; // vertex shader와 자료형, 이름 똑같이 맞춰서 연결시킴

void main()
{
	FragColor = vertexColor;
}