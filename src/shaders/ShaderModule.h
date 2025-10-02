#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>


class ShaderModule
{
public:
	ShaderModule(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	~ShaderModule();
	void bind() const;
	void release() const;

	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setVec2(const std::string& name, const glm::vec2& value);
	void setVec3(const std::string& name, const glm::vec3& value);
	void setVec4(const std::string& name, const glm::vec4& value);
    void setMat4(const std::string& name, const glm::mat4& mat);

private:
	std::string dataFromFile(const std::string& filePath);
	GLuint compileShader(GLuint typeShader, const std::string& ShaderPath);
	unsigned int m_shaderId;
	GLuint shader_programme;
	GLuint fs, vs;
};