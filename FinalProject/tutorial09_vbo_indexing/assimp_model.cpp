/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Dec 5, 2023

Description:

It's a class to manipulate the model using Assimp library. The model is loaded from a file and the model matrix is computed and returned.

*/

#include "common_header.h"

//#include "win_OpenGLApp.h"

#include "assimp_model.h"

#pragma comment(lib, "assimp.lib")

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <iostream>
#include <string>

#include <common/controls.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <unordered_set>

#include <vector>
#include <mutex>
#include <condition_variable>
#include <ctime>

using namespace std;
std::mutex mtx;
std::condition_variable cv;
bool ready = false;
bool stopThreads = false;
int finishedThreads = 0; // the number of threads
bool allThreadsFinished = false;
int repeatCount = 0;
std::vector<string> modelNameGroup = {"Gost", "Pumpkin2", "Pumpkin3", "Pumpkin4"};

CVertexBufferObject CAssimpModel::vboModelData;
UINT CAssimpModel::uiVAO;
vector<CTexture> CAssimpModel::tTextures;

/*-----------------------------------------------

Name:	GetDirectoryPath

Params:	sFilePath - guess ^^

Result: Returns directory name only from filepath.

/*---------------------------------------------*/

string GetDirectoryPath(string sFilePath)
{
	// Get directory path
	string sDirectory = "";
	RFOR(i, ESZ(sFilePath)-1)if(sFilePath[i] == '\\' || sFilePath[i] == '/')
	{
		sDirectory = sFilePath.substr(0, i+1);
		break;
	}
	return sDirectory;
}

CAssimpModel::CAssimpModel()
{
	bLoaded = false;
}

