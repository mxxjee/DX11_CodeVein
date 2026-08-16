#include "Engine_Define.h"
#include "PhysXManager.h"

#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"
#include "PhysX_Function.h"

#include <omnipvd/PxOmniPvd.h>
#include <omnipvd/OmniPvdDefines.h>
#include <omnipvd/OmniPvdWriter.h>
#include <omnipvd/OmniPvdWriteStream.h>
#include <omnipvd/OmniPvdFileWriteStream.h>
#include <omnipvd/OmniPvdFileReadStream.h>


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::PhysXManager::PhysXManager()
{
}

Engine::PhysXManager::~PhysXManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::PhysXManager::Initialize()
{
    // Foundation 생성 (메모리, 에러 처리)
    m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);
    if (!m_pFoundation) return E_FAIL;

    // PVD (비주얼 디버거) 연결 설정
    // 디버그 모드일 때만 연결 (최적화)
#ifdef _DEBUG
    if(g_bOmniPVDRecording)
    { 
        m_pPvd = PxCreatePvd(*m_pFoundation);

        // 로컬호스트(내 컴퓨터)의 5425 포트로 연결 시도
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        if (!m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eDEBUG))
        {
            // 연결 실패 시 정리
            Safe_PhysX_Release(m_pPvd);
            transport->release();
            COUT("PVD 연결 실패 - PVD 없이 진행");
        }
        // 네트워크 대신 파일 기록 방식
        //PxPvdTransport* transport = PxDefaultPvdFileTransportCreate("pvd_capture.pvd");
        //m_pPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

        // OmniPvd 레코딩
        /*if(g_bOmniPVDRecording == true)
            m_pOmniPvd = PxCreateOmniPvd(*m_pFoundation);
        if (m_pOmniPvd)
        {
            OmniPvdWriter* omniWriter = m_pOmniPvd->getWriter();
            OmniPvdFileWriteStream* fileStream = m_pOmniPvd->getFileWriteStream();

            if (omniWriter && fileStream)
            {
                omniWriter->setWriteStream(*fileStream);
            }
        }*/
    }
#endif

    // Physics (물리 객체 생성 공장) 생성
    // PVD가 있으면 연결해서 만들고, 없으면 그냥 만듦
    m_pPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_pFoundation, PxTolerancesScale(), true, m_pPvd, m_pOmniPvd);
    if (!m_pPhysics) return E_FAIL;

#ifdef _DEBUG
    // Physics 생성 후 파일명 설정 및 샘플링 시작
    /*if (m_pOmniPvd)
    {
        OmniPvdFileWriteStream* fileStream = m_pOmniPvd->getFileWriteStream();
        if (fileStream)
        {
            fileStream->setFileName("physx_debug.ovd");
        }
        m_pOmniPvd->startSampling();
        COUT("OmniPVD 샘플링 시작");
    }*/
