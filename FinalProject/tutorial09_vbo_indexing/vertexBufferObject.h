/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Dec 5, 2023

Description:

It's a class to manage the vertex buffer object.

*/

#pragma once

#include <vector>
using namespace std;
/********************************

Class:		CVertexBufferObject

Purpose:	Wraps OpenGL vertex buffer
			object.

********************************/

typedef unsigned char BYTE;
typedef unsigned int UINT;

class CVertexBufferObject
{
public:
	void CreateVBO(int a_iSize = 0);
	void DeleteVBO();

	void* MapBufferToMemory(int iUsageHint);
	void* MapSubBufferToMemory(int iUsageHint, UINT uiOffset, UINT uiLength);
	void UnmapBuffer();

	void BindVBO(int a_iBufferType = GL_ARRAY_BUFFER);
	void UploadDataToGPU(int iUsageHint);
	
	void AddData(void* ptrData, UINT uiDataSize);

	void* GetDataPointer();
    UINT GetBufferID();

	int GetCurrentSize();

	CVertexBufferObject();

private:
    UINT uiBuffer;
	int iSize;
	int iCurrentSize;
	int iBufferType;
	vector<BYTE> data;

	bool bDataUploaded;
};