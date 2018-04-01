//用于星球模型初始化，shader读入，星球的加载
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
	
	Shader PlanetShader;
	glm::mat4 view, projection;
	Planet(GLfloat Scale, GLfloat OrbitRadius,GLfloat RevolutionSpeed);
	~Planet();
	void ReadShaderAndModel(char *PathVt, char *PathFg,char *PathModel) {
		this->PlanetShader.fileShader(PathVt, PathFg);
		this->PlanetModel=new Model(PathModel);
		//this->InitOrbit();
	};
	
	//opengl三种矩阵的载入，根据构造函数参数进行model矩阵的变换
	void GetMatix(glm::mat4 view,glm::mat4 projection, glm::vec3 ViewPos) {
		this->view = view;
		this->projection = projection;
		glm::mat4 model;
		glm::vec3 PlanetPosition(this->OrbitRadius*glm::sin(glfwGetTime()*this->RevolutionSpeed), 0.0f, this->OrbitRadius*glm::cos(glfwGetTime()*this->RevolutionSpeed));
		model = glm::scale(model, glm::vec3(this->Scale));
		model = glm::translate(model, PlanetPosition);
		this->PlanetShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.LightDirection"), PlanetPosition.x,PlanetPosition.y,PlanetPosition.z);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(this->PlanetShader.Program, "material.shininess"), 32.0f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "viewPos"), ViewPos.x, ViewPos.y, ViewPos.z);
		
	}
	//通过程序自己传入model矩阵
	void GetMatix(glm::mat4 view, glm::mat4 projection,glm::vec3 ViewPos,glm::mat4 model) {
		this->view = view;
		this->projection = projection;
		this->PlanetShader.Use();
		glm::vec4 PlanetPosition(model[3]);
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.LightDirection"), PlanetPosition.x, PlanetPosition.y, PlanetPosition.z);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(this->PlanetShader.Program, "material.shininess"), 32.0f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "viewPos"), ViewPos.x,ViewPos.y,ViewPos.z);

	}

	void RenderPlanet() {
		this->PlanetModel->ModelRender(this->PlanetShader);
	};
	
	//绘制轨道，出现小问题，后期开发
	void RenderOrbit() {
		this->OrbitShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(this->OrbitShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->OrbitShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
		glBindVertexArray(this->VAO);
		this->OrbitShader.Use();
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}
protected:
	GLfloat Scale;
	GLfloat OrbitRadius;
	GLfloat RevolutionSpeed;
	Model* PlanetModel;
private:
	Shader OrbitShader;
	GLuint VAO, VBO;
	//轨道数据初始化，绑定进vao
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

class Sun:private Planet{
public:

	Sun(GLfloat Scale);
	~Sun();
	void GetMatix(glm::mat4 view, glm::mat4 projection, glm::vec3 ViewPos) {
		this->view = view;
		this->projection = projection;
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(Scale));
		this->PlanetShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "ambient"), 1.0f, 1.0f, 1.0f);

	}
	//通过程序自己传入model矩阵
	void GetMatix(glm::mat4 view, glm::mat4 projection, glm::vec3 ViewPos, glm::mat4 model) {
		this->view = view;
		this->projection = projection;
		this->PlanetShader.Use();
		glm::vec4 PlanetPosition(model[3]);

		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "ambient"), 0.2f, 0.2f, 0.2f);

	}
	void ReadShaderAndModel(char *PathVt, char *PathFg, char *PathModel) {
		Planet::ReadShaderAndModel(PathVt, PathFg, PathModel);
	};
	void RenderPlanet() {
		Planet::RenderPlanet();
	};
private:

};

Sun::Sun(GLfloat Scale):Planet(Scale, 0.0f,0.0f){
}

Sun::~Sun()
{
}