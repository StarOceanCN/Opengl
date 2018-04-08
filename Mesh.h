#pragma once
//C++
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
using namespace std;
//GLFW
#include<GLFW\glfw3.h>
//GLM
#include<glm\glm.hpp>
//SHADER
#include<GL\Shader.h>
#include<assimp\Importer.hpp>
#include<assimp\scene.h>
#include<assimp\postprocess.h>
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};
struct Texture {
	GLuint Texid;
	string Textype;
	aiString path;
};
class Mesh {
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) {
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		this->InitBuffer();
	};
	void render(Shader s,GLfloat shininess) {
		GLuint texturedn = 1;
		GLuint texturesn = 1;
		for (GLuint i = 0; i < this->textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			stringstream ss;
			if (this->textures[i].Textype == "texture_diffuse")ss << texturedn++;
			else if (this->textures[i].Textype == "texture_specular")ss << texturesn++;
			glUniform1i(glGetUniformLocation(s.Program, (this->textures[i].Textype + ss.str()).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, this->textures[i].Texid);
		}
		glUniform1f(glGetUniformLocation(s.Program, "material.shininess"), shininess);

		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		for (GLuint i = 0; i < this->textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	};
	~Mesh() {};

private:
	GLuint VAO, VBO, EBO;
	void InitBuffer() {
		//get
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);
		//bind
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		//initdata
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);
		//shaderpointer
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);

	};
};