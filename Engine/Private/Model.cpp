#include "Engine_Define.h"
#include "Model.h"

#include <set>

#include "Mesh.h"
#include "Material.h"
#include "Bone.h"
#include "Animation.h"
#include "Channel.h"
#include "Model_Struct.h"
#include "DebugDraw.h"
#include "GameInstance.h"
#include "Shader.h"
#include "StructuredBuffer.h"


_uint Model::s_iGlobalModelID = 0;

struct Engine::AnimFileInfo
{
    _wstring baseName = {};   // 확장자 제외 이름 (정렬 키)
    _wstring fullPath = {};   // 전체 경로
    _wstring ext = {};        // 확장자
    _wstring directory = {};  // 파일이 위치한 폴더 경로
};

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Model::Model()
{
}

Engine::Model::Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Component(pDevice, pContext)
{
}

Engine::Model::Model(const Model& original)
    : Component(original), m_eModelType(original.m_eModelType), m_iNumMeshes(original.m_iNumMeshes), m_iNumMaterials(original.m_iNumMaterials)
    , m_iNumBones(original.m_iNumBones), m_iNumAnimations(original.m_iNumAnimations), m_iCurrentAnimationIndex(original.m_iCurrentAnimationIndex)
    , m_PreTransformMatrix(original.m_PreTransformMatrix), m_iModelID(original.m_iModelID)
    , m_vecMeshes(original.m_vecMeshes), m_vecMaterials(original.m_vecMaterials)//, m_vecBones(original.m_vecBones), m_vecAnimations(original.m_vecAnimations)
    , m_Importer(), m_pAIScene(nullptr), m_vecFirstBoneInfo(original.m_vecFirstBoneInfo),
    m_iRootBoneIndex{ original.m_iRootBoneIndex }
    , m_vAccumulatedMotionDelta{ original.m_vAccumulatedMotionDelta }
    , m_fPrevTrackPosition{ original.m_fPrevTrackPosition }
    , m_vPrevRootPosition{ original.m_vPrevRootPosition }
    , m_wstrFilePath(original.m_wstrFilePath)
    , m_eModelRoleType(original.m_eModelRoleType), m_bNoneAnimFlag(original.m_bNoneAnimFlag), m_umapBoneIndexByName(original.m_umapBoneIndexByName)

{
    for (auto& mesh : m_vecMeshes)
    {
        Safe_AddRef(mesh);
    }
    for (auto& material : m_vecMaterials)
    {
        Safe_AddRef(material);
    }
    for (auto& bone : original.m_vecBones)
    {
        m_vecBones.push_back(bone->Clone());
    }
    for (auto& animation : original.m_vecAnimations)
    {
        m_vecAnimations.push_back(animation->Clone());
    }
#ifdef _DEBUG
#endif // _DEBUG

}

Engine::Model::~Model()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Model::Initialize_Prototype(MODEL _modeltype, const _wstring& _filepath, const _fmatrix& _preTransformMatrix, MODELROLE _RoleType, _bool bNoneAnimFlag)
{
    // 인스턴싱을 위해 모델 ID 생성
    m_iModelID = s_iGlobalModelID++;

    m_eModelType = _modeltype;
    m_eModelRoleType = _RoleType;
    m_bNoneAnimFlag = bNoneAnimFlag;

    XMStoreFloat4x4(&m_PreTransformMatrix, _preTransformMatrix);

    m_wstrFilePath = _filepath;

    _char filepath[MAX_PATH] = {};
    WideCharToMultiByte(CP_ACP, 0, _filepath.c_str(), (_int)_filepath.size(), filepath, MAX_PATH, NULL, NULL);

    _char filename[MAX_PATH];
    _char fileext[MAX_PATH];
    _splitpath_s(filepath, nullptr, 0, nullptr, 0, filename, MAX_PATH, fileext, MAX_PATH);

    if (!strcmp(fileext, ".fbx"))
    {
        MSG_FAIL(Initialize_FBX(_filepath), L"FBX파일의 로드에 실패했습니다!", L"모델 로드 실패", E_FAIL);
    }
    else if (!strcmp(fileext, ".siho"))
    {
        MSG_FAIL(Initialize_Binary(_filepath), L"SIHO파일의 로드에 실패했습니다!", L"모델 로드 실패", E_FAIL);
    }
    else if (!strcmp(fileext, ".glb"))
    {
        MSG_FAIL(Initialize_FBX(_filepath), L"GLB파일의 로드에 실패했습니다!", L"모델 로드 실패", E_FAIL);
    }
    else if (!strcmp(fileext, ".gltf"))
    {
        MSG_FAIL(Initialize_FBX(_filepath), L"GLTF파일의 로드에 실패했습니다!", L"모델 로드 실패", E_FAIL);
    }
    else
    {
        MSG_ON(L"잘못된 파일이 들어왔습니다.", L"모델 로드 실패");
        BREAK;
        return E_FAIL;
    }

    m_iNumBones = (_uint)m_vecBones.size();

    /*for (int i = 0; i < m_iNumAnimations; ++i)
    {
        COUT("Morph Count : " << m_pAIScene->mAnimations[i]->mNumMorphMeshChannels);
    }

    COUT("Bone Count : " << m_iNumBones);*/

    // 이러면 문제 생기나?
    //if (m_eModelRoleType == MODELROLE::MASTER) //마스터본은 메쉬가 0개이므로
    //{
    //    Initialize_UmapMasterBone();
    //}

    //Initialize_BoneIndexByName();
    

    return S_OK;
}

HRESULT Engine::Model::Initialize(void* arg)
{
    m_iCurrentAnimationIndex = 0;

    fill(begin(m_iReadbackBoneIndex), end(m_iReadbackBoneIndex), -1);

    for (auto& animation : m_vecAnimations)
    {
        animation->Set_ParentModel(this);
        animation->Build_BoneToChannelIndex(m_iNumBones);
    }
  
    if (m_eModelType == MODEL::NONANIM || m_eModelRoleType == MODELROLE::PART)
        return S_OK;

    // if (m_eModelType != MODEL::NONANIM && m_eModelRoleType != MODELROLE::PART) //애니메이션 아니고 파츠아닐때 컴퓨트 셰이더 빌드 및 생성
    {

        if (!m_bComputeBufferReady) //통모델 , 마스터 공통
        {
            //뼈 깊이 계층 준비 / 컴퓨트 셰이더는 수백개의 스레드가 동시에 돌아가기 때문에, 한 번 Dispatch로 모든 뼈의 월드행렬을 구할수 없음 , 깊이별로 순차적으로 Dispatch를 하기 위함
            MSG_FAIL(Build_BoneHierarchy(), L"뼈 깊이 계층 준비에 실패했습니다.", L"모델 복사 실패!", E_FAIL);
            //CS에서 ComBined 행렬을 연산하기 위한 컴퓨트셰이더와 버퍼들 준비
            MSG_FAIL(Ready_BoneCS_Buffers(), L"컴퓨트 쉐이더 및 버퍼 준비에 실패했습니다.", L"모델 복사 실패!", E_FAIL);
            if (m_eModelType == MODEL::PLAYERANIM && m_eModelRoleType == MODELROLE::MASTER)
            {
                // 플레이어의 마스터 모델이면(가슴, 옷의 초기 키프레임 데이터 덮어씌우기용)
                Override_KeyFrames_FromFemaleBindPose();
            }
            if (arg != nullptr) //통모델들 덮어씌우기
            {
                RESETPOSE_DESC* pDesc = static_cast<RESETPOSE_DESC*>(arg);
                if (!pDesc->vecTargetBoneNames.empty())
                {
                    MSG_FAIL(Override_KeyFrames_FromBindPose(*pDesc),
                        L"바인드포즈 키프레임 덮어씌우기 실패!", L"모델 초기화 실패!", E_FAIL);
                }
            }
            //기존 애니메이션->채널->키프레임의 구조를 GPU가 이해가능하도록 일차원 배열로 준비하는 작업
            MSG_FAIL(Build_AnimGPU(), L"GPU 배열 생성에 실패했습니다.", L"모델 복사 실패!", E_FAIL);
            //채널이 없는경우에 메쉬가 깨지지 않도록, 기존 로드 시점 로컬행렬을 SRT로 분해해서 버퍼에 저장해서 SRV로 사용용도
            MSG_FAIL(Ready_BindPose_Buffer(), L"분해 실패", L"모델 복사 실패!", E_FAIL);
            //상하체 블렌드용 가중치 준비
            if (m_eModelRoleType == MODELROLE::MASTER)
                MSG_FAIL(Ready_UpperBodyMaskWeight(), L"상체 블렌드용 가중치 생성 실패.", L"모델 복사 실패!", E_FAIL);
            //기존 채널에서 하던 채널 보간 + 플레이 애니메이션에서 하던 애니메이션 보간 을 CS에서 하기 위한 버퍼 + 셰이더 생성
            MSG_FAIL(Ready_AnimCS_Buffers(), L"컴쉐 생성 실패쓰.", L"모델 복사 실패!", E_FAIL);

            Reset_CurrentKeyIndex(); //애니메이션 바뀔때 현재 인덱스를 0으로 초기화하는용도
        }
        if (m_eModelRoleType == MODELROLE::STANDALONE)
        {
            MSG_FAIL(Build_StandAloneOffset(), L"스탠덜론 잘못됨", L"모델 복사 실패!", E_FAIL); //vector컨테이너에 메쉬 순회하면서 OffsetMatrix를 채워주고
            MSG_FAIL(Ready_StandAloneRemapSRV(), L"통모델(스탠덜론) 리맵 생성 실패", L"모델 복사 실패!", E_FAIL);
            CHECK_FAILED(Ready_Readback_Buffer(), E_FAIL); //CS에서 계산한 Combined행렬 읽어오기 위한 버퍼 생성
            
        }
        else if (m_eModelRoleType == MODELROLE::MASTER)
        {
            MSG_FAIL(Build_MastrOffset(), L"잘못됨", L"모델 복사 실패!", E_FAIL);
            CHECK_FAILED(Ready_Readback_Buffer(), E_FAIL); //CS에서 계산한 Combined행렬 읽어오기 위한 버퍼 생성
            //MSG_FAIL(Ready_DrapeBoneIndex(),L"망토 본 못찾음", L"망토 본 찾기 실패 !", E_FAIL);
            //MSG_FAIL(Ready_DrapeRule(), L"망토 데이터화 실패 ", L"망토 데이터화 실패 !", E_FAIL);
            //MSG_FAIL(Ready_DrapeBuffer(), L"망토 버퍼 못찾음", L"망토 버퍼 생성 실패 !", E_FAIL);

        }

        m_bComputeBufferReady = true;

        CHECK_FAILED(Update_OffsetToGPU(), E_FAIL); //Offset버퍼 GPU에 전달
    }



    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// FBX파일 로드 및 Export ////////////////////////////////////////////////////////
HRESULT Engine::Model::Initialize_FBX(const _wstring& _filepath)
{
    myModel mymodel = {};

    _uint flag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

    if (m_eModelType == MODEL::NONANIM)
    {
        if(!m_bNoneAnimFlag)
        flag |= aiProcess_PreTransformVertices;
    }
#ifdef _DEBUG
    flag |= aiProcess_ValidateDataStructure;
#endif
    _char filepath[MAX_PATH] = {};
    WideCharToMultiByte(CP_ACP, 0, _filepath.c_str(), (_int)_filepath.size(), filepath, MAX_PATH, NULL, NULL);

    _char szExt[MAX_PATH];
    _splitpath_s(filepath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

    if (!strcmp(szExt, ".fbx"))
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ALL_GEOMETRY_LAYERS, true);

    if (!strcmp(szExt, ".gltf"))
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ALL_GEOMETRY_LAYERS, true);

    m_pAIScene = m_Importer.ReadFile(filepath, flag);
    if (m_pAIScene == nullptr)
    {
        COUT(m_Importer.GetErrorString());

        MSG_ON(L"aiScene 생성에 실패했습니다.", L"오류!!!");
         BREAK;
        return E_FAIL;
    }

    mymodel.mRootNode = new myNode;
    CHECK_FAILED(Ready_Bones_FBX(m_pAIScene->mRootNode, -1, mymodel.mRootNode), E_FAIL);

    Initialize_BoneIndexByName();

    CHECK_FAILED(Ready_Meshes_FBX(mymodel), E_FAIL);

    CHECK_FAILED(Ready_Materials_FBX(_filepath, mymodel), E_FAIL);

    if (m_pAIScene->mNumAnimations != 0)
    {
        CHECK_FAILED(Ready_Animations_FBX(mymodel), E_FAIL);
    }
    else
    {
        _tchar drive[_MAX_DRIVE] = {};
        _tchar dir[_MAX_DIR] = {};
        _tchar fname[_MAX_FNAME] = {};
        _tchar ext[_MAX_EXT] = {};

        _wsplitpath_s(_filepath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

        wstring animFolderPath = wstring(drive) + dir;  // 예: .../Special_Week/Animation

        CHECK_FAILED(Add_Animations_And_Export_Binary(animFolderPath), E_FAIL);
    }

    //for (_uint i = 0; i < m_iNumMeshes; ++i)
    //{
    //    COUT(m_pAIScene->mMeshes[i]->mNumAnimMeshes);
    //    COUT(m_pAIScene->mMeshes[i]->mName.data);
    //    for (_uint j = 0; j < m_pAIScene->mMeshes[i]->mNumAnimMeshes; ++j)
    //    {
    //        COUT(m_pAIScene->mMeshes[i]->mAnimMeshes[j]->mName.data);
    //    }~
    //    COUT("");
    //    int a = 0;
    //}

    _char filedrive[MAX_PATH];
    _char filePath[MAX_PATH];
    _char filename[MAX_PATH];
    _splitpath_s(filepath, filedrive, MAX_PATH, filePath, MAX_PATH, filename, MAX_PATH, nullptr, 0);

    mymodel.mNumAllBones = m_iNumBones = (_uint)m_vecBones.size();

    _char savefilename[MAX_PATH];
    strcpy_s(savefilename, MAX_PATH, filedrive);
    strcat_s(savefilename, MAX_PATH, filePath);
    strcat_s(savefilename, MAX_PATH, filename);
    strcat_s(savefilename, MAX_PATH, ".siho");


    // 데이터 파일 저장 시작
    std::fstream file(savefilename, std::ios::binary | std::ios::out);

    if (!file.is_open()) {
        std::cerr << "파일 열기 실패: " << savefilename << std::endl;
        // 에러 이유 확인: perror() 사용
        perror("Error details");
        return E_FAIL;
    }

    // 파일 검증용 매직
    const _char magic[4] = { 'S', 'I', 'H', 'O' };
    file.write(magic, 4);

    // 모델 정보
    mymodel.Serialize(file);

    // 본(노드) 정보
    mymodel.mRootNode->Serialize(file);

    // 메쉬 정보
    for (_uint i = 0; i < m_iNumMeshes; ++i)
    {
        mymodel.mMeshes[i].Serialize(file);
    }

    // 마테리얼 정보
    for (_uint i = 0; i < m_iNumMaterials; ++i)
    {
        mymodel.mMaterials[i].Serialize(file);
    }

    // 애니메이션 정보
    for (_uint i = 0; i < mymodel.mAnimations.size(); ++i)
    {
        mymodel.mAnimations[i].Serialize(file);
    }

    file.close();

    COUT(filename << " 모델 FBX 프로토타입 생성 및 추출 완료.");

    //for (_uint i = 0; i <= m_iRootBoneIndex; ++i)
    //    COUT("Bone[" << i << "] = " << m_vecBones[i]->Get_BoneName());

    return S_OK;
}
/******************************************************* FBX파일 로드 및 Export *******************************************************/



//////////////////////////////////////////////////////// FBX 하위 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Model::Ready_Bones_FBX(const aiNode* _aiNode, _int _parentIndex, myNode* _mynode)
{
    Bone* bone = Bone::Create(m_pDevice, m_pContext, _aiNode, _parentIndex, _mynode, m_vecFirstBoneInfo);
    CHECK_NULLPTR(bone);

    m_vecBones.push_back(bone);

    if (strstr(bone->Get_BoneName().c_str(), "CHARA_OFFSET"))
        m_iRootBoneIndex = 2;

    //if (strstr(bone->Get_BoneName().c_str(), "RootPart2_M"))
    //    m_iRootBoneIndex = 35;


    _int iParentIndex = (_int)m_vecBones.size() - 1;
    _mynode->mNumChildren = _aiNode->mNumChildren;

    for (_uint i = 0; i < _aiNode->mNumChildren; ++i)
    {
        myNode* childnode = new myNode;
        Ready_Bones_FBX(_aiNode->mChildren[i], iParentIndex, childnode);
        _mynode->mChildren.push_back(childnode);
    }



    return S_OK;
}

HRESULT Engine::Model::Ready_Meshes_FBX(myModel& _mymodel)
{
    _mymodel.mNumMeshes = m_iNumMeshes = m_pAIScene->mNumMeshes;

    m_vecMeshes.reserve(m_iNumMeshes);
    _mymodel.mMeshes.resize(m_iNumMeshes);

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        Mesh* mesh = Mesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, XMLoadFloat4x4(&m_PreTransformMatrix), &_mymodel.mMeshes[i]);
        CHECK_NULLPTR(mesh);

        mesh->Calculate_LocalAABB();
        m_vecMeshes.push_back(mesh);
    }


    return S_OK;
}

HRESULT Engine::Model::Ready_Materials_FBX(const _wstring& _filepath, myModel& _mymodel)
{
    m_iNumMaterials = _mymodel.mNumMaterials = m_pAIScene->mNumMaterials;

    m_vecMaterials.reserve(m_iNumMaterials);
    _mymodel.mMaterials.resize(m_iNumMaterials);

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        if (_mymodel.mMeshes.size() <= i)
            continue;
        else if (true)//g_toolType == TOOLTYPE::SHADER_TOOL
            cout << _mymodel.mMeshes[i].mName << endl;

        Material* material = Material::Create(m_pDevice, m_pContext, m_pAIScene->mMaterials[i], _filepath, &_mymodel.mMaterials[i]);
        CHECK_NULLPTR(material);

        m_vecMaterials.push_back(material);
    }


    return S_OK;
}

HRESULT Engine::Model::Ready_Animations_FBX(myModel& _mymodel)
{
    m_iNumAnimations = _mymodel.mNumAnimations = m_pAIScene->mNumAnimations;

    m_vecAnimations.reserve(m_iNumAnimations);
    _mymodel.mAnimations.resize(m_iNumAnimations);

    for (_uint i = 0; i < m_iNumAnimations; ++i)
    {
        Animation* animation = Animation::Create(m_pDevice, m_pContext, m_pAIScene->mAnimations[i], this, &_mymodel.mAnimations[i]);
        CHECK_NULLPTR(animation);

        m_vecAnimations.push_back(animation);
    }


    return S_OK;
}