#endif

    // Scene (물리 월드) 설정
    PxSceneDesc sceneDesc(m_pPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f); // 중력 설정 (Y축 아래로)

    // CPU 스레드 디스패처 (2개의 작업 스레드 사용)
    m_pDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = m_pDispatcher;
    sceneDesc.filterShader = CustomFilterShader; // 커스텀 필터 셰이더 적용 << 이 부분이 중요함 ////////////////////////////

    m_pScene = m_pPhysics->createScene(sceneDesc);
    if (!m_pScene) return E_FAIL;

    // 씬 정보를 디버거로 전송하도록 설정
    if (m_pPvd)
    {
        PxPvdSceneClient* pPvdClient = m_pScene->getScenePvdClient();
        if (pPvdClient)
        {
            pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, false);
            pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
            pPvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, false);
            COUT("PhysX PVD 연동 성공");
        }
    }

    // Controller Manager (캐릭터 컨트롤러 관리자) 생성
    m_pControllerManager = PxCreateControllerManager(*m_pScene);
    if (!m_pControllerManager) return E_FAIL;

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::PhysXManager::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
void Engine::PhysXManager::Update(const _float fTimeDelta)
{
    if (nullptr == m_pScene)
        return;

    /* [중요] 물리 시뮬레이션 단계 */
    // 시뮬레이션 시작
    m_pScene->simulate(fTimeDelta);

    // 끝날 때까지 대기 (fetchResults의 true는 완료될 때까지 블로킹한다는 뜻)
    // 성능 최적화를 위해선 simulate와 fetch 사이에 렌더링 로직을 넣기도 한다는데 일단 잘 모르니까 패스
    m_pScene->fetchResults(true);
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::PhysXManager::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::PhysXManager::Render(const _float fTimeDelta)
{
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 쿠킹 함수 Triangle ////////////////////////////////////////////////////////
HRESULT Engine::PhysXManager::PhysX_Cooking_Static_Model(PHYSX_ACTOR_DESC& _actorDesc)
{
    if (!m_pPhysics)
    {
        MSG_ON(L"PhysX가 없거나 초기화 되지 않았습니다", L"아니 이러면 안 돼요");
        BREAK;
        return E_FAIL;
    }

    MSG_NULL(_actorDesc.pModel, L"모델이 업대...", L"피직스 슬퍼..", E_FAIL);

    // 캐시 폴더 확보(없으면 새로 만들기)
    if (!filesystem::exists(m_strCacheDir))
        filesystem::create_directories(m_strCacheDir);

    Model* model = _actorDesc.pModel;

    filesystem::path modelPath(model->Get_FilePath());
    _wstring modelName = modelPath.stem().wstring();

    auto& vecMeshes = model->Get_Meshes();
    _uint meshIndex = 0;

    for (Mesh* mesh : vecMeshes)
    {
        if (mesh == nullptr)
        {
            ++meshIndex;
            continue;
        }

        _wstring cachePath = Make_Cache_Path(modelName, meshIndex);

        // 이미 있으면 메쉬번호 올리고 패스
        if (filesystem::exists(cachePath))
        {
            ++meshIndex;
            continue;
        }

        // 쿠킹 후 저장
        PxTriangleMesh* cooked = Cook_And_Save_Mesh(mesh, cachePath);

        // 맵툴에서 저장할땐 필요없으니까 바로 릴리즈
        if (cooked)
            cooked->release();

        ++meshIndex;
    }

    return S_OK;
}

PxRigidStatic* Engine::PhysXManager::PhysX_Load_Static_Actor(PHYSX_ACTOR_DESC& _actorDesc)
{
    if (!m_pPhysics || !m_pScene || !_actorDesc.pModel)
    {
        MSG_ON(L"피직스 너 싫다", L"로드 안 되게 하는 너 싫다");
        return nullptr;
    }

    // 캐시 폴더 확보
    if (!filesystem::exists(m_strCacheDir))
        filesystem::create_directories(m_strCacheDir);

    // 행렬 분해
    _matrix worldMatrix = XMLoadFloat4x4(&_actorDesc.matWorld);
    _vector scale = {}, quaternion = {}, position = {};
    if (!XMMatrixDecompose(&scale, &quaternion, &position, worldMatrix))
        return nullptr;

    // 피직스 마테리얼 생성
    PxMaterial* material = m_pPhysics->createMaterial(
        _actorDesc.fStaticFriction,      // 정적 마찰 계수 
        _actorDesc.fDynamicFriction,     // 동적 마찰 계수
        _actorDesc.fRestitution);        // 반발 계수(탄성)

    // 정적 액터 생성
    PxTransform actorPose(ToPxVec3(position), ToPxQuaternion(quaternion));
    PxRigidStatic* actor = m_pPhysics->createRigidStatic(actorPose);

    if (actor == nullptr)
    {
        material->release();
        MSG_ON(L"정적 액터 생성 실패!", L"않되");
        BREAK;
        return nullptr;
    }

    // 식별 정보 부착
    PHYSX_USER_DATA* pUserData = new PHYSX_USER_DATA;
    pUserData->eActorType = PX_ACTOR_TYPE::STATIC_MAP;
    pUserData->iObjectID = _actorDesc.iObjectID;
    pUserData->pOwner = _actorDesc.pOwner;
    actor->userData = pUserData;

    PxFilterData filterData = PhysXFilter::Make_FilterData(PX_ACTOR_TYPE::STATIC_MAP);

    // 모델명 추출
    Model* model = _actorDesc.pModel;
    filesystem::path modelPath = model->Get_FilePath();
    _wstring modelName = modelPath.stem().wstring();

    // 메쉬 가져오기
    auto& vecMeshes = model->Get_Meshes();
    _uint meshIndex = 0;
    PxMeshScale pxScale(ToPxVec3(scale), PxQuat(PxIdentity));

    for (Mesh* mesh : vecMeshes)
    {
        if (mesh == nullptr)
        {
            ++meshIndex;
            continue;
        }

        _wstring cachePath = Make_Cache_Path(modelName, meshIndex);
        
        //PxTriangleMesh* pxMesh = nullptr;
        //
        //pxMesh = Load_Cooked_Mesh(cachePath);
        //
        //if (pxMesh == nullptr)
        //    pxMesh = Cook_And_Save_Mesh(mesh, cachePath);
        //
        //// Shape 생성 및 액터에 부착
        //if (pxMesh)
        //{
        //    PxShape* pShape = m_pPhysics->createShape(
        //        PxTriangleMeshGeometry(pxMesh, pxScale), *material);
        //
        //    if (pShape)
        //    {
        //        pShape->setSimulationFilterData(filterData);
        //        pShape->setQueryFilterData(filterData);
        //        actor->attachShape(*pShape);
        //        pShape->release();
        //    }
        //    pxMesh->release();
        //}
        
        PxConvexMesh* pxMesh = nullptr;

        // 먼저 캐시 로드
        pxMesh = Load_Cooked_ConvexMesh(cachePath);

        // 없으면 새로 Convex로 쿠킹
        if (pxMesh == nullptr)
            pxMesh = Cook_And_Save_ConvexMesh(mesh, cachePath);

        // Shape 생성 및 액터에 부착 (Triangle -> Convex)
        if (pxMesh)
        {
            // PxTriangleMeshGeometry -> PxConvexMeshGeometry로 변경
            PxShape* pShape = m_pPhysics->createShape(
                PxConvexMeshGeometry(pxMesh, pxScale), *material);

            if (pShape)
            {
                pShape->setSimulationFilterData(filterData);
                pShape->setQueryFilterData(filterData);
                actor->attachShape(*pShape);
                pShape->release();
            }
            pxMesh->release();
        }

        meshIndex++;
    }

    // 씬에 등록
    m_pScene->addActor(*actor);
    material->release();

    return actor;
}

PxTriangleMesh* Engine::PhysXManager::Cook_And_Save_Mesh(Mesh* _mesh, const _wstring& _savePath)
{
    CHECK_NULL_RESULT(_mesh, nullptr);

    // PhysX 단위 설정(kg, km 등등)
    PxCookingParams cookingParams(m_pPhysics->getTolerancesScale());

    // 메쉬 정보 등록
    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count = _mesh->GetNumVertices();
    meshDesc.points.stride = sizeof(_float3);
    meshDesc.points.data = _mesh->Get_VertexPositions();

    meshDesc.triangles.count = _mesh->Get_NumIndices() / 3;
    meshDesc.triangles.stride = sizeof(_uint) * 3;
    meshDesc.triangles.data = _mesh->Get_Indices();

    // 메쉬 베이킹
    PxDefaultMemoryOutputStream writeBuffer;
    if (!PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer))
    {
        MSG_ON(L"메쉬 베이킹 실패!", L"쿠킹 실패");
        BREAK;
        return nullptr;
    }

    // 캐시 파일로 저장
    ofstream saveFile(_savePath, ios::binary);
    if(saveFile.is_open())
    {
        saveFile.write((PCHAR)writeBuffer.getData(), writeBuffer.getSize());
        saveFile.close();
    }

    // 쿠킹된 데이터로 메쉬 생성
    PxDefaultMemoryInputData inputData{ writeBuffer.getData(), writeBuffer.getSize() };

    COUT("모델 Triangle로 쿠킹됨");

    // 리턴
    return m_pPhysics->createTriangleMesh(inputData);
}

PxTriangleMesh* Engine::PhysXManager::Load_Cooked_Mesh(const _wstring& strPath)
{
    // .physx파일 있는지 검사
    if (!filesystem::exists(strPath))
        return nullptr;

    // 있으면 읽기
    ifstream readFile(strPath, ios::binary);
    if (!readFile.is_open())
        return nullptr;

    // 파일 크기 확인
    readFile.seekg(0, ios::end);            // 커서를 파일 제일 끝으로 보냄
    streamsize fileSize = readFile.tellg(); // 커서의 위치 확인(파일 크기 확인)
    readFile.seekg(0, ios::beg);            // 커서 다시 처음으로 보냄

    // 뭣 파일 사이즈가 0이라고 말이 안 되잖아ㅋㅋ
    if (fileSize <= 0)
        return nullptr;

    // 파일 복-사
    vector<_char> buffer(fileSize);
    if (!readFile.read(buffer.data(), fileSize))
        return nullptr;

    // 복사한 파일을 physx용으로 만들기
    PxDefaultMemoryInputData inputData((PxU8*)buffer.data(), (PxU32)fileSize);

    // 만들어진 메쉬 반환
    return m_pPhysics->createTriangleMesh(inputData);
}
/******************************************************* 쿠킹 함수 Convex *******************************************************/



//////////////////////////////////////////////////////// 쿠킹 함수 Convex ////////////////////////////////////////////////////////
PxRigidStatic* Engine::PhysXManager::PhysX_Load_Static_Actor_ConvexMesh(PHYSX_ACTOR_DESC& _actorDesc)
{
    if (!m_pPhysics || !m_pScene || !_actorDesc.pModel)
    {
        MSG_ON(L"피직스 너 싫다", L"로드 안 되게 하는 너 싫다");
        return nullptr;
    }

    // 캐시 폴더 확보
    if (!filesystem::exists(m_strCacheDir))
        filesystem::create_directories(m_strCacheDir);

    // 행렬 분해
    _matrix worldMatrix = XMLoadFloat4x4(&_actorDesc.matWorld);
    _vector scale = {}, quaternion = {}, position = {};
    if (!XMMatrixDecompose(&scale, &quaternion, &position, worldMatrix))
        return nullptr;

    // 피직스 마테리얼 생성
    PxMaterial* material = m_pPhysics->createMaterial(
        _actorDesc.fStaticFriction,      // 정적 마찰 계수 
        _actorDesc.fDynamicFriction,     // 동적 마찰 계수
        _actorDesc.fRestitution);        // 반발 계수(탄성)

    // 정적 액터 생성
    PxTransform actorPose(ToPxVec3(position), ToPxQuaternion(quaternion));
    PxRigidStatic* actor = m_pPhysics->createRigidStatic(actorPose);

    if (actor == nullptr)
    {
        material->release();
        MSG_ON(L"정적 액터 생성 실패!", L"않되");
        BREAK;
        return nullptr;
    }

    // 식별 정보 부착
    PHYSX_USER_DATA* pUserData = new PHYSX_USER_DATA;
    pUserData->eActorType = PX_ACTOR_TYPE::STATIC_MAP;
    pUserData->iObjectID = _actorDesc.iObjectID;
    pUserData->pOwner = _actorDesc.pOwner;
    actor->userData = pUserData;

    PxFilterData filterData = PhysXFilter::Make_FilterData(PX_ACTOR_TYPE::STATIC_MAP);

    // 모델명 추출
    Model* model = _actorDesc.pModel;
    filesystem::path modelPath = model->Get_FilePath();
    _wstring modelName = modelPath.stem().wstring();

    // 메쉬 가져오기
    auto& vecMeshes = model->Get_Meshes();
    _uint meshIndex = 0;
    PxMeshScale pxScale(ToPxVec3(scale), PxQuat(PxIdentity));

    for (Mesh* mesh : vecMeshes)
    {
        if (mesh == nullptr)
        {
            ++meshIndex;
            continue;
        }

        _wstring cachePath = Make_Cache_Path(modelName, meshIndex);
        PxConvexMesh* pxMesh = nullptr;

        pxMesh = Load_Cooked_ConvexMesh(cachePath);

        if (pxMesh == nullptr)
            pxMesh = Cook_And_Save_ConvexMesh(mesh, cachePath);

        // Shape 생성 및 액터에 부착
        if (pxMesh)
        {
            PxShape* pShape = m_pPhysics->createShape(
                PxConvexMeshGeometry(pxMesh, pxScale), *material);

            if (pShape)
            {
                pShape->setSimulationFilterData(filterData);
                pShape->setQueryFilterData(filterData);
                actor->attachShape(*pShape);
                pShape->release();
            }
            pxMesh->release();
        }

        meshIndex++;
    }

    // 씬에 등록
    m_pScene->addActor(*actor);
    material->release();

    return actor;
}

PxConvexMesh* Engine::PhysXManager::Cook_And_Save_ConvexMesh(Mesh* _mesh, const _wstring& _savePath)
{
    CHECK_NULL_RESULT(_mesh, nullptr);

    PxCookingParams cookingParams(m_pPhysics->getTolerancesScale());
    cookingParams.convexMeshCookingType = PxConvexMeshCookingType::eQUICKHULL;

    PxConvexMeshDesc convexDesc;
    convexDesc.points.count = _mesh->GetNumVertices();
    convexDesc.points.stride = sizeof(_float3);
    convexDesc.points.data = _mesh->Get_VertexPositions();

    // 핵심 최적화 옵션!
    // eCOMPUTE_CONVEX: 물리 엔진이 알아서 볼록 다면체로 형태를 계산해서 만들어줍니다.
    // eSHIFT_VERTICES: 정점 데이터의 중심축을 최적화해줍니다.
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eSHIFT_VERTICES;

    // 정점 수 제한 (기본값 255 -> 64로 낮춤)
    // 이 숫자가 작을수록 엄청나게 빨리 구워지지만 둥근 모델이 각지게 변합니다. 
    // 만약 지형이 너무 뭉툭해져서 이상하다면 128이나 255로 올려주세요.
    convexDesc.vertexLimit = 128;

    // 메쉬 베이킹
    PxDefaultMemoryOutputStream writeBuffer;
    if (!PxCookConvexMesh(cookingParams, convexDesc, writeBuffer))
    {
        MSG_ON(L"Convex 메쉬 베이킹 실패!", L"쿠킹 실패");
        BREAK;
        return nullptr;
    }

    ofstream saveFile(_savePath, ios::binary);
    if (saveFile.is_open())
    {
        saveFile.write((PCHAR)writeBuffer.getData(), writeBuffer.getSize());
        saveFile.close();
    }

    PxDefaultMemoryInputData inputData{ writeBuffer.getData(), writeBuffer.getSize() };

    COUT("모델 Convex로 쿠킹됨");

    return m_pPhysics->createConvexMesh(inputData);
}

PxConvexMesh* Engine::PhysXManager::Load_Cooked_ConvexMesh(const _wstring& strPath)
{
    if (!filesystem::exists(strPath)) return nullptr;

    ifstream readFile(strPath, ios::binary);
    if (!readFile.is_open()) return nullptr;

    readFile.seekg(0, ios::end);
    streamsize fileSize = readFile.tellg();
    readFile.seekg(0, ios::beg);

    if (fileSize <= 0) return nullptr;

    vector<_char> buffer(fileSize);
    if (!readFile.read(buffer.data(), fileSize)) return nullptr;

    PxDefaultMemoryInputData inputData((PxU8*)buffer.data(), (PxU32)fileSize);

    return m_pPhysics->createConvexMesh(inputData);
}
/******************************************************* 쿠킹 함수 Convex *******************************************************/



//////////////////////////////////////////////////////// 쿠킹 함수 Auto ////////////////////////////////////////////////////////
PxRigidStatic* Engine::PhysXManager::PhysX_Load_Static_Actor_Auto(PHYSX_ACTOR_DESC& _actorDesc)
{
    if (!m_pPhysics || !m_pScene || !_actorDesc.pModel)
    {
        MSG_ON(L"피직스 너 싫다", L"로드 안 되게 하는 너 싫다");
        return nullptr;
    }

    if (!filesystem::exists(m_strCacheDir))
        filesystem::create_directories(m_strCacheDir);

    // 행렬 분해
    _matrix worldMatrix = XMLoadFloat4x4(&_actorDesc.matWorld);
    _vector scale = {}, quaternion = {}, position = {};
    if (!XMMatrixDecompose(&scale, &quaternion, &position, worldMatrix))
        return nullptr;

    PxMaterial* material = m_pPhysics->createMaterial(
        _actorDesc.fStaticFriction,
        _actorDesc.fDynamicFriction,
        _actorDesc.fRestitution);

    PxTransform actorPose(ToPxVec3(position), ToPxQuaternion(quaternion));
    PxRigidStatic* actor = m_pPhysics->createRigidStatic(actorPose);

    if (actor == nullptr)
    {
        material->release();
        MSG_ON(L"정적 액터 생성 실패!", L"않되");
        BREAK;
        return nullptr;
    }

    PHYSX_USER_DATA* pUserData = new PHYSX_USER_DATA;
    pUserData->eActorType = PX_ACTOR_TYPE::STATIC_MAP;
    pUserData->iObjectID = _actorDesc.iObjectID;
    pUserData->pOwner = _actorDesc.pOwner;
    actor->userData = pUserData;

    PxFilterData filterData = PhysXFilter::Make_FilterData(PX_ACTOR_TYPE::STATIC_MAP);

    Model* model = _actorDesc.pModel;
    filesystem::path modelPath = model->Get_FilePath();
    _wstring modelName = modelPath.stem().wstring();

    auto& vecMeshes = model->Get_Meshes();
    _uint meshIndex = 0;
    PxMeshScale pxScale(ToPxVec3(scale), PxQuat(PxIdentity));

    for (Mesh* mesh : vecMeshes)
    {
        if (mesh == nullptr)
        {
            ++meshIndex;
            continue;
        }

        _wstring cachePath = Make_Cache_Path(modelName, meshIndex);
        PxShape* pShape = nullptr;

        // 캐시 파일에서 자동 타입 판별 로드
        pShape = Load_Shape_Auto(cachePath, pxScale, material);

        // 캐시 없으면 기본 Convex로 쿠킹
        if (pShape == nullptr)
        {
            PxConvexMesh* pxMesh = Cook_And_Save_ConvexMesh(mesh, cachePath);
            if (pxMesh)
            {
                pShape = m_pPhysics->createShape(
                    PxConvexMeshGeometry(pxMesh, pxScale), *material);
                pxMesh->release();
            }
        }

        if (pShape)
        {
            pShape->setSimulationFilterData(filterData);
            pShape->setQueryFilterData(filterData);
            actor->attachShape(*pShape);
            pShape->release();
        }

        meshIndex++;
    }

    m_pScene->addActor(*actor);
    material->release();

    return actor;
}

PxShape* Engine::PhysXManager::Load_Shape_Auto(const _wstring& _cachePath, const PxMeshScale& _scale, PxMaterial* _material)
{
    // 캐시 파일 읽기
    ifstream file(_cachePath, ios::binary | ios::ate);
    if (!file.is_open())
        return nullptr;

    streamsize fileSize = file.tellg();
    file.seekg(0, ios::beg);

    vector<PxU8> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize))
        return nullptr;

    file.close();

    // TriangleMesh로 시도
    {
        PxDefaultMemoryInputData inputData(buffer.data(), (_uint)fileSize);
        PxTriangleMesh* triMesh = m_pPhysics->createTriangleMesh(inputData);

        if (triMesh)
        {
            PxShape* pShape = m_pPhysics->createShape(
                PxTriangleMeshGeometry(triMesh, _scale), *_material);
            triMesh->release();
            return pShape;
        }
    }

    // ConvexMesh로 시도
    {
        PxDefaultMemoryInputData inputData(buffer.data(), (_uint)fileSize);
        PxConvexMesh* cvxMesh = m_pPhysics->createConvexMesh(inputData);

        if (cvxMesh)
        {
            PxShape* pShape = m_pPhysics->createShape(
                PxConvexMeshGeometry(cvxMesh, _scale), *_material);
            cvxMesh->release();
            return pShape;
        }
    }

    return nullptr;
}
/******************************************************* 쿠킹 함수 Auto *******************************************************/



