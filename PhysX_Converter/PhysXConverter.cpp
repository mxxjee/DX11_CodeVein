#include "pch.h"
#include "PhysXConverter.h"

#ifdef _DEBUG

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
PhysXConverter::PhysXConverter()
{
}

PhysXConverter::~PhysXConverter()
{
    Release();
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 초기화 / 해제 ////////////////////////////////////////////////////////
bool PhysXConverter::Initialize()
{
    // 추가됨 : exe 위치 기준으로 출력 경로 설정
    char exePath[MAX_PATH] = {};
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);

    fs::path exeDir = fs::path(exePath).parent_path();
    m_strCacheDir = (exeDir / "../DataFiles/PhysXCache/").string();

    // 경로 정규화
    if (fs::exists(exeDir))
        m_strCacheDir = fs::canonical(exeDir / "../DataFiles/PhysXCache").string() + "/";

    cout << "[출력 경로] " << m_strCacheDir << endl;

    m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
    if (!m_pFoundation)
    {
        cout << "[실패] PxFoundation 생성 실패" << endl;
        return false;
    }

    m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale());
    if (!m_pPhysics)
    {
        cout << "[실패] PxPhysics 생성 실패" << endl;
        return false;
    }

    cout << "[PhysX] 초기화 성공" << endl;
    return true;
}

void PhysXConverter::Release()
{
    if (m_pPhysics)
    {
        m_pPhysics->release();
        m_pPhysics = {};
    }

    if (m_pFoundation)
    {
        m_pFoundation->release();
        m_pFoundation = {};
    }
}
/******************************************************* 초기화 / 해제 *******************************************************/



//////////////////////////////////////////////////////// 메인 변환 함수 ////////////////////////////////////////////////////////
bool PhysXConverter::Convert(const string& _inputPath, COOK_MODE _mode)
{
    // UTF-8 경로 변환
    string utf8Path = ConvertToUTF8(_inputPath);

    // FBX 로드 (좌표계 변환 + 삼각화)
    unsigned int flag = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_PreTransformVertices;

    const aiScene* pScene = m_Importer.ReadFile(utf8Path, flag);
    if (pScene == nullptr)
    {
        cout << "[실패] FBX 로드 실패: " << m_Importer.GetErrorString() << endl;
        return false;
    }

    // 모델명 추출
    fs::path filePath(_inputPath);
    string modelName = filePath.stem().string();

    cout << "FBX 로드 성공: " << _inputPath << endl;
    cout << "메쉬 개수: " << pScene->mNumMeshes << endl;
    cout << "쿠킹 모드: " << (_mode == COOK_MODE::CONVEX ? "CONVEX" : "TRIANGLE") << endl;
    cout << endl;

    // 출력 폴더 생성
    if (!fs::exists(m_strCacheDir))
        fs::create_directories(m_strCacheDir);

    unsigned int successCount = {};
    unsigned int failCount = {};

    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
    {
        const aiMesh* aiMesh = pScene->mMeshes[i];
        string outputPath = Make_OutputPath(modelName, i);

        cout << "  [메쉬 " << i << "] " << aiMesh->mName.data
            << " (정점: " << aiMesh->mNumVertices
            << ", 삼각형: " << aiMesh->mNumFaces << ")" << endl;

        bool result = false;

        if (_mode == COOK_MODE::CONVEX)
            result = Cook_Convex(aiMesh, outputPath);
        else
            result = Cook_Triangle(aiMesh, outputPath);

        if (result)
        {
            cout << "    -> 성공: " << outputPath << endl;
            ++successCount;
        }
        else
        {
            cout << "    -> 실패!" << endl;
            ++failCount;
        }
    }

    cout << endl;
    cout << "결과: 성공 " << successCount << " / 실패 " << failCount << endl;

    m_Importer.FreeScene();

    return (failCount == 0);
}
/******************************************************* 메인 변환 함수 *******************************************************/