HRESULT Model::Add_Animations_And_Export_Binary(const _wstring& _animFolderPath)
{
    // 재귀함수로 모든 하위 폴더에서 파일 수집
    vector<AnimFileInfo> animFiles;
    Collect_AnimFiles_Recursive(_animFolderPath + L"Animation/", animFiles);

    // 수집했는데 없으면 바로 컷컷컷
    if (animFiles.empty())
        return S_OK;

    // baseName 기준 중복 제거 (sihoani 우선, 같은 이름의 fbx 제거)
    {
        // baseName -> 인덱스 맵 (sihoani 우선)
        unordered_map<_wstring, size_t> baseNameMap;

        // for문 돌면서 혹시 있을지도 모르는 중복파일 걸러내기(fbx vs sihoani 있으면 sihoani로)
        for (size_t i = 0; i < animFiles.size(); ++i)
        {
            auto it = baseNameMap.find(animFiles[i].baseName);
            if (it == baseNameMap.end())
            {
                baseNameMap[animFiles[i].baseName] = i;
            }
            else
            {
                // 이미 있으면 sihoani를 우선
                _bool currentIsSihoani = (_wcsicmp(animFiles[i].ext.c_str(), L".sihoani") == 0);
                _bool existingIsSihoani = (_wcsicmp(animFiles[it->second].ext.c_str(), L".sihoani") == 0);

                if (currentIsSihoani && !existingIsSihoani)
                    it->second = i; // sihoani로 교체
            }
        }

        // 다 걸러져서 하나씩만 남은거 벡터에 담기
        vector<AnimFileInfo> uniqueFiles;
        uniqueFiles.reserve(baseNameMap.size());
        for (auto& pair : baseNameMap)
            uniqueFiles.push_back(animFiles[pair.second]);

        animFiles = std::move(uniqueFiles);
    }

    // baseName 기준 오름차순 정렬 (00_Idle, 01_Run, ...)
    std::sort(animFiles.begin(), animFiles.end(),
        [](const AnimFileInfo& _a, const AnimFileInfo& _b)
        {
            return _a.baseName < _b.baseName;
        });

    _uint totalAdded = 0;

    for (const auto& animFile : animFiles)
    {
        _wstring sihoaniPath = animFile.directory + animFile.baseName + L".sihoani";

        const aiScene* pAnimScene = nullptr;
        Assimp::Importer animImporter;

        // .sihoani가 있으면 바로 바이너리 로드
        if (_wcsicmp(animFile.ext.c_str(), L".sihoani") == 0 ||
            GetFileAttributes(sihoaniPath.c_str()) != INVALID_FILE_ATTRIBUTES)
        {
            _wstring loadPath = (_wcsicmp(animFile.ext.c_str(), L".sihoani") == 0)
                ? animFile.fullPath : sihoaniPath;

            // 바이너리 로드
            std::fstream file(loadPath, std::ios::binary | std::ios::in);
            if (!file.is_open()) continue;

            char magic[4]{};
            file.read(magic, 4);
            if (memcmp(magic, "SIHO", 4) != 0) { file.close(); continue; }

            MYMODEL header{};
            header.Deserialize(file);

            for (_uint i = 0; i < header.mNumAnimations; ++i)
            {
                myAnimation myanim{};
                myanim.Deserialize(file);

                Animation* pNewAnim = Animation::Create(m_pDevice, m_pContext, &myanim);
                if (pNewAnim)
                {
                    m_vecAnimations.push_back(pNewAnim);
                    totalAdded++;
                }
            }
            file.close();
            continue;   // .sihoani 로드했으면 다음 파일로
        }

        // .fbx만 있고 .sihoani가 없으면 FBX 로드 -> .sihoani로 변환 후 저장
        if (_wcsicmp(animFile.ext.c_str(), L".fbx") == 0)
        {
            pAnimScene = animImporter.ReadFile(wstringToString(animFile.fullPath).c_str(),
                aiProcess_ConvertToLeftHanded | aiProcess_LimitBoneWeights);

            if (!pAnimScene || !pAnimScene->HasAnimations())
                continue;

            // .sihoani 저장 시작
            std::fstream sihoaniFile(sihoaniPath, std::ios::binary | std::ios::out);
            if (!sihoaniFile.is_open())
            {
                COUT("sihoani 파일 생성 실패: " << wstringToString(sihoaniPath));
                continue;
            }

            const char magic[4] = { 'S', 'I', 'H', 'O' };
            sihoaniFile.write(magic, 4);

            MYMODEL header{};
            header.mNumAnimations = pAnimScene->mNumAnimations;
            header.Serialize(sihoaniFile);

            for (_uint i = 0; i < pAnimScene->mNumAnimations; ++i)
            {
                myAnimation myanim{};
                // 기존 Create와 똑같은 로직으로 myanim 채우기
                Animation* pNewAnim = Animation::Create(m_pDevice, m_pContext, pAnimScene->mAnimations[i], this, &myanim);

                if (g_toolType == TOOLTYPE::SHADER_TOOL)
                    COUT(pAnimScene->mAnimations[i]->mNumMorphMeshChannels);

                if (pNewAnim == nullptr)
                    continue;

                myanim.Serialize(sihoaniFile);

                // 메모리에도 바로 추가
                if (pNewAnim)
                {
                    m_vecAnimations.push_back(pNewAnim);
                    totalAdded++;
                    COUT("FBX -> SIHOANI 변환 + 추가 완료 -> " << pAnimScene->mAnimations[i]->mName.data
                        << " (" << wstringToString(animFile.baseName) << ")");
                }
            }
            sihoaniFile.close();
        }
    }

    m_iNumAnimations = (_uint)m_vecAnimations.size();

    return S_OK;
}/******************************************************* FBX 하위 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 바이너리 파일 로드 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Model::Initialize_Binary(const _wstring& _filepath)
{
    myModel mymodel = {};
    _char filepath[MAX_PATH];

    WideCharToMultiByte(CP_UTF8, 0, _filepath.c_str(), -1, filepath, MAX_PATH, nullptr, nullptr);
    // 데이터 파일 로드 시작
    fstream file(filepath, std::ios::binary | std::ios::in);

    if (!file.is_open())
    {
        MSG_ON((_filepath + L"\n파일을 찾을 수 없습니다.").c_str(), L"로드 실패");
        BREAK;
        return E_FAIL;
    }

    _char filename[MAX_PATH];
    _splitpath_s(filepath, nullptr, 0, nullptr, 0, filename, MAX_PATH, nullptr, 0);

    // 파일 검증용 매직
    _char magic[4]{};
    file.read(magic, 4);
    if (magic[0] != 'S' || magic[1] != 'I' || magic[2] != 'H' || magic[3] != 'O')
    {
        //COUT("헤더 맞추기 귀찮네 왜 안 맞는거죠?");
        MSG_ON(L"파일의 헤더가 맞지 않습니다.", L"로드 실패");
        BREAK;
        return E_FAIL;
    }

    // 모델 정보
    mymodel.Deserialize(file);

    //노드 정보
    mymodel.mRootNode = new myNode;
    mymodel.mRootNode->Deserialize(file);

    // 메쉬 정보
    mymodel.mMeshes.resize(mymodel.mNumMeshes);
    for (_uint i = 0; i < mymodel.mNumMeshes; ++i)
    {
        mymodel.mMeshes[i].Deserialize(file);
    }

    // 마테리얼 정보
    mymodel.mMaterials.resize(mymodel.mNumMaterials);
    for (_uint i = 0; i < mymodel.mNumMaterials; ++i)
    {
        mymodel.mMaterials[i].Deserialize(file);
    }

    // 마테리얼 정보
    mymodel.mAnimations.resize(mymodel.mNumAnimations);
    for (_uint i = 0; i < mymodel.mNumAnimations; ++i)
    {
        mymodel.mAnimations[i].Deserialize(file);
    }

    CHECK_FAILED(Ready_Bones_Binary(mymodel.mRootNode), E_FAIL);

    Initialize_BoneIndexByName();

    CHECK_FAILED(Ready_Meshes_Binary(&mymodel), E_FAIL);

    CHECK_FAILED(Ready_Materials_Binary(&mymodel, _filepath), E_FAIL);

    if (mymodel.mNumAnimations != 0)
    {
        CHECK_FAILED(Ready_Animations_Binary(&mymodel), E_FAIL);
    }
    else
    {
        _tchar drive[_MAX_DRIVE] = {};
        _tchar dir[_MAX_DIR] = {};
        _tchar fname[_MAX_FNAME] = {};
        _tchar ext[_MAX_EXT] = {};

        _wsplitpath_s(_filepath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

        wstring animFolderPath = wstring(drive) + dir;  // 예: .../Special_Week/Animation

        CHECK_FAILED(Add_Animations_And_Export_Binary(animFolderPath), E_FAIL);
    }

    m_iNumBones = mymodel.mNumAllBones;

    mymodel.mRootNode->Delete_Children();
    for (auto& mesh : mymodel.mMeshes)
        mesh.Delete_Bones();

    file.close();

    COUT(filename << " 모델 Binary 프로토타입 생성 완료.");

    return S_OK;
}

HRESULT Engine::Model::Ready_Bones_Binary(myNode* _mynode)
{
    Bone* bone = Bone::Create(m_pDevice, m_pContext, _mynode, m_vecFirstBoneInfo);
    CHECK_NULLPTR(bone);

    m_vecBones.push_back(bone);

    if (strstr(bone->Get_BoneName().c_str(), "CHARA_OFFSET"))
        m_iRootBoneIndex = 2;

    //if (strstr(bone->Get_BoneName().c_str(), "RootPart2_M"))
    //    m_iRootBoneIndex = 35;

    for (_uint i = 0; i < _mynode->mNumChildren; ++i)
    {
        Ready_Bones_Binary(_mynode->mChildren[i]);
    }

    return S_OK;
}

HRESULT Engine::Model::Ready_Meshes_Binary(myModel* _mymodel)
{
    m_iNumMeshes = _mymodel->mNumMeshes;

    m_vecMeshes.reserve(m_iNumMeshes);

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        Mesh* mesh = Mesh::Create(m_pDevice, m_pContext, m_eModelType, this, &_mymodel->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix));
        CHECK_NULLPTR(mesh);

        mesh->Calculate_LocalAABB();
        m_vecMeshes.push_back(mesh);
    }

    return S_OK;
}

HRESULT Engine::Model::Ready_Materials_Binary(myModel* _mymodel, const _wstring& _modelFilePath)
{
    m_iNumMaterials = _mymodel->mNumMaterials;

    m_vecMaterials.reserve(m_iNumMaterials);

    for (size_t i = 0; i < m_iNumMaterials; i++)
    {
        if (_mymodel->mMeshes.size() <= i)
            continue;
        else if (g_toolType != TOOLTYPE::TOOL_END)//g_toolType == TOOLTYPE::SHADER_TOOL
            COUT(_mymodel->mMeshes[i].mName);
            
        Material* material = Material::Create(m_pDevice, m_pContext, &_mymodel->mMaterials[i], _modelFilePath);
        CHECK_NULLPTR(material);

        m_vecMaterials.push_back(material);
    }


    return S_OK;
}

HRESULT Engine::Model::Ready_Animations_Binary(myModel* _mymodel)
{
    m_iNumAnimations = _mymodel->mNumAnimations;

    m_vecAnimations.reserve(m_iNumAnimations);

    for (_uint i = 0; i < m_iNumAnimations; ++i)
    {
        Animation* animation = Animation::Create(m_pDevice, m_pContext, &_mymodel->mAnimations[i]);
        CHECK_NULLPTR(animation);

        m_vecAnimations.push_back(animation);
    }


    return S_OK;
}
/******************************************************* 바이너리 파일 로드 함수 *******************************************************/



//////////////////////////////////////////////////////// 애니메이션 로드 헬퍼 함수 ////////////////////////////////////////////////////////
void Model::Collect_AnimFiles_Recursive(const _wstring& _folderPath, vector<AnimFileInfo>& _outFiles)
{
    // _folderPath로 들어온 폴더와 하위 폴더 전부 탐색
    _wstring searchPattern = _folderPath + L"*.*";
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(searchPattern.c_str(), &fd); // 첫 번째 파일 찾기

    // 아무 파일도 없으면 컷
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do
    {
        // 파일 상대경로(애니메이션 경로)
        _wstring fileName = fd.cFileName;

        // . 과 .. 무시
        if (fileName == L"." || fileName == L"..")
            continue;

        _wstring fullPath = _folderPath + fileName;

        // 탐색 대상이 하위 폴더라면(Animation폴더의 하위폴더 또는 더 하위 폴더라면) 재귀 탐색 한 번 더
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            Collect_AnimFiles_Recursive(fullPath + L"/", _outFiles);
            continue;
        }

        // 확장자 추출
        size_t dotPos = fileName.find_last_of(L'.');
        if (dotPos == _wstring::npos)
            continue;

        _wstring ext = fileName.substr(dotPos);
        _wstring baseName = fileName.substr(0, dotPos);

        // fbx 또는 sihoani만 수집
        if (_wcsicmp(ext.c_str(), L".fbx") != 0 && _wcsicmp(ext.c_str(), L".sihoani") != 0)
            continue;

        // 수집해서 일단 담아놓기
        AnimFileInfo info;
        info.baseName = baseName;
        info.fullPath = fullPath;
        info.ext = ext;
        info.directory = _folderPath;

        _outFiles.push_back(info);

    } while (FindNextFile(hFind, &fd));

    FindClose(hFind);
}
/******************************************************* 애니메이션 로드 헬퍼 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Model::Bind_Material(Shader* _shader, const _string& _constantname, _uint _meshindex, aiTextureType _texturetype, _uint _textureindex, _uint* bitFlag)
{
    if (_meshindex >= m_iNumMeshes)
    {
        MSG_ON((L"대입한 메쉬 번호\"" + to_wstring(_meshindex) + L"\"번이\n가지고 있는 메쉬 개수를 초과합니다.").c_str(), L"마테리얼 바인딩 실패");
        BREAK;
        return E_FAIL;
    }

    _uint materialindex = m_vecMeshes[_meshindex]->Get_MaterialIndex();

    if (materialindex >= m_iNumMaterials)
    {
        MSG_ON((L"메쉬에 등록된 마테리얼 번호\"" + to_wstring(_meshindex) + L"\"번이\n가지고 있는 마테리얼 개수를 초과합니다.").c_str(), L"마테리얼 바인딩 실패");
        BREAK;
        return E_FAIL;
    }

    return m_vecMaterials[materialindex]->Bind_ShaderResources(_shader, _constantname, _texturetype, _textureindex, bitFlag);
}

HRESULT Engine::Model::Bind_Material_FullSlot(Shader* _shader, _uint _slotNum, _uint _meshIndex, aiTextureType _textureType, _uint _textureIndex, _uint* bitFlag)
{
    if (_meshIndex >= m_iNumMeshes)
    {
        MSG_ON((L"대입한 메쉬 번호\"" + to_wstring(_meshIndex) + L"\"번이\n가지고 있는 메쉬 개수를 초과합니다.").c_str(), L"마테리얼 바인딩 실패");
        BREAK;
        return E_FAIL;
    }

    _uint materialindex = m_vecMeshes[_meshIndex]->Get_MaterialIndex();

    if (materialindex >= m_iNumMaterials)
    {
        MSG_ON((L"메쉬에 등록된 마테리얼 번호\"" + to_wstring(_meshIndex) + L"\"번이\n가지고 있는 마테리얼 개수를 초과합니다.").c_str(), L"마테리얼 바인딩 실패");
        BREAK;
        return E_FAIL;
    }

    return m_vecMaterials[materialindex]->Bind_ShaderResources_FullSlot(_shader, _slotNum, _textureType, _textureIndex, bitFlag);
}

HRESULT Engine::Model::Bind_Bones(Shader* _shader, const _string& _constantname, _uint _meshIndex)
{
    MSG_OUTOFINDEX(_meshIndex >= m_iNumMeshes, (L"대입한 메쉬 번호\"" + to_wstring(_meshIndex) + L"\"번이\n가지고 있는 메쉬 개수를 초과합니다.").c_str(), L"본 바인딩 실패", E_FAIL);

    return m_vecMeshes[_meshIndex]->Bind_Bones(_shader, _constantname, m_vecBones);
}

HRESULT Engine::Model::Bind_BonesMasterRig(Shader* _shader, const _string& _constantname, _uint _meshIndex, const vector<class Bone*>& _MasterRigBones, const vector<_uint>& _RemapBone)
{
    MSG_OUTOFINDEX(_meshIndex >= m_iNumMeshes, (L"대입한 메쉬 번호\"" + to_wstring(_meshIndex) + L"\"번이\n가지고 있는 메쉬 개수를 초과합니다.").c_str(), L"본 바인딩 실패", E_FAIL);

    return m_vecMeshes[_meshIndex]->Bind_BonesMasterRig(_shader, _constantname, _MasterRigBones, _RemapBone, m_vecBones);
}

HRESULT Engine::Model::Bind_Morphs(Shader* _shader, _uint _meshIndex)
{
    return m_vecMeshes[_meshIndex]->Bind_Morphs(_shader, m_vecUnifiedMorphNames, m_vecUnifiedMorphWeights);
}

HRESULT Engine::Model::Update_MorphWeight(const _string& _morphName, _float _weight, _uint _meshIndex)
{
    return S_OK;
}
HRESULT Engine::Model::Bind_CombiendAndOffsetSRV(Shader* _shader)
{
    // _shader->Bind_SRV_FullSlot(10, , stage_VS); 기존꺼 수정
    //만약에 플레이어가 shader 해당 셰이더에 10번에 내가 가지고있는 CombiendSRV를 바인딩해라였지 
    m_pCombinedMatrixBuffer->Bind_SRV(20, stage_VS);
    _shader->Bind_SRV_FullSlot(21, m_pOffsetSRV, stage_VS);

    return S_OK;
}
HRESULT Engine::Model::Bind_ReampSRV(Shader* pShader, _uint _meshIndex)
{
    return m_vecMeshes[_meshIndex]->Bind_RemapSRV(pShader);
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 애니메이션 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Model::Play_Animation(const _float fTimeDelta, const _bool _forceLoop)
{
    if (m_eModelType == MODEL::NONANIM) {}
    else if (!m_vecAnimations.empty())
    {
        if (m_bIsLerping)
        {
            LerpAnimation(fTimeDelta);
        }
        if (!m_bIsLerping)
        {
            if (_forceLoop)
            {
                /* m_iCurrentAnimIndex에 해당하는 애니메이션 중, 현재 재생 시간에 맞는 상태행렬(TransformationMatrix)을 실제 뼈에게 전달해 준다.  */
                m_bIsAnimationFinished = m_vecAnimations[m_iCurrentAnimationIndex]->Update_TransformationMatrices(m_vecBones, fTimeDelta, _forceLoop, m_fAnimationSpeed);
            }
            else
            {
                m_bIsAnimationFinished = m_vecAnimations[m_iCurrentAnimationIndex]->Update_TransformationMatrices_SelfLoop(m_vecBones, fTimeDelta);
            }
        }
    }
    else
    {
        //COUT("Animation 벡터가 비어있습니다!");
    }

    /* 위에서 갱신해준 뼈들의 TrnasformationMatrix를 기반으로하여 실제 뼈의 상태(CombinedTransformationMatrix)행렬을 만들어준다. */
    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        m_vecBones[i]->Update_CombinedTransformationMatrix(m_vecBones, XMLoadFloat4x4(&m_PreTransformMatrix));
    }

    if (m_bIsAnimationFinished && m_iNextAnimationIndex != -1)
    {
        Set_Animation(m_iNextAnimationIndex, m_fNextLerpDuration);
        m_iNextAnimationIndex = -1;
        m_fNextLerpDuration = 0.05f;
    }

    return S_OK;
}

void Engine::Model::Update_CombinedMatrices_Weapon(_matrix _socketMatrix)
{
    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        m_vecBones[i]->Update_CombinedTransformationMatrix(m_vecBones, _socketMatrix);
    }
}

void Engine::Model::Play_Animation_CS(const _float fTimeDelta)
{
    m_fPrevFrame = m_vecAnimations[m_iCurrentAnimationIndex]->Get_CurrentFrame();

    if (m_bUpperBlendEnable) //상체용 
        m_fUpperPrevFrame = m_vecAnimations[m_iUpperCurrentAnimationIndex]->Get_CurrentFrame();

    if (m_bIsLerping) //보간중일때
    {
        if (m_fLerpDuration <= 0.f) {
            m_tAnimLocalCompute.g_LfLerpRatio = 1.f;
            m_bIsLerping = false;
        }
        else {
            m_fLerpTimer += fTimeDelta;
            m_tAnimLocalCompute.g_LfLerpRatio = min(1.f, m_fLerpTimer / m_fLerpDuration);
            if (m_tAnimLocalCompute.g_LfLerpRatio >= 1.f) m_bIsLerping = false;
        }
    }

    if (m_bUpperBlendEnable) //상체용 
    {
        if (m_bUpperIsLerping) //보간중일때
        {
            if (m_fUpperLerpDuration <= 0.f) {
                m_tAnimLocalCompute.g_LfUpperLerpRatio = 1.f;
                m_bUpperIsLerping = false;
            }
            else {
                m_fUpperLerpTimer += fTimeDelta;
                m_tAnimLocalCompute.g_LfUpperLerpRatio = min(1.f, m_fUpperLerpTimer / m_fUpperLerpDuration);
                if (m_tAnimLocalCompute.g_LfUpperLerpRatio >= 1.f) m_bUpperIsLerping = false;
            }
        }
    }

    _bool bSameAnim = (m_bUpperBlendEnable && m_iCurrentAnimationIndex == m_iUpperCurrentAnimationIndex); //상체블렌드 + 전신/상체 같은경우


    m_bIsAnimationFinished = m_vecAnimations[m_iCurrentAnimationIndex]->Update_Animation_CS(fTimeDelta, m_bIsLoopAnimation, m_fAnimationSpeed);

    if (m_bUpperBlendEnable && !bSameAnim)//상체용 
    m_bUpperIsAnimationFinished = m_vecAnimations[m_iUpperCurrentAnimationIndex]->Update_Animation_CS(fTimeDelta, m_bUpperIsLoopAnimation, m_fUpperAnimationSpeed);
    else if (bSameAnim)
    {
        m_bUpperIsAnimationFinished = m_bIsAnimationFinished;
    }



    m_fCurrentFrame = m_vecAnimations[m_iCurrentAnimationIndex]->Get_CurrentFrame();

    if (m_bUpperBlendEnable)//상체용 
        m_fUpperCurrentFrame = m_vecAnimations[m_iUpperCurrentAnimationIndex]->Get_CurrentFrame();

    Update_RootMotion_CS(m_fPrevFrame, m_fCurrentFrame);

 
    if (m_bComputeBufferReady && m_bUseComputeSKinning)
    {
        Update_DispatchLocalPose(); //로컬 행렬 업로드
        Update_DispatchCombinedByDepth(); //ComBined 생성
#ifdef _DEBUG
        //Update_DispatchReadback(); //CPU로 읽어오기
#endif // _DEBUG
        //  Update_DispatchDrivenBone(); //Combined 전에 타겟뼈SRT에 회전 가산적용
      /*  if (m_bUseDrapeDelta)
            Update_DrapeDelta();*/
    }

    m_vecAnimations[m_iCurrentAnimationIndex]->Evaluate_Notify(m_fPrevFrame, m_fCurrentFrame, m_bIsLoopAnimation, m_iOwnerId, m_iCurrentAnimationIndex);

    //    if (m_bUpperBlendEnable && !bSameAnim)//상체용 
    if (m_bUpperBlendEnable && !bSameAnim)//상체용 
        m_vecAnimations[m_iUpperCurrentAnimationIndex]->Evaluate_Notify(m_fUpperPrevFrame, m_fUpperCurrentFrame, m_bUpperIsLoopAnimation, m_iOwnerId, m_iUpperCurrentAnimationIndex);
}

void Engine::Model::Play_Animation_New(const _float fTimeDelta)
{
    m_fPrevFrame = m_vecAnimations[m_iCurrentAnimationIndex]->Get_CurrentFrame();

    if (m_bIsLerping) //보가눚ㅇ이라면
    {
        m_fLerpTimer += fTimeDelta;
        _float fLerpRatio = min(1.f, m_fLerpTimer / m_fLerpDuration);

        m_bIsAnimationFinished = m_vecAnimations[m_iCurrentAnimationIndex]->Update_TransformationMatrices(m_vecBones, fTimeDelta, m_bIsLoopAnimation, m_fAnimationSpeed);

        _float4x4 TransformationMatrix = {};

        _vector		vScale{}, vRotation{}, vTranslation{};
        _vector     vLeftScale{}, vRightScale{};
        _vector     vLeftRotation{}, vRightRotation{};
        _vector     vLeftTranslation{}, vRightTranslation{};

        for (_uint i = 0; i < m_vecBones.size(); i++)
        {
            _float4x4 CurrentTransformationMatrix = m_vecBones[i]->Get_TransformationMatrix();
            XMMatrixDecompose(&vLeftScale, &vLeftRotation, &vLeftTranslation, XMLoadFloat4x4(&m_vecPrevBoneTransforms[i]));
            XMMatrixDecompose(&vRightScale, &vRightRotation, &vRightTranslation, XMLoadFloat4x4(&CurrentTransformationMatrix));

            vScale = XMVectorLerp(vLeftScale, vRightScale, fLerpRatio);
            vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fLerpRatio);
            vTranslation = XMVectorLerp(vLeftTranslation, vRightTranslation, fLerpRatio);

            XMStoreFloat4x4(&TransformationMatrix,
                XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));

            m_vecBones[i]->Set_TransformationMatrix(TransformationMatrix);
        }

        if (fLerpRatio >= 1.f)
            m_bIsLerping = false;
    }
    else // 보간 중이지 않다면 원래 하던데로 애니메이션 재생
    {
        // m_iCurrentAnimIndex에 해당하는 애니메이션 중, 현재 재생 시간에 맞는 상태행렬(TransformationMatrix)을 실제 뼈에게 전달해준다.
        m_bIsAnimationFinished = m_vecAnimations[m_iCurrentAnimationIndex]->Update_TransformationMatrices(m_vecBones, fTimeDelta, m_bIsLoopAnimation, m_fAnimationSpeed);

    }

    m_fCurrentFrame = m_vecAnimations[m_iCurrentAnimationIndex]->Get_CurrentFrame();

    // RootNode는 움직임이 없다 (뼈 최상위 노드)
    if (m_iRootBoneIndex != -1) //RootNode 아닌 애들
    {
        _vector vCurRootPosition{}, vScale{}, vRotation{};
        _float4x4 TransformationMatrix = m_vecBones[m_iRootBoneIndex]->Get_TransformationMatrix();

        XMMatrixDecompose(&vScale, &vRotation, &vCurRootPosition, XMLoadFloat4x4(&TransformationMatrix));

        _float fCurrentTrackPosition = m_vecAnimations[m_iCurrentAnimationIndex]->Get_CurrentFrame();

        // 현재 애니메이션의 재생 위치가 0으로 바뀌면 한 사이클이 끝났다는 의미
        if (fCurrentTrackPosition < m_fPrevTrackPosition)
        {
            // 이전 프레임의 로컬 위치를 누적 위치에 저장
            // ex) 루트 본이 로컬에서 0 ~ 10까지 간다 했을 때 10을 저장
            m_vAccumulatedMotionDelta = m_vPrevRootPosition;
            m_vAccumulatedMotionDelta = XMVectorSetW(m_vAccumulatedMotionDelta, 0.f);
        }

        // 한 사이클이 끝났으니 애니메이션의 이전 재생 위치를 갱신해준다.
        m_fPrevTrackPosition = fCurrentTrackPosition;

        // 루트 본의 월드 위치 계산 = 루트 본의 로컬 위치 + 루트 본의 누적 이동량
        _vector vWorldTranslation = XMVectorSetW(vCurRootPosition + m_vAccumulatedMotionDelta, 1.f);

        // Delta = 현재 실제 위치 - 이전 실제 위치
        // GameObject에 던져줘서 더할 것 이라서 w = 0으로 설정
        m_vRootMotionDelta = vWorldTranslation - m_vPrevRootPosition;
        m_vRootMotionDelta = XMVectorSetW(m_vRootMotionDelta, 0.f);

        // 루프 애니메이션 아니면 델타 없애기
        if (!m_bIsLerping && m_bIsAnimationFinished)
            m_vRootMotionDelta = XMVectorZero();

        // 이전 위치 업데이트
        m_vPrevRootPosition = vWorldTranslation;

        // 루트 본의 Translation을 제거한다.
        XMStoreFloat4x4(&TransformationMatrix,
            XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorZero()));
        m_vecBones[m_iRootBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
    }

    if (m_bComputeBufferReady && m_bUseComputeSKinning)
    {
        Update_LocalMatricesToGPU(); //로컬 행렬 업로드
        Update_DispatchCombinedByDepth(); //ComBined 생성
    }
    else //비교용
    {
        // 위에서 갱신해준 뼈들의 TransformationMatrix를 기반으로 실제 뼈의 상태행렬(CombinedTransformationMatrix)을 만들어준다.
        for (auto& pBone : m_vecBones)
        {
            // 각 뼈의 월드 변환을 계산한다.
            pBone->Update_CombinedTransformationMatrix(m_vecBones, XMLoadFloat4x4(&m_PreTransformMatrix));
        }
    }

    m_vecAnimations[m_iCurrentAnimationIndex]->Evaluate_Notify(m_fPrevFrame, m_fCurrentFrame, m_bIsLoopAnimation, m_iOwnerId, m_iCurrentAnimationIndex);

}