//////////////////////////////////////////////////////// 컨트롤러 생성 함수 ////////////////////////////////////////////////////////
PxController* Engine::PhysXManager::PhysX_Create_Controller(const PHYSX_CONTROLLER_DESC& _controllerDesc)
{
    if (!m_pControllerManager || !m_pPhysics)
        return nullptr;

    // 컨트롤러용 머티리얼 (바닥과의 마찰)
    PxMaterial* pMaterial = m_pPhysics->createMaterial(_controllerDesc.vMaterial.x, _controllerDesc.vMaterial.y, _controllerDesc.vMaterial.z);

    PxCapsuleControllerDesc capsuleDesc;
    capsuleDesc.position = _controllerDesc.vPosition;
    capsuleDesc.radius = _controllerDesc.fRadius;                            // 캡슐의 넓이
    capsuleDesc.height = _controllerDesc.fHeight;                            // 캡슐의 높이
    capsuleDesc.stepOffset = _controllerDesc.fStepOffset;                    // 이 높이 이하의 턱은 자동으로 올라감
    capsuleDesc.slopeLimit = cosf(PxDegToRad(_controllerDesc.fSlopeLimit));  // 도 단위로 받아서 cos 변환
    capsuleDesc.contactOffset = _controllerDesc.fContactOffset;              // 캐릭터 표면에서 이 거리 안에 들어오면 충돌로 처리
    capsuleDesc.material = pMaterial;
    capsuleDesc.upDirection = PxVec3(0.0f, 1.0f, 0.0f);
    capsuleDesc.nonWalkableMode = PxControllerNonWalkableMode::ePREVENT_CLIMBING; // 경사 제한 넘으면 올라가기 방지
    capsuleDesc.behaviorCallback = this;

    if (!capsuleDesc.isValid())
    {
        MSG_ON(L"컨트롤러 DESC가 유효하지 않습니다", L"확인해주세요");
        Safe_PhysX_Release(pMaterial);
        return nullptr;
    }

    PxController* pController = m_pControllerManager->createController(capsuleDesc);

    if (!pController)
    {
        Safe_PhysX_Release(pMaterial);
        return nullptr;
    }

    // 컨트롤러 내부 액터를 scene query에서 필터링할 수 있도록 이름 지정
    PxRigidDynamic* pActor = pController->getActor();
    if (pActor)
    {
        // 식별 정보
        PHYSX_USER_DATA* pUserData = new PHYSX_USER_DATA;
        pUserData->eActorType = _controllerDesc.eActorType;
        pUserData->iObjectID = _controllerDesc.iObjectID;  
        pUserData->pOwner = _controllerDesc.pOwner;        
        pActor->userData = pUserData;

        // 필터 데이터 (CCT의 Shape에 적용)
        PxFilterData filterData = PhysXFilter::Make_FilterData(_controllerDesc.eActorType);
        PxShape* pShape = nullptr;
        if (pActor->getShapes(&pShape, 1) > 0 && pShape)
        {
            pShape->setSimulationFilterData(filterData);
            pShape->setQueryFilterData(filterData);
        }
    }

    Safe_PhysX_Release(pMaterial);

    return pController;
}

