#pragma once
#define GLEW_STATIC
#include<GL\glew.h>
#include<GLFW\glfw3.h>

#include"Shader.h"
#include"Model.h"

#include<glm\glm.hpp>
#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\matrix_transform.hpp>


class Planet{
public:
	Shader PlanetShader, OrbitShader;
	Planet(GLfloat Scale, GLfloat OrbitRadius,GLfloat RevolutionSpeed);
	~Planet();
	void ReadShaderAndModel(char *PathVt, char *PathFg,char *PathModel) {
		this->PlanetShader.fileShader(PathVt, PathFg);
		this->PlanetModel=new Model(PathModel);
		this->InitOrbit();
	};
	void GetMatix(glm::mat4 view,glm::mat4 projection) {
		this->view = view;
		this->projection = projection;
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(this->Scale));
		model = glm::translate(model, glm::vec3(this->OrbitRadius*glm::sin(glfwGetTime()*this->RevolutionSpeed), 0.0f, this->OrbitRadius*glm::cos(glfwGetTime()*this->RevolutionSpeed)));
		this->PlanetShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.position"), 0.0f,0.0f,30.0f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(this->PlanetShader.Program, "material.shininess"), 32.0f);
		
	}
	void GetMatix(glm::mat4 view, glm::mat4 projection,glm::mat4 model) {
		this->view = view;
		this->projection = projection;
		this->PlanetShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.position"), 0.0f, 0.0f, 30.0f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(this->PlanetShader.Program, "material.shininess"), 32.0f);

	}

	void RenderPlanet() {
		this->PlanetModel->ModelRender(this->PlanetShader);
	};
	void RenderOrbit() {
		this->OrbitShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(this->OrbitShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->OrbitShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
		glBindVertexArray(this->VAO);
		this->OrbitShader.Use();
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}
	
private:
	GLfloat Scale;
	GLfloat OrbitRadius;
	GLfloat RevolutionSpeed;
	Model* PlanetModel;
	GLuint VAO, VBO;
	glm::mat4 view, projection;

	void InitOrbit() {
		GLfloat Origin[] = { 0.0f,0.0f,0.0f,0.0f,1.0f,0.0f};
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Origin), Origin, GL_STATIC_DRAW);
		glBindVertexArray(this->VAO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
		this->OrbitShader.fileShader("OrbitVShader.vt", "OrbitFShader.fg" , "OrbitGShader.gt");
	}
};

Planet::Planet(GLfloat Scale,GLfloat OrbitRadius,GLfloat RevolutionSpeed){
	this->Scale = Scale;
	this->OrbitRadius = OrbitRadius;
	this->RevolutionSpeed = RevolutionSpeed;
}

Planet::~Planet(){
	glDeleteVertexArrays(1,&this->VAO);
	glDeleteBuffers(1,&this->VBO);
}