void CAssimpModel::ProcessNode(aiNode* node, const aiScene* scene)
{
    // Store the name and the meshes associated with it
    string strNodeName(node->mName.C_Str());
    std::size_t found = strNodeName.find("default");

    if (node->mNumMeshes)
    {
        if (found == std::string::npos)
        {
            m_strLastName = strNodeName;
        }
        else
        {
            strNodeName = m_strLastName;
        }
        // See if the object is already present
        auto objItr = m_mapObjects.find(strNodeName);
        if (objItr == m_mapObjects.end() && node->mNumMeshes)
        {
            m_setIds.insert(node->mMeshes[0]);
            pair< std::string, std::unordered_set<unsigned int>> tmp_pair =  make_pair(string(strNodeName), unordered_set<unsigned int>({node->mMeshes[0]}));
            auto newElement = m_mapObjects.insert(tmp_pair);
            if (node->mNumMeshes > 1)
            {
                objItr = m_mapObjects.find(strNodeName);
                for (int ii = 1; ii < node->mNumMeshes; ++ii)
                {
                    objItr->second.insert(node->mMeshes[ii]);
                    m_setIds.insert(node->mMeshes[ii]);

                }
            }
        }
        else
        {
            for (int ii = 0; ii < node->mNumMeshes; ++ii)
            {
                objItr->second.insert(node->mMeshes[ii]);
                m_setIds.insert(node->mMeshes[ii]);

            }
        }
    }

    // Recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

/*-----------------------------------------------

Name:	LoadModelFromFile

Params:	sFilePath - guess ^^

Result: Loads model using Assimp library.

/*---------------------------------------------*/

bool CAssimpModel::LoadModelFromFile(char* sFilePath)
{
	if(vboModelData.GetBufferID() == 0)
	{
		vboModelData.CreateVBO();
		tTextures.reserve(50);
	}
	Assimp::Importer importer;
	scene = importer.ReadFile( sFilePath,
		aiProcess_CalcTangentSpace       | 
		aiProcess_Triangulate            |
		aiProcess_JoinIdenticalVertices  |
		aiProcess_SortByPType);

	if(!scene)
	{
//		MessageBox(appMain.hWnd, "Couldn't load model ", "Error Importing Asset", MB_ICONERROR);
        std::cout << "Couldn't load model " << sFilePath << std::endl;
		return false;
	}

    // Get the obj names
    ProcessNode(scene->mRootNode, scene);

    const int iVertexTotalSize = sizeof(aiVector3D)*2+sizeof(aiVector2D);
	
	int iTotalVertices = 0;

    // Initialize the model centers
    for (auto& obj : m_mapObjects)
    {
        m_mapObjectsRadius[obj.first] = 0;
        vModelCenters[obj.first] = glm::vec3(0.0f, 0.0f, 0.0f);
        vModelSizes[obj.first] = 0;
    }

	FOR(i, scene->mNumMeshes)
	{
        // Just draw a pumpkin
//        if (m_mapObjects["Pumpkin3"].find(i) == m_mapObjects["Pumpkin3"].end())
        if (m_setIds.find(i) == m_setIds.end())
        {
            continue;
        }
		aiMesh* mesh = scene->mMeshes[i];
        if (!mesh->HasTextureCoords(0))
        {
            continue;
        }
		int iMeshFaces = mesh->mNumFaces;
		iMaterialIndices.push_back(mesh->mMaterialIndex);
		int iSizeBefore = vboModelData.GetCurrentSize();
		iMeshStartIndices.push_back(iSizeBefore/iVertexTotalSize);

		FOR(j, iMeshFaces)
		{
			const aiFace& face = mesh->mFaces[j];
			FOR(k, 3)
			{
				aiVector3D pos = mesh->mVertices[face.mIndices[k]];
				aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[k]];
				aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[face.mIndices[k]] : aiVector3D(1.0f, 1.0f, 1.0f);
				vboModelData.AddData(&pos, sizeof(aiVector3D));
				vboModelData.AddData(&uv, sizeof(aiVector2D));
				vboModelData.AddData(&normal, sizeof(aiVector3D));
			}
		}
        FOR(v, mesh->mNumVertices)
        {
            aiVector3D pos = mesh->mVertices[v];
            for (auto& obj : m_mapObjects)
            {
                if (obj.second.find(i) != obj.second.end())
                {
                    vModelCenters[obj.first] += glm::vec3(pos.x, pos.y, pos.z);
                    vModelSizes[obj.first]++;
                }
            }
        }
		int iMeshVertices = mesh->mNumVertices;
		iTotalVertices += iMeshVertices;
		iMeshSizes.push_back((vboModelData.GetCurrentSize()-iSizeBefore)/iVertexTotalSize);
	}

    // Calculate the average coordinates of each m_mapObject
    for (auto& obj : m_mapObjects)
    {
        vModelCenters[obj.first] /= vModelSizes[obj.first];
    }

    // Calculate the radius of each m_mapObject
    for (auto& obj : m_mapObjects)
    {
        for (auto& i : obj.second)
        {
            aiMesh* mesh = scene->mMeshes[i];
            FOR(v, mesh->mNumVertices)
            {
                aiVector3D pos = mesh->mVertices[v];
                glm::vec3 center = vModelCenters[obj.first];
                glm::vec3 vertex = glm::vec3(pos.x, pos.y, pos.z);
                float distance = glm::distance(center, vertex);
                if (distance > m_mapObjectsRadius[obj.first])
                {
                    m_mapObjectsRadius[obj.first] = distance;
                }
            }
        }
    }

    m_mapObjectsRadius["Gost"] = 10.0f;

	iNumMaterials = scene->mNumMaterials;

	vector<int> materialRemap(iNumMaterials);

	FOR(i, iNumMaterials)
	{
		const aiMaterial* material = scene->mMaterials[i];
		int a = 5;
		int texIndex = 0;
		aiString path;  // filename

		if(material->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
		{
			string sDir = GetDirectoryPath(sFilePath);
			string sTextureName = path.data;
			string sFullPath = sDir+sTextureName;
			int iTexFound = -1;
			FOR(j, ESZ(tTextures))if(sFullPath == tTextures[j].GetPath())
			{
				iTexFound = j;
				break;
			}
			if(iTexFound != -1)materialRemap[i] = iTexFound;
			else
			{
				CTexture tNew;
				tNew.LoadTexture2D(sFullPath, true);
				materialRemap[i] = ESZ(tTextures);
				tTextures.push_back(tNew);
			}
		}
	}

	FOR(i, ESZ(iMeshSizes))
	{
		int iOldIndex = iMaterialIndices[i];
		iMaterialIndices[i] = materialRemap[iOldIndex];
	}

	return bLoaded = true;
}

/*-----------------------------------------------

Name:	FinalizeVBO

Params: none

Result: Uploads all loaded model data in one global
		models' VBO.

/*---------------------------------------------*/

void CAssimpModel::FinalizeVBO()
{
	glGenVertexArrays(1, &uiVAO);
	glBindVertexArray(uiVAO);
	vboModelData.BindVBO();
	vboModelData.UploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), (void*)sizeof(aiVector3D));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), (void*)(sizeof(aiVector3D)+sizeof(aiVector2D)));
}

