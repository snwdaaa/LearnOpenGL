#pragma once

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID; // 셰이더 프로그램 ID

    // 생성자: 셰이더 Read / Build
    Shader(const char* vertexPath, const char* fragmentPath);

    // 셰이더 활성화/비활성화
    void use();

    // 유틸리티 함수
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
};

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // 1. 파일 경로에서 vertex/fragment 셰이더 소스 코드 가져오기
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ifstream exception
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
	// 파일 열기
	vShaderFile.open(vertexPath);
	fShaderFile.open(fragmentPath);
	std::stringstream vShaderStream, fShaderStream;

	// 파일 버퍼 컨텐츠 stream으로 읽어들이기
	vShaderStream << vShaderFile.rdbuf();
	fShaderStream << fShaderFile.rdbuf();

	// 파일 핸들러 닫기
	vShaderFile.close();
	fShaderFile.close();

	// stream -> string 변환
	vertexCode = vShaderStream.str();
	fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e) {
	std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. 셰이더 컴파일
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // vertex shader 오류 확인
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
	glGetShaderInfoLog(vertex, 512, NULL, infoLog);
	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);

    // fragment shader 오류 확인
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
	glGetShaderInfoLog(fragment, 512, NULL, infoLog);
	std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 셰이더 프로그램
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // 링킹 오류 확인
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
	glGetProgramInfoLog(ID, 512, NULL, infoLog);
	std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // 셰이더 삭제; 프로그램에 링크되면 더 이상 필요 없음
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    glUseProgram(ID);
}

// Uniform 값 설정
void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

#endif