//��������ģ�ͳ�ʼ����shader���룬����ļ���(�������ǡ�����)
#pragma once
#define GLEW_STATIC
#include<GL\glew.h>
#include<GLFW\glfw3.h>
#include"Shader.h"
#include"Model.h"

#include<glm\glm.hpp>
#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\matrix_transform.hpp>

struct DataBlock{
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 PlanetPosition;
	GLfloat Radius;
	GLfloat Speed;
};
class Planet{
public:
	Planet(GLfloat Scale, GLfloat OrbitRadius,GLfloat RevolutionSpeed);
	~Planet();
	void ReadShaderAndModel(char *PathVt, char *PathFg,char *PathModel) {
		this->PlanetShader.fileShader(PathVt, PathFg);
		this->PlanetModel=new Model(PathModel);
		//this->InitOrbit();
	};
	//opengl���־�������룬���ݹ��캯����������model����ı任
	void SetMatrix(glm::mat4 view,glm::mat4 projection, glm::vec3 ViewPos) {
		this->view = view;
		this->projection = projection;
		glm::mat4 model;
		PlanetPosition=glm::vec3(this->OrbitRadius*glm::cos(glfwGetTime()*this->RevolutionSpeed), 0.0f, this->OrbitRadius*glm::sin(glfwGetTime()*this->RevolutionSpeed));
		/*
			ע�⣺��ʱtranslate,rotate��scale�任˳�����෴
		*/
		model = glm::translate(model, PlanetPosition);
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(this->Scale));
		this->PlanetShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.LightDirection"), PlanetPosition.x,PlanetPosition.y,PlanetPosition.z);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		//glUniform1f(glGetUniformLocation(this->PlanetShader.Program, "material.shininess"), 1.0f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "viewPos"), ViewPos.x, ViewPos.y, ViewPos.z);
	}
	//ͨ�������Լ�����model����
	void SetMatrix(glm::mat4 view, glm::mat4 projection,glm::vec3 ViewPos,glm::mat4 model) {
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
	DataBlock GetPlanetData() {
		DataBlock PlanetData;
		PlanetData.projection = this->projection;
		PlanetData.view = this->view;
		PlanetData.Speed = this->RevolutionSpeed;
		PlanetData.Radius = this->OrbitRadius;
		PlanetData.PlanetPosition = this->PlanetPosition;
		return PlanetData;
	}
	void RenderPlanet() {
		this->PlanetModel->ModelRender(this->PlanetShader,2.0f);
	};
	//���ƹ��������С���⣬���ڿ���
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
	Shader PlanetShader;
	glm::mat4 view, projection;
	glm::vec3 PlanetPosition;
private:
	Shader OrbitShader;
	GLuint VAO, VBO;
	Model* PlanetModel;
	//������ݳ�ʼ�����󶨽�vao
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
//̫�������ǣ���
class Sun:private Planet{
public:
	Sun(GLfloat Scale);
	~Sun();
	void SetMatrix(glm::mat4 view, glm::mat4 projection, glm::vec3 ViewPos) {
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
	//ͨ�������Լ�����model����
	void SetMatrix(glm::mat4 view, glm::mat4 projection, glm::vec3 ViewPos, glm::mat4 model) {
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
	void RenderSun() {
		Planet::RenderPlanet();
	};
private:

};
Sun::Sun(GLfloat Scale):Planet(Scale, 0.0f,0.0f){
}
Sun::~Sun(){
	Planet::~Planet();
}
//�����࣬�̳����ǣ���Ҫ���ǵĹ���뾶
class Satellite:private	Planet{
public:
	Satellite(Planet &planet, GLfloat Scale, GLfloat SatelliteOrbitRadius,GLfloat SurroundSpeed);
	~Satellite();
	void SetMatrix(glm::vec3 ViewPos){
		PlanetData = planet->GetPlanetData();
		this->view = PlanetData.view;
		this->projection = PlanetData.projection;
		glm::mat4 model;
		glm::vec3 PlanetPosition = PlanetData.PlanetPosition;
		glm::vec3 SatellitePosition(PlanetPosition.x+this->OrbitRadius*cos(this->RevolutionSpeed*glfwGetTime()),PlanetPosition.y,PlanetPosition.z+this->OrbitRadius*sin(this->RevolutionSpeed*glfwGetTime()));
		model = glm::translate(model, SatellitePosition); 
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(this->Scale));
		this->PlanetShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.LightDirection"), SatellitePosition.x, SatellitePosition.y, SatellitePosition.z);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "viewPos"), ViewPos.x, ViewPos.y, ViewPos.z);
	}
	//������Ⱥoffset�ȳ�ʼ�����洢����֤ÿ������ÿһ֡���offset������ֵͬ
	void SatelliteGroupInit(GLuint SatelliteNumber) {
		this->SatelliteNumber = SatelliteNumber;
		SGD.SatellitePositionOffset = new glm::vec3[SatelliteNumber];
		SGD.scale = new float[SatelliteNumber];
		SGD.rotate = new float[SatelliteNumber];
		srand(glfwGetTime()); 
		GLfloat offset = 2.5f;
		for (GLuint i = 0; i < SatelliteNumber; i++) {
			//������λ�ý������ƫ��������
			GLfloat x = ((rand() % (GLuint)(2 * offset * 1000)) / 1000 - offset);
			GLfloat y = (rand() % (GLuint)(2 * offset * 1000)) / 1000 - offset;
			GLfloat z= ((rand() % (GLuint)(2 * offset * 1000)) / 1000 - offset);
			SGD.SatellitePositionOffset[i].x = x;
			SGD.SatellitePositionOffset[i].y = y;
			SGD.SatellitePositionOffset[i].z = z;
			SGD.scale[i] = (GLfloat)(rand() % 10) / 10.0f;
			SGD.rotate[i] = rand() % 360;
		}
	}
	void SetMatrixAndRender(glm::vec3 ViewPos) {
		PlanetData = planet->GetPlanetData();
		this->view = PlanetData.view;
		this->projection = PlanetData.projection;
		this->PlanetShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));

		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "viewPos"), ViewPos.x, ViewPos.y, ViewPos.z);

		GLdouble CurrentTime = glfwGetTime();//��ȡϵͳʱ�䣬��֤i������λ�ñ仯��ʹ����ͬ��ϵͳʱ�����
		for (int i = 0; i < SatelliteNumber; i++) {
			glm::mat4 model;
			glm::vec3 SatellitePosition;
			GLfloat angle = (GLfloat)i / (GLfloat)SatelliteNumber*360.0f;
			SatellitePosition.x = PlanetData.PlanetPosition.x + this->OrbitRadius*cos(angle + CurrentTime*this->RevolutionSpeed)+SGD.SatellitePositionOffset[i].x;
			SatellitePosition.y = PlanetData.PlanetPosition.y +SGD.SatellitePositionOffset[i].y;
			SatellitePosition.z = PlanetData.PlanetPosition.z + this->OrbitRadius*sin(angle + CurrentTime*this->RevolutionSpeed)+SGD.SatellitePositionOffset[i].z;
			model = glm::translate(model, SatellitePosition);
			model = glm::scale(model, glm::vec3(SGD.scale[i]));
			model = glm::rotate(model, glm::radians(SGD.rotate[i]), glm::vec3(0.1f));
			glUniform3f(glGetUniformLocation(this->PlanetShader.Program, "light.LightDirection"), SatellitePosition.x,SatellitePosition.y,SatellitePosition.z);
			glUniformMatrix4fv(glGetUniformLocation(this->PlanetShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
			this->RenderSatellite();
		}
	}
	void ReadShaderAndModel(char *PathVt, char *PathFg, char *PathModel) {
		Planet::ReadShaderAndModel(PathVt, PathFg, PathModel);
	};
	void RenderSatellite() {
		Planet::RenderPlanet();
	};
private:
	Planet *planet;
	struct SatelliteGroupData{
		glm::vec3 *SatellitePositionOffset;//����Ⱥ�����������ƫ��λ��
		GLfloat *scale;//����Ⱥ���������������ϵ��
		GLfloat *rotate;//����Ⱥ�������������תϵ��
	}SGD;
	DataBlock PlanetData;
	GLuint SatelliteNumber;
};
Satellite::Satellite(Planet &planet, GLfloat Scale, GLfloat SatelliteOrbitRadius, GLfloat SurroundSpeed) :Planet(Scale, SatelliteOrbitRadius, SurroundSpeed) {
	this->planet = &planet;
}
Satellite::~Satellite(){
	Planet::~Planet();
}

