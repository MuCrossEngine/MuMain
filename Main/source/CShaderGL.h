#pragma once

#ifdef SHADER_VERSION_TEST
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // Necesario para glm::value_ptr


class CShaderGL
{
public:
	CShaderGL();
	virtual~CShaderGL();

	void Init();
	void RenderShader();
	bool CheckedShader();
	GLuint GetShaderId();

	bool readshader(const char* filename, std::string& shader_text);
	GLuint run_shader(const char* shader_text, GLenum type);

	void run_projection();
	void SetPerspective(float Fov, float Aspect, float ZNear, float ZFar);

	// Funciones para establecer uniforms
	void setBool(const char* name, bool value) const;
	void setInt(const char* name, int value) const;
	void setFloat(const char* name, float value) const;
	void setVec2(const char* name, float x, float y) const;
	void setVec3(const char* name, float x, float y, float z) const;
	void setVec4(const char* name, float x, float y, float z, float w) const;
	void setMat4(const char* name, glm::mat4& matrix) const;

	static CShaderGL* Instance();
private:
	GLuint shader_id;
};

#define gShaderGL				(CShaderGL::Instance())
#endif // SHADER_VERSION_TEST