void Engine::PhysXManager::PhysX_Remove_Actor(PxRigidActor* _pActor)
{
    if (_pActor == nullptr || m_pScene == nullptr)
        return;

    // userData 정리
    if (_pActor->userData)
    {
        delete static_cast<PHYSX_USER_DATA*>(_pActor->userData);
        _pActor->userData = nullptr;
    }

    // 씬에서 제거
    m_pScene->removeActor(*_pActor);
}

void Engine::PhysXManager::PhysX_Remove_Controller(PxController* _pController)
{
    if (_pController == nullptr)
        return;

    // 내부 액터의 userData 정리 (release 전에 해야 함)
    PxRigidDynamic* pActor = _pController->getActor();
    if (pActor && pActor->userData)
    {
        delete static_cast<PHYSX_USER_DATA*>(pActor->userData);
        pActor->userData = nullptr;
    }

    // 컨트롤러 해제 (내부 액터 + Shape도 함께 정리됨)
    _pController->release();
}
/******************************************************* 컨트롤러 생성 함수 *******************************************************/



//////////////////////////////////////////////////////// 충돌 검사 함수 ////////////////////////////////////////////////////////
PHYSX_RAYCAST_RESULT Engine::PhysXManager::PhysX_Raycast(const PxVec3& origin, const PxVec3& direction, _float maxDistance)
{
    PHYSX_RAYCAST_RESULT result;

    if (!m_pScene) return result;

    PxRaycastBuffer hit;
    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER;

    if (m_pScene->raycast(origin, direction.getNormalized(), maxDistance, hit, PxHitFlag::eDEFAULT, filterData, &m_QueryFilter))
    {
        if (hit.hasBlock)
        {
            result.bHit = true;
            result.vPosition = To_float3(hit.block.position);
            result.vNormal = To_float3(hit.block.normal);
            result.fDistance = hit.block.distance;
            result.pActor = hit.block.actor;
            result.pUserData = static_cast<PHYSX_USER_DATA*>(hit.block.actor->userData);
        }
    }

    return result;
}

PHYSX_SWEEP_RESULT Engine::PhysXManager::PhysX_Sweep_Capsule(_float radius, _float halfHeight, const PxTransform& pose, const PxVec3& direction, _float maxDistance)
{
    PHYSX_SWEEP_RESULT result;

    if (!m_pScene) return result;

    PxCapsuleGeometry capsule(radius, halfHeight);
    PxSweepBuffer hit;

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER;

    if (m_pScene->sweep(capsule, pose, direction.getNormalized(), maxDistance, hit, PxHitFlag::eDEFAULT, filterData, &m_QueryFilter))
    {
        if (hit.hasBlock)
        {
            result.bHit = true;
            result.vPosition = To_float3(hit.block.position);
            result.vNormal = To_float3(hit.block.normal);
            result.fDistance = hit.block.distance;
            result.pActor = hit.block.actor;
            result.pUserData = static_cast<PHYSX_USER_DATA*>(hit.block.actor->userData);
        }
    }

    return result;
}

PHYSX_SWEEP_RESULT Engine::PhysXManager::PhysX_Sweep_Sphere(_float radius, const PxTransform& pose, const PxVec3& direction, _float maxDistance)
{
    PHYSX_SWEEP_RESULT result;

    if (!m_pScene) return result;

    PxSphereGeometry sphere(radius);
    PxSweepBuffer hit;

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER;

    if (m_pScene->sweep(sphere, pose, direction.getNormalized(), maxDistance, hit, PxHitFlag::eDEFAULT, filterData, &m_QueryFilter))
    {
        if (hit.hasBlock)
        {
            result.bHit = true;
            result.vPosition = To_float3(hit.block.position);
            result.vNormal = To_float3(hit.block.normal);
            result.fDistance = hit.block.distance;
            result.pActor = hit.block.actor;
            result.pUserData = static_cast<PHYSX_USER_DATA*>(hit.block.actor->userData);
        }
    }

    return result;
}