//////////////////////////////////////////////////////// 쿠킹 함수 ////////////////////////////////////////////////////////
bool PhysXConverter::Cook_Convex(const aiMesh* _aiMesh, const string& _outputPath)
{
    if (_aiMesh->mNumVertices == 0)
        return false;

    // 정점 위치 추출 (aiVector3D는 float x, y, z 연속 배열이라 그대로 캐스팅 가능)
    PxCookingParams cookingParams(m_pPhysics->getTolerancesScale());
    cookingParams.convexMeshCookingType = PxConvexMeshCookingType::eQUICKHULL;

    PxConvexMeshDesc convexDesc;
    convexDesc.points.count = _aiMesh->mNumVertices;
    convexDesc.points.stride = sizeof(aiVector3D);
    convexDesc.points.data = _aiMesh->mVertices;
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eSHIFT_VERTICES;
    convexDesc.vertexLimit = 128;

    // 쿠킹
    PxDefaultMemoryOutputStream writeBuffer;
    if (!PxCookConvexMesh(cookingParams, convexDesc, writeBuffer))
    {
        cout << "    [실패] Convex 쿠킹 실패" << endl;
        return false;
    }

    // 파일 저장
    ofstream file(_outputPath, ios::binary);
    if (!file.is_open())
    {
        cout << "    [실패] 파일 생성 실패: " << _outputPath << endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(writeBuffer.getData()), writeBuffer.getSize());
    file.close();

    return true;
}

bool PhysXConverter::Cook_Triangle(const aiMesh* _aiMesh, const string& _outputPath)
{
    if (_aiMesh->mNumVertices == 0 || _aiMesh->mNumFaces == 0)
        return false;

    // 인덱스 추출
    vector<unsigned int> indices;
    indices.reserve(_aiMesh->mNumFaces * 3);

    for (unsigned int i = 0; i < _aiMesh->mNumFaces; ++i)
    {
        const aiFace& face = _aiMesh->mFaces[i];
        if (face.mNumIndices != 3)
            continue;

        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    // 쿠킹 설정
    PxCookingParams cookingParams(m_pPhysics->getTolerancesScale());

    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = _aiMesh->mNumVertices;
    meshDesc.points.stride = sizeof(aiVector3D);
    meshDesc.points.data = _aiMesh->mVertices;
    meshDesc.triangles.count = (unsigned int)(indices.size() / 3);
    meshDesc.triangles.stride = sizeof(unsigned int) * 3;
    meshDesc.triangles.data = indices.data();

    // 쿠킹
    PxDefaultMemoryOutputStream writeBuffer;
    if (!PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer))
    {
        cout << "    [실패] Triangle 쿠킹 실패" << endl;
        return false;
    }

    // 파일 저장
    ofstream file(_outputPath, ios::binary);
    if (!file.is_open())
    {
        cout << "    [실패] 파일 생성 실패: " << _outputPath << endl;
        return false;
    }

    file.write(reinterpret_cast<const char*>(writeBuffer.getData()), writeBuffer.getSize());
    file.close();

    return true;
}
/******************************************************* 쿠킹 함수 *******************************************************/



//////////////////////////////////////////////////////// 유틸리티 함수 ////////////////////////////////////////////////////////
string PhysXConverter::Make_OutputPath(const string& _modelName, unsigned int _meshIndex)
{
    // 런타임 Make_Cache_Path 호환: 모델명_메쉬인덱스.physx
    return m_strCacheDir + _modelName + "_" + to_string(_meshIndex) + ".physx";
}

string PhysXConverter::ConvertToUTF8(const string& _inputPath)
{
    string utf8Path = _inputPath;

    int wideSize = MultiByteToWideChar(CP_ACP, 0, _inputPath.c_str(), -1, nullptr, 0);
    if (wideSize > 0)
    {
        wstring widePath(wideSize, 0);
        MultiByteToWideChar(CP_ACP, 0, _inputPath.c_str(), -1, &widePath[0], wideSize);

        int utf8Size = WideCharToMultiByte(CP_UTF8, 0, widePath.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (utf8Size > 0)
        {
            utf8Path.resize(utf8Size);
            WideCharToMultiByte(CP_UTF8, 0, widePath.c_str(), -1, &utf8Path[0], utf8Size, nullptr, nullptr);
        }
    }

    return utf8Path;
}
/******************************************************* 유틸리티 함수 *******************************************************/

#endif // _DEBUG