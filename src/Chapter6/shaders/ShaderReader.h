#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

std::string readShaderFile(const char* path) {
    std::ifstream file(path);

    if (!file) {
	std::cout << "ERROR::SHADER::FILE_NOT_READ: " << path << std::endl;
	return "";
    }

    std::stringstream ss;
    ss << file.rdbuf(); // 파일 전체 한 번에 읽기
    return ss.str();
}