PHYSX_MULTI_SWEEP_RESULT Engine::PhysXManager::PhysX_Sweep_Sphere_Multi(_float _radius, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance)
{
    PHYSX_MULTI_SWEEP_RESULT result;
    if (!m_pScene) return result;

    PxSphereGeometry sphere(_radius);

    const PxU32 iMaxHits = 16;
    PxSweepHit hitBuffer[16] = {};
    PxSweepBuffer sweepBuffer(hitBuffer, iMaxHits);

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER;

    // eMULTIPLE로 모든 히트 수집
    PxHitFlags hitFlags = PxHitFlag::eDEFAULT;

    m_QueryFilter.m_bMultiMode = true;

    if (m_pScene->sweep(sphere, _pose, _direction.getNormalized(), _maxDistance, sweepBuffer, hitFlags, filterData, &m_QueryFilter))
    {
        result.bHit = true;

        // block hit
        if (sweepBuffer.hasBlock && sweepBuffer.block.actor && sweepBuffer.block.actor->userData)
        {
            result.vecUserData.push_back(CAST(PHYSX_USER_DATA*)(sweepBuffer.block.actor->userData));

            // 초기 겹침 시 position이 (0,0,0)이므로 시작 위치로 대체
            if (sweepBuffer.block.distance <= 0.f)
                result.vecPositions.push_back(To_float3(_pose.p));
            else
                result.vecPositions.push_back(To_float3(sweepBuffer.block.position));

            result.vecDistances.push_back(sweepBuffer.block.distance);
            result.vecNormals.push_back(To_float3(sweepBuffer.block.normal));
        }

        // touch hits
        for (PxU32 i = 0; i < sweepBuffer.getNbTouches(); ++i)
        {
            const PxSweepHit& touch = sweepBuffer.getTouch(i);
            if (touch.actor == nullptr || touch.actor->userData == nullptr)
                continue;

            result.vecUserData.push_back(CAST(PHYSX_USER_DATA*)(touch.actor->userData));

            // 초기 겹침 시 position이 (0,0,0)이므로 시작 위치로 대체
            if (touch.distance <= 0.f)
                result.vecPositions.push_back(To_float3(_pose.p));
            else
                result.vecPositions.push_back(To_float3(touch.position));

            result.vecDistances.push_back(touch.distance);
            result.vecNormals.push_back(To_float3(touch.normal));
        }
    }

    m_QueryFilter.m_bMultiMode = false;

    return result;
}


PHYSX_OVERLAP_RESULT Engine::PhysXManager::PhysX_Overlap_Sphere(_float radius, const PxTransform& pose)
{
    PHYSX_OVERLAP_RESULT result;
    if (!m_pScene) return result;
    PxSphereGeometry sphere(radius);
    // 최대 32개까지 검출 (필요하면 늘리기)
    const PxU32 bufferSize = 32;
    PxOverlapHit hitBuffer[bufferSize];
    PxOverlapBuffer hit(hitBuffer, bufferSize);
    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER;
    m_QueryFilter.m_bMultiMode = true;

    m_pScene->overlap(sphere, pose, hit, filterData, &m_QueryFilter); // touch만 있어도 버퍼에 결과 존재

    // touch 포함 전체 결과 체크
    if (hit.getNbAnyHits() > 0)
    {
        result.bHit = true;
        for (PxU32 i = 0; i < hit.getNbAnyHits(); i++)
        {
            PxRigidActor* pActor = (PxRigidActor*)hit.getAnyHit(i).actor;
            if (pActor)
            {
                result.vecActors.push_back(pActor);
                result.vecUserData.push_back(static_cast<PHYSX_USER_DATA*>(pActor->userData));
                result.vecHitData.push_back(To_float3(pActor->getGlobalPose().p));
            }
        }
    }

    m_QueryFilter.m_bMultiMode = false;
    return result;
}

PHYSX_OVERLAP_RESULT Engine::PhysXManager::PhysX_Overlap_Capsule(_float radius, _float halfHeight, const PxTransform& pose)
{
    PHYSX_OVERLAP_RESULT result;
    if (!m_pScene) return result;

    PxCapsuleGeometry capsule(radius, halfHeight); // 수정됨 : Sphere -> Capsule

    // 최대 32개까지 검출 (필요하면 늘리기)
    const PxU32 bufferSize = 32;
    PxOverlapHit hitBuffer[bufferSize];
    PxOverlapBuffer hit(hitBuffer, bufferSize);

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER;

    m_QueryFilter.m_bMultiMode = true;
    m_pScene->overlap(capsule, pose, hit, filterData, &m_QueryFilter);

    if (hit.getNbAnyHits() > 0)
    {
        result.bHit = true;
        for (PxU32 i = 0; i < hit.getNbAnyHits(); i++)
        {
            PxRigidActor* pActor = (PxRigidActor*)hit.getAnyHit(i).actor;
            if (pActor)
            {
                result.vecActors.push_back(pActor);
                result.vecUserData.push_back(static_cast<PHYSX_USER_DATA*>(pActor->userData));
                result.vecHitData.push_back(To_float3(pActor->getGlobalPose().p));
            }
        }
    }

    m_QueryFilter.m_bMultiMode = false;
    return result;
}

PHYSX_MULTI_SWEEP_RESULT Engine::PhysXManager::PhysX_Sweep_Capsule_Multi(_float _radius, _float _halfHeight, const PxTransform& _pose,
    const PxVec3& _direction, _float _maxDistance)
{
    PHYSX_MULTI_SWEEP_RESULT result;
    if (!m_pScene) return result;

    PxCapsuleGeometry capsule(_radius, _halfHeight);

    const PxU32 iMaxHits = 16;
    PxSweepHit hitBuffer[16] = {};
    PxSweepBuffer sweepBuffer(hitBuffer, iMaxHits);

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eDYNAMIC | PxQueryFlag::eSTATIC | PxQueryFlag::ePREFILTER;

    PxHitFlags hitFlags = PxHitFlag::eDEFAULT;

    m_QueryFilter.m_bMultiMode = true;

    if (m_pScene->sweep(capsule, _pose, _direction.getNormalized(), _maxDistance, sweepBuffer, hitFlags, filterData, &m_QueryFilter))
    {
        result.bHit = true;

        // block hit
        if (sweepBuffer.hasBlock && sweepBuffer.block.actor && sweepBuffer.block.actor->userData)
        {
            result.vecUserData.push_back(CAST(PHYSX_USER_DATA*)(sweepBuffer.block.actor->userData));

            if (sweepBuffer.block.distance <= 0.f)
                result.vecPositions.push_back(To_float3(_pose.p));
            else
                result.vecPositions.push_back(To_float3(sweepBuffer.block.position));

            result.vecDistances.push_back(sweepBuffer.block.distance);
            result.vecNormals.push_back(To_float3(sweepBuffer.block.normal));
        }

        // touch hits
        for (PxU32 i = 0; i < sweepBuffer.getNbTouches(); ++i)
        {
            const PxSweepHit& touch = sweepBuffer.getTouch(i);
            if (touch.actor == nullptr || touch.actor->userData == nullptr)
                continue;

            result.vecUserData.push_back(CAST(PHYSX_USER_DATA*)(touch.actor->userData));

            if (touch.distance <= 0.f)
                result.vecPositions.push_back(To_float3(_pose.p));
            else
                result.vecPositions.push_back(To_float3(touch.position));

            result.vecDistances.push_back(touch.distance);
            result.vecNormals.push_back(To_float3(touch.normal));
        }
    }

    m_QueryFilter.m_bMultiMode = false;

    return result;
}

