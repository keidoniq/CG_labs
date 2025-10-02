#include "ShaderModule.h"

std::string ShaderModule::dataFromFile(const std::string& filePath)
{
	std::stringstream ss;
	std::ifstream input(filePath);
	if (input) {
		std::string line;
		while (std::getline(input, line))
			ss << line << '\n';
	}
	input.close();
	return ss.str();
}

GLuint ShaderModule::compileShader(GLuint typeShader, const std::string& shaderPath)
{
	GLuint shader;
	shader = (typeShader == GL_VERTEX_SHADER ? glCreateShader(GL_VERTEX_SHADER) : glCreateShader(GL_FRAGMENT_SHADER));
	std::string tmp = dataFromFile(shaderPath);
	const char* pth = tmp.c_str();
	glShaderSource(shader, 1, &pth, NULL);
	glCompileShader(shader);

	return shader;
}

ShaderModule::ShaderModule(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	shader_programme = glCreateProgram();
    
	vs = compileShader(GL_VERTEX_SHADER, vertexShaderPath);
	fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderPath);

	glAttachShader(shader_programme, vs);
	glAttachShader(shader_programme, fs);
	glLinkProgram(shader_programme);

	glValidateProgram(shader_programme);
	glDetachShader(shader_programme, vs);
	glDetachShader(shader_programme, fs);
}

 ShaderModule::~ShaderModule()
{
	glDeleteProgram(shader_programme);
}


void ShaderModule::bind() const
{
	glUseProgram(shader_programme);
}

 void ShaderModule::release() const
{
	glUseProgram(0);
}

 void ShaderModule::setInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(m_shaderId, name.c_str()), value);
}

 void ShaderModule::setFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(m_shaderId, name.c_str()), value);
}

 void ShaderModule::setVec2(const std::string& name, const glm::vec2& value)
{
	glUniform2f(glGetUniformLocation(m_shaderId, name.c_str()), value.x, value.y);
}

 void ShaderModule::setVec3(const std::string& name, const glm::vec3& value)
{
	glUniform3f(glGetUniformLocation(m_shaderId, name.c_str()), value.x, value.y, value.z);
}

 void ShaderModule::setVec4(const std::string& name, const glm::vec4& value)
{
	glUniform4f(glGetUniformLocation(m_shaderId, name.c_str()), value.x, value.y, value.z, value.w);
}

void ShaderModule::setMat4(const std::string& name, const glm::mat4& mat) {
    glUniformMatrix4fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}