void Engine::Model::Play_Animation_AnimationTool(const _float fTimeDelta)
{
    if (m_bIsLerping) //보가눚ㅇ이라면
    {
        m_fLerpTimer += fTimeDelta;
        _float fLerpRatio = min(1.f, m_fLerpTimer / m_fLerpDuration);

        m_bIsAnimationFinished = m_vecAnimations[m_iCurrentAnimationIndex]->Update_TransformationMatrices_AnimationTool(m_vecBones, fTimeDelta, m_bIsLoopAnimation, m_fAnimationSpeed);

        _float4x4 TransformationMatrix = {};

        _vector		vScale{}, vRotation{}, vTranslation{};
        _vector     vLeftScale{}, vRightScale{};
        _vector     vLeftRotation{}, vRightRotation{};
        _vector     vLeftTranslation{}, vRightTranslation{};

        for (_uint i = 0; i < m_vecBones.size(); i++)
        {
            _float4x4 CurrentTransformationMatrix = m_vecBones[i]->Get_TransformationMatrix();
            XMMatrixDecompose(&vLeftScale, &vLeftRotation, &vLeftTranslation, XMLoadFloat4x4(&m_vecPrevBoneTransforms[i]));
            XMMatrixDecompose(&vRightScale, &vRightRotation, &vRightTranslation, XMLoadFloat4x4(&CurrentTransformationMatrix));

            vScale = XMVectorLerp(vLeftScale, vRightScale, fLerpRatio);
            vRotation = XMQuaternionSlerp(vLeftRotation, vRightRotation, fLerpRatio);
            vTranslation = XMVectorLerp(vLeftTranslation, vRightTranslation, fLerpRatio);

            XMStoreFloat4x4(&TransformationMatrix,
                XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vTranslation));

            m_vecBones[i]->Set_TransformationMatrix(TransformationMatrix);
        }

        if (fLerpRatio >= 1.f)
            m_bIsLerping = false;
    }
    else // 보간 중이지 않다면 원래 하던데로 애니메이션 재생
    {
        // m_iCurrentAnimIndex에 해당하는 애니메이션 중, 현재 재생 시간에 맞는 상태행렬(TransformationMatrix)을 실제 뼈에게 전달해준다.
        m_bIsAnimationFinished = m_vecAnimations[m_iCurrentAnimationIndex]->Update_TransformationMatrices_AnimationTool(m_vecBones, fTimeDelta, m_bIsLoopAnimation, m_fAnimationSpeed);
    }


    // RootNode는 움직임이 없다 (뼈 최상위 노드)
    if (m_iRootBoneIndex != -1) //RootNode 아닌 애들
    {
        _vector vCurRootPosition{}, vScale{}, vRotation{};
        _float4x4 TransformationMatrix = m_vecBones[m_iRootBoneIndex]->Get_TransformationMatrix();

        XMMatrixDecompose(&vScale, &vRotation, &vCurRootPosition, XMLoadFloat4x4(&TransformationMatrix));

        _float fCurrentTrackPosition = m_vecAnimations[m_iCurrentAnimationIndex]->Get_CurrentFrame();

        // 현재 애니메이션의 재생 위치가 0으로 바뀌면 한 사이클이 끝났다는 의미
        if (fCurrentTrackPosition < m_fPrevTrackPosition)
        {
            // 이전 프레임의 로컬 위치를 누적 위치에 저장
            // ex) 루트 본이 로컬에서 0 ~ 10까지 간다 했을 때 10을 저장
            m_vAccumulatedMotionDelta = m_vPrevRootPosition;
            m_vAccumulatedMotionDelta = XMVectorSetW(m_vAccumulatedMotionDelta, 0.f);
        }

        // 한 사이클이 끝났으니 애니메이션의 이전 재생 위치를 갱신해준다.
        m_fPrevTrackPosition = fCurrentTrackPosition;

        // 루트 본의 월드 위치 계산 = 루트 본의 로컬 위치 + 루트 본의 누적 이동량
        _vector vWorldTranslation = XMVectorSetW(vCurRootPosition + m_vAccumulatedMotionDelta, 1.f);

        // Delta = 현재 실제 위치 - 이전 실제 위치
        // GameObject에 던져줘서 더할 것 이라서 w = 0으로 설정
        m_vRootMotionDelta = vWorldTranslation - m_vPrevRootPosition;
        m_vRootMotionDelta = XMVectorSetW(m_vRootMotionDelta, 0.f);

        // 루프 애니메이션 아니면 델타 없애기
        if (!m_bIsLoopAnimation && m_bIsAnimationFinished)
            m_vRootMotionDelta = XMVectorZero();

        // 이전 위치 업데이트
        m_vPrevRootPosition = vWorldTranslation;

        // 루트 본의 Translation을 제거한다.
        XMStoreFloat4x4(&TransformationMatrix,
            XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, XMVectorZero()));
        m_vecBones[m_iRootBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
    }

    // 위에서 갱신해준 뼈들의 TransformationMatrix를 기반으로 실제 뼈의 상태행렬(CombinedTransformationMatrix)을 만들어준다.
    if (m_bComputeBufferReady && m_bUseComputeSKinning)
    {
        Update_LocalMatricesToGPU(); //로컬 행렬 업로드
        Update_DispatchCombinedByDepth(); //ComBined 생성
    }
    else //비교용
    {
        // 위에서 갱신해준 뼈들의 TransformationMatrix를 기반으로 실제 뼈의 상태행렬(CombinedTransformationMatrix)을 만들어준다.
        for (auto& pBone : m_vecBones)
        {
            // 각 뼈의 월드 변환을 계산한다.
            pBone->Update_CombinedTransformationMatrix(m_vecBones, XMLoadFloat4x4(&m_PreTransformMatrix));
        }
    }
}


// 애니메이션 변경
void Engine::Model::Set_Animation(_uint _animationIndex, _bool _isLoop)
{
    // 모델이 NONANIM타입이면 패스
    if (m_eModelType == MODEL::NONANIM)
        return;

    if (m_iNumAnimations <= _animationIndex)
    {
        COUT("모델이 소유한 애니메이션 개수보다 대입한 인덱스가 더 높습니다.");
        return;
    }

    m_iCurrentAnimationIndex = _animationIndex; // 애니메이션 인덱스 변경
    m_bIsLoopAnimation = _isLoop; // 루프 확인
    // 만약 애니메이션이 변경되었다면(애니메이션간 보간)
    if (m_iPrevAnimationIndex != m_iCurrentAnimationIndex)
    {
        m_vecAnimations[m_iCurrentAnimationIndex]->Start_Animation();
        m_bIsLerping = true;

        // 보간을 위해 이 전 상태의 모든 본 상태 보관(애니메이션이 적용되지 않은 본은 Identity)
        m_vecPrevBoneInfo.clear();
        m_vecPrevBoneInfo.resize(m_iNumBones);
        m_vecPrevBoneInfo = m_vecFirstBoneInfo;
        m_vecAnimations[m_iPrevAnimationIndex]->Push_CurrentMatrices(m_vecPrevBoneInfo);

        // 보간을 위해 다음 애니메이션의 첫 번째 키 프레임 모든 본 상태 보관(애니메이션이 적용되지 않은 본은 Identity)
        m_vecNextBoneInfo.clear();
        m_vecNextBoneInfo.resize(m_iNumBones);
        m_vecNextBoneInfo = m_vecFirstBoneInfo;
        m_vecAnimations[m_iCurrentAnimationIndex]->Push_FirstMatrices(m_vecNextBoneInfo);
    }
    // 이 전 애니메이션과 같다면 첫 프레임부터 다시 시작
    else if (m_iPrevAnimationIndex == m_iCurrentAnimationIndex)
    {
        m_vecAnimations[m_iCurrentAnimationIndex]->Start_Animation();
    }

    m_iPrevAnimationIndex = m_iCurrentAnimationIndex;
}

// 애니메이션 변경
void Engine::Model::Set_Animation_Siho(_uint _animationIndex, _bool _isLoop, _float fLerpDuration, _float fAnimationSpeed)
{
    // 모델이 NONANIM타입이면 패스
    if (m_eModelType == MODEL::NONANIM)
        return;

    if (m_iNumAnimations <= _animationIndex)
    {
        COUT("모델이 소유한 애니메이션 개수보다 대입한 인덱스가 더 높습니다.");
        return;
    }

    //인자로 들어온 애니메이션이 현재 애니메이션과 같고 돌고있으면 return 
    if (_animationIndex == m_iCurrentAnimationIndex && _isLoop)
        return;

    //return 안했으면 바뀌어야하므로
    m_iPrevAnimationIndex = m_iCurrentAnimationIndex; //이전 애니메이션인덱스는 이제 현재 인덱스로 바꿔주고
    m_iCurrentAnimationIndex = _animationIndex;
    m_bIsLoopAnimation = _isLoop; //얘가 이제 반복재생할것이냐고
    m_bIsLerping = true; //이게 이제 보간 시작
    m_fLerpTimer = 0.f;
    m_fLerpDuration = fLerpDuration; //인자로 빼야지 
    m_fPrevTrackPosition = 0.f;

    m_fAnimationSpeed = fAnimationSpeed;

    m_vecPrevBoneInfo.clear();
    m_vecPrevBoneInfo.resize(m_vecBones.size());

    // 이전 애니메이션 상태가 적용된 뼈의 행렬을 저장
    for (_uint i = 0; i < m_vecBones.size(); ++i)
    {
        m_vecPrevBoneTransforms.push_back(m_vecBones[i]->Get_TransformationMatrix());

        _vector vScale{}, vRotation{};

        if (m_iRootBoneIndex == i)
            XMMatrixDecompose(&vScale, &vRotation, &m_vPrevRootPosition, XMLoadFloat4x4(&m_vecPrevBoneTransforms[i]));

    }

    //현재 트랙 위치 초기화
    m_vecAnimations[m_iCurrentAnimationIndex]->Reset_TrackPostion();

    // 누적 이동량 초기화 안하면 애니메이션 상태 바뀔 때 마다 이전 애니메이션에 저장된 누적 이동량이 적용되서 텔포함.
    m_vAccumulatedMotionDelta = XMVectorZero();

}

void Engine::Model::Set_Animation_CS(_uint _animationIndex, _bool _isLoop, _float fLerpDuration, _float fAnimationSpeed)
{
    // 모델이 NONANIM타입이면 패스
    if (m_eModelType == MODEL::NONANIM)
        return;

    if (m_iNumAnimations <= _animationIndex)
    {
        COUT("모델이 소유한 애니메이션 개수보다 대입한 인덱스가 더 높습니다.");
        return;
    }

    //인자로 들어온 애니메이션이 현재 애니메이션과 같고 돌고있으면 return 
    if (_animationIndex == m_iCurrentAnimationIndex && _isLoop)
        return;

    if (m_bComputeBufferReady && m_bUseComputeSKinning)
        Copy_CurrentPoseToFrozenPos();

    if (m_iPrevAnimationIndex >= 0 && m_iPrevAnimationIndex < (_int)m_vecAnimations.size())
    {
        m_fPrevAnimFrameForSocket = m_vecAnimations[m_iCurrentAnimationIndex]->Get_CurrentFrame();
        m_bPrevAnimLoopForSocket = m_bIsLoopAnimation;
    }
    m_bSocketLerpStart = true;
    //return 안했으면 바뀌어야하므로

    m_iPrevAnimationIndex = m_iCurrentAnimationIndex; //이전 애니메이션인덱스는 이제 현재 인덱스로 바꿔주고
    m_iCurrentAnimationIndex = _animationIndex;
    m_bIsLoopAnimation = _isLoop; //얘가 이제 반복재생할것이냐고
    m_bIsLerping = true; //이게 이제 보간 시작
    m_fLerpTimer = 0.f;
    if (!m_bFirstSetAnimation)
        m_fLerpDuration = 0.0f, m_bIsLerping = false, m_bFirstSetAnimation = true;
    else
        m_fLerpDuration = fLerpDuration;

    m_fPrevTrackPosition = 0.f;

    m_fAnimationSpeed = fAnimationSpeed;

    Reset_CurrentKeyIndex();
    m_vecAnimations[m_iCurrentAnimationIndex]->Set_CurrentFrame(0.f);
    m_bIsAnimationFinished = false;

    // 누적 이동량 초기화 안하면 애니메이션 상태 바뀔 때 마다 이전 애니메이션에 저장된 누적 이동량이 적용되서 텔포함.
    Ready_NextRootMotion();
}

void Engine::Model::Set_AnimationUpper_CS(_uint iUpperAnimationIndex, _bool bUpperBlendEnable, _float fUpperLayerWeight, _bool _isLoop, _float fLerpDuration , _float fAnimationSpeed)
{
    if (iUpperAnimationIndex == 0xFFFFFFFFu) //-1
    {
        Clear_AnimationUpper();

        return;
    }

    // 모델이 NONANIM타입이면 패스
    if (m_eModelType == MODEL::NONANIM)
        return;

    if (m_iNumAnimations <= iUpperAnimationIndex)
    {
        COUT("모델이 소유한 애니메이션 개수보다 대입한 인덱스가 더 높습니다.");
        return;
    }

    //인자로 들어온 애니메이션이 현재 애니메이션과 같고 돌고있으면 return 
    if (iUpperAnimationIndex == m_iUpperCurrentAnimationIndex && _isLoop)
    {
        m_bUpperBlendEnable = bUpperBlendEnable;
        m_fUpperLayerWeight = fUpperLayerWeight;
        return;
    }
      
    if(m_iUpperCurrentAnimationIndex >= 0 &&
        m_iUpperCurrentAnimationIndex < (int)m_vecAnimations.size())
    {
        m_fUpperPrevAnimFrameForSocket = m_vecAnimations[m_iUpperCurrentAnimationIndex]->Get_CurrentFrame();
        m_bUpperPrevAnimLoopForSocket = m_bUpperIsLoopAnimation;
    }
    else
    {
        m_fUpperPrevAnimFrameForSocket = 0.f;
        m_bUpperPrevAnimLoopForSocket = false;
    }
    m_bUpperSocketLerpStart = true;

    if (m_bComputeBufferReady && m_bUseComputeSKinning) //이거는 
        Copy_CurrentPoseToFrozenPos();


    m_iUpperPrevAnimationIndex = m_iUpperCurrentAnimationIndex;
    m_iUpperCurrentAnimationIndex = iUpperAnimationIndex; //Upper용 현재재생중인 애니메이션 인덱스
    m_bUpperIsLoopAnimation = _isLoop;
    m_bUpperIsLerping = true; //이게 이제 보간 시작
    m_fUpperLerpTimer = 0.f;
    if (!m_bFirstSetAnimation)
        m_fUpperLerpDuration = 0.0f, m_bUpperIsLerping = false, m_bFirstSetAnimation = true;
    else
        m_fUpperLerpDuration = fLerpDuration;

    m_bUpperBlendEnable = bUpperBlendEnable; //상체 블렌드 사용할것인지
    m_fUpperLayerWeight = fUpperLayerWeight; //상체전체 블렌드 가중치 얼마나 섞을것인지
    m_fUpperAnimationSpeed = fAnimationSpeed; //애니메이션 재생속도 디폴트(1.f)로 사용하면서, 인자로 넘기려고 만듬 

    Reset_UpperCurrentKeyIndex();
    m_vecAnimations[m_iUpperCurrentAnimationIndex]->Set_CurrentFrame(0.f);
    m_bIsAnimationFinished = false;

}

void Engine::Model::Clear_AnimationUpper()
{
    m_iUpperCurrentAnimationIndex = -1; //-1로 보내서 최댓값으로 전달되게끔 0xFFFFFFFF
    m_bUpperBlendEnable = false;
    m_fUpperLayerWeight = 0.f;
    m_bIsAnimationFinished = false;
}

void Engine::Model::Request_ClearAnimationUpper(_float fFadeOutTime)
{
    if (m_iUpperCurrentAnimationIndex < 0 || m_bUpperBlendEnable == false)
    {
        Clear_AnimationUpper();
        return;
    }

    m_bUpperFadeOut = true;
    m_fUpperFadeOutTime = fFadeOutTime;
    m_fUpperFadeOutAccTime = 0.f;

    m_bUpperBlendEnable = true; //상체블렌드는 아직 유지

}

void Engine::Model::Update_UpperAnimFadeOut(_float fTimeDelta)
{
    if (!m_bUpperFadeOut)
        return;

    m_fUpperFadeOutAccTime += fTimeDelta;
    _float fTime = m_fUpperFadeOutAccTime / m_fUpperFadeOutTime;
    if (fTime > 1.f)fTime = 1.f;

    m_fUpperLayerWeight = 1.f - fTime;

    if (fTime >= 1.f)
    {
        m_bUpperFadeOut = false;
        Clear_AnimationUpper();
    }
}

void Engine::Model::Sync_BaseToUpperAnim()
{
    if (!m_bUpperBlendEnable)
        return;

    _float fUpperCurrentFrame = m_vecAnimations[m_iUpperCurrentAnimationIndex]->Get_CurrentFrame();

    m_iCurrentAnimationIndex = m_iUpperCurrentAnimationIndex;
    m_vecAnimations[m_iCurrentAnimationIndex]->Set_CurrentFrame(fUpperCurrentFrame);

}

HRESULT Engine::Model::Ready_DrapeBoneIndex()
{
    //망토에 적용할 회전을 가져올 뼈의 인덱스 
    m_tDrapeBones.iLeftArm_BoneIndex = Get_BoneIndex("LeftArm");
    m_tDrapeBones.iRightArm_BoneIndex = Get_BoneIndex("RightArm");

    //가져온 회전을 적용할 망토 뼈의 인덱스
    m_tDrapeBones.LeftOuterOpne_BoneIndex = Get_BoneIndex("LeftOuterOpen");
    m_tDrapeBones.LeftOuterOpne1_BoneIndex = Get_BoneIndex("LeftOuterOpen1");
    m_tDrapeBones.LeftOuterUp_BoneIndex = Get_BoneIndex("LeftOuterUp");
    m_tDrapeBones.TempLeftDrape_BoneIndex = Get_BoneIndex("Temp_GCLeftDrape");

    m_tDrapeBones.RightOuterOpen_BoneIndex = Get_BoneIndex("RightOuterOpen");
    m_tDrapeBones.RightOuterOpne1_BoneIndex = Get_BoneIndex("RightOuterOpen1");
    m_tDrapeBones.RightOuterUp_BoneIndex = Get_BoneIndex("RightOuterUp");
    m_tDrapeBones.TempRightDrape_BoneIndex = Get_BoneIndex("Temp_GCRightDrape");

    if (m_tDrapeBones.iLeftArm_BoneIndex ==-1|| m_tDrapeBones.iRightArm_BoneIndex == -1 || m_tDrapeBones.LeftOuterOpne1_BoneIndex == -1 || m_tDrapeBones.LeftOuterOpne_BoneIndex == -1 || m_tDrapeBones.LeftOuterUp_BoneIndex == -1 ||
        m_tDrapeBones.RightOuterOpen_BoneIndex == -1 || m_tDrapeBones.RightOuterOpne1_BoneIndex == -1 || m_tDrapeBones.RightOuterUp_BoneIndex == -1 || m_tDrapeBones.TempLeftDrape_BoneIndex == -1 || m_tDrapeBones.TempRightDrape_BoneIndex == -1)
        return E_FAIL;

    return S_OK;

}

HRESULT Engine::Model::Ready_DrapeRule()
{
    //언리얼 블루프린트 기준
    //_int iSourceBone; //LeftArm or RightArm 뼈
    //_int iTargetBone; //해당 소스에 영향 받는 본들
    //_uint iSourceAxis; //Source뼈에서 무슨 축을 가져올것인지 / 언리얼 기준 Z , Y가 영향울 주고 0:Y , 1: Z 
    //_uint iTargetAxis; //해동 Source뼈의 축을 어디 축에 영향을 줄것인지 / X Y Z 가 영향을 받고 0:X , 1: Y , 2: Z 
    //_uint iCurveId; // LeftOpenZ , RightOpenZ, LeftUp,RightUp  = 각도에 따라 Open(x축으로 벌림) Up(y축으로 위아래 들림) Id로 0:L_OpenZ, 1:L_Up, 2:R_OpenZ, 3:R_Up
    //_uint iAlphaSlot; //Open 과 Up을 결정하는 용도 0:Open, 1:Up
    //_float fMultipy; //값 반전용도 축 바꾸거나 

    //LeftArm RotZ → LeftOpenZ를 아래 3개 뼈에 회전을 가산
    m_vecDrapeRules.push_back({ m_tDrapeBones.iLeftArm_BoneIndex, m_tDrapeBones.LeftOuterOpne_BoneIndex, 1, 1, 0, 0, -1.f }); // RotY에 더함
    m_vecDrapeRules.push_back({ m_tDrapeBones.iLeftArm_BoneIndex, m_tDrapeBones.LeftOuterOpne1_BoneIndex,1, 1, 0, 0, -1.f }); // RotY에 더함
    m_vecDrapeRules.push_back({ m_tDrapeBones.iLeftArm_BoneIndex, m_tDrapeBones.TempLeftDrape_BoneIndex, 1, 2, 0, 0, -1.f }); // RotZ에 더함

    // LeftArm RotY -> LeftUp을
    m_vecDrapeRules.push_back({ m_tDrapeBones.iLeftArm_BoneIndex, m_tDrapeBones.LeftOuterUp_BoneIndex,0, 0, 1, 1, -1.f }); // RotX에 더함

    //RightArm RotZ → RightOpenZ를 아래 3개 뼈에 회전을 가산
    //m_vecDrapeRules.push_back({ m_tDrapeBones.iRightArm_BoneIndex, m_tDrapeBones.RightOuterOpen_BoneIndex, 1, 1, 2, 0, -1.f }); // RotY에 더함
    //m_vecDrapeRules.push_back({ m_tDrapeBones.iRightArm_BoneIndex, m_tDrapeBones.RightOuterOpne1_BoneIndex, 1, 1, 2, 0, -1.f }); //RotY에 더함
    //m_vecDrapeRules.push_back({ m_tDrapeBones.iRightArm_BoneIndex, m_tDrapeBones.TempRightDrape_BoneIndex, 1, 2, 2, 0, -1.f }); // RotZ에 더함

    //RightArm RotY -> RightUp을
    m_vecDrapeRules.push_back({ m_tDrapeBones.iRightArm_BoneIndex, m_tDrapeBones.RightOuterUp_BoneIndex, 0, 0, 3, 1, -1.f }); // RotX에 더함

    m_vecCurveKey.resize(4);

    m_vecCurveKey[0].assign({ {-45.f,-45.f},{0.f,-15.f},{30.f,0.f} }); //LeftOpenZ
    m_vecCurveKey[1].assign({ {0.f,0.f},{90.f,-60.f} }); //LeftUp
    m_vecCurveKey[2].assign({ {-30.f,0.f},{0.f,15.f},{45.f,45.f} }); //RightOpenZ
    m_vecCurveKey[3].assign({ {-90.f,-60.f},{0.f,0.f} });//RightUp

    return S_OK;
}

HRESULT Engine::Model::Ready_DrapeBuffer()
{
    m_pCShaderDrivenBone = static_cast<ComputeShader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, L"Prototype_Component_Shader_Compute_DrivenBones"));

    if (m_pCShaderDrivenBone == nullptr)
        return E_FAIL;

    /* 하나로 압축 */
    m_pDrapeDeltaBuffer = StructuredBuffer::Create(m_pDevice, m_pContext, SBUSAGE::DYNAMIC, (_uint)m_vecDrapeRules.size(), sizeof(DRAPEDELTA_DESC));
    CHECK_NULLPTR(m_pDrapeDeltaBuffer);

    m_bUseDrapeDelta = true;

    return S_OK;
}