PHYSX_MULTI_SWEEP_RESULT Engine::PhysXManager::PhysX_Sweep_Capsule_Multi_ExceptStatic(_float _radius, _float _halfHeight, const PxTransform& _pose,
    const PxVec3& _direction, _float _maxDistance)
{
    PHYSX_MULTI_SWEEP_RESULT result;
    if (!m_pScene) return result;

    PxCapsuleGeometry capsule(_radius, _halfHeight);

    const PxU32 iMaxHits = 16;
    PxSweepHit hitBuffer[16] = {};
    PxSweepBuffer sweepBuffer(hitBuffer, iMaxHits);

    PxQueryFilterData filterData;
    filterData.flags = PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER;

    PxHitFlags hitFlags = PxHitFlag::eDEFAULT;

    m_QueryFilter.m_bMultiMode = true;

    if (m_pScene->sweep(capsule, _pose, _direction.getNormalized(), _maxDistance, sweepBuffer, hitFlags, filterData, &m_QueryFilter))
    {
        result.bHit = true;

        // block hit
        if (sweepBuffer.hasBlock && sweepBuffer.block.actor && sweepBuffer.block.actor->userData)
        {
            result.vecUserData.push_back(CAST(PHYSX_USER_DATA*)(sweepBuffer.block.actor->userData));

            if (sweepBuffer.block.distance <= 0.f)
                result.vecPositions.push_back(To_float3(_pose.p));
            else
                result.vecPositions.push_back(To_float3(sweepBuffer.block.position));

            result.vecDistances.push_back(sweepBuffer.block.distance);
            result.vecNormals.push_back(To_float3(sweepBuffer.block.normal));
        }

        // touch hits
        for (PxU32 i = 0; i < sweepBuffer.getNbTouches(); ++i)
        {
            const PxSweepHit& touch = sweepBuffer.getTouch(i);
            if (touch.actor == nullptr || touch.actor->userData == nullptr)
                continue;

            result.vecUserData.push_back(CAST(PHYSX_USER_DATA*)(touch.actor->userData));

            if (touch.distance <= 0.f)
                result.vecPositions.push_back(To_float3(_pose.p));
            else
                result.vecPositions.push_back(To_float3(touch.position));

            result.vecDistances.push_back(touch.distance);
            result.vecNormals.push_back(To_float3(touch.normal));
        }
    }

    m_QueryFilter.m_bMultiMode = false;

    return result;
}
/******************************************************* 충돌 검사 함수 *******************************************************/



//////////////////////////////////////////////////////// PhysX Debug Visualization ////////////////////////////////////////////////////////
#ifdef _DEBUG
void Engine::PhysXManager::Enable_DebugVisualization()
{
    if (m_pScene == nullptr)
        return;

    // 글로벌 스케일 ON
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);

    // 저장된 개별 상태 복원
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, m_bVis_CollisionShapes ? 1.0f : 0.0f);
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AABBS, m_bVis_CollisionAABBs ? 1.0f : 0.0f);
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, m_bVis_ActorAxes ? 1.0f : 0.0f);
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_POINT, m_bVis_ContactPoint ? 1.0f : 0.0f);
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eCONTACT_NORMAL, m_bVis_ContactNormal ? 1.0f : 0.0f);
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eBODY_LIN_VELOCITY, m_bVis_BodyLinVelocity ? 1.0f : 0.0f);
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eBODY_ANG_VELOCITY, m_bVis_BodyAngVelocity ? 1.0f : 0.0f);
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, m_bVis_JointLocalFrames ? 1.0f : 0.0f);
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, m_bVis_JointLimits ? 1.0f : 0.0f);

    m_bDebugVisualization = true;
    COUT("PhysX Debug Visualization ON");
}

void Engine::PhysXManager::Disable_DebugVisualization()
{
    if (m_pScene == nullptr)
        return;

    // 글로벌 스케일을 0으로 하면 전부 꺼짐
    m_pScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 0.0f);

    m_bDebugVisualization = false;
    COUT("PhysX Debug Visualization OFF");
}

void Engine::PhysXManager::Toggle_DebugVisualization()
{
    // 토글
    if (m_bDebugVisualization)
        Disable_DebugVisualization();
    else
        Enable_DebugVisualization();
}

void Engine::PhysXManager::Set_VisualizationParam(PxVisualizationParameter::Enum _eParam, _bool _bEnable)
{
    // 개별 파라미터 설정 + 시각화 켜져있으면 바로 적용
    if (m_pScene == nullptr)
        return;

    if (m_bDebugVisualization)
        m_pScene->setVisualizationParameter(_eParam, _bEnable ? 1.0f : 0.0f);
}

_bool Engine::PhysXManager::Get_VisualizationParam(PxVisualizationParameter::Enum _eParam) const
{
    // 현재 파라미터 상태 확인
    if (m_pScene == nullptr)
        return false;

    return m_pScene->getVisualizationParameter(_eParam) > 0.0f;
}

const PxRenderBuffer* Engine::PhysXManager::Get_PhysXRenderBuffer() const
{
    if (m_pScene == nullptr || m_bDebugVisualization == false)
        return nullptr;

    return &m_pScene->getRenderBuffer();
}
#endif
/******************************************************* PhysX Debug Visualization *******************************************************/



//////////////////////////////////////////////////////// Pooling용 Disable, Enable ////////////////////////////////////////////////////////
void PhysXManager::Disable_Controller(PxController* _pController)
{
    if (nullptr == _pController)
        return;

    // 맵 밖으로 치워두기
    _pController->setFootPosition(PxExtendedVec3(0.0, -1000.0, 0.0));

    // 시뮬레이션에서 완전히 제외
    PxRigidDynamic* actor = _pController->getActor();
    PxShape* shape = {};
    actor->getShapes(&shape, 1);

    PxFilterData filterData = shape->getQueryFilterData();
    filterData.word3 |= FILTER_DISABLED;
    shape->setQueryFilterData(filterData);
}

void PhysXManager::Enable_Controller(PxController* _pController, const PxExtendedVec3& _spawnPos, PX_ACTOR_TYPE _eType)
{
    if (nullptr == _pController)
        return;

    PxRigidDynamic* pActor = _pController->getActor();

    // 시뮬레이션 복구
    pActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);

    // 스폰 위치로 이동
    _pController->setFootPosition(_spawnPos);

    // 필터 재설정 (혹시 모를 상태 보장)
    PxShape* pShape = nullptr;
    pActor->getShapes(&pShape, 1);
    PxFilterData filterData = pShape->getQueryFilterData();
    filterData.word3 &= ~FILTER_DISABLED;
    pShape->setQueryFilterData(filterData);
}
/******************************************************* Pooling용 Disable, Enable *******************************************************/



