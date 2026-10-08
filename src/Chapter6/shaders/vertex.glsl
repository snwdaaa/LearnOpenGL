#version 330 core
layout(location = 0) in vec3 aPos; // 0번 위치 attribute
out vec4 vertexColor; // fragment shader로 전달할 vertex color

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	vertexColor = vec4(0.5, 0.0, 0.0, 1.0);
}