void Engine::Model::Update_DrapeDelta()
{
    if (m_vecDrapeRules.empty() || !m_pDrapeDeltaBuffer)
        return;

    _vector vLeftArmDegree = m_pGameInstance->QuaternionToDegrees(XMLoadFloat4(&m_vLeftArmRotation));
    _vector vRightArmDegree = m_pGameInstance->QuaternionToDegrees(XMLoadFloat4(&m_vRightArmRotation));

    m_vecDrapeDeltas.resize(m_vecDrapeRules.size());

    _float fDrapeBlend = 1.f;

    if (m_bIsLerping || m_bUpperIsLerping)
    {
        _float fBase = (m_fLerpDuration > 0.f) ? (m_fLerpTimer / m_fLerpDuration) : 1.f;
        fBase = clamp(fBase, 0.f, 1.f);

        fDrapeBlend = fBase;
    }

    for (size_t i = 0; i < m_vecDrapeRules.size(); ++i)
    {
        _bool bLeft = (m_vecDrapeRules[i].iSourceBone == m_tDrapeBones.iLeftArm_BoneIndex); //소스본이 왼쪽팔인지
        _vector vSourceDegree = bLeft ? vLeftArmDegree : vRightArmDegree;  //왼쪽팔이면 위에서 구한 왼쪽팔각도 아니면 오른쪽팔각도

        _uint iDxSourceAxis = UE_SourceAix_ToDX(m_vecDrapeRules[i].iSourceAxis); //소스(언리얼)축을 DX축으로 변환시켜주고
        _float fSourceDegree = Get_AxisDegree(vSourceDegree, iDxSourceAxis); //소스축기준 각도 뽑아내주고 0:X , 1:Y

        _float fInputDegree = fSourceDegree * m_vecDrapeRules[i].fMultipy; //-1을 곱해서 좌우 대칭 만들어주고

        //CurveKey에 데이터화한 값과 입력값 기준으로 평가 + 선형보간해서 각도 뽑아내고
        _float fCurveOutDegree = EvaluateCurve(m_vecCurveKey[m_vecDrapeRules[i].iCurveId], fInputDegree);

        _float fAlpha = (m_vecDrapeRules[i].iAlphaSlot) == 0 ? m_fDrapeOpenControl : m_fDrapeUpControl; //동적으로 제어해야할듯(상태 및 환경에 따라서)
        _float fFinalDegree = fCurveOutDegree * fAlpha * fDrapeBlend; //최종 각도

        _uint iDxTargetAxis = UEAxisToDxAxis(m_vecDrapeRules[i].iTargetAxis);//타겟본 축 Dx로 바꿔서 어디축에 더할지

        //해당 뼈가 가진 고유 방향중 어디를 회전축으로 사용할것인지
        _vector vAxis = (iDxTargetAxis == 0) ? XMVectorSet(1.0f, 0.f, 0.f, 0.f) :
            (iDxTargetAxis == 1) ? XMVectorSet(0.f, 1.0f, 0.f, 0.f) :
            XMVectorSet(0.f, 0.f, 1.0f, 0.f);

        _vector vQuaternionDelta = XMQuaternionRotationAxis(vAxis, XMConvertToRadians(fFinalDegree));
        vQuaternionDelta = XMQuaternionNormalize(vQuaternionDelta);

        m_vecDrapeDeltas[i].iTargetBone = m_vecDrapeRules[i].iTargetBone;
        XMStoreFloat4(&m_vecDrapeDeltas[i].QuaternionDelta, vQuaternionDelta);

    }

    m_pDrapeDeltaBuffer->Update(m_vecDrapeDeltas.data(), (_uint)(sizeof(DRAPEDELTA_DESC) * m_vecDrapeDeltas.size()));
}

HRESULT Engine::Model::Update_DispatchDrivenBone()
{
    if (!m_pCShaderDrivenBone || !m_pDrapeDeltaBuffer)
        return S_OK;

    _uint iDeltaCount = (_uint)m_vecDrapeDeltas.size();
    if (iDeltaCount == 0)
        return S_OK;

    m_pDrapeDeltaBuffer->Bind_SRV(9, stage_CS);
    m_pLocalSRTBuffer->Bind_UAV(4);    
    m_pLocalMatricesBuffer->Bind_UAV(1); 
    _uint iGroupCount = (m_iNumBones + 255) / 256;

    _uint iDrapeDeltaCount = (_uint)m_vecDrapeDeltas.size();
;   m_pCShaderDrivenBone->Bind_RawValue_ByHandle(g_iDrapeDeltaCount, &iDrapeDeltaCount, sizeof(_uint));

    m_pCShaderDrivenBone->None_UnbiendDispatch(iGroupCount, 1, 1);

    m_pCShaderDrivenBone->Unbind_Resources();

    return S_OK;
}

_float Engine::Model::EvaluateCurve(const vector<CURVEKEY_DESC>& CurveKeys, _float fDegree)
{
    if (CurveKeys.empty())
        return 0.f;

    //m_vecCurveKey[0].assign({ {-45.f,-45.f},{0.f,-15.f},{30.f,0.f} }); 기준으로 
    //입력으로 들어온 각도가 -45보다 작으면 -45반환, 입력으로 들어온 각도가 30보다 크다면 0을 반환해줌으로써 최대~최소 클램프 작업

    if (fDegree <= CurveKeys.front().fTime)
        return CurveKeys.front().fValue;
    if (fDegree >= CurveKeys.back().fTime)
        return CurveKeys.back().fValue;

    //i가 0일때 CurveKeys[0]과[1]을 1일때 [1]과[2]를 2일때 2일때[2]와[3]을 계산해줌으로써 두 키를 기준으로 보간을 하기 위해서
    for (size_t i = 0; i + 1< CurveKeys.size(); ++i)
    {
        _float fCurrentTime = CurveKeys[i].fTime;
        _float fNextTime = CurveKeys[i + 1].fTime;

        if (fDegree < fNextTime)
        {
            _float fCurrentValue = CurveKeys[i].fValue;
            _float fNextValue = CurveKeys[i + 1].fValue;

            _float fDenom = (fNextTime - fCurrentTime); //분모
            _float fRatio = (fDenom > 1e-6f) ? (fDegree - fCurrentTime) / fDenom : 0.f; //(현재각도-시작 각도) / 다음점각도-시작점 각도)로 비율구하고
            

            return fCurrentValue + (fNextValue - fCurrentValue) * fRatio;
        }

    }

    return CurveKeys.back().fValue;
}

_uint Engine::Model::UEAxisToDxAxis(_uint iUEAix)
{
    switch (iUEAix)
    {
    case 0: return 2; // X ->Z
    case 1: return 0; // Y ->X
    case 2: return 1; // Z- >Y 
    default: return 0;
    }
}

_uint Engine::Model::UE_SourceAix_ToDX(_uint iSourceAxis)
{
    //UE Y를 X로 , UE Z를 Y로 
    return (iSourceAxis == 0) ? 0 : 1;
}

_float Engine::Model::Get_AxisDegree(_vector vDegreeXYZ, _uint iAxis)
{
    if (iAxis == 0) return XMVectorGetX(vDegreeXYZ);
    if (iAxis == 1) return XMVectorGetY(vDegreeXYZ);
    return XMVectorGetZ(vDegreeXYZ);
}

void Engine::Model::Set_Animation(_uint _animationIndex, _float _lerpDuration)
{
    // 모델이 NONANIM타입이면 패스
    if (m_eModelType == MODEL::NONANIM)
        return;

    if (m_iNumAnimations <= _animationIndex)
    {
        COUT("모델이 소유한 애니메이션 개수보다 대입한 인덱스가 더 높습니다.");
        return;
    }

    m_iCurrentAnimationIndex = _animationIndex; // 애니메이션 인덱스 변경
    m_fLerpDuration = _lerpDuration;
    if (m_fLerpDuration != 0.f)
    {
        // 만약 애니메이션이 변경되었다면(애니메이션간 보간)
        if (m_iPrevAnimationIndex != m_iCurrentAnimationIndex)
        {
            m_vecAnimations[m_iCurrentAnimationIndex]->Start_Animation();
            m_bIsLerping = true;

            // 보간을 위해 이 전 상태의 모든 본 상태 보관(애니메이션이 적용되지 않은 본은 Identity)
            m_vecPrevBoneInfo.clear();
            m_vecPrevBoneInfo.resize(m_iNumBones);
            m_vecPrevBoneInfo = m_vecFirstBoneInfo;
            m_vecAnimations[m_iPrevAnimationIndex]->Push_CurrentMatrices(m_vecPrevBoneInfo);

            // 보간을 위해 다음 애니메이션의 첫 번째 키 프레임 모든 본 상태 보관(애니메이션이 적용되지 않은 본은 Identity)
            m_vecNextBoneInfo.clear();
            m_vecNextBoneInfo.resize(m_iNumBones);
            m_vecNextBoneInfo = m_vecFirstBoneInfo;
            m_vecAnimations[m_iCurrentAnimationIndex]->Push_FirstMatrices(m_vecNextBoneInfo);
        }
    }
    else
    {
        m_vecAnimations[m_iCurrentAnimationIndex]->Start_Animation();
    }

    m_iPrevAnimationIndex = m_iCurrentAnimationIndex;
}

void Engine::Model::Set_NextAnimation(_uint _animationIndex, _float _nextLerpDuration)
{
    m_iNextAnimationIndex = _animationIndex;
    m_fNextLerpDuration = _nextLerpDuration;
    m_vecAnimations[m_iCurrentAnimationIndex]->Set_StackNextAnimation(true);
}

void Engine::Model::Set_Animation_Speed(const _float _speed)
{
    for (Animation* anim : m_vecAnimations)
    {
        anim->Set_Animation_Speed(_speed);
    }
}

void Engine::Model::Set_SingleAnimation_Speed(_float fSpeed)
{
    m_fAnimationSpeed = fSpeed;
}

void Engine::Model::LerpAnimation(const _float fTimeDelta)
{
    if (m_fLerpDuration == 0.f)
    {
        m_fLerpTimer = 0.f;
        m_bIsLerping = false;
        return;
    }
    m_fLerpTimer += fTimeDelta;

    if (m_fLerpTimer <= m_fLerpDuration)
    {
        _vector scale{}, rotation{}, translation{};
        _vector leftscale{}, leftrotation{}, lefttranslation{};
        _vector rightscale{}, rightrotation{}, righttranslation{};
        _float4x4 lerpmatrix = {};
        for (_uint i = 0; i < m_iNumBones; ++i)
        {
            leftscale = XMLoadFloat3(&m_vecPrevBoneInfo[i].vScale);
            rightscale = XMLoadFloat3(&m_vecNextBoneInfo[i].vScale);

            leftrotation = XMLoadFloat4(&m_vecPrevBoneInfo[i].vRotation);
            rightrotation = XMLoadFloat4(&m_vecNextBoneInfo[i].vRotation);

            lefttranslation = XMLoadFloat3(&m_vecPrevBoneInfo[i].vPosition);
            righttranslation = XMLoadFloat3(&m_vecNextBoneInfo[i].vPosition);

            XMVectorSetW(lefttranslation, 1.f);
            XMVectorSetW(righttranslation, 1.f);

            _float ration = (m_fLerpTimer) / (m_fLerpDuration); // 비율

            scale = XMVectorLerp(leftscale, rightscale, ration);
            rotation = XMQuaternionSlerp(leftrotation, rightrotation, ration);
            translation = XMVectorLerp(lefttranslation, righttranslation, ration);

            XMStoreFloat4x4(&lerpmatrix,
                XMMatrixAffineTransformation(scale, XMVectorSet(0.f, 0.f, 0.f, 1.f), rotation, translation));

            m_vecBones[i]->Set_TransformationMatrix(lerpmatrix);
        }
    }
    else
    {
        m_fLerpTimer = 0.f;
        m_bIsLerping = false;
    }
}

_float Engine::Model::Get_Animation_Speed()
{
    return m_vecAnimations[m_iCurrentAnimationIndex]->Get_Animation_Speed();
}
/******************************************************* 애니메이션 함수 *******************************************************/





//////////////////////////////////////////////////////// 모프 애니메이션 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Model::Register_StructuredBuffer(Shader* _shader)
{
    for (_uint i = 0; i < m_iNumMeshes; ++i)
    {
        m_vecMeshes[i]->Register_StructuredBuffer(_shader);
    }

    return S_OK;
}

HRESULT Engine::Model::Initialize_UnifiedMorphs()
{
    // 모든 메쉬에서 모프 정보를 받아와서 하나로 묶는 작업
    if (m_vecMeshes.empty()) return S_OK;

    set<_string> uniqueNames;

    for (auto& mesh : m_vecMeshes)
    {
        if (mesh->Has_Morph())
        {
            const auto& morphs = mesh->Get_Morphs();
            for (const auto& morph : morphs)
            {
                uniqueNames.insert(morph.strName);
            }
        }
    }

    m_vecUnifiedMorphNames.assign(uniqueNames.begin(), uniqueNames.end());
    m_vecUnifiedMorphWeights.assign(m_vecUnifiedMorphNames.size(), 0.f);

    //COUT("=== Unified Morphs Initialized ===");
    //COUT("  Total unique morphs: " << m_vecUnifiedMorphNames.size());
    //COUT("  Total meshes: " << m_vecMeshes.size());

    // 모프 그룹(WaraiA), (WaraiB)등을 묶는 작업
    unordered_map<_string, vector<_string>> groupMap;

    for (const auto& morphName : m_vecUnifiedMorphNames)
    {
        size_t start = morphName.find('(');
        size_t end = morphName.find(')');

        // (와 )를 모두 찾았고 end)가 start(보다 뒤에 있으면
        if (start != _string::npos && end != _string::npos && end > start)
        {
            _string groupName = morphName.substr(start + 1, end - start - 1);
            groupMap[groupName].push_back(morphName);
            // 만약 (WaraiA)라는 이름을 가진 메쉬 애니메이션이 있었으면
            // umap의 (WaraiA)라는 vector컨테이너에 mouth_32_(WaraiA) 이런식의 이름을 저장
        }
    }

    for (const auto& [groupName, morphNames] : groupMap)
    {
        MorphPreset preset;
        preset.presetName = groupName;
        preset.morphs.reserve(morphNames.size());

        for (const auto& morphName : morphNames)
        {
            preset.morphs[morphName] = 1.f;
        }

        m_umapMorphPresets[groupName] = preset;
    }

    for (_uint i = 0; i < m_vecUnifiedMorphNames.size(); ++i)
    {
        m_umapUnifiedMorphNameToIndex[m_vecUnifiedMorphNames[i]] = i;
    }

    return S_OK;
}

HRESULT Engine::Model::Update_UnifiedMorphWeight(const _string& _morphName, _float _weight)
{
    // Unified weight 업데이트
    for (_uint i = 0; i < m_vecUnifiedMorphNames.size(); ++i)
    {
        if (m_vecUnifiedMorphNames[i] == _morphName)
        {
            m_vecUnifiedMorphWeights[i] = clamp(_weight, 0.f, 1.f);
            break;
        }
    }

    return S_OK;
}

HRESULT Engine::Model::Apply_MorphPreset(const _string& _presetName, _float _intensity)
{
    auto iter = m_umapMorphPresets.find(_presetName);
    if (iter == m_umapMorphPresets.end())
    {
        COUT("해당하는 모프 프리셋 이름이 없습니다.");
        return E_FAIL;
    }

    const MorphPreset& preset = iter->second;

    for (const auto& morph : preset.morphs)
    {
        _float finalWeight = morph.second * _intensity;
        Update_UnifiedMorphWeight(morph.first, finalWeight);
    }


    return S_OK;
}

HRESULT Engine::Model::Blend_MorphPreset(const _string& _presetName1, const _string& _presetName2, _float _blend)
{
    auto iter1 = m_umapMorphPresets.find(_presetName1);
    auto iter2 = m_umapMorphPresets.find(_presetName2);

    // 모프 프리셋 이름으로 찾기
    if (iter1 == m_umapMorphPresets.end() || iter2 == m_umapMorphPresets.end())
    {
        COUT("대입한 프리셋중 하나의 이름이 잘못됐습니다");
        return E_FAIL;
    }

    // 모든 모프 0으로 만들기
    for (_uint i = 0; i < m_vecUnifiedMorphWeights.size(); ++i)
    {
        m_vecUnifiedMorphWeights[i] = 0.f;
    }

    // preset1에 적용
    _float firstWeight = 1.f - _blend;
    for (const auto& [morphName, baseWeight] : iter1->second.morphs)
    {
        Update_UnifiedMorphWeight(morphName, baseWeight * firstWeight);
    }

    // preset2에 적용
    _float lastWeight = _blend;
    for (const auto& [morphName, baseWeight] : iter2->second.morphs)
    {
        for (_uint i = 0; i < m_vecUnifiedMorphNames.size(); ++i)
        {
            // 기존 weight에 더하기
            if (m_vecUnifiedMorphNames[i] == morphName)
            {
                m_vecUnifiedMorphWeights[i] += baseWeight * lastWeight;
                break;
            }
        }
    }

    return S_OK;
}

HRESULT Engine::Model::Apply_MorphPresets_Additive(const vector<pair<_string, _float>>& _presets)
{
    // 모든 모프 0으로 초기화
    for (_uint i = 0; i < m_vecUnifiedMorphWeights.size(); ++i)
    {
        m_vecUnifiedMorphWeights[i] = 0.f;
    }

    // 각 프리셋의 웨이트를 누적 (Additive)
    for (const auto& [presetName, intensity] : _presets)
    {
        auto iter = m_umapMorphPresets.find(presetName);
        if (iter == m_umapMorphPresets.end())
        {
            COUT("프리셋을 찾을 수 없습니다: " << presetName);
            continue;  // 못 찾아도 나머지는 계속 적용
        }

        const MorphPreset& preset = iter->second;

        for (const auto& [morphName, baseWeight] : preset.morphs)
        {
            // Unified 배열에서 해당 모프 찾아서 누적
            auto it = m_umapUnifiedMorphNameToIndex.find(morphName);
            if (it != m_umapUnifiedMorphNameToIndex.end())
            {
                m_vecUnifiedMorphWeights[it->second] += baseWeight * intensity;
                m_vecUnifiedMorphWeights[it->second] = clamp(m_vecUnifiedMorphWeights[it->second], 0.f, 1.f);
            }
        }
    }

    return S_OK;
}

HRESULT Engine::Model::Start_MorphLerp(const _string& _targetPresetName, _float _duration)
{
    auto iter = m_umapMorphPresets.find(_targetPresetName);
    if (iter == m_umapMorphPresets.end())
    {
        COUT("해당하는 모프 프리셋 이름이 없습니다: " << _targetPresetName);
        return E_FAIL;
    }

    // 현재 웨이트 저장
    m_vecPrevMorphWeights = m_vecUnifiedMorphWeights;

    // 목표 웨이트 초기화 (0으로)
    m_vecTargetMorphWeights.assign(m_vecUnifiedMorphNames.size(), 0.f);

    // 목표 프리셋의 웨이트 설정
    const MorphPreset& preset = iter->second;
    for (const auto& [morphName, weight] : preset.morphs)
    {
        auto it = m_umapUnifiedMorphNameToIndex.find(morphName);
        if (it != m_umapUnifiedMorphNameToIndex.end())
        {
            m_vecTargetMorphWeights[it->second] = weight;
        }
    }

    m_fMorphLerpDuration = _duration;
    m_fMorphLerpTimer = 0.f;
    m_bIsMorphLerping = true;

    return S_OK;
}

void Engine::Model::Update_MorphLerp(_float _timeDelta)
{
    if (!m_bIsMorphLerping)
        return;

    m_fMorphLerpTimer += _timeDelta;

    if (m_fMorphLerpTimer >= m_fMorphLerpDuration)
    {
        // 보간 완료 - 목표 값으로 확정
        m_vecUnifiedMorphWeights = m_vecTargetMorphWeights;
        m_bIsMorphLerping = false;
        return;
    }

    // 비율 계산
    _float ratio = m_fMorphLerpTimer / m_fMorphLerpDuration;

    // 선형 보간
    for (_uint i = 0; i < m_vecUnifiedMorphWeights.size(); ++i)
    {
        m_vecUnifiedMorphWeights[i] = m_vecPrevMorphWeights[i] +
            (m_vecTargetMorphWeights[i] - m_vecPrevMorphWeights[i]) * ratio;
    }
}

void Engine::Model::Clear_MorphPreset()
{
    for (_uint i = 0; i < m_vecUnifiedMorphWeights.size(); ++i)
    {
        m_vecUnifiedMorphWeights[i] = 0.f;
    }
}
/******************************************************* 모프 애니메이션 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
_int Engine::Model::Get_BoneIndex(const _char* _name)
{
    auto iter = m_umapBoneIndexByName.find(_name);
    if (iter == m_umapBoneIndexByName.end())
        return -1;

    return (_int)iter->second;
}

HRESULT Engine::Model::Update_SkinnedMeshes()
{
    // 메쉬 피킹용
    // 당장은 안 써서 주석처리
    //for (_uint i = 0; i < m_iNumMeshes; ++i)
    //{
    //    m_vecMeshes[i]->Update_Skinned_VertexPositions(m_vecBones);
    //}

    return S_OK;
}

// 기존 CPU 행렬 업데이트 받았던 Combined Matrix 받아오는거
// 이걸로 정확한 위치 받고싶으면 ComputeShader가 아닌 CPU행렬연산 해야함
const _float4x4* Engine::Model::Get_BoneMatrixPtr(const _string& _boneName)
{
    _int index = Get_BoneIndex(_boneName.c_str());
    if (index <= -1)
        return nullptr;

    return m_vecBones[index]->Get_CombinedTransformationMatrixPtr();
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 본 찾기 함수 ////////////////////////////////////////////////////////
Bone* Engine::Model::Get_Bone_ByName(const _string& _boneName)
{
    _int index = Get_BoneIndex(_boneName.c_str());
    if (index == -1)
        return nullptr;

    return m_vecBones[index];
}

Bone* Engine::Model::Get_Bone_ByIndex(_int iBoneIndex)
{
    return m_vecBones[iBoneIndex];
}
/******************************************************* 본 찾기 함수 *******************************************************/



//////////////////////////////////////////////////////// 바운딩박스 생성 및 찾기 ////////////////////////////////////////////////////////
BoundingBox Engine::Model::Get_LocalAABB()
{
    // 이미 계산되었다면 계산된 값 반환
    if (m_bIsAABBCalculated)
        return m_tLocalAABB;

    // 메쉬가 없다면 기본값 대입 후 반환
    if (m_vecMeshes.empty())
    {
        m_tLocalAABB.Center = _float3(0.f, 0.f, 0.f);
        m_tLocalAABB.Extents = _float3(0.5f, 0.5f, 0.5f);
        m_bIsAABBCalculated = true;
        return m_tLocalAABB;
    }

    _float3 vMin = _float3(FLT_MAX, FLT_MAX, FLT_MAX);
    _float3 vMax = _float3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    // 메쉬들을 순회
    for (auto& mesh : m_vecMeshes)
    {
        const BoundingBox& aabb = mesh->Get_BoundingBox();

        vMin.x = min(vMin.x, aabb.Center.x - aabb.Extents.x);
        vMin.y = min(vMin.y, aabb.Center.y - aabb.Extents.y);
        vMin.z = min(vMin.z, aabb.Center.z - aabb.Extents.z);

        vMax.x = max(vMax.x, aabb.Center.x + aabb.Extents.x);
        vMax.y = max(vMax.y, aabb.Center.y + aabb.Extents.y);
        vMax.z = max(vMax.z, aabb.Center.z + aabb.Extents.z);
    }

    m_tLocalAABB.Center = _float3(
        (vMin.x + vMax.x) * 0.5f,
        (vMin.y + vMax.y) * 0.5f,
        (vMin.z + vMax.z) * 0.5f
    );

    m_tLocalAABB.Extents = _float3(
        (vMax.x - vMin.x) * 0.5f,
        (vMax.y - vMin.y) * 0.5f,
        (vMax.z - vMin.z) * 0.5f
    );

    m_bIsAABBCalculated = true;

    return m_tLocalAABB;
}
/******************************************************* 바운딩박스 생성 및 찾기 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Model::Render(const _float fTimeDelta, const _uint _meshIndex)
{
    m_vecMeshes[_meshIndex]->Bind_Resource();
    m_vecMeshes[_meshIndex]->Render(fTimeDelta);

    return S_OK;
}

#ifdef _DEBUG
void Engine::Model::Toggle_DebugBone(_uint _boneIndex)
{
    if (_boneIndex >= m_iNumBones)
        return;

    auto iter = find(m_vecDebugBoneIndices.begin(), m_vecDebugBoneIndices.end(), _boneIndex);

    if (iter != m_vecDebugBoneIndices.end())
    {
        // 이미 등록된 본이면 제거
        m_vecDebugBoneIndices.erase(iter);
    }
    else
    {
        // 슬롯 여유 확인 후 등록
        _uint usedSlots = 0;
        for (_uint i = 0; i < g_iReadbackBone; ++i)
        {
            if (m_iReadbackBoneIndex[i] < 0)
                continue;
            ++usedSlots;
        }

        if (usedSlots + (_uint)m_vecDebugBoneIndices.size() + 1 > g_iReadbackBone)
        {
            COUT("Readback 슬롯 부족 (최대 " << g_iReadbackBone << "개)");
            return;
        }

        m_vecDebugBoneIndices.push_back(_boneIndex);
    }
}

_int Engine::Model::Toggle_DebugBone(const _string& _boneName)
{
    _int index = Get_BoneIndex(_boneName.c_str());
    if (index < 0)
        return -1;

    Toggle_DebugBone((_uint)index);

    return index;
}

_bool Engine::Model::Is_DebugBone(_uint _boneIndex) const
{
    return find(m_vecDebugBoneIndices.begin(), m_vecDebugBoneIndices.end(), _boneIndex)
        != m_vecDebugBoneIndices.end();
}

void Engine::Model::Render_DebugBones(_matrix _worldMatrix)
{
    for (_uint i = 0; i < (_uint)m_vecDebugBoneIndices.size(); ++i)
    {
        _uint readbackSlot = m_iReadbackBaseSlot + i;
        if (readbackSlot >= g_iReadbackBone)
            break;

        _matrix matBoneWorld = XMLoadFloat4x4(&m_ReadbackCombinedMatrix[readbackSlot]) * _worldMatrix;

        // Readback 결과에서 본 위치 추출하여 sphere 등록
        BoundingSphere sphere;
        XMStoreFloat3(&sphere.Center, matBoneWorld.r[3]);
        sphere.Radius = 0.03f;
        m_pGameInstance->Add_Debug_Sphere(sphere);
    }
}

void Engine::Model::Render_DebugBones_NoCS(_matrix _worldMatrix, _int _boneIndex)
{
    _matrix matBoneWorld = m_vecBones[_boneIndex]->Get_CombinedTransformationMatrix() * _worldMatrix;

    // Readback 결과에서 본 위치 추출하여 sphere 등록
    BoundingSphere sphere;
    XMStoreFloat3(&sphere.Center, matBoneWorld.r[3]);
    sphere.Radius = 0.03f;
    m_pGameInstance->Add_Debug_Sphere(sphere);
}

void Engine::Model::Render_Debug(PrimitiveBatch<DirectX::VertexPositionColor>* _batch)
{
    DX::Draw(_batch, m_tWorldAABB, DirectX::Colors::DarkRed);
}


#endif // _DEBUG
/******************************************************* 렌더 함수 *******************************************************/