//////////////////////////////////////////////////////// PhysXCloth ////////////////////////////////////////////////////////
HRESULT Engine::PhysXManager::PhysX_Create_BoneChain(const PHYSX_BONE_CHAIN_DESC& _desc, Model* _pMasterModel, const _float4x4& _objectWorldMatrix)
{
    if (_desc.vecBoneNames.empty() || !_pMasterModel || !m_pPhysics || !m_pScene)
        return E_FAIL;

    PHYSX_BONE_CHAIN chain = {};
    chain.iChainID = _desc.iChainID;
    chain.pMaterial = m_pPhysics->createMaterial(0.5f, 0.5f, 0.0f);

    if (!chain.pMaterial)
        return E_FAIL;

    _matrix matWorld = XMLoadFloat4x4(&_objectWorldMatrix);

    // 각 본의 초기 월드 포즈를 미리 계산
    vector<PxTransform> vecInitialPoses = {};
    vecInitialPoses.resize(_desc.vecBoneNames.size());

    for (_uint i = 0; i < (_uint)_desc.vecBoneNames.size(); ++i)
    {
        _matrix matBone = {};

        // 외부 초기 포즈가 제공되었으면 사용, 아니면 기존 방식
        if (i < (_uint)_desc.vecInitialPoses.size())
        {
            matBone = XMLoadFloat4x4(&_desc.vecInitialPoses[i]) * matWorld;
        }
        else
        {
            const _float4x4* pCombined = _pMasterModel->Get_BoneMatrixPtr(_desc.vecBoneNames[i]);

            if (pCombined)
                matBone = XMLoadFloat4x4(pCombined) * matWorld; // CombinedMatrix(PreTransform 포함) * World
            else
                matBone = matWorld;
        }

        _vector vScale = {};
        _vector vRotation = {};
        _vector vTranslation = {};
        XMMatrixDecompose(&vScale, &vRotation, &vTranslation, matBone);

        _float4 fQuat = {};
        _float4 fPos = {};
        XMStoreFloat4(&fQuat, vRotation);
        XMStoreFloat4(&fPos, vTranslation);

        vecInitialPoses[i] = PxTransform(
            PxVec3(fPos.x, fPos.y, fPos.z),
            PxQuat(fQuat.x, fQuat.y, fQuat.z, fQuat.w)
        );
    }

    chain.vecActors.reserve(_desc.vecBoneNames.size());

    // 각 본에 대해 PxRigidDynamic 생성
    for (_uint i = 0; i < (_uint)_desc.vecBoneNames.size(); ++i)
    {
        PHYSX_BONE_CHAIN_ACTOR tActor = {};
        tActor.strBoneName = _desc.vecBoneNames[i];
        tActor.iBoneIndex = _pMasterModel->Get_BoneIndex(tActor.strBoneName.c_str());

        if (tActor.iBoneIndex == -1)
        {
            // 본을 찾지 못하면 체인 생성 실패
            Safe_PhysX_Release(chain.pMaterial);
            for (auto& actor : chain.vecActors)
                Safe_PhysX_Release(actor.pActor);
            
            MSG_ON(L"본 체인 생성 실패!", L"끼야아아아아악");
            BREAK;
            return E_FAIL;
        }

        tActor.bKinematic = (i == 0); // 첫 번째 본만 kinematic

        // PxIdentity 대신 실제 본 위치로 생성
        PxRigidDynamic* pRigid = m_pPhysics->createRigidDynamic(vecInitialPoses[i]);

        if (!pRigid)
        {
            Safe_PhysX_Release(chain.pMaterial);
            for (auto& actor : chain.vecActors)
                Safe_PhysX_Release(actor.pActor);
            return E_FAIL;
        }

        // shape 생성 (작은 sphere)
        PxShape* pShape = m_pPhysics->createShape(
            PxSphereGeometry(_desc.fBoneRadius),
            *chain.pMaterial,
            true
        );

        if (pShape)
        {
            // bone chain 전용 필터 (다른 물체와 충돌하지 않도록)
            PxFilterData filterData;
            filterData.word0 = CAST(PxU32)(PX_ACTOR_TYPE::BONE_CHAIN);
            filterData.word1 = 0u;
            pShape->setSimulationFilterData(filterData);
            pShape->setQueryFilterData(filterData);

            // shpae의 충돌 비활성화
            pShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
            pShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
            pShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);

            pRigid->attachShape(*pShape);
            Safe_PhysX_Release(pShape);
        }

        if (tActor.bKinematic)
        {
            pRigid->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
        }
        else
        {
            // dynamic 본은 질량 설정 + damping
            PxRigidBodyExt::updateMassAndInertia(*pRigid, _desc.fDensity);
            pRigid->setLinearDamping(_desc.fDamping * 5.f);
            pRigid->setAngularDamping(_desc.fDamping * 10.f);
            // 중력 비활성화 (옷이 아래로 축 처지는거 방지)
            //pRigid->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
        }

        m_pScene->addActor(*pRigid);
        tActor.pActor = pRigid;
        chain.vecActors.push_back(tActor);
    }

    // Joint 생성 시 localFrame을 본 간 상대 트랜스폼으로 계산
    chain.vecJoints.reserve(_desc.vecBoneNames.size() - 1);

    m_pScene->simulate(0.0001f);
    m_pScene->fetchResults(true);

    for (_uint i = 1; i < (_uint)chain.vecActors.size(); ++i)
    {
        _int iParent = 0;
        if (i < (_uint)_desc.vecParentIndices.size())
        {
            _int iDefined = _desc.vecParentIndices[i];
            if (iDefined >= 0 && iDefined < (_int)chain.vecActors.size())
                iParent = iDefined;
        }

        PxRigidDynamic* pParent = chain.vecActors[iParent].pActor;
        PxRigidDynamic* pChild = chain.vecActors[i].pActor;

        PxTransform parentPose = vecInitialPoses[iParent];
        PxTransform childPose = vecInitialPoses[i];

        PxTransform parentLocalFrame = parentPose.getInverse() * childPose;
        PxTransform childLocalFrame = PxTransform(PxIdentity);

        PxD6Joint* pJoint = PxD6JointCreate(
            *m_pPhysics,
            pParent, parentLocalFrame,
            pChild, childLocalFrame
        );

        if (!pJoint)
            continue;

        // 이동은 완전 잠금
        pJoint->setMotion(PxD6Axis::eX, PxD6Motion::eLOCKED);
        pJoint->setMotion(PxD6Axis::eY, PxD6Motion::eLOCKED);
        pJoint->setMotion(PxD6Axis::eZ, PxD6Motion::eLOCKED);

        // 회전을 FREE로 변경 (제한 없이 자유 회전)
        pJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
        pJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
        pJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);

        // Drive로 약한 복원력만 (바인드포즈로 느리게 돌아옴)
        PxD6JointDrive drive(_desc.fStiffness, _desc.fDamping, PX_MAX_F32, true);
        pJoint->setDrive(PxD6Drive::eSLERP, drive);
        pJoint->setDrivePosition(PxTransform(PxIdentity));

        chain.vecJoints.push_back(pJoint);
    }

    m_umapBoneChains.emplace(chain.iChainID, std::move(chain));

    return S_OK;
}

void Engine::PhysXManager::PhysX_Update_BoneChain_Kinematic(_uint _iChainID, const _float4x4& _rootCombinedMatrix, const _float4x4& _objectWorldMatrix)
{
    auto iter = m_umapBoneChains.find(_iChainID);
    if (iter == m_umapBoneChains.end())
        return;

    PHYSX_BONE_CHAIN& chain = iter->second;
    if (chain.vecActors.empty())
        return;

    // 체인 루트의 최종 월드 행렬 = CombinedMatrix * ObjectWorldMatrix
    _matrix matCombined = XMLoadFloat4x4(&_rootCombinedMatrix);
    _matrix matWorld = XMLoadFloat4x4(&_objectWorldMatrix);
    _matrix matFinal = matCombined * matWorld;

    // XMMATRIX -> PxTransform 변환
    _vector vScale = {};
    _vector vRotation = {};
    _vector vTranslation = {};
    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, matFinal);

    _float4 fQuat = {};
    _float4 fPos = {};
    XMStoreFloat4(&fQuat, vRotation);
    XMStoreFloat4(&fPos, vTranslation);

    PxTransform target(
        PxVec3(fPos.x, fPos.y, fPos.z),
        PxQuat(fQuat.x, fQuat.y, fQuat.z, fQuat.w)
    );

    // kinematic 루트만 타겟 세팅
    if (chain.vecActors[0].bKinematic && chain.vecActors[0].pActor)
    {
        chain.vecActors[0].pActor->setKinematicTarget(target);
    }

    // 루트 이동량으로 관성 외력 계산
    PxVec3 vDelta = target.p - chain.prevKinematicPose.p;
    PxQuat qDelta = target.q * chain.prevKinematicPose.q.getConjugate();

    // 회전 변화량을 각속도 벡터로 변환
    PxVec3 vAngularDelta(0.f);
    if (qDelta.w < 0.f) // 최단 경로 보정
        qDelta = -qDelta;

    _float fAngle = 2.f * acosf(min(1.f, qDelta.w));
    if (fAngle > 0.001f)
    {
        _float fSinHalf = sinf(fAngle * 0.5f);
        if (fSinHalf > 0.001f)
            vAngularDelta = PxVec3(qDelta.x, qDelta.y, qDelta.z) * (fAngle / fSinHalf);
    }

    // 관성 증폭 계수 (이 값이 클수록 격하게 반응)
    _float fInertiaMultiplier = 15.f;
    _float fAngularMultiplier = 1.f;

    // 반대 방향으로 외력 (루트가 오른쪽으로 가면 자식은 왼쪽으로 밀림)
    PxVec3 vLinearForce = -vDelta * fInertiaMultiplier;
    PxVec3 vAngularForce = -vAngularDelta * fAngularMultiplier;

    // 체인의 끝으로 갈수록 더 크게 반응
    for (_uint i = 1; i < (_uint)chain.vecActors.size(); ++i)
    {
        if (!chain.vecActors[i].pActor || chain.vecActors[i].bKinematic)
            continue;

        _float fChainRatio = (_float)i / (_float)(chain.vecActors.size() - 1);

        chain.vecActors[i].pActor->addForce(vLinearForce * fChainRatio, PxForceMode::eIMPULSE);
        chain.vecActors[i].pActor->addTorque(vAngularForce * fChainRatio, PxForceMode::eIMPULSE);
    }

    chain.prevKinematicPose = target; // 이번 프레임 포즈 저장
}