/*-----------------------------------------------

Name:	BindModelsVAO

Params: none

Result: Binds VAO of models with their VBO.

/*---------------------------------------------*/

void CAssimpModel::BindModelsVAO()
{
	glBindVertexArray(uiVAO);
}

/*-----------------------------------------------

Name:	RenderModel

Params: none

Result: Guess what it does ^^.

/*---------------------------------------------*/

void CAssimpModel::RenderModel(GLuint& programID)
{
	if(!bLoaded)return;

    for (auto& obj : m_mapObjects)
    {
        // Get a handle for our "MVP" uniform
        GLuint MatrixID = glGetUniformLocation(programID, "MVP");
        GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
        GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

        // Use our shader
        glUseProgram(programID);

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = m_mapModelMatrices[obj.first];
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
        for (auto& i : obj.second)
        {
            int iMatIndex = iMaterialIndices[i];
            if (tTextures.size() > iMatIndex)
                tTextures[iMatIndex].BindTexture();
            glDrawArrays(GL_TRIANGLES, iMeshStartIndices[i], iMeshSizes[i]);
        }
    }
}

/*-----------------------------------------------

 * Get the center of the model

/*---------------------------------------------*/

glm::vec3 CAssimpModel::GetModelCenter(const string& modelName)
{
    glm::vec3 center = vModelCenters[modelName];
    return center;
}

/*-----------------------------------------------

 * Get the number of models

/*---------------------------------------------*/
int CAssimpModel::GetNumModels()
{
    numModels = m_mapObjects.size();
    return numModels;
}

/*-----------------------------------------------

 * Get the model matrix of the model, return the m_mapModelPositions

/*---------------------------------------------*/

std::unordered_map<std::string, glm::mat4> CAssimpModel::InitModelPos(glm::vec3 initialPos)
{
    for (auto& obj : m_mapObjects)
    {
        m_mapModelPositions[obj.first] = initialPos;
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), initialPos);
        m_mapModelMatrices[obj.first] = modelMatrix;
        m_mapModelRotations[obj.first] = 0.0f;
    }
    return m_mapModelMatrices;
}

/*-----------------------------------------------

 * thread function

/*---------------------------------------------*/
void CAssimpModel::threadFunction(const std::string& name)
{
    int repeatCountThread = 0;
    do
    {
        {
            // wait until main() sends data
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [] { return ready; });
        }

        for (auto& obj : m_mapObjects)
        {
            if (obj.first != name)
            {
                checkCollision(name, obj.first);
            }
        }

        respondToSpace(name);

        // Update the number of finished threads
        {
            unique_lock<mutex> lock(mtx);
            finishedThreads++;

            if (finishedThreads == numModels)
            {
                allThreadsFinished = true;
                cv.notify_all();
            }
        }

        // wait until the main function tell the thread to continue
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [repeatCountThread] { return repeatCount != repeatCountThread; });
        }
        repeatCountThread++;
        if (stopThreads)
        {
            break;
        }
    } while (repeatCount == repeatCountThread);
}


/*-----------------------------------------------

 * Compute the model matrix of the model, return the m_mapModelPositions

/*---------------------------------------------*/


std::unordered_map<std::string, glm::mat4> CAssimpModel::ComputeModelMatrix()
{
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Signal worker threads to start processing
    {
        unique_lock<mutex> lock(mtx);
        ready = true;
        cv.notify_all();
    }

    // waiting for other threads to finish their works
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return allThreadsFinished; });
        ready = false;
        allThreadsFinished = false;
        finishedThreads = 0;
    }

    // Update the position of the objects
    for (auto& obj : m_mapObjects)
    {
        const float rotationSpeed = 2.50f;
        float angle = rotationSpeed * deltaTime;
        m_mapModelRotations[obj.first] += angle;
        glm::vec3 orientation = glm::vec3(0.0f, 0.0f, 1.0f);

        m_mapModelPositions[obj.first] += deltaTime * m_mapObjectsVelocity[obj.first];

        glm::mat4 modelMatrix = glm::mat4(1.0f);

        glm::mat4 modelMatrix1 = glm::translate(modelMatrix, m_mapModelPositions[obj.first]);
        glm::mat4 modelMatrix2 = glm::rotate(modelMatrix, m_mapModelRotations[obj.first], orientation);
        m_mapModelMatrices[obj.first] = modelMatrix1 * modelMatrix2;
    }

    {
        unique_lock<mutex> lock(mtx);
        repeatCount++;
        // Update the last time
        lastTime = currentTime;
        cv.notify_all();
    }

    return m_mapModelMatrices;
}