void Engine::Model::LocalAABB_To_WorldAABB(_matrix _worldmatrix)
{
    m_tLocalAABB.Transform(m_tWorldAABB, _worldmatrix);
}

void Engine::Model::LocalAABB_To_WorldAABB(BoundingBox _aabb)
{
    m_tWorldAABB = _aabb;
}


//////////////////////////////////////////////////////// 쉐이더 패스 선택 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Model::ShaderPass_Auto_Selection(vector<_int>* PassVec)
{
    if (m_iNumMeshes > 0)
        PassVec->resize(m_iNumMeshes);
    _uint materialindex = {};
    for (_int i = 0; i < (_int)m_iNumMeshes; i++)
    {
        materialindex = m_vecMeshes[i]->Get_MaterialIndex();

        if (materialindex >= m_iNumMaterials)
        {
            MSG_ON((L"메쉬에 등록된 마테리얼 번호\"" + to_wstring(i) + L"\"번이\n가지고 있는 마테리얼 개수를 초과합니다.").c_str(), L"마테리얼 찾기 실패");
            BREAK;
            return E_FAIL;
        }
        (*PassVec)[i] = m_vecMaterials[materialindex]->ShaderPass_Auto_Selection_M();
    }

    return S_OK;
}

/******************************************************* 쉐이더 패스 선택 함수 *******************************************************/


HRESULT Engine::Model::Render_Instancing(Shader* pShader, const vector<_float4x4>& MatrixData)
{
    for (_uint i = 0; i < m_iNumMeshes; ++i)
    {
        _uint iMaterialIndex = m_vecMeshes[i]->Get_MaterialIndex();

        if (iMaterialIndex < m_iNumMaterials)
        {
            m_vecMaterials[iMaterialIndex]->Bind_ShaderResources(pShader, "g_DiffuseTexture", aiTextureType_DIFFUSE, 0);
            m_vecMaterials[iMaterialIndex]->Bind_ShaderResources(pShader, "g_NormalTexture", aiTextureType_NORMALS, 0);
        }

        CHECK_FAILED(pShader->Begin(0), E_FAIL);
        CHECK_FAILED(pShader->Bind_Resources(0), E_FAIL);

        m_vecMeshes[i]->Render_Instancing(MatrixData);
    }

    return S_OK;
}

HRESULT Engine::Model::Initialize_UmapMasterBone()
{
    m_umapMasterBoneIndexByName.clear();
    m_umapMasterBoneIndexByName.reserve(m_vecBones.size());

    for (_uint i = 0; i < m_vecBones.size(); ++i)
    {
        m_umapMasterBoneIndexByName.emplace(m_vecBones[i]->Get_BoneName(), i);
    }

    //안채워져서 마스터본 맵핑 실패하면 , E_FAIL로 체크
    CHECK_FAILED(m_umapMasterBoneIndexByName.empty(), E_FAIL);

    return S_OK;
}

HRESULT Engine::Model::Initialize_BoneIndexByName()
{
    // 본 인덱스와 이름 캐싱
    m_umapBoneIndexByName.clear();
    m_umapBoneIndexByName.reserve(m_vecBones.size());

    // 본 개수만큼 전부 돌면서 채워주기
    for (_uint i = 0; i < m_vecBones.size(); ++i)
    {
        m_umapBoneIndexByName.emplace(m_vecBones[i]->Get_BoneName(), i);
    }

    // 이래도 empty면 진짜 있을수가 없는일임ㅋㅋ FAIL해버려
    CHECK_FAILED(m_umapBoneIndexByName.empty(), E_FAIL);

    return S_OK;
}

HRESULT Engine::Model::RegisterPartModelCom(Model* pPartModelCom)
{
    if (pPartModelCom == nullptr)
        return E_FAIL;

    if (find(m_vecPartModesl.begin(), m_vecPartModesl.end(), pPartModelCom) != m_vecPartModesl.end()) //등록되어있으면 return
        return E_FAIL;

    m_vecPartModesl.push_back(pPartModelCom);

    Safe_AddRef(pPartModelCom);

    m_bMasterOffsetDirty = true;

    return S_OK;
}

HRESULT Engine::Model::UnRegisterPartModelCom(Model* pPartModelCom)
{

    auto iter = find(m_vecPartModesl.begin(), m_vecPartModesl.end(), pPartModelCom);
    if (iter == m_vecPartModesl.end())
        return E_FAIL;

    Safe_Release(*iter);
    m_vecPartModesl.erase(iter);

    m_bMasterOffsetDirty = true;

    return S_OK;
}

void Engine::Model::Update_MasterOffset()
{
    if (!m_bMasterOffsetDirty)
        return;

    Build_MastrOffset();
    Update_OffsetToGPU();

    //빌드 끝났으면 갱신 끝났으므로 더티플래그 false로
    m_bMasterOffsetDirty = false;
}

HRESULT Engine::Model::Build_MastrOffset()
{
    if (m_vecPartModesl.empty())
    {
        m_bMasterOffsetDirty = true;
    }

    //이제 여기서 파츠들의 뼈,메쉬 접근해서 오프셋 행렬을 저장해야함
    _float4x4 matIdentity;
    XMStoreFloat4x4(&matIdentity, XMMatrixIdentity());
    m_vecBoneOffsetMatrix.assign(m_vecBones.size(), matIdentity);
    m_vecOffsetFilled.assign(m_vecBones.size(), false);

    for (Model* pPartModel : m_vecPartModesl)
    {
        if (pPartModel == nullptr) //파츠 없는 경우 ex)장착해제와 같은 Crash안나도록 건너뛰기 
            continue;

        const vector<Bone*>& PartBones = pPartModel->Get_Bones();
        const vector<Mesh*>& PartMesehes = pPartModel->Get_Meshes();

        for (Mesh* pMesh : PartMesehes)
        {
            const vector<_uint>& PartBoneIndices = pMesh->Get_BoneIndices();
            const vector<_float4x4>& PartoffsetMatrices = pMesh->Get_OffsetMatrices();

            _uint iOffsetMatrixNumber = min(PartBoneIndices.size(), PartoffsetMatrices.size()); //min으로 잡는 이유 두개의 개수가 다른거부터가 문제 발생 / Crash안나도록

            if (PartBoneIndices.size() != PartoffsetMatrices.size())
            {
                MSG_BOX("메쉬 뼈의 인덱스 개수와 메쉬 오프셋행렬이 같지 않습니다");
                continue;
            }

            for (size_t i = 0; i < iOffsetMatrixNumber; ++i)
            {
                _uint iPartBoneIndex = PartBoneIndices[i];
                if (iPartBoneIndex >= PartBones.size())
                    continue;

                _string PartBoneName = PartBones[iPartBoneIndex]->Get_BoneName(); //이게 이제 이 메쉬에 영향을 주는 뼈의 인덱스의 이름
                _int iMasterBoneIndex = Get_BoneIndex(PartBoneName.c_str()); //이게 이제 마스터본뼈와 파츠뼈의 이름이 같은 마스터본 인덱스  

                if (iMasterBoneIndex < 0 || (size_t)iMasterBoneIndex >= m_vecBones.size())
                    continue;

                if (!m_vecOffsetFilled[iMasterBoneIndex])
                {
                    m_vecBoneOffsetMatrix[iMasterBoneIndex] = PartoffsetMatrices[i]; //여기에 이제 마스터오프셋행렬을 해당 메쉬의 오프셋 행렬로 채워준다.
                    m_vecOffsetFilled[iMasterBoneIndex] = true;
                }
                else
                {
                    //나중에 디버깅용도로 활용

                }
            }


        }
    }

    return S_OK;
}

HRESULT Engine::Model::Build_StandAloneOffset()
{
    _float4x4 matIdentity;
    XMStoreFloat4x4(&matIdentity, XMMatrixIdentity());
    m_vecBoneOffsetMatrix.assign(m_vecBones.size(), matIdentity);
    m_vecOffsetFilled.assign(m_vecBones.size(), false);

    for (Mesh* pMesh : m_vecMeshes)
    {
        const vector<_uint>& myMeshBoneIndices = pMesh->Get_BoneIndices();
        const vector<_float4x4>& MyMeshOffsetMatrices = pMesh->Get_OffsetMatrices();

        _uint iOffsetMatrixNumber = min(myMeshBoneIndices.size(), MyMeshOffsetMatrices.size()); //min으로 잡는 이유 두개의 개수가 다른거부터가 문제 발생 / Crash안나도록

        if (myMeshBoneIndices.size() != MyMeshOffsetMatrices.size())
        {
            MSG_BOX("메쉬 뼈의 인덱스 개수와 메쉬 오프셋행렬이 같지 않습니다");
            return E_FAIL;
        }

        for (size_t i = 0; i < iOffsetMatrixNumber; ++i)
        {

            _uint iBoneIndex = myMeshBoneIndices[i];
            if (iBoneIndex >= m_vecBoneOffsetMatrix.size())
                continue;

            _string MyBoneName = m_vecBones[iBoneIndex]->Get_BoneName(); //이게 이제 이 메쉬에 영향을 주는 뼈의 인덱스의 이름
            _int MYBoneIndex = Get_BoneIndex(MyBoneName.c_str()); //이게 이제 마스터본뼈와 파츠뼈의 이름이 같은 마스터본 인덱스  

            if (MYBoneIndex < 0 || (size_t)MYBoneIndex >= m_vecBones.size())
                continue;

            if (!m_vecOffsetFilled[MYBoneIndex])
            {
                m_vecBoneOffsetMatrix[MYBoneIndex] = MyMeshOffsetMatrices[i];
                m_vecOffsetFilled[MYBoneIndex] = true;
            }
        }

    }


    return S_OK;
}

HRESULT Engine::Model::Ready_BoneCS_Buffers()
{
    if (m_iNumBones == 0)
        return S_OK;

    m_pCShader = static_cast<ComputeShader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, L"Prototype_Component_Shader_Compute_Bone"));
    ///m_pCShader = ComputeShader::Create(m_pDevice, m_pContext, L"../../Shader/CShader_Bone.hlsl","CS_BONECOMBINED");


    if (m_pCShader == nullptr)
        return E_FAIL;

    { //LocalBuffer / 매프레임 CPU 가 갱신
        //D3D11_BUFFER_DESC Desc = {};
        //Desc.ByteWidth = sizeof(_float4x4) * m_iNumBones;
        //Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        //Desc.CPUAccessFlags = 0;
        //Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; //구조화된 버퍼로선언 / 일반적인 배열처럼 셰이더에서 인덱스로 접근가능
        //Desc.StructureByteStride = sizeof(_float4x4);
        //Desc.Usage = D3D11_USAGE_DEFAULT;

        //CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pLocalMatricesBuffer), E_FAIL);

        //D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {}; //셰이더가 이 메모리를 어떻게 해석해야 할지 알려주는 구조체
        //SRVDesc.Format = DXGI_FORMAT_UNKNOWN; //구조화된 버퍼(STRUCTURED BUFFER) 반드시 UNKNOWN으로 설정해야함.
        //SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER; //버퍼를 바라보고 있음을 명시
        //SRVDesc.Buffer.FirstElement = 0; //버퍼의 처음(0번 인덱스)부터 읽기 시작하겠다.
        //SRVDesc.Buffer.NumElements = m_iNumBones;
        //CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pLocalMatricesBuffer, &SRVDesc, &m_pLocalMatricesSRV), E_FAIL);

        ////UAV = GPU는 수천개의 스레드가 동시에 돌아가기 때문에, 어떤 스레드가 먼저 데이터를 쓸지 순서가 정해져 있지 않음, 
        ////UVA는 이런 병렬 환경에서 여러 스레드가 동시에 메모리의 아무 곳에나 데이터를 기록할 수 있게 해줌.
        //D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};//셰이더가 읽고 쓰는 것을 동시에 가능하게 해주는 용도 , CS의 연산 결과를 다시 버퍼에 저장해야 할때 필수
        //UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        //UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        //UAVDesc.Buffer.FirstElement = 0;
        //UAVDesc.Buffer.NumElements = m_iNumBones;
        //CHECK_FAILED(m_pDevice->CreateUnorderedAccessView(m_pLocalMatricesBuffer, &UAVDesc, &m_pLocalMatricesUAV), E_FAIL);

        /* 하나로 압축 */
        m_pLocalMatricesBuffer = StructuredBuffer::Create(m_pDevice, m_pContext, SBUSAGE::GPU_READWRITE, m_iNumBones, sizeof(_float4x4));
        CHECK_NULLPTR(m_pLocalMatricesBuffer);
    }

    { //HierarchyBuffer / 로딩 시점
        //D3D11_BUFFER_DESC Desc{};
        //Desc.ByteWidth = sizeof(BONEINFO_GPU) * m_iNumBones;
        //Desc.Usage = D3D11_USAGE_DEFAULT;
        //Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        //Desc.CPUAccessFlags = 0;
        //Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        //Desc.StructureByteStride = sizeof(BONEINFO_GPU);

        //D3D11_SUBRESOURCE_DATA initData{};
        //initData.pSysMem = m_vecBoneHierarchy.data();

        //CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, &initData, &m_pHierarchyBuffer), E_FAIL);

        //D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
        //SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        //SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        //SRVDesc.Buffer.FirstElement = 0;
        //SRVDesc.Buffer.NumElements = m_iNumBones;
        //CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pHierarchyBuffer, &SRVDesc, &m_pHierarchySRV), E_FAIL);

        /* 하나로 압축 */
        m_pHierarchyBuffer = StructuredBuffer::Create(m_pDevice, m_pContext,
            SBUSAGE::IMMUTABLE, m_iNumBones, sizeof(BONEINFO_GPU), m_vecBoneHierarchy.data());
        CHECK_NULLPTR(m_pHierarchyBuffer);
    }

    { //CombinedMatrixBuffer
        //D3D11_BUFFER_DESC Desc{};
        //Desc.ByteWidth = sizeof(_float4x4) * m_iNumBones;
        //Desc.Usage = D3D11_USAGE_DEFAULT;
        //Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE; //CS가 결과값을 쓸수도 있고 , VS가 데이터를 읽어갈 수도 있다.
        //Desc.CPUAccessFlags = 0;
        //Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        //Desc.StructureByteStride = sizeof(_float4x4);

        //CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pCombinedMatrixBuffer), E_FAIL);

        ////UAV = GPU는 수천개의 스레드가 동시에 돌아가기 때문에, 어떤 스레드가 먼저 데이터를 쓸지 순서가 정해져 있지 않음, 
        ////UVA는 이런 병렬 환경에서 여러 스레드가 동시에 메모리의 아무 곳에나 데이터를 기록할 수 있게 해줌.
        //D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};//셰이더가 읽고 쓰는 것을 동시에 가능하게 해주는 용도 , CS의 연산 결과를 다시 버퍼에 저장해야 할때 필수
        //UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        //UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        //UAVDesc.Buffer.FirstElement = 0;
        //UAVDesc.Buffer.NumElements = m_iNumBones;
        //CHECK_FAILED(m_pDevice->CreateUnorderedAccessView(m_pCombinedMatrixBuffer, &UAVDesc, &m_pCombinedMatrixUAV), E_FAIL);

        //D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
        //SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        //SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        //SRVDesc.Buffer.FirstElement = 0;
        //SRVDesc.Buffer.NumElements = m_iNumBones;
        //CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pCombinedMatrixBuffer, &SRVDesc, &m_pCombiendMatrixSRV), E_FAIL);

        /* 하나로 압축 */
        m_pCombinedMatrixBuffer = StructuredBuffer::Create(m_pDevice, m_pContext,
            SBUSAGE::GPU_READWRITE, m_iNumBones, sizeof(_float4x4));
        CHECK_NULLPTR(m_pCombinedMatrixBuffer);
    }

    {//OffsetBuffer
        D3D11_BUFFER_DESC Desc = {};
        Desc.ByteWidth = sizeof(_float4x4) * m_iNumBones;
        Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; //셰이더에서 읽을 수 있는 자원선언 
        Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; //구조화된 버퍼로선언 / 일반적인 배열처럼 셰이더에서 인덱스로 접근가능
        Desc.StructureByteStride = sizeof(_float4x4);
        Desc.Usage = D3D11_USAGE_DEFAULT;

        vector<_float4x4> InitMatrices(m_iNumBones);
        _float4x4 Identity;
        XMStoreFloat4x4(&Identity, XMMatrixIdentity());
        for (auto& matrix : InitMatrices) matrix = Identity;

        D3D11_SUBRESOURCE_DATA InitData = {};
        InitData.pSysMem = InitMatrices.data();

        CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, &InitData, &m_pOffsetBuffer), E_FAIL);

        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {}; //셰이더가 이 메모리를 어떻게 해석해야 할지 알려주는 구조체
        SRVDesc.Format = DXGI_FORMAT_UNKNOWN; //구조화된 버퍼(STRUCTURED BUFFER) 반드시 UNKNOWN으로 설정해야함.
        SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER; //버퍼를 바라보고 있음을 명시
        SRVDesc.Buffer.FirstElement = 0; //버퍼의 처음(0번 인덱스)부터 읽기 시작하겠다.
        SRVDesc.Buffer.NumElements = m_iNumBones;
        CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pOffsetBuffer, &SRVDesc, &m_pOffsetSRV), E_FAIL);
    }

    return S_OK;
}

HRESULT Engine::Model::Build_BoneHierarchy()
{
    m_vecBoneHierarchy.clear();
    m_vecBoneHierarchy.resize(m_iNumBones);

    m_iMaxBoneDepth = 0;

    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        _int iParentIndex = m_vecBones[i]->Get_ParentBoneIndex();
        m_vecBoneHierarchy[i].iParent = iParentIndex;
        m_vecBoneHierarchy[i].iDepth = 0;
    }

    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        _int iDepth = 0;
        _int iParentIndex = m_vecBoneHierarchy[i].iParent;

        while (iParentIndex >= 0) //결과적으로 2000번대 뼈라도 0번인덱스의 부모인 iParentIndex가 -1인 Root가 될때까지 깊이를 저장, 즉 자신부터 시작해서 조상을 만날때까지 역으로 올라가기
        {
            ++iDepth;
            iParentIndex = m_vecBoneHierarchy[iParentIndex].iParent; //처음 해당 부모의 인덱스를 찾는 작업
        }

        m_vecBoneHierarchy[i].iDepth = iDepth;
        m_iMaxBoneDepth = max(m_iMaxBoneDepth, (_uint)iDepth);

    }

    m_vecInputLocalMatrices.clear();
    m_vecInputLocalMatrices.resize(m_iNumBones);

    return S_OK;
}

HRESULT Engine::Model::Update_LocalMatricesToGPU() //이거 기존에 CS아닌 애니메이션 디버그용으로 냅뒀었는데 현재는 안쓰는거긴함 
{
    if (!m_bComputeBufferReady || m_pLocalMatricesBuffer == nullptr)
        return E_FAIL;

    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        m_vecInputLocalMatrices[i] = m_vecBones[i]->Get_TransformationMatrix();
    }


    //discard 는 이전 프레임의 데이터는 필요없으니 다 버리고, 새로운 메모리 영역 할당해줘 / LocalMatrix는 매 프레임 전체를 갈아치워야하는 데이터 
    //D3D11_MAPPED_SUBRESOURCE mappedResource{};
    //if (FAILED(m_pContext->Map(m_pLocalMatricesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    //    return E_FAIL;

    //memcpy(mappedResource.pData, m_vecInputLocalMatrices.data(), sizeof(_float4x4) * m_iNumBones);
    //m_pContext->Unmap(m_pLocalMatricesBuffer, 0);

    /* 코드 변경(시호 -> 승우) */
    m_pContext->UpdateSubresource(m_pLocalMatricesBuffer->Get_Buffer(), 0, nullptr,
        m_vecInputLocalMatrices.data(), 0, 0);

    return S_OK;
}

HRESULT Engine::Model::Update_DispatchCombinedByDepth()
{
    if (!m_bComputeBufferReady || m_pCShader == nullptr)
        return E_FAIL;

    if (!m_bUseComputeSKinning)
        return S_OK;

    //CS에 바인딩
    //m_pCShader->Bind_SRV_FullSlot(0, m_pLocalMatricesSRV);
    //m_pCShader->Bind_SRV_FullSlot(1, m_pHierarchySRV);
    //m_pCShader->Bind_UAV_FullSlot(0, m_pCombinedMatrixUAV);
    /* 코드 변경 (시호 -> 승우)*/
    m_pLocalMatricesBuffer->Bind_SRV(0, stage_CS);
    m_pHierarchyBuffer->Bind_SRV(1, stage_CS);
    m_pCombinedMatrixBuffer->Bind_UAV(0);


    m_tBoneCompute.g_NumBones = m_iNumBones;
    m_tBoneCompute.g_PreTransformMatrix = m_PreTransformMatrix;
    //MaxDepth 순서로 여러번 Dispatch

    _uint iGroupCount = (m_iNumBones + 255) / 256; //일반화 공식 (전체 개수 + (그룹당 스레드 수 -1) / 그룹당 스레드 수로 모든 본이 최소 하나의 스레드는 할당받을 수 있도록
    //셰이더에서 [numthreads(256, 1, 1)]에 맞게 맞춰야함 두개는 항상 세트

    //ex) Depth 0(Root) Dispatch 한번 해서 루트본 계산, Detph1(그다음 뼈)그 다음 Dispatch 해서 부모가 계산된 자식들 계산 완료, 이 과정을 m_iMaxBoneDepth만큼 반복하여 모든 계층을 순차적으로
    for (_uint iDepth = 0; iDepth <= m_iMaxBoneDepth; ++iDepth)
    {
        m_tBoneCompute.g_CurrentDepth = iDepth;

        m_pCShader->Bind_RawValue_ByHandle(g_NumBones, &m_tBoneCompute.g_NumBones, sizeof(_uint));
        m_pCShader->Bind_RawValue_ByHandle(g_CurrentDepth, &m_tBoneCompute.g_CurrentDepth, sizeof(_uint));
        m_pCShader->Bind_Matrix_ByHandle(g_PreTransformMatrix, m_tBoneCompute.g_PreTransformMatrix);


        //m_pCShader->Bind_RawValue_FullSlot(BUFFER_COMPUTEBONE, "g_NumBones", &m_tBoneCompute.g_NumBones, sizeof(_uint));
        //m_pCShader->Bind_RawValue_FullSlot(BUFFER_COMPUTEBONE, "g_CurrentDepth", &m_tBoneCompute.g_CurrentDepth, sizeof(_uint));
        //m_pCShader->Bind_Matrix_FullSlot(BUFFER_COMPUTEBONE, "g_PreTransformMatrix", m_tBoneCompute.g_PreTransformMatrix);

        m_pCShader->None_UnbiendDispatch(iGroupCount, 1, 1);
    }

    //렌더(버텍스 셰이더 등)에서 계산된 행렬을 읽어오려면 SRV로 꽂아야 하기 때문에 UAV를 nullptr로
    //ID3D11UnorderedAccessView* NullUAV[1] = { nullptr };
    //m_pContext->CSSetUnorderedAccessViews(0, 1, NullUAV, nullptr);
    m_pCShader->Unbind_Resources();

    return S_OK;
}