void Engine::PhysXManager::PhysX_Fetch_BoneChain_Results(_uint _iChainID, const _float4x4& _objectWorldMatrix, vector<pair<_int, _float4x4>>& _outBoneMatrices)
{
    // BoneChain ID로 등록된 체인 검색
    auto iter = m_umapBoneChains.find(_iChainID);

    // 해당 ID의 체인이 존재하지 않으면 early return
    if (iter == m_umapBoneChains.end())
        return;

    PHYSX_BONE_CHAIN& tChain = iter->second;

    // 역행렬 만들기
    // PhysX 시뮬레이션 결과는 월드 스페이스 기준이므로,
    // 오브젝트의 월드 행렬의 역행렬을 구해서
    // 월드 -> 모델(로컬) 스페이스로 변환할 준비를 한다.
    _matrix matWorldInverse = XMMatrixInverse(nullptr, XMLoadFloat4x4(&_objectWorldMatrix));

    _outBoneMatrices.clear();
    _outBoneMatrices.reserve(tChain.vecActors.size());

    for (_uint i = 0; i < (_uint)tChain.vecActors.size(); ++i)
    {
        const PHYSX_BONE_CHAIN_ACTOR& tActor = tChain.vecActors[i];

        // Actor가 유효하지 않거나, Kinematic(애니메이션 추종) 본이면
        // 시뮬레이션 결과를 가져올 필요가 없으므로 스킵
        if (!tActor.pActor || tActor.bKinematic)
            continue;

        // PhysX Actor의 현재 월드 스페이스 Transform(위치 + 회전) 획득
        PxTransform globalPose = tActor.pActor->getGlobalPose();

        // PhysX 쿼터니언 -> DirectX 회전 행렬로 변환
        _matrix matPhysX = XMMatrixRotationQuaternion(
            XMVectorSet(globalPose.q.x, globalPose.q.y, globalPose.q.z, globalPose.q.w)
        );

        // 회전 행렬의 4번째 행(Translation)에 PhysX 위치값을 직접 세팅
        // (XMMatrixRotationQuaternion은 회전만 생성하므로 위치를 별도로 넣어준다)
        matPhysX.r[3] = XMVectorSet(globalPose.p.x, globalPose.p.y, globalPose.p.z, 1.f);

        // 월드 스페이스 PhysX 결과 * 월드 역행렬 = 모델 스페이스 본 행렬
        // 이 결과를 본 원래의 로컬 행렬 대신 적용하면
        // 래그돌/물리 시뮬레이션이 반영된 본 자세가 된다.
        _matrix matModelSpace = matPhysX * matWorldInverse;

        _float4x4 matResult = {};
        XMStoreFloat4x4(&matResult, matModelSpace);

        // (본 인덱스, 모델 스페이스 행렬) 쌍으로 출력 버퍼에 추가
        // 호출부에서 이 인덱스를 기반으로 스키닝 행렬 팔레트를 갱신한다.
        _outBoneMatrices.push_back({ tActor.iBoneIndex, matResult });
    }
}

void Engine::PhysXManager::PhysX_Remove_BoneChain(_uint _iChainID)
{
    auto iter = m_umapBoneChains.find(_iChainID);
    if (iter == m_umapBoneChains.end())
        return;

    PHYSX_BONE_CHAIN& chain = iter->second;

    // Joint 먼저 해제
    for (auto& pJoint : chain.vecJoints)
    {
        if (pJoint)
            pJoint->release();
    }
    chain.vecJoints.clear();

    // Actor 해제
    for (auto& tActor : chain.vecActors)
    {
        if (tActor.pActor)
        {
            m_pScene->removeActor(*tActor.pActor);
            Safe_PhysX_Release(tActor.pActor);
        }
    }
    chain.vecActors.clear();

    // Material 해제
    Safe_PhysX_Release(chain.pMaterial);

    m_umapBoneChains.erase(iter);
}

void Engine::PhysXManager::PhysX_Remove_All_BoneChains()
{
    // 모든 bone chain 제거
    vector<_uint> vecIDs = {};
    vecIDs.reserve(m_umapBoneChains.size());

    for (auto& pair : m_umapBoneChains)
        vecIDs.push_back(pair.first);

    for (_uint iID : vecIDs)
        PhysX_Remove_BoneChain(iID);
}
/******************************************************* PhysXCloth *******************************************************/



//////////////////////////////////////////////////////// 헬퍼 함수 ////////////////////////////////////////////////////////
_wstring Engine::PhysXManager::Make_Cache_Path(const _wstring& strModelName, _uint meshIndex)
{
    return m_strCacheDir + strModelName + L"_" + to_wstring(meshIndex) + L".physx";
}
/******************************************************* 헬퍼 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
PhysXManager* Engine::PhysXManager::Create()
{
	PhysXManager* pInstance = new PhysXManager();

	MSG_FAIL(pInstance->Initialize(), L"PhysXManager 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::PhysXManager::Free()
{
    __super::Free();

    // 모든 액터의 userData 정리 (씬 해제 전에 해야 함)
    if (m_pScene)
    {
        // 씬에 등록된 액터 목록 가져오기
        PxU32 iNumActors = m_pScene->getNbActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC);

        if (iNumActors > 0)
        {
            vector<PxActor*> vecActors(iNumActors);
            m_pScene->getActors(PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC,
                vecActors.data(), iNumActors);

            for (PxU32 i = 0; i < iNumActors; ++i)
            {
                if (vecActors[i] && vecActors[i]->userData)
                {
                    delete static_cast<PHYSX_USER_DATA*>(vecActors[i]->userData);
                    vecActors[i]->userData = nullptr;
                }
            }
        }
    }

    PhysX_Remove_All_BoneChains();


    // 생성의 역순으로 해제 (매우 중요! 순서 틀리면 터짐)
    Safe_PhysX_Release(m_pControllerManager);
    Safe_PhysX_Release(m_pScene);
    Safe_PhysX_Release(m_pDispatcher);
    Safe_PhysX_Release(m_pPhysics);

    if (m_pPvd)
    {
        //m_pPvd->();
        PxPvdTransport* transport = m_pPvd->getTransport();
        Safe_PhysX_Release(m_pPvd);
        if (transport) transport->release();
    }
    Safe_PhysX_Release(m_pOmniPvd);

    Safe_PhysX_Release(m_pFoundation);


    //Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