/*-----------------------------------------------

 * Check collision between two models

-----------------------------------------------*/
void CAssimpModel::checkCollision(const std::string& name1, const std::string& name2)
{
    unique_lock<mutex> lock(mtx);
    glm::vec4 originCenter1 = glm::vec4(vModelCenters[name1], 1.0f);
    glm::vec3 center1 = glm::vec3(m_mapModelMatrices[name1] * originCenter1);
    glm::vec4 originCenter2 = glm::vec4(vModelCenters[name2], 1.0f);
    glm::vec3 center2 = glm::vec3(m_mapModelMatrices[name2] * originCenter2);

    float distance = glm::distance(center1, center2);
    float radius1 = m_mapObjectsRadius[name1];
    float radius2 = m_mapObjectsRadius[name2];

    if (distance < radius1 + radius2)
    {
        glm::vec3 normal = glm::normalize(center2 - center1);
        glm::vec3 tangentComponent = glm::dot(m_mapObjectsVelocity[name1], normal) * normal;
        m_mapObjectsVelocity[name1] -= 2.0f * tangentComponent;
    }
}

/*-----------------------------------------------

 * The objects should also be confined to the space

-----------------------------------------------*/
bool CAssimpModel::respondToSpace(const std::string& name)
{
//    glm::vec4 originCenter = glm::vec4(vModelCenters[name], 1.0f);
//    glm::vec4 center = m_mapModelPositions[name] * originCenter;

    bool res = false;
    // Space
    const float radius = 30.0f; // radius of the cylinder
    const float minZ = 0.0f;            // minimum Z coordinate


    glm::vec3 newModelMatrix = m_mapModelPositions[name];
    glm::vec3 newCenter = glm::vec3(newModelMatrix[0], newModelMatrix[1], newModelMatrix[2]);
    glm::vec3 newVelocity = m_mapObjectsVelocity[name];

    float newX = newModelMatrix[0];
    float newY = newModelMatrix[1];
    float newZ = newModelMatrix[2];

    if (newZ < minZ) {
        res = true;
        newVelocity.z = -newVelocity.z;
        newZ = minZ;
    }

    float distance = sqrt(newX * newX + newY * newY + newZ * newZ);
    if (distance > radius) {
        res = true;
        glm::vec3 normal = -glm::normalize(newCenter);
        float len = glm::length(newVelocity);
        newVelocity = len * normal;

        float scale = radius / distance;
        newX *= scale;
        newY *= scale;
        newZ *= scale;
    }

    {
        unique_lock<mutex> lock(mtx);
        m_mapObjectsVelocity[name] = newVelocity;
        m_mapModelPositions[name].x = newX;
        m_mapModelPositions[name].y = newY;
        m_mapModelPositions[name].z = newZ;
    }

    return res;
}

/*-----------------------------------------------

 * Randomly initialize the velocity of the object

-----------------------------------------------*/
void CAssimpModel::initVelocity()
{
    srand(static_cast<unsigned int>(std::time(nullptr)));
    for (auto& obj : m_mapObjects)
    {
        float x = (rand() % 100) / 10.0f;
        float y = (rand() % 100) / 10.0f;
        float z = (rand() % 100) / 10.0f;
        m_mapObjectsVelocity[obj.first] = glm::vec3(x, y, z);
        threads.emplace_back(&CAssimpModel::threadFunction, this, obj.first);
    }
}

/*-----------------------------------------------

 * Close the model

-----------------------------------------------*/
void CAssimpModel::closeCAssimpModel()
{
    {
        unique_lock<mutex> lock(mtx);
        ready = true;
        stopThreads = true;
        repeatCount++;
        cv.notify_all();
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
}