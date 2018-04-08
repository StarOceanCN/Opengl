

#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sstream>
#include<map>
using namespace std;
//Mesh
#include"Mesh.h"
//Shader
#include<GL\Shader.h>
//Assimp
#include<assimp\Importer.hpp>
#include<assimp\scene.h>
#include<assimp\postprocess.h>
using namespace Assimp;
//soil
#include<SOIL.h>
//glew
#include<GL\glew.h>

class Model {
public:
	Model(string path);
	void ModelRender(Shader s,GLfloat shininess) {
		for (GLuint i = 0; i < this->meshes.size(); i++) {
			this->meshes[i].render(s,shininess);
		}
	};
	~Model();
private:
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;

	void processNode(aiNode* node, const aiScene* scene) {
		for (GLuint i = 0; i < node->mNumMeshes; i++)this->meshes.push_back(this->TransferMesh(scene->mMeshes[node->mMeshes[i]], scene));
		for (GLuint i = 0; i < node->mNumChildren; i++)processNode(node->mChildren[i], scene);
	};
	Mesh TransferMesh(aiMesh* mesh, const aiScene* scene) {
		vector<Vertex>vertices;
		vector<GLuint>indices;
		vector<Texture>textures;
		for (GLuint i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			if (mesh->mTextureCoords[0])vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			else vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			vertices.push_back(vertex);
		}
		for (GLuint i = 0; i < mesh->mNumFaces; i++) {
			aiFace f = mesh->mFaces[i];
			for (GLuint j = 0; j < f.mNumIndices; j++)indices.push_back(f.mIndices[j]);
		}
		if (mesh->mMaterialIndex >= 0) {

			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture>diffuseMaps = this->MaterialToTexture(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			vector<Texture>specularMaps = this->MaterialToTexture(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			return Mesh(vertices, indices, textures);
		}
	};
	vector<Texture> MaterialToTexture(aiMaterial* material, aiTextureType type, string texname) {
		vector<Texture>textures;
		for (GLuint i = 0; i < material->GetTextureCount(type); i++) {
			aiString aistr;
			material->GetTexture(type, i, &aistr);
			GLboolean skip = false;
			for (GLuint j = 0; j < textures_loaded.size(); j++) {
				if (textures_loaded[j].path == aistr) {
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) { 
				Texture texture;
				texture.Texid = TextureFromFile(aistr.C_Str(), this->directory);
				texture.Textype = texname;
				texture.path = aistr;
				textures.push_back(texture);
				this->textures_loaded.push_back(texture); 
			}
		}
		return textures;
	}
	GLuint TextureFromFile(const char* path, string directory) {
		string filename = directory + '/' + string(path);
		GLuint TextureID;
		glGenTextures(1, &TextureID);
		int w, h;
		unsigned char*image = SOIL_load_image(filename.c_str(), &w, &h, 0, SOIL_LOAD_RGB);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		SOIL_free_image_data(image);

		return TextureID;
	}
};
Model::Model(string path) {
	Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}
	this->directory = path.substr(0, path.find_last_of("/"));
	this->processNode(scene->mRootNode, scene);
}
Model::~Model() {}