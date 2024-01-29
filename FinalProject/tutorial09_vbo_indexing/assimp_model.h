/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Dec 5, 2023

Description:

It's a class to manipulate the model using Assimp library. The model is loaded from a file and the model matrix is computed and returned.

*/

#pragma once
#include <unordered_map>
#include <unordered_set>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "./common/shader.hpp"
#include "vertexBufferObject.h"
#include "texture.h"

#include <vector>
#include <thread>

class CMaterial
{
public:
	int iTexture;
};

typedef unsigned int UINT;

class CAssimpModel
{
public:
	bool LoadModelFromFile(char* sFilePath);

	static void FinalizeVBO();
	static void BindModelsVAO();
    void ProcessNode(aiNode* node, const aiScene* scene);

	void RenderModel(GLuint& programID);
	CAssimpModel();
    glm::vec3 GetModelCenter(const std::string& modelName); // get the center of the mesh
    const aiScene* scene;
    std::unordered_map<std::string, glm::vec3> vModelCenters; // store the center of each mesh
    std::unordered_map<std::string, int> vModelSizes; // store the size of each mesh
    int GetNumModels(); // get the number of models
    std::unordered_map<std::string, glm::mat4> InitModelPos(glm::vec3 initialPos); // compute the initial model matrix of each model
    std::unordered_map<std::string, glm::mat4> ComputeModelMatrix(); // compute the model matrix of each model
    void checkCollision(const std::string& name1, const std::string& name2); // check collision
    bool respondToSpace(const std::string& name); // respond to space
    void initVelocity(); // initialize the velocity of the object
    void threadFunction(const std::string& name); // thread function

    void closeCAssimpModel();
private:
    std::unordered_map<std::string, std::unordered_set<unsigned int>> m_mapObjects;
    std::unordered_map<std::string, float> m_mapObjectsRadius;
    std::unordered_map<std::string, glm::vec3> m_mapObjectsVelocity;
    std::unordered_set<unsigned int> m_setIds;
    std::string m_strLastName;
	bool bLoaded;
	static CVertexBufferObject vboModelData;
	static UINT uiVAO;
	static vector<CTexture> tTextures;
	vector<int> iMeshStartIndices;
	vector<int> iMeshSizes;
	vector<int> iMaterialIndices;
	int iNumMaterials;
    int numModels; // store the number of models
    std::unordered_map<std::string, glm::vec3> m_mapModelPositions;
    std::unordered_map<std::string, float> m_mapModelRotations;
    std::unordered_map<std::string, glm::mat4> m_mapModelMatrices;

    std::vector<std::thread> threads;
};