HRESULT Engine::Model::Update_OffsetToGPU()
{
    if (!m_bComputeBufferReady || m_pOffsetBuffer == nullptr)
        return E_FAIL;

    if (m_vecBoneOffsetMatrix.size() != m_iNumBones)
        return E_FAIL;

    m_pContext->UpdateSubresource(m_pOffsetBuffer, //목적지: 데이터를 받을 GPU버퍼
        0, // 인덱스 복사할 서브리소스 인덱스 버퍼는 보통 0 
        nullptr, //복사할 영역 지정(nullptr이면 전체 복사)
        m_vecBoneOffsetMatrix.data(),//소스 : CPU에 있는 실제 데이터 주소
        0, //가로 피치 2D/3D 텍스처일때만
        0);//깊이 피치 3D 텍스처일때만

    return S_OK;
}

HRESULT Engine::Model::Ready_ReampSRV(Model* pMasterModel, vector<_uint>& vecRemapBone)
{
    for (Mesh* pMesh : m_vecMeshes)
    {
        CHECK_FAILED(pMesh->Create_RemapBuffer
        (this, pMasterModel, vecRemapBone), E_FAIL);
    }

    return S_OK;
}

HRESULT Engine::Model::Ready_StandAloneRemapSRV()
{
    for (Mesh* pMesh : m_vecMeshes)
    {
        CHECK_FAILED(pMesh->Create_StandAloneRemapBuffer(this), E_FAIL);
    }

    return S_OK;
}

HRESULT Engine::Model::Ready_BindPose_Buffer()
{
    if (m_iNumBones == 0)
        return S_OK;

    vector<POSESRT_GPU> bindPoseSRT;
    bindPoseSRT.resize(m_iNumBones);

    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        _float4x4 bindLocal = m_vecBones[i]->Get_TransformationMatrix();

        _vector vScale, vRoation, vTranslation;
        bool bisSuccess = XMMatrixDecompose(&vScale, &vRoation, &vTranslation, XMLoadFloat4x4(&bindLocal));

        POSESRT_GPU PoseSRT{};
        if (!bisSuccess) //행렬 분해 실패했을때 안전하게 항등
        {
            PoseSRT.vScale = _float3(1.f, 1.f, 1.f);
            PoseSRT.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
            PoseSRT.vPosition = _float3(0.f, 0.f, 0.f);
            PoseSRT.fPadding0 = 0.f; PoseSRT.fPadding1 = 0.f;
        }
        else
        {

            _float3 s; XMStoreFloat3(&s, vScale);
            _float4 r; XMStoreFloat4(&r, XMQuaternionNormalize(vRoation));
            _float3 t; XMStoreFloat3(&t, vTranslation);

            PoseSRT.vScale = _float3(s.x, s.y, s.z);
            PoseSRT.vRotation = _float4(r.x, r.y, r.z, r.w);
            PoseSRT.vPosition = _float3(t.x, t.y, t.z);
            PoseSRT.fPadding0 = 0.f; PoseSRT.fPadding1 = 0.f;
        }
        bindPoseSRT[i] = PoseSRT;
    }

    {//BindPoseBuffer
        //D3D11_BUFFER_DESC Desc = {};
        //Desc.ByteWidth = UINT(sizeof(POSESRT_GPU) * bindPoseSRT.size());
        //Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        //Desc.Usage = D3D11_USAGE_IMMUTABLE;
        //Desc.CPUAccessFlags = 0;
        //Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        //Desc.StructureByteStride = sizeof(POSESRT_GPU);

        //D3D11_SUBRESOURCE_DATA init{};
        //init.pSysMem = bindPoseSRT.data();

        //CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, &init, &m_pBindPoseSRTBuffer), E_FAIL);

        //D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
        //SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        //SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        //SRVDesc.Buffer.FirstElement = 0;
        //SRVDesc.Buffer.NumElements = (_uint)bindPoseSRT.size();

        //CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pBindPoseSRTBuffer, &SRVDesc, &m_pBindPoseSRTSRV), E_FAIL);

        /* 하나로 통합 */
        m_pBindPoseSRTBuffer = StructuredBuffer::Create(m_pDevice, m_pContext,
            SBUSAGE::IMMUTABLE, (_uint)bindPoseSRT.size(), sizeof(POSESRT_GPU), bindPoseSRT.data());
        CHECK_NULLPTR(m_pBindPoseSRTBuffer);
    }

    return S_OK;

}

HRESULT Engine::Model::Ready_UpperBodyMaskWeight()
{
    m_vecUpperMaskWeight.resize(m_iNumBones,0.f); 

    _int iUpperRoot = Get_BoneIndex("Spine"); // 상하체 분리용 척추
    if (iUpperRoot >= 0)
        m_vecUpperMaskWeight[iUpperRoot] = 1.0f;
    else
        return E_FAIL; //이상하다 ? Spine이 없을리 없을텐데 
    
    //이제 여기서 이 인덱스가 부모인 애들을 찾아야함

    for (_uint i = 0; i < m_iNumBones; ++i)
    {
        _int iParentIndex = m_vecBones[i]->Get_ParentBoneIndex();

        if (iParentIndex != -1 && m_vecUpperMaskWeight[iParentIndex]>0.5f) //부모 가중치가 있으면 
        {
            m_vecUpperMaskWeight[i] = 1.0f;
        }
    }

    m_vecUpperMaskWeight[m_iRootBoneIndex] = 0.0f; //루트본의 경우에는 항상 애니메이션 이동 경로를 100% 따르도록 가중치 섞지 않기
    _int iHpis = Get_BoneIndex("Hips");
    if (iHpis >= 0) m_vecUpperMaskWeight[iHpis] = 0.0f;

    if (iUpperRoot >= 0)
    m_vecUpperMaskWeight[iUpperRoot] = 0.25f; //Spine은 조금만 따라가게 이게 아마 상하체 분리가 시작되는 지점

    _int iSpineOne = Get_BoneIndex("Spine1");
    if(iSpineOne>=0)m_vecUpperMaskWeight[iSpineOne] = 0.7f; //2번째 지점 조금더 강하게 
    _int iSpineTwo = Get_BoneIndex("Spine2");
    if(iSpineTwo>=0)m_vecUpperMaskWeight[iSpineTwo] = 1.0f; //완전
    _int iSpineThree = Get_BoneIndex("Spine3");
    if (iSpineThree >= 0)m_vecUpperMaskWeight[iSpineThree] = 1.0f;

    return S_OK;
}

HRESULT Engine::Model::Ready_AnimCS_Buffers()
{
    m_pCShaderLocalMatrix = static_cast<ComputeShader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, L"Prototype_Component_Shader_Compute_LocalMatrix"));

    if (m_pCShaderLocalMatrix == nullptr)
        return E_FAIL;


    {//KeyFrameBuffer
        //D3D11_BUFFER_DESC Desc = {};
        //Desc.ByteWidth = sizeof(KEYFRAME_GPU) * (_uint)m_vecKeyFramesGPU.size();
        //Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        //Desc.CPUAccessFlags = 0;
        //Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        //Desc.StructureByteStride = sizeof(KEYFRAME_GPU);
        //Desc.Usage = D3D11_USAGE_IMMUTABLE; //절대 바뀌지 않는 값이므로

        //D3D11_SUBRESOURCE_DATA InitData = {};
        //InitData.pSysMem = m_vecKeyFramesGPU.data();

        //CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, &InitData, &m_pKeyFrameBuffer), E_FAIL);

        //D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {}; //셰이더가 이 메모리를 어떻게 해석해야 할지 알려주는 구조체
        //SRVDesc.Format = DXGI_FORMAT_UNKNOWN; //구조화된 버퍼(STRUCTURED BUFFER) 반드시 UNKNOWN으로 설정해야함.
        //SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER; //버퍼를 바라보고 있음을 명시
        //SRVDesc.Buffer.FirstElement = 0; //버퍼의 처음(0번 인덱스)부터 읽기 시작하겠다.
        //SRVDesc.Buffer.NumElements = (_uint)m_vecKeyFramesGPU.size();
        //CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pKeyFrameBuffer, &SRVDesc, &m_pKeyFrameSRV), E_FAIL);

        /* 하나로 압축 */
        m_pKeyFrameBuffer = StructuredBuffer::Create(m_pDevice, m_pContext,
            SBUSAGE::IMMUTABLE, (_uint)m_vecKeyFramesGPU.size(), sizeof(KEYFRAME_GPU), m_vecKeyFramesGPU.data());
        CHECK_NULLPTR(m_pKeyFrameBuffer);
    }

    {//ChannelBuffer
        //D3D11_BUFFER_DESC Desc = {};
        //Desc.ByteWidth = sizeof(CHANNELINFO_GPU) * (_uint)m_vecChannelGPU.size();
        //Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        //Desc.CPUAccessFlags = 0;
        //Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        //Desc.StructureByteStride = sizeof(CHANNELINFO_GPU);
        //Desc.Usage = D3D11_USAGE_IMMUTABLE;

        //D3D11_SUBRESOURCE_DATA InitData = {};
        //InitData.pSysMem = m_vecChannelGPU.data();

        //CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, &InitData, &m_pChannelBuffer), E_FAIL);

        //D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {}; //셰이더가 이 메모리를 어떻게 해석해야 할지 알려주는 구조체
        //SRVDesc.Format = DXGI_FORMAT_UNKNOWN; //구조화된 버퍼(STRUCTURED BUFFER) 반드시 UNKNOWN으로 설정해야함.
        //SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER; //버퍼를 바라보고 있음을 명시
        //SRVDesc.Buffer.FirstElement = 0; //버퍼의 처음(0번 인덱스)부터 읽기 시작하겠다.
        //SRVDesc.Buffer.NumElements = (_uint)m_vecChannelGPU.size();
        //CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pChannelBuffer, &SRVDesc, &m_pChannelSRV), E_FAIL);

        /* 하나로 압축 */
        m_pChannelBuffer = StructuredBuffer::Create(m_pDevice, m_pContext,
            SBUSAGE::IMMUTABLE, (_uint)m_vecChannelGPU.size(), sizeof(CHANNELINFO_GPU), m_vecChannelGPU.data());
        CHECK_NULLPTR(m_pChannelBuffer);
    }

    {//AnimInfoBuffer
        //D3D11_BUFFER_DESC Desc = {};
        //Desc.ByteWidth = sizeof(ANIMINFO_GPU) * (_uint)m_vecAnimGPU.size();
        //Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        //Desc.Usage = D3D11_USAGE_IMMUTABLE;
        //Desc.CPUAccessFlags = 0;
        //Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        //Desc.StructureByteStride = sizeof(ANIMINFO_GPU);

        //D3D11_SUBRESOURCE_DATA InitData = {};
        //InitData.pSysMem = m_vecAnimGPU.data();

        //CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, &InitData, &m_pAnimInfoBuffer), E_FAIL);

        //D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
        //SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        //SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        //SRVDesc.Buffer.FirstElement = 0;
        //SRVDesc.Buffer.NumElements = (_uint)m_vecAnimGPU.size();
        //CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pAnimInfoBuffer, &SRVDesc, &m_pAnimInfoSRV), E_FAIL);

        m_pAnimInfoBuffer = StructuredBuffer::Create(m_pDevice, m_pContext,
            SBUSAGE::IMMUTABLE, (_uint)m_vecAnimGPU.size(), sizeof(ANIMINFO_GPU), m_vecAnimGPU.data());
        CHECK_NULLPTR(m_pAnimInfoBuffer);
    }

    {//CurrentKeyIndexBuffer
        //D3D11_BUFFER_DESC Desc = {};
        //Desc.ByteWidth = sizeof(_uint) * m_iNumBones;
        //Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        //Desc.CPUAccessFlags = 0;
        //Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; //구조화된 버퍼로선언 / 일반적인 배열처럼 셰이더에서 인덱스로 접근가능
        //Desc.StructureByteStride = sizeof(_uint);
        //Desc.Usage = D3D11_USAGE_DEFAULT;

        //vector<_uint> Init(m_iNumBones, 0);
        //D3D11_SUBRESOURCE_DATA InitData = {};
        //InitData.pSysMem = Init.data();

        //CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, &InitData, &m_pCurrentKeyIndexBuffer), E_FAIL);

        //D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {}; //셰이더가 이 메모리를 어떻게 해석해야 할지 알려주는 구조체
        //SRVDesc.Format = DXGI_FORMAT_UNKNOWN; //구조화된 버퍼(STRUCTURED BUFFER) 반드시 UNKNOWN으로 설정해야함.
        //SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER; //버퍼를 바라보고 있음을 명시
        //SRVDesc.Buffer.FirstElement = 0; //버퍼의 처음(0번 인덱스)부터 읽기 시작하겠다.
        //SRVDesc.Buffer.NumElements = m_iNumBones;
        //CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pCurrentKeyIndexBuffer, &SRVDesc, &m_pCurrentKeyIndexSRV), E_FAIL);

        ////UAV = GPU는 수천개의 스레드가 동시에 돌아가기 때문에, 어떤 스레드가 먼저 데이터를 쓸지 순서가 정해져 있지 않음, 
        ////UVA는 이런 병렬 환경에서 여러 스레드가 동시에 메모리의 아무 곳에나 데이터를 기록할 수 있게 해줌.
        //D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};//셰이더가 읽고 쓰는 것을 동시에 가능하게 해주는 용도 , CS의 연산 결과를 다시 버퍼에 저장해야 할때 필수
        //UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        //UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        //UAVDesc.Buffer.FirstElement = 0;
        //UAVDesc.Buffer.NumElements = m_iNumBones;
        //CHECK_FAILED(m_pDevice->CreateUnorderedAccessView(m_pCurrentKeyIndexBuffer, &UAVDesc, &m_pCurrentKeyIndexURV), E_FAIL);


        vector<_uint> Init(m_iNumBones, 0);
        m_pCurrentKeyIndexBuffer = StructuredBuffer::Create(m_pDevice, m_pContext,
            SBUSAGE::GPU_READWRITE, m_iNumBones, sizeof(_uint), Init.data());
        CHECK_NULLPTR(m_pCurrentKeyIndexBuffer);
    }

    {//FrozenSRTBuffer
        //D3D11_BUFFER_DESC Desc = {};
        //Desc.ByteWidth = sizeof(POSESRT_GPU) * m_iNumBones;
        //Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        //Desc.CPUAccessFlags = 0;
        //Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; //구조화된 버퍼로선언 / 일반적인 배열처럼 셰이더에서 인덱스로 접근가능
        //Desc.StructureByteStride = sizeof(POSESRT_GPU);
        //Desc.Usage = D3D11_USAGE_DEFAULT;

        //CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pFrozenSRTBuffer), E_FAIL);

        //D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {}; //셰이더가 이 메모리를 어떻게 해석해야 할지 알려주는 구조체
        //SRVDesc.Format = DXGI_FORMAT_UNKNOWN; //구조화된 버퍼(STRUCTURED BUFFER) 반드시 UNKNOWN으로 설정해야함.
        //SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER; //버퍼를 바라보고 있음을 명시
        //SRVDesc.Buffer.FirstElement = 0; //버퍼의 처음(0번 인덱스)부터 읽기 시작하겠다.
        //SRVDesc.Buffer.NumElements = m_iNumBones;
        //CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pFrozenSRTBuffer, &SRVDesc, &m_pFrozenSRTSRV), E_FAIL);

        m_pFrozenSRTBuffer = StructuredBuffer::Create(m_pDevice, m_pContext,
            SBUSAGE::GPU_READWRITE, m_iNumBones, sizeof(POSESRT_GPU));
        CHECK_NULLPTR(m_pFrozenSRTBuffer);
    }

    {//LocalSRTBuffer
        //D3D11_BUFFER_DESC Desc = {};
        //Desc.ByteWidth = sizeof(POSESRT_GPU) * m_iNumBones;
        //Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        //Desc.CPUAccessFlags = 0;
        //Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; //구조화된 버퍼로선언 / 일반적인 배열처럼 셰이더에서 인덱스로 접근가능
        //Desc.StructureByteStride = sizeof(POSESRT_GPU);
        //Desc.Usage = D3D11_USAGE_DEFAULT;

        //CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pLocalSRTBuffer), E_FAIL);

        //D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {}; //셰이더가 이 메모리를 어떻게 해석해야 할지 알려주는 구조체
        //SRVDesc.Format = DXGI_FORMAT_UNKNOWN; //구조화된 버퍼(STRUCTURED BUFFER) 반드시 UNKNOWN으로 설정해야함.
        //SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER; //버퍼를 바라보고 있음을 명시
        //SRVDesc.Buffer.FirstElement = 0; //버퍼의 처음(0번 인덱스)부터 읽기 시작하겠다.
        //SRVDesc.Buffer.NumElements = m_iNumBones;
        //CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pLocalSRTBuffer, &SRVDesc, &m_pLocalSRTSRV), E_FAIL);

        ////UAV = GPU는 수천개의 스레드가 동시에 돌아가기 때문에, 어떤 스레드가 먼저 데이터를 쓸지 순서가 정해져 있지 않음, 
        ////UVA는 이런 병렬 환경에서 여러 스레드가 동시에 메모리의 아무 곳에나 데이터를 기록할 수 있게 해줌.
        //D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};//셰이더가 읽고 쓰는 것을 동시에 가능하게 해주는 용도 , CS의 연산 결과를 다시 버퍼에 저장해야 할때 필수
        //UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        //UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        //UAVDesc.Buffer.FirstElement = 0;
        //UAVDesc.Buffer.NumElements = m_iNumBones;
        //CHECK_FAILED(m_pDevice->CreateUnorderedAccessView(m_pLocalSRTBuffer, &UAVDesc, &m_pLocalSRTUAV), E_FAIL);

        m_pLocalSRTBuffer = StructuredBuffer::Create(m_pDevice, m_pContext,
            SBUSAGE::GPU_READWRITE, m_iNumBones, sizeof(POSESRT_GPU));
        CHECK_NULLPTR(m_pLocalSRTBuffer);
    }

    //UpperBodyMaskWeightBuffer
    if (m_eModelRoleType == MODELROLE::MASTER)
    {
        m_pUpperBodyMaskWeightBuffer = StructuredBuffer::Create(m_pDevice, m_pContext,
            SBUSAGE::IMMUTABLE, (_uint)m_vecUpperMaskWeight.size(), sizeof(_float), m_vecUpperMaskWeight.data());
        CHECK_NULLPTR(m_pUpperBodyMaskWeightBuffer);
    }

    {//UpperCurrentKeyIndexBuffer
        vector<_uint> Init(m_iNumBones, 0);
        m_pUpperCurrentKeyIndexBuffer = StructuredBuffer::Create(m_pDevice, m_pContext,
            SBUSAGE::GPU_READWRITE, m_iNumBones, sizeof(_uint), Init.data());
        CHECK_NULLPTR(m_pCurrentKeyIndexBuffer);
    }
    return S_OK;
}

HRESULT Engine::Model::Build_AnimGPU()
{
    //애니메이션 데이터를 일렬로 GPU에 던지기 위한 과정

    m_vecKeyFramesGPU.clear();
    m_vecChannelGPU.clear();
    m_vecAnimGPU.resize(m_iNumAnimations); //애니메이션 개수는 정해져있으므로 미리 resize해주고

    _uint iNumBones = (_uint)m_vecBones.size();

    for (_uint i = 0; i < m_iNumAnimations; ++i) //전체 애니메이션 개수 만큼 순회하면서
    {
        Animation* pAnimation = m_vecAnimations[i];

        ANIMINFO_GPU AnimInfo = {};
        AnimInfo.iChannelStart = (_uint)m_vecChannelGPU.size(); //현재까지 m_vecChannelGPU에 쌓인 크기가 이 애니메이션 채널 정보의 시작 지점
        AnimInfo.iNumBones = iNumBones;
        AnimInfo.fDuration = pAnimation->Get_Duration();
        AnimInfo.fTickPerSecond = pAnimation->Get_Animation_Speed();

        //채널 공간 확보 이번 애니메이션이 쓸 뼈 정보칸을 전체 뼈 개수 만큼 미리 예약 
        //이렇게 해야 데이터가 없는 뼈도 인덱스로 접근 가능
        m_vecChannelGPU.resize(AnimInfo.iChannelStart + iNumBones);

        for (_uint j = 0; j < iNumBones; ++j) //채널 초기화, 새로 만든 칸들을 안전하게 0으로 초기화
        {
            m_vecChannelGPU[AnimInfo.iChannelStart + j].iKeyStart = 0;
            m_vecChannelGPU[AnimInfo.iChannelStart + j].iKeyCount = 0;
        }

        vector<Channel*>& pChannels = pAnimation->Get_Channel();

        for (Channel* pChannel : pChannels) //이 애니메이션에서 움직이는 채널들 순회
        {
            _int iBoneIndex = pChannel->Get_ChannelIndex(); //이게 채널이름으로 뼈 인덱스 가져온값
            if (iBoneIndex < 0 || iBoneIndex >= m_vecBones.size())
                continue;

            _uint iNumKeyFrames = pChannel->Get_NumKeyFrames();

            CHANNELINFO_GPU ChannelInfo = {};
            ChannelInfo.iKeyStart = (_uint)m_vecKeyFramesGPU.size();//현재까지 쌓인 전체 키프레임 개수가 이 뼈의 키프레임 시작 위치
            ChannelInfo.iKeyCount = iNumKeyFrames;

            const vector<KEYFRAME>& vecKeyFrames = pChannel->Get_KeyFrame();
            for (_uint k = 0; k < iNumKeyFrames; ++k)
            {

                const KEYFRAME& KeyFrame = vecKeyFrames[k];

                KEYFRAME_GPU KeyFrame_GPU = {};
                KeyFrame_GPU.vScale = KeyFrame.vScale;
                KeyFrame_GPU.vRotation = KeyFrame.vRotation;
                KeyFrame_GPU.vPosition = KeyFrame.vPosition;
                KeyFrame_GPU.fTrackPosition = KeyFrame.fTrackPosition;
                KeyFrame_GPU.fPadding = 0;

                m_vecKeyFramesGPU.push_back(KeyFrame_GPU); //현재 채널의 키프레임 다 넣어주고
            }

            m_vecChannelGPU[AnimInfo.iChannelStart + (_uint)iBoneIndex] = ChannelInfo;//초기화해둔 빈 칸 중, 이채널의 인덱스 위치에 실제 정보를 덮어쓰기
            //위에서 resize로 공간 전체를 늘려주고 아래와 같이 기본값0으로 다 채웠기 때문에 , [AnimInfo.iChannelStart + (_uint)iBoneIndex] = ChannelInfo 로 하나만 대입함으로써 
            //셰이더에서 FinalIndex = AnimInfo.ChannelStart + TargetBoneIndex 로 현재 1번 애니메이션 재생한다면 AnimInfo.iChannelStart(2800) 근데 그중에서 TargetBoneIndex(10) = 2810을 함으로써
            // 2810번을 읽어서 실제 움직임을 만들어준다.
            //for (_uint j = 0; j < iNumBones; ++j) //채널 초기화, 새로 만든 칸들을 안전하게 0으로 초기화
            //{
            //    m_vecChannelGPU[AnimInfo.iChannelStart + j].iKeyStart = 0;
            //    m_vecChannelGPU[AnimInfo.iChannelStart + j].iKeyCount = 0;
            //}
        }

        m_vecAnimGPU[i] = AnimInfo;
    }


    return S_OK;
}

