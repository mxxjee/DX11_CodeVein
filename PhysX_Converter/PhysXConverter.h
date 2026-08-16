#pragma once

#ifdef _DEBUG

#include <Windows.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "PhysX/PxPhysicsAPI.h"

using namespace std;
using namespace physx;

namespace fs = std::filesystem;

//////////////////////////////////////////////////////// 쿠킹 모드 ////////////////////////////////////////////////////////
enum class COOK_MODE
{
    CONVEX,
    TRIANGLE
};

//////////////////////////////////////////////////////// PhysX 컨버터 클래스 ////////////////////////////////////////////////////////
class PhysXConverter
{
public:
    PhysXConverter();
    ~PhysXConverter();

public:
    // 초기화 / 해제
    bool Initialize();
    void Release();

    // 메인 변환 함수
    bool Convert(const string& _inputPath, COOK_MODE _mode);

private:
    // 쿠킹
    bool Cook_Convex(const aiMesh* _aiMesh, const string& _outputPath);
    bool Cook_Triangle(const aiMesh* _aiMesh, const string& _outputPath);

    // 유틸리티
    string Make_OutputPath(const string& _modelName, unsigned int _meshIndex);
    string ConvertToUTF8(const string& _inputPath);

private:
    // PhysX 기본 객체
    PxFoundation* m_pFoundation = {};
    PxPhysics* m_pPhysics = {};
    PxDefaultAllocator m_Allocator = {};
    PxDefaultErrorCallback m_ErrorCallback = {};

    // Assimp
    Assimp::Importer m_Importer;

    // 출력 경로
    string m_strCacheDir = "../DataFiles/PhysXCache/";
};
/******************************************************* PhysX 컨버터 클래스 *******************************************************/

#endif // _DEBUG