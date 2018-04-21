#include<vector>
#include<string>
#include<algorithm>
using namespace std;

#define GLEW_STATIC
#include<GL\glew.h>

#include<GLFW\glfw3.h>

#include"Camera.h"
#include"Shader.h"
#include<SOIL.h>

#include<glm\glm.hpp>
#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\matrix_transform.hpp>

#include"Planet.h"

GLuint WIDTH = 1000, HEIGHT = 700;
bool isFirst = true;
double LastxPos = (double)WIDTH / 2.0f;
double LastyPos = (double)HEIGHT / 2.0f;
GLfloat lastTime = 0.0f;
Camera SolarSystemCamera(glm::vec3(0.0f, 0.0f, 100.0f));
bool keys[1024];
GLfloat deltaTime;
glm::vec3 LightPos(50.0f, 30.0f, 40.0f);
//键盘回调函数，参数顺序不可打乱
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE&&action == GLFW_PRESS)glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS) { keys[key] = true; }
	else if (action == GLFW_RELEASE)keys[key] = false;
}
//鼠标回调函数，用于摄像机镜头的旋转
void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (isFirst) {
		LastxPos = xPos;
		LastyPos = yPos;
		isFirst = false;
	}
	GLfloat MouseSpeed = 0.5f;
	GLfloat xoffset = xPos - LastxPos;
	GLfloat yoffset = LastyPos - yPos;
	LastxPos = xPos;
	LastyPos = yPos;
	SolarSystemCamera.ProcessMouseMovement(MouseSpeed*xoffset, MouseSpeed*yoffset);
}
void cameraMove() {
	if (keys[GLFW_KEY_W])SolarSystemCamera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])SolarSystemCamera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])SolarSystemCamera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])SolarSystemCamera.ProcessKeyboard(RIGHT, deltaTime);
}
GLuint generateAttachmentTexture(GLboolean depth,GLboolean stencil) {
	GLenum attachmenttype;
	if (!depth && !stencil)
		attachmenttype = GL_RGB;
	else if (!depth&&stencil)
		attachmenttype = GL_STENCIL_INDEX;
	else if (!stencil&&depth)
		attachmenttype = GL_DEPTH_COMPONENT;

	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	if (!depth && !stencil)
		glTexImage2D(GL_TEXTURE_2D, 0, attachmenttype, WIDTH, HEIGHT, 0, attachmenttype, GL_UNSIGNED_BYTE, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureId;
};
//纹理加载函数
/*
GLuint loadTex(GLchar* path) {
GLuint TexID;
glGenTextures(1, &TexID);
int width, height;
unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);

glBindTexture(GL_TEXTURE_2D, TexID);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
glGenerateMipmap(GL_TEXTURE_2D);

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glBindTexture(GL_TEXTURE_2D,0);
SOIL_free_image_data(image);
return TexID;
}
*/
//天空盒加载函数
GLuint loadBox(vector<const char*> faces) {
	GLuint TexID;
	glGenTextures(1, &TexID);
	int width, height;
	unsigned char* image;
	glBindTexture(GL_TEXTURE_CUBE_MAP, TexID);
	for (GLuint i = 0; i < faces.size(); i++) {
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return TexID;
}
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "SolarSystem", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initizlize GLEW" << std::endl;
		return -1;
	}
	glViewport(0, 0, WIDTH, HEIGHT);
	//开启深度测试
	glDepthFunc(GL_LESS);
	//glEnable(GL_PROGRAM_POINT_SIZE);
	//开启面剔除优化,默认情况下逆时针为正面，构造矩形需要注意
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	GLfloat UniverseBox[] = {//天空盒顶点数据      
		-50.0f,  50.0f, -50.0f,
		-50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,

		-50.0f, -50.0f,  50.0f,
		-50.0f, -50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f,  50.0f,
		-50.0f, -50.0f,  50.0f,

		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,

		-50.0f, -50.0f,  50.0f,
		-50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f, -50.0f,  50.0f,
		-50.0f, -50.0f,  50.0f,

		-50.0f,  50.0f, -50.0f,
		50.0f,  50.0f, -50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		-50.0f,  50.0f,  50.0f,
		-50.0f,  50.0f, -50.0f,

		-50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f,  50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f,  50.0f,
		50.0f, -50.0f,  50.0f
	};
	GLfloat frameVertices[] = {
		-1.0f,0.5f,0.0f,0.0f,
		-0.5f,1.0f,1.0f,1.0f,
		-1.0f,1.0f,0.0f,1.0f,
		
		-1.0f,0.5f,0.0f,0.0f,
		-0.5f,0.5f,1.0f,0.0f,
		-0.5f,1.0f,1.0f,1.0f
	
	};


	GLuint UbVAO, UbVBO;
	glGenVertexArrays(1, &UbVAO);
	glGenBuffers(1, &UbVBO);
	glBindVertexArray(UbVAO);
	glBindBuffer(GL_ARRAY_BUFFER, UbVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UniverseBox), UniverseBox, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindVertexArray(0);

	GLuint frameVAO, frameVBO;
	glGenVertexArrays(1, &frameVAO);
	glGenBuffers(1, &frameVBO);
	glBindVertexArray(frameVAO);
	glBindBuffer(GL_ARRAY_BUFFER,frameVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), frameVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));
	glBindVertexArray(0);

	vector<const GLchar*> faces;
	faces.push_back("UniverseBox/U_rt.jpg");
	faces.push_back("UniverseBox/U_lf.jpg");
	faces.push_back("UniverseBox/U_up.jpg");
	faces.push_back("UniverseBox/U_dn.jpg");
	faces.push_back("UniverseBox/U_bk.jpg");
	faces.push_back("UniverseBox/U_ft.jpg");
	GLuint UniverseBoxTexture = loadBox(faces);
	faces.clear();


	Shader UniverseBoxShader;
	UniverseBoxShader.fileShader("UniverseBox/U_Sader.vt", "UniverseBox/U_Sader.fg");
	Shader frameBufferShader;
	frameBufferShader.fileShader("frameBufferShader.vt","frameBufferShader.fg");

	/*行星类使用说明：
	*基类（Planet）使用：构造时传入参数顺序为（缩放，轨道半径，公转速度）
	使用RendShaderAndModel函数参数顺序（顶点着色器文件，片元着色器文件，星球模型文件（assimp可加载类型））
	使用成员函数SetMatrix参数顺序为（view矩阵，projection矩阵，viewposition视角位置）
	渲染使用成员函数RenderPlanet

	*Sun类使用：		 构造时传入缩放系数
	使用RendShaderAndModel函数参数顺序（顶点着色器文件，片元着色器文件，星球模型文件（assimp可加载类型））
	成员函数SetMatrix传入viewposition视角位置
	渲染使用RenderSun函数

	*Satellite类使用：	 构造时传入参数顺序为（环绕行星对象，缩放，轨道半径，公转速度）
	使用RendShaderAndModel函数参数顺序（顶点着色器文件，片元着色器文件，星球模型文件（assimp可加载类型））
	如果只有一个卫星，请使用成员函数SetMatrix，参数为viewposition
	如果有多个卫星（卫星群），请使用SatelliteGroupInit进行初始化
	渲染使用SetMatrixAndRender，参数为viewposition视角位置
	*/
	//行星模型初始化
	Sun sun(13.0f);
	sun.ReadShaderAndModel("SolarAndPlanet/Shader/SunShader.vt", "SolarAndPlanet/Shader/SunShader.fg", "SolarAndPlanet/Sun/Sun.obj");

	Planet Jupiter(4.0f, 100.0f, 0.0f);
	Jupiter.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
		"SolarAndPlanet/Shader/PlanetBeanShader.fg",
		"SolarAndPlanet/Jupiter/Jupiter.obj");


	Satellite SatelliteGroup(Jupiter, 1.0f, 30.0f, 1.0f);
	SatelliteGroup.ReadShaderAndModel("SolarAndPlanet/Shader/SatelliteShader.vt",
		"SolarAndPlanet/Shader/PlanetBeanShader.fg",
		"SolarAndPlanet/rock.obj");
	SatelliteGroup.SatelliteGroupInit(100);
	/*
	Satellite Moon(Jupiter, 0.5f, 20.0f, 1.5f);
	Moon.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
	"SolarAndPlanet/Shader/PlanetBeanShader.fg",
	"SolarAndPlanet/Earth/Moon.obj");

	Planet Mercury(1.0f, 70.0f,0.0f);
	Mercury.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
	"SolarAndPlanet/Shader/PlanetBeanShader.fg",
	"SolarAndPlanet/Mercury/Mercury.obj");

	Planet Venus(1.5f, 80.0f,0.0f);
	Venus.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
	"SolarAndPlanet/Shader/PlanetBeanShader.fg",
	"SolarAndPlanet/Venus/Venus.obj");

	Planet Earth(1.0f,100.0f,0.0f);
	Earth.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
	"SolarAndPlanet/Shader/PlanetBeanShader.fg",
	"SolarAndPlanet/Earth/Earth.obj");



	Planet Mars(1.0f, 0.0f);
	Mars.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
	"SolarAndPlanet/Shader/PlanetBeanShader.fg",
	"SolarAndPlanet/Mars/Mars.obj");



	Planet Neptune(1.0f, 0.0f);
	Neptune.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
	"SolarAndPlanet/Shader/PlanetBeanShader.fg",
	"SolarAndPlanet/Neptune/Neptune.obj");

	Planet Uranus(1.0f, 0.0f);
	Uranus.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
	"SolarAndPlanet/Shader/PlanetBeanShader.fg",
	"SolarAndPlanet/Uranus/Uranus.obj");

	Planet Saturn(1.0f, 0.0f);
	Saturn.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
	"SolarAndPlanet/Shader/PlanetBeanShader.fg",
	"SolarAndPlanet/Saturn/Saturn.obj");
	*/

	GLuint frameBuffer;
	glGenFramebuffers(1,&frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	GLuint textureColorBuffer = generateAttachmentTexture(false,false);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER ,GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER::framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;

		glfwPollEvents();
		cameraMove();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		glm::mat4 BoxView, projection;
		BoxView = SolarSystemCamera.GetViewMatrix();
		projection = glm::perspective(SolarSystemCamera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.0f);

		glDepthFunc(GL_LEQUAL);
		UniverseBoxShader.Use();
		BoxView = glm::mat4(glm::mat3(SolarSystemCamera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(UniverseBoxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(BoxView));
		glUniformMatrix4fv(glGetUniformLocation(UniverseBoxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(UbVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(UniverseBoxShader.Program, "UniverseBoxTexture"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, UniverseBoxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		frameBufferShader.Use();
		glBindVertexArray(frameVAO);
		glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		sun.SetMatrix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		sun.RenderSun();
		//Sun.RenderOrbit();

		Jupiter.SetMatrix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Jupiter.RenderPlanet();

		SatelliteGroup.SetMatrixAndRender(glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		/*

		Moon.SetMatrix(glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Moon.RenderSatellite();

		Mercury.SetMatrix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Mercury.RenderPlanet();

		Venus.SetMatrix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Venus.RenderPlanet();

		Earth.SetMatrix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Earth.RenderPlanet();



		Mars.SetMatrix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Mars.RenderPlanet();

		Neptune.SetMatrix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Neptune.RenderPlanet();

		Uranus.SetMatrix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Uranus.RenderPlanet();

		Saturn.SetMatrix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Saturn.RenderPlanet();
		*/
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		//glm::mat4 BoxView, projection;
		BoxView = SolarSystemCamera.GetViewMatrix();
		projection = glm::perspective(SolarSystemCamera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.0f);

		glDepthFunc(GL_LEQUAL);
		UniverseBoxShader.Use();
		BoxView = glm::mat4(glm::mat3(SolarSystemCamera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(UniverseBoxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(BoxView));
		glUniformMatrix4fv(glGetUniformLocation(UniverseBoxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(UbVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(UniverseBoxShader.Program, "UniverseBoxTexture"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, UniverseBoxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		sun.SetMatrix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		sun.RenderSun();
		//Sun.RenderOrbit();

		Jupiter.SetMatrix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Jupiter.RenderPlanet();

		SatelliteGroup.SetMatrixAndRender(glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));

		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}