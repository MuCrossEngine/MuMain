#include "stdafx.h"
#include "CShaderGL.h"

#ifdef SHADER_VERSION_TEST
#include "Utilities/Log/muConsoleDebug.h"

CShaderGL::CShaderGL()
{
	shader_id = 0;
}

CShaderGL::~CShaderGL()
{
	glDeleteProgram(shader_id);
}

void CShaderGL::Init()
{
	std::string vertex_shader;

	if (!readshader("Shaders\\shader.vs", vertex_shader))
	{
		return;
	}

	std::string frgmen_shader;

	if (!readshader("Shaders\\shader.fs", frgmen_shader))
	{
		return;
	}

	GLuint shader_vertex = run_shader(vertex_shader.data(), GL_VERTEX_SHADER);

	GLuint shader_frgmen = run_shader(frgmen_shader.data(), GL_FRAGMENT_SHADER);

	shader_id = glCreateProgram();
	glAttachShader(shader_id, shader_vertex);
	glAttachShader(shader_id, shader_frgmen);
	glLinkProgram(shader_id);

	int success;
	glGetProgramiv(shader_id, GL_LINK_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shader_id, 512, NULL, infoLog);
		g_ConsoleDebug->Write(5, "Error al enlazar el Shader Program:");
		g_ConsoleDebug->Write(5, infoLog);
	}

	// Eliminar los shaders compilados
	glDeleteShader(shader_vertex);
	glDeleteShader(shader_frgmen);
}

void CShaderGL::RenderShader()
{
	if (this->CheckedShader())
	{
		glUseProgram(shader_id);
	}
}

bool CShaderGL::CheckedShader()
{
	return (shader_id != 0);
}

GLuint CShaderGL::GetShaderId()
{
	return shader_id;
}

bool CShaderGL::readshader(const char* filename, std::string& shader_text)
{
	FILE* compressedFile = fopen(filename, "rb");

	if (compressedFile)
	{
		fseek(compressedFile, 0, SEEK_END);
		long fileSize = ftell(compressedFile);
		fseek(compressedFile, 0, SEEK_SET);

		shader_text.resize(fileSize, 0);
		fread(shader_text.data(), 1, fileSize, compressedFile);
		fclose(compressedFile);

		return true;
	}

	return false;
}

GLuint CShaderGL::run_shader(const char* shader_text, GLenum type)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &shader_text, NULL);
	glCompileShader(shader);

	// Verificar errores de compilación
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		g_ConsoleDebug->Write(5, "Error al compilar shader:");
		g_ConsoleDebug->Write(5, infoLog);
	}

	return shader;
}

void CShaderGL::run_projection()
{
	if (shader_id != 0)
	{
		glUseProgram(shader_id);

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);

		view = glm::rotate(view, glm::radians(CameraAngle[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		if (CameraTopViewEnable == false)
			view = glm::rotate(view, glm::radians(CameraAngle[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::rotate(view, glm::radians(CameraAngle[2]), glm::vec3(0.0f, 0.0f, 1.0f));

		view = glm::translate(view, glm::vec3(-CameraPosition[0], -CameraPosition[1], -CameraPosition[2]));


		this->setMat4("view", view);
		this->setMat4("model", model);

		glUseProgram(0);

		glUniform1i(glGetUniformLocation(shader_id, "texture1"), 0);

	}
}

void CShaderGL::SetPerspective(float Fov, float Aspect, float ZNear, float ZFar)
{
	if (shader_id != 0)
	{
		glUseProgram(shader_id);
		glm::mat4 projection = glm::perspective(glm::radians(Fov), Aspect, ZNear, ZFar);
		this->setMat4("projection", projection);
		glUseProgram(0);
	}
}

// Funciones para establecer uniforms
void CShaderGL::setBool(const char* name, bool value) const
{
	glUniform1i(glGetUniformLocation(shader_id, name), (int)value);
}

void CShaderGL::setInt(const char* name, int value) const
{
	glUniform1i(glGetUniformLocation(shader_id, name), value);
}

void CShaderGL::setFloat(const char* name, float value) const
{
	glUniform1f(glGetUniformLocation(shader_id, name), value);
}

void CShaderGL::setVec2(const char* name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(shader_id, name), x, y);
}

void CShaderGL::setVec3(const char* name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(shader_id, name), x, y, z);
}

void CShaderGL::setVec4(const char* name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(shader_id, name), x, y, z, w);
}

void CShaderGL::setMat4(const char* name, glm::mat4& matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(shader_id, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

CShaderGL* CShaderGL::Instance()
{
	static CShaderGL sInstance;
	return &sInstance;
}
#endif // SHADER_VERSION_TEST



