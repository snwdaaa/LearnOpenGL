#version 330 core
layout (location = 0) in vec3 aPos; // 0번 위치 attribute
layout (location = 1) in vec3 aColor;

out vec3 ourColor; // fragment shader로 전달할 vertex color

// Practice 2
uniform float xOffset;

// Practice 3
out vec3 ourPosition;

void main()
{
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	ourColor = aColor;

	// Practice 1
	//gl_Position = vec4(-aPos.x, -aPos.y, -aPos.z, 1.0);

	// Practice 2
	//gl_Position = vec4(aPos.x + xOffset, aPos.y, aPos.z, 1.0);

	// Practice 3
	ourPosition = aPos;	
}