HRESULT Engine::Model::Update_DispatchLocalPose()
{
    if (!m_bComputeBufferReady || m_pCShaderLocalMatrix == nullptr)
        return E_FAIL;

    if (!m_bUseComputeSKinning)
        return S_OK;

    //CS에 바인딩
    //m_pCShaderLocalMatrix->Bind_SRV_FullSlot(2, m_pKeyFrameSRV);
    //m_pCShaderLocalMatrix->Bind_SRV_FullSlot(3, m_pChannelSRV);
    //m_pCShaderLocalMatrix->Bind_SRV_FullSlot(4, m_pAnimInfoSRV);
    //m_pCShaderLocalMatrix->Bind_SRV_FullSlot(5, m_pBindPoseSRTSRV);
    //m_pCShaderLocalMatrix->Bind_SRV_FullSlot(6, m_pFrozenSRTSRV);
    m_pKeyFrameBuffer->Bind_SRV(2, stage_CS);
    m_pChannelBuffer->Bind_SRV(3, stage_CS);
    m_pAnimInfoBuffer->Bind_SRV(4, stage_CS);
    m_pBindPoseSRTBuffer->Bind_SRV(5, stage_CS);
    m_pFrozenSRTBuffer->Bind_SRV(6, stage_CS);
    if(m_pUpperBodyMaskWeightBuffer)
        m_pUpperBodyMaskWeightBuffer->Bind_SRV(8, stage_CS);

    //m_pCShaderLocalMatrix->Bind_UAV_FullSlot(1, m_pLocalMatricesUAV);
    //m_pCShaderLocalMatrix->Bind_UAV_FullSlot(2, m_pCurrentKeyIndexURV);
    //m_pCShaderLocalMatrix->Bind_UAV_FullSlot(4, m_pLocalSRTUAV);
    m_pLocalMatricesBuffer->Bind_UAV(1);
    m_pCurrentKeyIndexBuffer->Bind_UAV(2);
    m_pLocalSRTBuffer->Bind_UAV(4);
    m_pUpperCurrentKeyIndexBuffer->Bind_UAV(6);

    m_tAnimLocalCompute.g_LiAnimIndex = m_iCurrentAnimationIndex;
    m_tAnimLocalCompute.g_LiNumBones = m_iNumBones;
    m_tAnimLocalCompute.g_LiRootBoneIndex = m_iRootBoneIndex;
    m_tAnimLocalCompute.g_LbAnimLoop = m_bIsLoopAnimation;
    m_tAnimLocalCompute.g_LfCurrentFrame = m_vecAnimations[m_iCurrentAnimationIndex]->Get_CurrentFrame();
    m_tAnimLocalCompute.g_LbLerping = m_bIsLerping ? 1u : 0u; //uint 값으로 넘겨주기
    m_tAnimLocalCompute.g_LfLerpRatio = (m_bIsLerping && m_fLerpDuration > 0.f) ? (m_fLerpTimer / m_fLerpDuration) : 1.f;
    m_tAnimLocalCompute.g_LbRemoveRootTranslation = 1;

    //상체 블렌드 전용 
    m_tAnimLocalCompute.g_LiUpperAnimIndex = -1; //블렌드 true 일때만 
    m_tAnimLocalCompute.g_LbUpperAnimLoop = m_bUpperIsLoopAnimation;
    m_tAnimLocalCompute.g_LfUpperCurrentFrame = 0.f; //블렌드 true 일때만
    m_tAnimLocalCompute.g_LbUpperBodyBlendEnalbe = m_bUpperBlendEnable;
    m_tAnimLocalCompute.g_LfUpperLayerWeight = m_fUpperLayerWeight;
    m_tAnimLocalCompute.g_LbUpperLerping = m_bUpperIsLerping ? 1u : 0u; //uint 값으로 넘겨주기
    m_tAnimLocalCompute.g_LfUpperLerpRatio = (m_bUpperIsLerping && m_fUpperLerpDuration > 0.f) ? (m_fUpperLerpTimer / m_fUpperLerpDuration) : 1.f;
    m_tAnimLocalCompute.g_LbUpperRemoveRootTranslation = 1;

    if (m_bUpperBlendEnable)
    {
        m_tAnimLocalCompute.g_LiUpperAnimIndex = m_iUpperCurrentAnimationIndex;
        m_tAnimLocalCompute.g_LfUpperCurrentFrame = m_vecAnimations[m_iUpperCurrentAnimationIndex]->Get_CurrentFrame();
    }

    // 26.03.03 승우가 바꿈
    // 개별적으로 바인딩해주던거 구조체 한 번에 바인딩하게 수정
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LiAnimIndex, &m_tAnimLocalCompute.g_LiAnimIndex, sizeof(_uint));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LiNumBones, &m_tAnimLocalCompute.g_LiNumBones, sizeof(_uint));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LiRootBoneIndex, &m_tAnimLocalCompute.g_LiRootBoneIndex, sizeof(_uint));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LbAnimLoop, &m_tAnimLocalCompute.g_LbAnimLoop, sizeof(_uint));

    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LfCurrentFrame, &m_tAnimLocalCompute.g_LfCurrentFrame, sizeof(_float));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LbLerping, &m_tAnimLocalCompute.g_LbLerping, sizeof(_uint));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LfLerpRatio, &m_tAnimLocalCompute.g_LfLerpRatio, sizeof(_float));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LbRemoveRootTranslation, &m_tAnimLocalCompute.g_LbRemoveRootTranslation, sizeof(_uint));

    ////상체블렌드용
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LiUpperAnimIndex, &m_tAnimLocalCompute.g_LiUpperAnimIndex, sizeof(_uint));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LbUpperAnimLoop, &m_tAnimLocalCompute.g_LbUpperAnimLoop, sizeof(_uint));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LfUpperCurrentFrame, &m_tAnimLocalCompute.g_LfUpperCurrentFrame, sizeof(_float));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LbUpperRemoveRootTranslation, &m_tAnimLocalCompute.g_LbUpperRemoveRootTranslation, sizeof(_uint));

    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LbUpperBodyBlendEnalbe, &m_tAnimLocalCompute.g_LbUpperBodyBlendEnalbe, sizeof(_float));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LfUpperLayerWeight, &m_tAnimLocalCompute.g_LfUpperLayerWeight, sizeof(_float));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LbUpperLerping, &m_tAnimLocalCompute.g_LbUpperLerping, sizeof(_uint));
    //m_pCShaderLocalMatrix->Bind_RawValue_ByHandle(g_LfUpperLerpRatio, &m_tAnimLocalCompute.g_LfUpperLerpRatio, sizeof(_float));

    // 위에껄 아래처럼 구조체 한 번에 던지게
    m_pCShaderLocalMatrix->Bind_EntireBuffer_BySlot(6, &m_tAnimLocalCompute, sizeof(m_tAnimLocalCompute));

    _uint iGroupCount = (m_iNumBones + 255) / 256; //일반화 공식 (전체 개수 + (그룹당 스레드 수 -1) / 그룹당 스레드 수로 모든 본이 최소 하나의 스레드는 할당받을 수 있도록
    //셰이더에서 [numthreads(256, 1, 1)]에 맞게 맞춰야함 두개는 항상 세트
    m_pCShaderLocalMatrix->None_UnbiendDispatch(iGroupCount, 1, 1);
    m_pCShaderLocalMatrix->Unbind_Resources(); //SRV, UAV해제 

    return S_OK;

}

HRESULT Engine::Model::Copy_CurrentPoseToFrozenPos()
{
    m_pContext->CopyResource(m_pFrozenSRTBuffer->Get_Buffer(), m_pLocalSRTBuffer->Get_Buffer());

    return S_OK;
}

void Engine::Model::Reset_CurrentKeyIndex()
{
    //애니메이션 시간이 0으로 됐을때 현재 키인덱스를 0으로 초기화
    //Map,UnMap 안쓰는 이유는 매프레임 발생하는게 아니라 새 애니메이션 시작을 위한 리셋이므로
    vector<_uint> vecCurrentKeyIndex(m_iNumBones, 0);
    m_pContext->UpdateSubresource(m_pCurrentKeyIndexBuffer->Get_Buffer(), 0, nullptr, vecCurrentKeyIndex.data(), 0, 0);

}

void Engine::Model::Reset_UpperCurrentKeyIndex()
{
    vector<_uint> vecUpperCurrentKeyIndex(m_iNumBones, 0);
    m_pContext->UpdateSubresource(m_pUpperCurrentKeyIndexBuffer->Get_Buffer(), 0, nullptr, vecUpperCurrentKeyIndex.data(), 0, 0);
}

void Engine::Model::Ready_NextRootMotion()
{
    m_vAccumulatedMotionDelta = XMVectorZero();
    m_fPrevTrackPosition = 0.f;

    if (m_iRootBoneIndex == -1)
    {
        m_vPrevRootPosition = XMVectorZero();
        m_qPrevRootRotation = XMQuaternionIdentity();
        return;
    }

    _vector vStartScale = {};
    _vector vStartRotation = {};
    _vector vStartPos = {};
    // 시작 회전도 함께 가져오기
    m_vecAnimations[m_iCurrentAnimationIndex]->Get_InterpolatedBonePosition(m_iRootBoneIndex, 0.f, m_bIsLoopAnimation, vStartScale, vStartRotation, vStartPos);

    _float fLastTrackPos = m_vecAnimations[m_iCurrentAnimationIndex]->Get_LastTrackPosition(m_iRootBoneIndex);
    _vector vEndScale = {};
    _vector vEndRotation = {};
    _vector vEndPos = {};

    m_vecAnimations[m_iCurrentAnimationIndex]->Get_InterpolatedBonePosition(m_iRootBoneIndex, fLastTrackPos, m_bIsLoopAnimation, vEndScale, vEndRotation, vEndPos);

    m_vRootStartPos = XMVectorSetW(vStartPos, 0.f);
    m_vRootEndPos = XMVectorSetW(vEndPos, 0.f);

    m_vPrevRootPosition = XMVectorSetW(vStartPos, 0.f);

    // 이전 회전을 시작 회전으로 초기화
    m_qPrevRootRotation = vStartRotation;

    if (m_iPrevAnimationIndex >= 0 && m_iPrevAnimationIndex < (_int)m_vecAnimations.size()) //보간중 루트 블렌딩을 시작하기 위해 전환시점 루트 저장
    {
        _vector vPrevScale{};
        _vector vPrevRotation{};
        _vector vPrevRoot{};

        m_vecAnimations[m_iPrevAnimationIndex]->Get_InterpolatedBonePosition(m_iRootBoneIndex, m_vecAnimations[m_iPrevAnimationIndex]->Get_CurrentFrame(), m_bIsLoopAnimation, vPrevScale, vPrevRotation, vPrevRoot);
        m_vRootLastPosFromPrevAnim = XMVectorSetW(vPrevRoot, 0.f);
    }
}

void Engine::Model::Update_RootMotion_CS(_float fPrevFrame, _float fCurrentFrame)
{
    if (m_iRootBoneIndex == -1)
    {
        m_vRootMotionDelta = XMVectorZero();
        m_qRootMotionRotationDelta = XMQuaternionIdentity();
        return;
    }

    _vector vCurrntScale = {};
    _vector vCurrentRotation = {};
    _vector vCurrentPos = {};
    m_vecAnimations[m_iCurrentAnimationIndex]->Get_InterpolatedBonePosition(m_iRootBoneIndex, fCurrentFrame, m_bIsLoopAnimation, vCurrntScale, vCurrentRotation, vCurrentPos);

    vCurrentPos = XMVectorSetW(vCurrentPos, 0.f);

    _vector qPrevInverse = XMQuaternionInverse(m_qPrevRootRotation);
    _vector qLocalDelta = XMQuaternionNormalize(
        XMQuaternionMultiply(qPrevInverse, vCurrentRotation));

    _float4 qSwapped = {};
    XMStoreFloat4(&qSwapped, qLocalDelta);
    m_qRootMotionRotationDelta = XMQuaternionNormalize(
        XMVectorSet(qSwapped.x, -qSwapped.z, qSwapped.y, qSwapped.w));

    if (m_bIsLoopAnimation && fCurrentFrame < fPrevFrame)//루프 + 현재 재생위치가 이전재생위치보다 작으면 위치가 0으로 초기화 됐을때
    {
        //마지막 위치 - 시작 위치로 한 사이클의 전체 이동량을 누적 / 기존과 동일한데 조금더 정밀 
        m_vAccumulatedMotionDelta += (m_vRootEndPos - m_vRootStartPos);
        m_vAccumulatedMotionDelta = XMVectorSetW(m_vAccumulatedMotionDelta, 0.f);

        // 루프 시 회전 델타 리셋 (급격한 점프 방지)
        m_qRootMotionRotationDelta = XMQuaternionIdentity();
    }
    // 다음 프레임을 위해 현재 회전 저장
    m_qPrevRootRotation = vCurrentRotation;

    _vector vWorldTranslation = XMVectorSetW(vCurrentPos + m_vAccumulatedMotionDelta, 0.f);

    m_vRootMotionDelta = vWorldTranslation - m_vPrevRootPosition; //이번 프레임의 실제 이동량
    m_vRootMotionDelta = XMVectorSetW(m_vRootMotionDelta, 0.f);

    if (!m_bIsLoopAnimation && !m_bIsLerping && m_bIsAnimationFinished) //논루프,보간x,애니메이션 재생끝났으면 즉, 정지해있는 상태에서 안전하게 0으로 고정
    {
        m_vRootMotionDelta = XMVectorZero();
        m_qRootMotionRotationDelta = XMQuaternionIdentity();
    }

    m_vPrevRootPosition = vWorldTranslation; //다음 프레임을 위해 현재 위치 저장

}

_vector Engine::Model::Get_RootParentRotation()
{
    if (m_iRootBoneIndex <= 0)
        return XMQuaternionIdentity();

    // 루트 본의 부모부터 최상위까지 로컬 회전 누적
    _vector qAccum = XMQuaternionIdentity();
    _int iCurrent = m_vecBones[m_iRootBoneIndex]->Get_ParentBoneIndex();

    while (iCurrent >= 0)
    {
        _float4x4 matLocal = m_vecBones[iCurrent]->Get_TransformationMatrix();
        _vector vS = {}, vR = {}, vT = {};
        XMMatrixDecompose(&vS, &vR, &vT, XMLoadFloat4x4(&matLocal));
        qAccum = XMQuaternionMultiply(vR, qAccum); // 부모 먼저 적용
        iCurrent = m_vecBones[iCurrent]->Get_ParentBoneIndex();
    }

    // PreTransformMatrix의 회전도 포함
    _vector vS = {}, vR = {}, vT = {};
    XMMatrixDecompose(&vS, &vR, &vT, XMLoadFloat4x4(&m_PreTransformMatrix));
    qAccum = XMQuaternionMultiply(vR, qAccum);

    return XMQuaternionNormalize(qAccum);
}

HRESULT Engine::Model::Ready_Readback_Buffer()
{
    m_pCShaderReadback = static_cast<ComputeShader*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, L"Prototype_Component_Shader_Compute_Readback"));

    if (m_pCShaderReadback == nullptr)
        return E_FAIL;

    {//ReadbackBuffer
        D3D11_BUFFER_DESC Desc{};
        Desc.ByteWidth = sizeof(_float4x4) * g_iReadbackBone;
        Desc.Usage = D3D11_USAGE_DEFAULT;
        Desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        Desc.CPUAccessFlags = 0;
        Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        Desc.StructureByteStride = sizeof(_float4x4);

        CHECK_FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pReadbackOutBuffer),E_FAIL);

        D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};
        UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        UAVDesc.Buffer.FirstElement = 0;
        UAVDesc.Buffer.NumElements = g_iReadbackBone;

        CHECK_FAILED(m_pDevice->CreateUnorderedAccessView(m_pReadbackOutBuffer, &UAVDesc, &m_pReadbackOutUAV),E_FAIL);
    }

    {//ReadbackStagingBuffer
        D3D11_BUFFER_DESC Desc{};
        Desc.ByteWidth = sizeof(_float4x4) * g_iReadbackBone;
        Desc.Usage = D3D11_USAGE_STAGING;
        Desc.BindFlags = 0;
        Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        if (FAILED(m_pDevice->CreateBuffer(&Desc, nullptr, &m_pReadbackStagingBuffer)))
            return E_FAIL;

        for (_uint i = 0; i < g_iReadbackBone; ++i)
            XMStoreFloat4x4(&m_ReadbackCombinedMatrix[i], XMMatrixIdentity());
    }
  

    return S_OK;

}

HRESULT Engine::Model::Register_ReadbackBoneName(const _string& BoneName ,_uint iIndexNumber)
{
    if (iIndexNumber >= g_iReadbackBone)
        return E_FAIL;

    m_iReadbackBoneIndex[iIndexNumber] = Get_BoneIndex(BoneName.c_str());
    if (m_iReadbackBoneIndex[iIndexNumber] == -1)
        return E_FAIL;

    return S_OK;
}

HRESULT Engine::Model::Unregister_ReadbackBoneName(_uint _indexNumber)
{
    if (_indexNumber >= g_iReadbackBone)
        return E_FAIL;

    // 해당 인덱스의 뼈 등록을 해제
    m_iReadbackBoneIndex[_indexNumber] = -1;

    return S_OK;
}

void Engine::Model::Update_DispatchReadback()
{
    if (!m_pReadbackOutBuffer || !m_pReadbackStagingBuffer || !m_pCShaderReadback)
        return;

    ZeroMemory(m_tReadback.iReadbackIndices, sizeof(_uint) * 8);

    _uint iCount = 0;
    for (_uint i = 0; i < g_iReadbackBone; ++i)
    {
        if (m_iReadbackBoneIndex[i] < 0) continue; 
        m_tReadback.iReadbackIndices[iCount++] = (_uint)m_iReadbackBoneIndex[i];
    }

#ifdef _DEBUG
    // 디버그 본 인덱스를 기존 슬롯 뒤에 이어서 등록
    m_iReadbackBaseSlot = iCount;
    for (_uint i = 0; i < (_uint)m_vecDebugBoneIndices.size(); ++i)
    {
        if (iCount >= g_iReadbackBone)
            break;
        m_tReadback.iReadbackIndices[iCount++] = m_vecDebugBoneIndices[i];
    }
#endif

    m_tReadback.iReadbackCount = iCount;

    // m_pCShaderReadback->Bind_SRV_FullSlot(7, m_pCombiendMatrixSRV);
    m_pCombinedMatrixBuffer->Bind_SRV(7, stage_CS);
    m_pCShaderReadback->Bind_UAV_FullSlot(5, m_pReadbackOutUAV);


    m_pCShaderReadback->Bind_RawValue_ByHandle(g_iReadbackIndices, m_tReadback.iReadbackIndices, sizeof(_uint) * 8); //우선 배열 8개로 선언해놨기 때문에 8개로
    m_pCShaderReadback->Bind_RawValue_ByHandle(g_iReadbackCount, &m_tReadback.iReadbackCount, sizeof(_uint));

    _uint group = (m_tReadback.iReadbackCount + 63) / 64;
    m_pCShaderReadback->None_UnbiendDispatch(group,1,1);

    m_pCShaderReadback->Unbind_Resources();

    m_pContext->CopyResource(m_pReadbackStagingBuffer, m_pReadbackOutBuffer);

    D3D11_MAPPED_SUBRESOURCE mapped{};
    HRESULT hr = m_pContext->Map(m_pReadbackStagingBuffer, 0, D3D11_MAP_READ, 0, &mapped);
    if (FAILED(hr))
        return;

    const _float4x4* pReadbackMatrix = reinterpret_cast<const _float4x4*>(mapped.pData);

    const _uint count = m_tReadback.iReadbackCount;
    for (_uint i = 0; i < count; ++i)
    {
        m_ReadbackCombinedMatrix[i] = pReadbackMatrix[i];
    }

    m_pContext->Unmap(m_pReadbackStagingBuffer, 0);
}

const _float4x4* Engine::Model::Get_ReadbackBoneMatrixPtr(const _string& _boneName)
{
    _int iBoneIndex = Get_BoneIndex(_boneName.c_str());
    if (iBoneIndex == -1) return nullptr;

    for (_uint i = 0; i < g_iReadbackBone; ++i)
    {
        if (m_iReadbackBoneIndex[i] == iBoneIndex)
            return &m_ReadbackCombinedMatrix[i];
    }

    return nullptr;
}

_float4x4* Engine::Model::Get_ReadbackBoneMatrixPtr_Nonconst(const _string& _boneName)
{
    _int iBoneIndex = Get_BoneIndex(_boneName.c_str());
    if (iBoneIndex == -1) return nullptr;

    for (_uint i = 0; i < g_iReadbackBone; ++i)
    {
        if (m_iReadbackBoneIndex[i] == iBoneIndex)
            return &m_ReadbackCombinedMatrix[i];
    }

    return nullptr;
}

HRESULT Engine::Model::Register_SocketBoneName(const _string& _boneName)
{
    // 이미 추가된 본은 스킵
    for (const auto& tSocket : m_vecSockets)
    {
        if (tSocket.strBoneName == _boneName)
            return S_OK;
    }

    SOCKET_DESC tSocketDesc;
    tSocketDesc.strBoneName = _boneName;
    tSocketDesc.iTargetBoneIndex = Get_BoneIndex(_boneName.c_str());

    if (tSocketDesc.iTargetBoneIndex == -1)
        return E_FAIL;

    _int iCurrentBoneIndex = tSocketDesc.iTargetBoneIndex;

    while (iCurrentBoneIndex != -1) // 최상위(Root)뼈전까지
    {
        tSocketDesc.vecHierarchy.push_back(iCurrentBoneIndex); // 찾고싶은 뼈 넣어주고
        iCurrentBoneIndex = m_vecBones[iCurrentBoneIndex]->Get_ParentBoneIndex(); // 현재뼈를 찾고싶은 뼈의 부모를 기준으로 계속 갱신하면서 계층별 뼈 넣어주고
    }

    reverse(tSocketDesc.vecHierarchy.begin(), tSocketDesc.vecHierarchy.end()); // 부모기준 자식으로 계산해야하므로 자식<->부모 순서 뒤집어주고

    m_vecSockets.push_back(tSocketDesc);
    m_bSocketCacheDirty = true; // 캐시 리빌드 필요

    return S_OK;
}

void Engine::Model::Rebuild_SocketCache()
{
    // 본 인덱스 -> 캐시 인덱스 매핑 (빌드 타임에만 사용, 멤버로 안 들고감)
    unordered_map<_int, _uint> mapBoneToCache;

    for (auto& Socket : m_vecSockets)
    {
        Socket.vecCacheIndices.resize(Socket.vecHierarchy.size());

        for (size_t i = 0; i < Socket.vecHierarchy.size(); ++i)
        {
            _int iBoneIndex = Socket.vecHierarchy[i];

            auto iter = mapBoneToCache.find(iBoneIndex);
            if (iter == mapBoneToCache.end())
            {
                _uint iNewCacheIndex = (_uint)mapBoneToCache.size();
                mapBoneToCache[iBoneIndex] = iNewCacheIndex;
                Socket.vecCacheIndices[i] = iNewCacheIndex;
            }
            else
            {
                Socket.vecCacheIndices[i] = iter->second;
            }
        }
    }

    _uint iCacheSize = (_uint)mapBoneToCache.size();
    m_vecSocketCombinedCache.resize(iCacheSize);
    m_vecSocketCacheFrame.assign(iCacheSize, 0);
    m_iSocketCacheCounter = 1;
    m_bSocketCacheDirty = false;
}

HRESULT Engine::Model::Overwrite_CombinedMatrices(const vector<pair<_int, _float4x4>>& _vecBoneMatrices)
{
    if (!m_pCombinedMatrixBuffer || _vecBoneMatrices.empty())
        return E_FAIL;

    // 각 본 인덱스 위치에 해당하는 행렬만 부분 업데이트
    for (const auto& pair : _vecBoneMatrices)
    {
        _int iBoneIndex = pair.first;
        if (iBoneIndex < 0 || iBoneIndex >= (_int)m_iNumBones)
            continue;

        // 해당 본 인덱스 위치에만 덮어쓰기
        D3D11_BOX destBox = {};
        destBox.left = sizeof(_float4x4) * iBoneIndex;
        destBox.right = destBox.left + sizeof(_float4x4);
        destBox.top = 0;
        destBox.bottom = 1;
        destBox.front = 0;
        destBox.back = 1;

        m_pContext->UpdateSubresource(
            m_pCombinedMatrixBuffer->Get_Buffer(),
            0,
            &destBox,
            &pair.second,
            0,
            0
        );
    }

    return S_OK;
}

HRESULT Engine::Model::Overwrite_CombinedMatrices_Batched(const vector<pair<_int, _float4x4>>& _vecBoneMatrices)
{
    if (!m_pCombinedMatrixBuffer || _vecBoneMatrices.empty())
        return E_FAIL;

    // 인덱스 기준 정렬 (스크래치 버퍼 재사용)
    m_vecSortScratch.assign(_vecBoneMatrices.begin(), _vecBoneMatrices.end());
    sort(m_vecSortScratch.begin(), m_vecSortScratch.end(),
        [](const auto& _a, const auto& _b) { return _a.first < _b.first; });

    // 범위 밖 본 제거
    while (!m_vecSortScratch.empty() && m_vecSortScratch.back().first >= (_int)m_iNumBones)
        m_vecSortScratch.pop_back();

    while (!m_vecSortScratch.empty() && m_vecSortScratch.front().first < 0)
        m_vecSortScratch.erase(m_vecSortScratch.begin());

    if (m_vecSortScratch.empty())
        return S_OK;

    // 연속 구간 탐색 후 한 번에 업데이트
    _uint iStart = 0;
    while (iStart < (_uint)m_vecSortScratch.size())
    {
        _int iBeginBone = m_vecSortScratch[iStart].first;
        _uint iEnd = iStart + 1;

        while (iEnd < (_uint)m_vecSortScratch.size()
            && m_vecSortScratch[iEnd].first == m_vecSortScratch[iEnd - 1].first + 1)
        {
            ++iEnd;
        }

        _uint iCount = iEnd - iStart;

        m_vecBatchScratch.resize(iCount);
        for (_uint i = 0; i < iCount; ++i)
            m_vecBatchScratch[i] = m_vecSortScratch[iStart + i].second;

        D3D11_BOX destBox = {};
        destBox.left = sizeof(_float4x4) * iBeginBone;
        destBox.right = destBox.left + sizeof(_float4x4) * iCount;
        destBox.top = 0;
        destBox.bottom = 1;
        destBox.front = 0;
        destBox.back = 1;

        m_pContext->UpdateSubresource(
            m_pCombinedMatrixBuffer->Get_Buffer(),
            0,
            &destBox,
            m_vecBatchScratch.data(),
            0,
            0
        );

        iStart = iEnd;
    }

    return S_OK;
}

void Engine::Model::Update_Socket()
{
    if (m_vecSockets.empty())
        return;

    if (m_bSocketCacheDirty) // 소켓 변경시 한번만 리빌드
        Rebuild_SocketCache();

    ++m_iSocketCacheCounter;

    //기존 베이스
    Animation* pCurrentAnim = m_vecAnimations[m_iCurrentAnimationIndex];
    _float fCurrentFrame = pCurrentAnim->Get_CurrentFrame();

    _float fLerpRatio = 1.f;
    _bool bLerping = (m_bIsLerping && m_fLerpDuration > 0.f && m_iPrevAnimationIndex < m_iNumAnimations);
    if (bLerping)
        fLerpRatio = min(1.f, m_fLerpTimer / m_fLerpDuration);

    //상체
    _bool bUpperEnable = (m_bUpperBlendEnable && m_iUpperCurrentAnimationIndex >= 0 && m_iUpperCurrentAnimationIndex <= m_iNumAnimations);

    Animation* pUpperCurrentAnim = bUpperEnable ? m_vecAnimations[m_iUpperCurrentAnimationIndex] : nullptr;
    _float fUpperCurrentFrame = bUpperEnable ? pUpperCurrentAnim->Get_CurrentFrame() : 0.f;

    _float fUpperLerpRatio = 1.f;
    _bool bUpperLerping = (bUpperEnable && m_bUpperIsLerping && m_fUpperLerpDuration > 0.f && m_iUpperPrevAnimationIndex < m_iNumAnimations);
    if (bUpperLerping)
        fUpperLerpRatio = min(1.f, m_fUpperLerpTimer / m_fUpperLerpDuration);

    if (m_bSocketLerpStart)
    {
        fLerpRatio = 0.f;
        m_bSocketLerpStart = false;
    }

    if (m_bUpperSocketLerpStart)
    {
        fUpperLerpRatio = 0.f;
        m_bUpperSocketLerpStart = false;
    }

    //베이스
    Animation* pPrevAnim = bLerping ? m_vecAnimations[m_iPrevAnimationIndex] : nullptr;
    _float fPrevFrame = bLerping ? m_fPrevAnimFrameForSocket : 0.f;

    //상체
    Animation* pUpperPrevAnim = nullptr;
    if (bUpperEnable && bUpperLerping &&
        m_iUpperPrevAnimationIndex >= 0 &&
        m_iUpperPrevAnimationIndex < (_int)m_vecAnimations.size())
    {
        pUpperPrevAnim = m_vecAnimations[m_iUpperPrevAnimationIndex];
    }
    _float fUpperPrevFrame = bUpperLerping ? m_fUpperPrevAnimFrameForSocket : 0.f;


    for (auto& Socket : m_vecSockets)
    {
        // 역순회로 캐싱된 조상 탐색 (배열 인덱스 직접 사용, map lookup 없음)
        _int iStartIndex = 0;
        _matrix CombinedMatrix = XMLoadFloat4x4(&m_PreTransformMatrix);

        for (_int i = (_int)Socket.vecHierarchy.size() - 1; i >= 0; --i)
        {
            _uint iCacheIndex = Socket.vecCacheIndices[i];

            if (m_vecSocketCacheFrame[iCacheIndex] == m_iSocketCacheCounter)
            {
                CombinedMatrix = XMLoadFloat4x4(&m_vecSocketCombinedCache[iCacheIndex]);
                iStartIndex = i + 1;
                break;
            }
        }

        // iStartIndex부터만 계산
        for (size_t i = iStartIndex; i < Socket.vecHierarchy.size(); ++i)
        {
            _int iBoneIndex = Socket.vecHierarchy[i];

            _vector vBaseScale, vBaseRotation, vBaseTranslation;

            Evaluate_Pose(pCurrentAnim, fCurrentFrame, m_bIsLoopAnimation,
                pPrevAnim, fPrevFrame, m_bPrevAnimLoopForSocket,
                bLerping, fLerpRatio, iBoneIndex,
                vBaseScale, vBaseRotation, vBaseTranslation);

            // mask 먼저 체크해서 상체 evaluate 스킵
            _float fMask = 0.f;
            if (bUpperEnable && !m_vecUpperMaskWeight.empty() &&
                iBoneIndex >= 0 && iBoneIndex < (_int)m_vecUpperMaskWeight.size())
            {
                fMask = m_vecUpperMaskWeight[iBoneIndex];
            }

            _float fWeight = bUpperEnable ? clamp(m_fUpperLayerWeight * fMask, 0.f, 1.f) : 0.f;

            _matrix LocalMatrix;

            if (fWeight > 0.f) // weight 있을때만 상체 처리
            {
                _vector vUpperScale, vUpperRotation, vUpperTranslation;
            //if (Socket.iTargetBoneIndex == m_tDrapeBones.iLeftArm_BoneIndex)
            //    XMStoreFloat4(&m_vLeftArmRotation,vFinaleRotation);

                Evaluate_Pose(pUpperCurrentAnim, fUpperCurrentFrame, m_bUpperIsLoopAnimation,
                    pUpperPrevAnim, fUpperPrevFrame, m_bUpperPrevAnimLoopForSocket,
                    bUpperLerping, fUpperLerpRatio, iBoneIndex,
                    vUpperScale, vUpperRotation, vUpperTranslation);
            //if(Socket.iTargetBoneIndex == m_tDrapeBones.iRightArm_BoneIndex)
            //    XMStoreFloat4(&m_vRightArmRotation, vFinaleRotation);

                _vector vFinalScale = XMVectorLerp(vBaseScale, vUpperScale, fWeight);
                _vector vFinalRotation = XMQuaternionNormalize(
                    XMQuaternionSlerp(vBaseRotation, vUpperRotation, fWeight));
                _vector vFinalTranslation = XMVectorLerp(vBaseTranslation, vUpperTranslation, fWeight);

                LocalMatrix = XMMatrixAffineTransformation(
                    vFinalScale, XMVectorZero(), vFinalRotation, vFinalTranslation);
            }
            else
            {
                LocalMatrix = XMMatrixAffineTransformation(
                    vBaseScale, XMVectorZero(), vBaseRotation, vBaseTranslation);
            }

            CombinedMatrix = LocalMatrix * CombinedMatrix;

            // 캐시 저장 (배열 직접 접근)
            _uint iCacheIndex = Socket.vecCacheIndices[i];
            XMStoreFloat4x4(&m_vecSocketCombinedCache[iCacheIndex], CombinedMatrix);
            m_vecSocketCacheFrame[iCacheIndex] = m_iSocketCacheCounter;
        }

        XMStoreFloat4x4(&Socket.CombinedMatrix, CombinedMatrix);
    }
}

void Engine::Model::Update_Socket_Simple()
{
    if (m_vecSockets.empty())
        return;

    if (m_bSocketCacheDirty)
        Rebuild_SocketCache();

    ++m_iSocketCacheCounter;

    Animation* pCurrentAnim = m_vecAnimations[m_iCurrentAnimationIndex];
    _float fCurrentFrame = pCurrentAnim->Get_CurrentFrame();

    // 러프 처리
    _float fLerpRatio = 1.f;
    _bool bLerping = (m_bIsLerping && m_fLerpDuration > 0.f && m_iPrevAnimationIndex < m_iNumAnimations);
    if (bLerping)
        fLerpRatio = min(1.f, m_fLerpTimer / m_fLerpDuration);

    if (m_bSocketLerpStart)
    {
        fLerpRatio = 0.f;
        m_bSocketLerpStart = false;
    }

    Animation* pPrevAnim = bLerping ? m_vecAnimations[m_iPrevAnimationIndex] : nullptr;
    _float fPrevFrame = bLerping ? m_fPrevAnimFrameForSocket : 0.f;

    for (auto& Socket : m_vecSockets)
    {
        // 역순회로 캐싱된 조상 탐색
        _int iStartIndex = 0;
        _matrix CombinedMatrix = XMLoadFloat4x4(&m_PreTransformMatrix);

        for (_int i = (_int)Socket.vecHierarchy.size() - 1; i >= 0; --i)
        {
            _uint iCacheIndex = Socket.vecCacheIndices[i];

            if (m_vecSocketCacheFrame[iCacheIndex] == m_iSocketCacheCounter)
            {
                CombinedMatrix = XMLoadFloat4x4(&m_vecSocketCombinedCache[iCacheIndex]);
                iStartIndex = i + 1;
                break;
            }
        }

        // iStartIndex부터만 계산
        for (size_t i = iStartIndex; i < Socket.vecHierarchy.size(); ++i)
        {
            _int iBoneIndex = Socket.vecHierarchy[i];

            _vector vScale = {}, vRotation = {}, vTranslation = {};

            Evaluate_Pose(pCurrentAnim, fCurrentFrame, m_bIsLoopAnimation,
                pPrevAnim, fPrevFrame, m_bPrevAnimLoopForSocket,
                bLerping, fLerpRatio, iBoneIndex,
                vScale, vRotation, vTranslation);

            _matrix LocalMatrix = XMMatrixAffineTransformation(
                vScale, XMVectorZero(), vRotation, vTranslation);

            CombinedMatrix = LocalMatrix * CombinedMatrix;

            // 캐시 저장
            _uint iCacheIndex = Socket.vecCacheIndices[i];
            XMStoreFloat4x4(&m_vecSocketCombinedCache[iCacheIndex], CombinedMatrix);
            m_vecSocketCacheFrame[iCacheIndex] = m_iSocketCacheCounter;
        }

        XMStoreFloat4x4(&Socket.CombinedMatrix, CombinedMatrix);
    }
}

void Model::Evaluate_Pose(Animation* pCurrentAnim, _float fCurrentFrame, _bool bCurrentLoop,Animation* pPrevAnim, _float fPrevFrame, _bool bPrevLoop,
    _bool bIsLerping, _float fLerpRatio, _int iBoneIndex, _vector& vOutScale, _vector& vOutRot, _vector& vOutPos)
{
    if (bIsLerping && pPrevAnim)
    {
        _vector vScale0{}, vRotaion0{}, vTranslation0{};
        _vector vScale1{}, vRotaion1{}, vTranslation1{};

        //_uint DebIndex = Get_BoneIndex("RightOuterOpen");

        pPrevAnim->Get_InterpolatedBonePositionSocket(iBoneIndex, fPrevFrame, bPrevLoop, vScale0, vRotaion0, vTranslation0);
        pCurrentAnim->Get_InterpolatedBonePositionSocket(iBoneIndex, fCurrentFrame, bCurrentLoop, vScale1, vRotaion1, vTranslation1);

        // 안전 normalize
        vRotaion0 = XMQuaternionNormalize(vRotaion0);
        vRotaion1 = XMQuaternionNormalize(vRotaion1);

        _float fRatio = clamp(fLerpRatio, 0.f, 1.f);

        vOutScale = XMVectorLerp(vScale0, vScale1, fRatio);
        vOutRot = XMQuaternionNormalize(XMQuaternionSlerp(vRotaion0, vRotaion1, fRatio));
        vOutPos = XMVectorLerp(vTranslation0, vTranslation1, fRatio);
    }
    else
    {
        pCurrentAnim->Get_InterpolatedBonePositionSocket(iBoneIndex, fCurrentFrame, bCurrentLoop, vOutScale, vOutRot, vOutPos);
        vOutRot = XMQuaternionNormalize(vOutRot);
    }
}


const _float4x4* Engine::Model::Get_SocketBoneMatrixPtr(const _string& _boneName)
{
    for (auto& Socket : m_vecSockets)
    {
        if (Socket.strBoneName == _boneName)
            return &Socket.CombinedMatrix;
    }
  
    return nullptr;
}

const _float4x4* Engine::Model::Get_SocketBoneMatrixPtr_Index(const _string& _boneName)
{
    _int boneIndex = Get_SocketBoneIndex(_boneName);
    return &m_vecSockets[boneIndex].CombinedMatrix;
}

_int Engine::Model::Get_SocketBoneIndex(const _string& _boneName)
{
    for (_int i = 0; i < (_int)m_vecSockets.size(); ++i)
    {
        if (m_vecSockets[i].strBoneName == _boneName)
            return i;
    }
    return -1;
}

const _float4x4* Engine::Model::Get_SocketBoneMatrixPtr(_int _socketIndex)
{
    return &m_vecSockets[_socketIndex].CombinedMatrix;
}



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Model::Override_KeyFrames_FromFemaleBindPose()
{
    // 모델 임시 로드 (바인드포즈 로컬 행렬만 필요)
    Model* femaleModel = Model::Create(m_pDevice, m_pContext, MODEL::ANIM,
        L"../../Resources/Models/OverrideModel/FemaleBase.siho", XMMatrixIdentity(), MODELROLE::STANDALONE);

    if (femaleModel == nullptr)
        return E_FAIL;

    // 덮어씌울 본 이름들
    vector<_string> vecTargetBoneNames =
    {
        "RightBust",
        "RightBust1",
        "Attach_RightBust",
        "LeftBust",
        "LeftBust1",
        "Attach_LeftBust",
    };

    // 덮어씌울 본들 찾아서 각각 덮어주기
    for (const auto& boneName : vecTargetBoneNames)
    {
        // 이 모델의 본 인덱스번호 탐색
        _int boneIndex = Get_BoneIndex(boneName.c_str());
        if (boneIndex == -1)
            continue;

        // 여성 모델에서 본 가져오기
        Bone* femaleBone = femaleModel->Get_Bone_ByName(boneName);
        if (femaleBone == nullptr)
            continue;

        // 본 행렬 가져오기(레스트포즈 기본값)
        _float4x4 femaleBindPose = femaleBone->Get_TransformationMatrix();

        // 크자이(SRT) 추출
        _vector vScale = {};
        _vector vRotation = {};
        _vector vPosition = {};
        XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&femaleBindPose));

        _float3 scale = {};
        _float4 rotation = {};
        _float3 position = {};
        XMStoreFloat3(&scale, vScale);
        XMStoreFloat4(&rotation, vRotation);
        XMStoreFloat3(&position, vPosition);

        // 모든 애니메이션 키프레임에 덮어씌우기
        for (_uint i = 0; i < m_iNumAnimations; ++i)
        {
            _int iChannelIndex = m_vecAnimations[i]->Get_ChannelByBoneIndex(boneIndex);
            if (iChannelIndex == -1)
                continue;

            Channel* pChannel = m_vecAnimations[i]->Get_Channel()[iChannelIndex];
            vector<KEYFRAME>& vecKeyFrames = pChannel->Get_KeyFrames();

            for (auto& keyframe : vecKeyFrames)
            {
                keyframe.vScale = scale;
                keyframe.vRotation = rotation;
                keyframe.vPosition = position;
            }
        }
    }

    Safe_Release(femaleModel);

    //// 모델 임시 로드 (바인드포즈 로컬 행렬만 필요)
    //Model* femaleDrape = Model::Create(m_pDevice, m_pContext, MODEL::ANIM,
    //    L"../../Resources/Models/OverrideModel/Drape/Drape5.siho", XMMatrixIdentity(), MODELROLE::STANDALONE);

    //if (femaleDrape == nullptr)
    //    return E_FAIL;

    //// 덮어씌울 본 이름들
    //vector<_string> vecClothBoneNames =
    //{
    //    "Temp_GCLeftDrape",
    //    "Temp_GCLeftDrape1",
    //    "LeftOuterOpen",
    //    "LeftOuterOpen1",
    //    "LeftOuterUp",
    //    "Temp_GCRightDrape",
    //    "Temp_GCRightDrape1",
    //    "RightOuterOpen",
    //    "RightOuterOpen1",
    //    "RightOuterUp",
    //};

    //// 덮어씌울 본들 찾아서 각각 덮어주기
    //for (const auto& boneName : vecClothBoneNames)
    //{
    //    // 이 모델의 본 인덱스번호 탐색
    //    _int boneIndex = Get_BoneIndex(boneName.c_str());
    //    if (boneIndex == -1)
    //        continue;

    //    // femaleDrape 모델에서 본 가져오기
    //    Bone* drapeBone = femaleDrape->Get_Bone_ByName(boneName);
    //    if (drapeBone == nullptr)
    //        continue;

    //    // 본 행렬 가져오기(레스트포즈 기본값)
    //    _float4x4 drapeBindPose = drapeBone->Get_TransformationMatrix();

    //    // 크자이(SRT) 추출
    //    _vector vScale = {};
    //    _vector vRotation = {};
    //    _vector vPosition = {};
    //    XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&drapeBindPose));

    //    _float3 scale = {};
    //    _float4 rotation = {};
    //    _float3 position = {};
    //    XMStoreFloat3(&scale, vScale);
    //    XMStoreFloat4(&rotation, vRotation);
    //    XMStoreFloat3(&position, vPosition);

    //    // 모든 애니메이션 키프레임에 덮어씌우기
    //    for (_uint i = 0; i < m_iNumAnimations; ++i)
    //    {
    //        _int iChannelIndex = m_vecAnimations[i]->Get_ChannelByBoneIndex(boneIndex);
    //        if (iChannelIndex == -1)
    //            continue;

    //        Channel* pChannel = m_vecAnimations[i]->Get_Channel()[iChannelIndex];
    //        vector<KEYFRAME>& vecKeyFrames = pChannel->Get_KeyFrames();

    //        for (auto& keyframe : vecKeyFrames)
    //        {
    //            keyframe.vScale = scale;
    //            keyframe.vRotation = rotation;
    //            keyframe.vPosition = position;
    //        }
    //    }
    //}

    //Safe_Release(femaleDrape);

    return S_OK;
}

HRESULT Engine::Model::Override_KeyFrames_FromBindPose(const RESETPOSE_DESC& tResetPoseDesc)
{
    if (tResetPoseDesc.wstrFilePath.empty() || tResetPoseDesc.vecTargetBoneNames.empty())
        return S_OK;

    // 바인드포즈 참조용 모델 임시 로드
    Model* pBindPoseModel = Model::Create(m_pDevice, m_pContext, MODEL::ANIM,
        tResetPoseDesc.wstrFilePath.c_str(), XMMatrixIdentity(), MODELROLE::STANDALONE);
    if (pBindPoseModel == nullptr)
        return E_FAIL;

    for (const auto& boneName : tResetPoseDesc.vecTargetBoneNames)
    {
        // 이 모델의 본 인덱스번호 탐색
        _int boneIndex = Get_BoneIndex(boneName.c_str());
        if (boneIndex == -1)
            continue;

        // 바인드포즈 모델에서 본 가져오기
        Bone* pBindPoseBone = pBindPoseModel->Get_Bone_ByName(boneName);
        if (pBindPoseBone == nullptr)
            continue;

        // 본 행렬 가져오기(레스트포즈 기본값)
        _float4x4 bindPoseMatrix = pBindPoseBone->Get_TransformationMatrix();

        // 크자이(SRT) 추출
        _vector vScale = {};
        _vector vRotation = {};
        _vector vPosition = {};
        XMMatrixDecompose(&vScale, &vRotation, &vPosition, XMLoadFloat4x4(&bindPoseMatrix));

        _float3 scale = {};
        _float4 rotation = {};
        _float3 position = {};
        XMStoreFloat3(&scale, vScale);
        XMStoreFloat4(&rotation, vRotation);
        XMStoreFloat3(&position, vPosition);

        // 모든 애니메이션 키프레임에 덮어씌우기
        for (_uint i = 0; i < m_iNumAnimations; ++i)
        {
            _int iChannelIndex = m_vecAnimations[i]->Get_ChannelByBoneIndex(boneIndex);
            if (iChannelIndex == -1)
                continue;

            Channel* pChannel = m_vecAnimations[i]->Get_Channel()[iChannelIndex];
            vector<KEYFRAME>& vecKeyFrames = pChannel->Get_KeyFrames();

            for (auto& keyframe : vecKeyFrames)
            {
                keyframe.vScale = scale;
                keyframe.vRotation = rotation;
                keyframe.vPosition = position;
            }
        }
    }

    Safe_Release(pBindPoseModel);

    return S_OK;
}


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Model* Engine::Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL _modeltype, const _wstring& _filepath, const _fmatrix& _preTransformMatrix, MODELROLE _RoleType, _bool bNoneAnimFlag)
{
    Model* pInstance = new Model(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(_modeltype, _filepath, _preTransformMatrix, _RoleType, bNoneAnimFlag), L"Model 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Component* Engine::Model::Clone(void* arg)
{
    Model* pInstance = new Model(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"Model 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Model::Free()
{
    __super::Free();

    for (auto& mesh : m_vecMeshes)
    {
        Safe_Release(mesh);
    }
    m_vecMeshes.clear();

    for (auto& material : m_vecMaterials)
    {
        Safe_Release(material);
    }
    m_vecMaterials.clear();

    for (auto& bone : m_vecBones)
    {
        Safe_Release(bone);
    }
    m_vecBones.clear();

    for (auto& animation : m_vecAnimations)
    {
        Safe_Release(animation);
    }
    m_vecAnimations.clear();

    if (m_pAIScene)
    {
        m_Importer.FreeScene();
    }

    for (auto& PartModel : m_vecPartModesl)
    {
        Safe_Release(PartModel);
    }
    m_vecPartModesl.clear();

    Safe_Release(m_pLocalMatricesBuffer);
    Safe_Release(m_pHierarchyBuffer);
    Safe_Release(m_pCombinedMatrixBuffer);
    Safe_Release(m_pOffsetBuffer);
    Safe_Release(m_pOffsetSRV);
    Safe_Release(m_pCShader);
    Safe_Release(m_pKeyFrameBuffer);
    Safe_Release(m_pChannelBuffer);
    Safe_Release(m_pAnimInfoBuffer);
    Safe_Release(m_pCShaderLocalMatrix);
    Safe_Release(m_pFrozenSRTBuffer);
    Safe_Release(m_pLocalSRTBuffer);
    Safe_Release(m_pCurrentKeyIndexBuffer);
    Safe_Release(m_pBindPoseSRTBuffer);
    Safe_Release(m_pUpperBodyMaskWeightBuffer);
    Safe_Release(m_pUpperCurrentKeyIndexBuffer);
    Safe_Release(m_pDrapeDeltaBuffer);
    Safe_Release(m_pCShaderDrivenBone);

    Safe_Release(m_pReadbackStagingBuffer);
    Safe_Release(m_pReadbackOutBuffer);
    Safe_Release(m_pReadbackOutUAV);
    Safe_Release(m_pCShaderReadback);

#ifdef _DEBUG

#endif // _DEBUG


}
/******************************************************* 객체 반환 함수 *******************************************************/