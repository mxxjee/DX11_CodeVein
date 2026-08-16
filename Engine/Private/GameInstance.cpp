#include "Engine_Define.h"
#include "GameInstance.h"
#include "Graphic_Device.h"

#include "RenderTargetManager.h"
#include "RenderStateManager.h"
#include "GameObjectManager.h"
#include "PrototypeManager.h"
#include "ColliderManager.h"
#include "ResourceManager.h"
#include "SamplerManager.h"
#include "CameraManager.h"
#include "ObjectFactory.h"
#include "EffectManager.h"
#include "DecalManager.h"
#include "PhysXManager.h"
#include "TimerManager.h"
#include "LevelManager.h"
#include "SoundManager.h"
#include "SpawnManager.h"
#include "EventManager.h"
#include "InputManager.h"
#include "LightManager.h"
#include "FileManager.h"
#include "FontManager.h"
#include "Calculator.h"
#include "UIManager.h"
#include "Renderer.h"
#include "PipeLine.h"
#include "Picking.h"
#include "Frustum.h"
#include "Shadow.h"
#include "Mouse.h"

#include "Layer.h"
#include "UIObject.h"

//#include "VIBuffer.h"

IMPLEMENT_SINGLETON(GameInstance);

namespace Engine {
    HWND eg_hWnd;
}


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
GameInstance::GameInstance()
{
}

GameInstance::~GameInstance()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



void Test_PhysX_Drop()
{
    // 1. 매니저에서 핵심 객체들 가져오기
    PxPhysics* pPhysics = GameInstance::GetInstance()->Get_Physics();
    PxScene* pScene = GameInstance::GetInstance()->Get_Scene();

    if (!pPhysics || !pScene) return;

    // 2. 재질 만들기 (정지 마찰력, 운동 마찰력, 반발계수)
    // 0.5f = 적당히 미끄러움, 0.6f = 탱탱볼처럼 꽤 튕김
    PxMaterial* pMaterial = pPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    // 3. 바닥 만들기 (RigidStatic)
    // PxPlaneGeometry: 무한한 평면
    // PxCreatePlane(물리객체, 평면방정식, 재질) -> 확장(Extensions) 라이브러리 함수
    PxRigidStatic* pFloor = PxCreatePlane(*pPhysics, PxPlane(0, 1, 0, 0), *pMaterial);
    pScene->addActor(*pFloor);

    // 4. 상자 만들기 (RigidDynamic)
    // 공중 10미터 높이(0, 10, 0)에 가로세로 2미터짜리 박스 생성
    PxTransform transform(PxVec3(0.0f, 10.0f, 0.0f));
    PxBoxGeometry geometry(1.0f, 1.0f, 1.0f); // 반지름 개념이라 실제 크기는 2x2x2
    PxRigidDynamic* pBox = PxCreateDynamic(*pPhysics, transform, geometry, *pMaterial, 10.0f); // 10kg

    // 중요: 상자가 잠들지 않게 깨움 (WakeUp)
    pBox->wakeUp();

    // 5. 씬에 추가
    pScene->addActor(*pBox);
}



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Initialize_Device(HWND _hWnd, WINMODE _isWindowed, _float _wincx, _float _wincy, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
	m_pGraphic_Device = Graphic_Device::Create(_hWnd, _isWindowed, (_uint)_wincx, (_uint)_wincy, ppDevice, ppContext);
	CHECK_NULLPTR(m_pGraphic_Device);
	
	m_pDevice = *ppDevice;
	m_pContext = *ppContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

    m_fWincx = _wincx;
    m_fWincy = _wincy;
    Engine_WINCX = m_fWincx;
    Engine_WINCY = m_fWincy;

    eg_hWnd = m_hWnd = _hWnd;

    m_Viewport.TopLeftX = 0.f;
    m_Viewport.TopLeftY = 0.f;
    m_Viewport.Width = _wincx;  // 창 너비
    m_Viewport.Height = _wincy; // 창 높이
    m_Viewport.MinDepth = 0.f;
    m_Viewport.MaxDepth = 1.f;

    std::wcout.imbue(std::locale("korean"));

	return S_OK;
}

HRESULT Engine::GameInstance::Initialize(ENGINE_DESC _tDesc)
{
    m_pTimerManager = TimerManager::Create();
	m_vecCleanupTargets.push_back(m_pTimerManager);

    m_pEventManager = EventManager::Create(m_pDevice, m_pContext);

	m_pGameObjectManager = GameObjectManager::Create(m_pDevice, m_pContext, _tDesc.iNumLevels);
	m_vecCleanupTargets.push_back(m_pGameObjectManager);

	m_pLevelManager = LevelManager::Create(m_pDevice, m_pContext, _tDesc.iNumLevels, m_pGameObjectManager);
	m_vecCleanupTargets.push_back(m_pLevelManager);

	m_pPrototypeManager = PrototypeManager::Create(m_pDevice, m_pContext, _tDesc.iNumLevels);
	m_vecCleanupTargets.push_back(m_pPrototypeManager);

    m_pSamplerManager = SamplerManager::Create(m_pDevice, m_pContext);
    m_vecCleanupTargets.push_back(m_pSamplerManager);

	m_pPipeLine = PipeLine::Create(m_pDevice, m_pContext);
	m_vecCleanupTargets.push_back(m_pPipeLine);

    m_pResourceManager = ResourceManager::Create(m_pDevice, m_pContext);

	m_pInputManager = InputManager::Create(m_pDevice, m_pContext, _tDesc.hInstance, _tDesc.hWnd);
	m_vecCleanupTargets.push_back(m_pInputManager);

	m_pLightManager = LightManager::Create(m_pDevice, m_pContext);
	m_vecCleanupTargets.push_back(m_pLightManager);

    m_pRenderTargetManager = RenderTargetManager::Create(m_pDevice, m_pContext);
    m_vecCleanupTargets.push_back(m_pRenderTargetManager);

    m_pCalculator = Calculator::Create(m_pDevice, m_pContext);
    m_vecCleanupTargets.push_back(m_pCalculator);

    m_pFontManager = FontManager::Create(m_pDevice, m_pContext);
    m_vecCleanupTargets.push_back(m_pFontManager);

    m_pCameraManager = CameraManager::Create();
    m_vecCleanupTargets.push_back(m_pCameraManager);

    m_pRenderStateManager = RenderStateManager::Create(m_pDevice, m_pContext);
    m_vecCleanupTargets.push_back(m_pRenderStateManager);

    m_pShadow = Shadow::Create(m_pDevice, m_pContext);
    m_vecCleanupTargets.push_back(m_pShadow);

    m_pUIManager = UIManager::Create(m_pDevice, m_pContext);
    m_vecCleanupTargets.push_back(m_pUIManager);

    m_pSoundManager = SoundManager::Create();
    m_vecCleanupTargets.push_back(m_pSoundManager);

    m_pFileManagaer = FileManager::Create(m_pDevice, m_pContext);
    m_vecCleanupTargets.push_back(m_pFileManagaer);

    m_pPicking = Picking::Create(m_pDevice, m_pContext, _tDesc.hWnd, (_uint)m_fWincx, (_uint)m_fWincy);
    m_vecCleanupTargets.push_back(m_pPicking);

    m_pFrustum = Frustum::Create();
    m_vecCleanupTargets.push_back(m_pFrustum);
    
    m_pObjectFactory = ObjectFactory::Create();
    m_vecCleanupTargets.push_back(m_pObjectFactory);

    m_pRenderer = Renderer::Create(m_pDevice, m_pContext);
    m_vecCleanupTargets.push_back(m_pRenderer);

    m_pPhysXManager = PhysXManager::Create();

    m_pEffectManager = EffectManager::Create();
    m_vecCleanupTargets.push_back(m_pEffectManager);

    m_pDecalManager = DecalManager::Create();
    m_vecCleanupTargets.push_back(m_pDecalManager);

    m_pSpawnManager = SpawnManager::Create(m_pDevice, m_pContext);
    m_vecCleanupTargets.push_back(m_pSpawnManager);

    m_pColliderManager = ColliderManager::Create(m_pDevice, m_pContext);
    m_vecCleanupTargets.push_back(m_pColliderManager);

    // Test_PhysX_Drop();

    // 이벤트 매니저는 마지막에 릴리즈 되도록 마지막에 pushback
    m_vecCleanupTargets.push_back(m_pResourceManager);
    m_vecCleanupTargets.push_back(m_pEventManager);
    m_vecCleanupTargets.push_back(m_pPhysXManager);

    m_pMouse = Mouse::GetInstance();
    m_pMouse->Initialize(m_pDevice, m_pContext);

    SHADERENTRY entry[1] = { {"VS_MAIN", "PS_MAIN"} };
    SHADERENTRIES entries;
    entries.pEntries = entry;
    entries.iNumpass = 1;

    Shader* tempshader = Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Initialize.hlsl", VTXPOS::Elements, VTXPOS::iNumElements, entries);
    //m_pResourceManager->Initialize_Shader_Value(tempshader);
    Safe_Release(tempshader);

	return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
void Engine::GameInstance::Update_Level(const _float fTimeDelta)
{
    m_pEventManager->Publish_StackedEvent();
	m_pInputManager->Update();

    /* 마우스 가두기 */
    if (KeyDown(DIK_TAB))
    {
        static _bool mouseLock = true;
        mouseLock = !mouseLock;

        MOUSELOCK_EVENT mouseevent;
        mouseevent.bLock = mouseLock;
        Publish(mouseevent);
    }

    m_pSoundManager->Update(fTimeDelta);
    //m_pPicking->Update();
    m_pMouse->Update_Priority(fTimeDelta);
	m_pLevelManager->Update_Priority(fTimeDelta);
    m_pLevelManager->Update_Player_Priority(fTimeDelta);

    m_pMouse->Update(fTimeDelta);
    m_pLevelManager->Update(fTimeDelta);
    m_pLevelManager->Update_Player(fTimeDelta);

    m_pColliderManager->Update(fTimeDelta);
    m_pPhysXManager->Update(fTimeDelta);

    m_pCameraManager->Update_Cameras(fTimeDelta);
    m_pFrustum->Update();    // 카메라 업데이트 이후에 절두체 업데이트

    m_pMouse->Update_Late(fTimeDelta);
    m_pLevelManager->Update_Late(fTimeDelta);
    m_pLevelManager->Update_Player_Late(fTimeDelta);

	m_pPipeLine->Update(fTimeDelta);
}

void Engine::GameInstance::Update_Priority(const _float fTimeDelta)
{
	m_pGameObjectManager->Update_Priority(fTimeDelta);
    m_pUIManager->Update_Priority(fTimeDelta);
    m_pLightManager->Update_LightPlayer_Distance();
}

void Engine::GameInstance::Update_Parallel(const _float fTimeDelta)
{
    m_pGameObjectManager->Update_Parallel(fTimeDelta);
 
}

void Engine::GameInstance::Update(const _float fTimeDelta)
{
	m_pGameObjectManager->Update(fTimeDelta);
    m_pUIManager->Update(fTimeDelta);
}

void Engine::GameInstance::Update_Late(const _float fTimeDelta)
{
	m_pGameObjectManager->Update_Late(fTimeDelta);
    m_pUIManager->Update_Late(fTimeDelta);
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 광선 발사 함수 ////////////////////////////////////////////////////////
MyRay Engine::GameInstance::Get_LocalRay(const _fmatrix& _worldMatrix) {
    // NDC 변환: x = (2 * screenX / width - 1) / projection[0][0], y = -(2 * screenY / height - 1) / projection[1][1]

    POINT pt{};
    GetCursorPos(&pt);
    ScreenToClient(m_hWnd, &pt);

    MyRay ray = {};

    if (pt.x > m_fWincx || pt.y > m_fWincy) return ray;

    _float4x4 proj = m_pPipeLine->Get_PipeLineMatrix(D3DTS_PROJ);
    _matrix projection = XMLoadFloat4x4(&proj);
    _float vx = (2.0f * pt.x / m_fWincx - 1.0f) / XMVectorGetX(XMVectorSplatX(projection.r[0]));
    _float vy = -(2.0f * pt.y / m_fWincy - 1.0f) / XMVectorGetY(XMVectorSplatY(projection.r[1]));
    //XMVECTOR origin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);  // 뷰 스페이스 원점
    XMVECTOR direction = XMVector4Normalize(XMVectorSet(vx, vy, 1.0f, 0.0f));   // 방향 벡터 (far plane 방향)
    
    // 6번: 뷰 스페이스 레이
    _vector viewOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

    _float4x4 mat = m_pPipeLine->Get_PipeLineInversMatrix(D3DTS_VIEW);
    _matrix invView = XMLoadFloat4x4(&(mat));

    // 월드 스페이스 변환
    _vector worldOrigin = XMVector3TransformCoord(viewOrigin, invView);
    _vector worldDirection = XMVector3Normalize(XMVector3TransformNormal(direction, invView));

    _matrix world = _worldMatrix;
    _matrix invWorld = XMMatrixInverse(nullptr, world);

    XMStoreFloat3(&ray.vOrigin, XMVector3TransformCoord(worldOrigin, invWorld));
    XMStoreFloat3(&ray.vDirection, XMVector3Normalize(XMVector3TransformNormal(worldDirection, invWorld)));

    return ray;
}

_bool Engine::GameInstance::RayTriangleIntersect(const _vector& rayOrigin, const _vector& rayDir, const _vector& v0, const _vector& v1, const _vector& v2
    ,_float& t, _float& u, _float& v)
{
    const float EPSILON = 0.000001f;
    _vector edge1 = v1 - v0;
    _vector edge2 = v2 - v0;
    _vector h = XMVector3Cross(rayDir, edge2);
    float a = XMVectorGetX(XMVector3Dot(edge1, h));
    if (a > -EPSILON && a < EPSILON) return false;  // 평행

    float f = 1.0f / a;
    _vector s = rayOrigin - v0;
    u = f * XMVectorGetX(XMVector3Dot(s, h));
    if (u < 0.0f || u > 1.0f) return false;

    _vector q = XMVector3Cross(s, edge1);
    v = f * XMVectorGetX(XMVector3Dot(rayDir, q)); 
    if (v < 0.0f || u + v > 1.0f) return false;

    t = f * XMVectorGetX(XMVector3Dot(edge2, q));
    if (t > EPSILON) return true;  // 양의 거리

    return false;
}

// 메쉬피킹용인데 안 쓸듯
_bool Engine::GameInstance::IntersectBuffer(const MyRay& _localRay, VIBuffer* _buffer, const _fmatrix& _worldmatrix
    ,_vector& _outworld, _float& _outDist, _uint* _outFaceIndex, _float* _outU, _float* _outV)
{
    if (!_buffer) return false;

    const _float3* vertices = {};
    // 버텍스/인덱스 버퍼 접근 (Mesh 클래스에 GetVertices/GetIndices 가정, 또는 Lock)
    /*if(_buffer->IsAnimation())
        vertices = _buffer->Get_VertexAnimPositions();
    else
        vertices = _buffer->Get_VertexPositions();*/

    const _uint* indices = _buffer->Get_Indices();
    _uint numTriangles = _buffer->Get_NumIndices() / 3;

    float minDist = FLT_MAX;
    bool hit = false;

    for (_uint i = 0; i < numTriangles; ++i) {
        _uint idx0 = indices[i * 3];
        _uint idx1 = indices[i * 3 + 1];
        _uint idx2 = indices[i * 3 + 2];

        _vector v0 = XMLoadFloat3(&vertices[idx0]);
        _vector v1 = XMLoadFloat3(&vertices[idx1]);
        _vector v2 = XMLoadFloat3(&vertices[idx2]);

        _float t;
        _float u{}, v{};
        _vector localorigin = XMLoadFloat3(&_localRay.vOrigin); 
        _vector localdirection = XMLoadFloat3(&_localRay.vDirection);
        //if (RayTriangleIntersect(localorigin, localdirection, v0, v1, v2, t, u, v)) {
        //    if (t < minDist) {
        //        minDist = t;
        //        _outDist = t;
        //        _vector localPoint = localorigin + (localdirection * _outDist);
        //        _outworld = XMVector3TransformCoord(localPoint, _worldmatrix);
        //        if(_outFaceIndex) *_outFaceIndex = i;
        //        //if(_outU) *_outU = u;
        //        //if(_outV) *_outV = v;
        //        hit = true;
        //    }
        //}
        if (TriangleTests::Intersects(localorigin, localdirection, v0, v1, v2, t)) {
            if (t < minDist) {
                minDist = t;
                _outDist = t;
                _vector localPoint = localorigin + (localdirection * _outDist);
                _outworld = XMVector3TransformCoord(localPoint, _worldmatrix);
                if(_outFaceIndex) *_outFaceIndex = i;
                //if(_outU) *_outU = u;
                //if(_outV) *_outV = v;
                hit = true;
            }
        }
    }

    return hit;
}
/******************************************************* 광선 발사 함수 *******************************************************/



//////////////////////////////////////////////////////// Com객체 만들기 함수 ////////////////////////////////////////////////////////
_string Engine::GameInstance::Make_SavefilePath(const _char* _filter, const _char* _defaultExt)
{
    _char filename[MAX_PATH] = {};
    OPENFILENAMEA ofn = {};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = _filter;
    ofn.nFilterIndex = -1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt = _defaultExt;

    if (GetSaveFileNameA(&ofn))
        return filename;
    else
        return "";

    return _string();
}

_string Engine::GameInstance::Make_OpenfilePath(const _char* _filter)
{
    _char filename[MAX_PATH] = {};
    OPENFILENAMEA ofn = {};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = _filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn))
        return filename;

    return "";
}

HRESULT Engine::GameInstance::CreateStructuredBuffer(const void* _data, _uint _elementCount, _uint _structureStride, ID3D11Buffer** _outBuffer, ID3D11ShaderResourceView** _outSRV)
{
    if (!_outBuffer || !_outSRV || !_data) 
        return E_FAIL;

    _uint byteWidth = _elementCount * _structureStride;

    D3D11_BUFFER_DESC bufferdesc{};
    bufferdesc.ByteWidth = byteWidth;
    bufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // 바뀌지 않는다
    bufferdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    bufferdesc.CPUAccessFlags = 0;
    bufferdesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufferdesc.StructureByteStride = _structureStride;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = _data;

    ID3D11Buffer* pBuffer = nullptr;
    HRESULT hr = m_pDevice->CreateBuffer(&bufferdesc, _data ? &initData : nullptr, &pBuffer);
    if (FAILED(hr))
    {
        Safe_Release(pBuffer);
        return hr;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = DXGI_FORMAT_UNKNOWN;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = _elementCount;

    ID3D11ShaderResourceView* pSRV = nullptr;
    hr = m_pDevice->CreateShaderResourceView(pBuffer, &srvDesc, &pSRV);
    if (FAILED(hr))
    {
        Safe_Release(pBuffer);
        Safe_Release(pSRV);
    }

    *_outBuffer = pBuffer;
    *_outSRV = pSRV;

    return S_OK;
}
/******************************************************* Com객체 만들기 함수 *******************************************************/



//////////////////////////////////////////////////////// 헬퍼 함수 ////////////////////////////////////////////////////////
void Engine::GameInstance::Load_Folder(_wstring& _folderPath, _uint _level, _matrix _prematrix, _wstring _ext)
{
    _uint i = 0;
    for (const auto& entry : fs::recursive_directory_iterator(_folderPath))
    {
        // .siho 파일만 처리
        if (entry.is_regular_file() && entry.path().extension() == _ext)
        {
            ++i;
            // 현재 경로 저장
            fs::path currentPath = entry.path();

            // 파일 이름 추출 (확장자 제외)
            wstring fileName = currentPath.stem().wstring();
            //if (fileName == L"SM_FloorA_st02a1_3db03f97")
            //	cout << 1;
            // Prototype 이름 생성: "Prototype_Component_Model_Wonder_Acute"
            wstring prototypeName = L"Prototype_Component_Model_" + fileName;

            // 전체 파일 경로
            wstring filePath = entry.path().wstring();

            // Prototype 등록
            MSG_FAIL(Add_Prototype(_level, prototypeName, Model::Create(m_pDevice, m_pContext, MODEL::NONANIM, filePath, _prematrix))
                , L"Model 로딩에 실패했습니다!", L"뭔가 잘못 불러옴", );
        }
    }
}
/******************************************************* 헬퍼 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Draw_Begin(const _float4* pClearColor)
{
	MSG_FAIL(m_pGraphic_Device->Clear_BackBufferView(pClearColor), L"백버퍼 클리어에 실패했습니다.", L"경고!!!", E_FAIL);

	MSG_FAIL(m_pGraphic_Device->Clear_DepthStencilView(), L"깊이 스텐실 클리어에 실패했습니다.", L"경고!!!", E_FAIL);

	return S_OK;
}

HRESULT Engine::GameInstance::Draw(const _float fTimeDelta)
{
	m_pRenderer->Render(fTimeDelta);
    m_pUIManager->Render(fTimeDelta);
	m_pLevelManager->Render_Level(fTimeDelta);
    m_pLevelManager->Render_Player(fTimeDelta);

	return S_OK;
}

HRESULT Engine::GameInstance::Draw_End()
{
	m_pGraphic_Device->Present();

    // 한 프레임이 끝나고 이벤트 구독 해제 및 Dead오브젝트 삭제 
    m_pGameObjectManager->Delete_DeadGameObject();
    m_pEventManager->Flush_PendingRemove();

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// Graphic_Device ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::ResizeBuffers(_int _windowWidth, _int _windowHeight, HWND& _hWnd)
{
    m_fWincx = (_float)_windowWidth;
    m_fWincy = (_float)_windowHeight;
    Engine_WINCX = m_fWincx;
    Engine_WINCY = m_fWincy;

    RECT window = { 0, 0, LONG(m_fWincx), LONG(m_fWincy) };

    //DWORD style = GetWindowLong(g_hWnd, GWL_STYLE);
    //DWORD exStyle = GetWindowLong(g_hWnd, GWL_EXSTYLE);

    //// 클라이언트 영역 기준으로 전체 윈도우 크기 계산
    //AdjustWindowRectEx(&window, style, FALSE, exStyle);

    AdjustWindowRect(&window, WS_OVERLAPPEDWINDOW, FALSE);

    _int screenCX = GetSystemMetrics(SM_CXSCREEN);
    _int screenCY = GetSystemMetrics(SM_CYSCREEN);

    _int windowStartX = int((screenCX - m_fWincx) * 0.5f);
    _int windowStartY = int((screenCY * 0.47) - (m_fWincy * 0.5f));

    _int totalWidth  = window.right - window.left;
    _int totalHeight = window.bottom - window.top;

    //windowStartX -= 1500;
    //windowStartY += 230;

    SetWindowPos(_hWnd,
        NULL,
        windowStartX, windowStartY,          // 위치 유지
        totalWidth, totalHeight,      // 전체 윈도우 크기
        SWP_NOZORDER);

    // 모든 렌더 타겟들의 버퍼 크기 변경
    m_pRenderTargetManager->ResizeBuffers(m_fWincx, m_fWincy);

    // 렌더러 행렬 재설정
    m_pRenderer->ResizeBuffers(m_fWincx, m_fWincy);

    // 그림자용 행렬 재설정
    m_pShadow->ResizeBuffers(m_fWincx, m_fWincy);


	return m_pGraphic_Device->ResizeBuffers(_windowWidth, _windowHeight, m_Viewport);
}

ID3D11RenderTargetView* Engine::GameInstance::Get_BackBufferRTV()
{
    return m_pGraphic_Device->Get_BackBufferRTV();
}

ID3D11DepthStencilView* Engine::GameInstance::Get_DepthStencilView()
{
    return m_pGraphic_Device->Get_DepthStencilView();
}

HRESULT Engine::GameInstance::Swap_OriginalBackbuffer()
{
    return m_pGraphic_Device->Swap_OriginalBackbuffer();
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// TimerManager ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Add_Timer(const _wstring& _timername)
{
	return m_pTimerManager->Add_Timer(_timername);
}

_float Engine::GameInstance::Compute_Timer(const _wstring& _timername)
{
	return m_pTimerManager->Compute_Timer(_timername);
}

_float Engine::GameInstance::Get_Time(const _wstring& _timername)
{
	return m_pTimerManager->Get_Time(_timername);
}

GameClock* Engine::GameInstance::Add_Clock(const _wstring& _clockName, _float _targetFPS)
{
    return m_pTimerManager->Add_Clock(_clockName, _targetFPS);
}

GameClock* Engine::GameInstance::Get_Clock(const _wstring& _clockName)
{
    return m_pTimerManager->Get_Clock(_clockName);
}
/******************************************************* TimerManager *******************************************************/



//////////////////////////////////////////////////////// LevelManager ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Add_Level(_uint _levelname, Level* level)
{
	return m_pLevelManager->Add_Level(_levelname, level);
}

HRESULT Engine::GameInstance::Add_PlayerLevel(Level* level)
{
    return m_pLevelManager->Add_PlayerLevel(level);
}

HRESULT Engine::GameInstance::Change_Level(_uint _levelname)
{
	return m_pLevelManager->Change_Level(_levelname);
}

HRESULT Engine::GameInstance::Delete_Level(_uint _levelname)
{
	return m_pLevelManager->Delete_Level(_levelname);
}

Level* Engine::GameInstance::Get_Current_Level()
{
    return m_pLevelManager->Get_Current_Level();
}

_uint Engine::GameInstance::Get_Current_LevelID()
{
    return m_pLevelManager->Get_Current_LevelID();
}


Level* Engine::GameInstance::Get_Level(_uint _levelID)
{
    return m_pLevelManager->Get_Level(_levelID);
}
/******************************************************* LevelManager *******************************************************/



//////////////////////////////////////////////////////// PrototypeManager ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Add_Prototype(const _uint _levelID, const _wstring& prototypename, Base* pPrototype)
{
	return m_pPrototypeManager->Add_Prototype(_levelID, prototypename, pPrototype);
}

Base* Engine::GameInstance::Clone_Prototype(const PROTOTYPE _prototypeID, const _uint _levelID, const _wstring& prototypename, void* pArg)
{
	return m_pPrototypeManager->Clone_Prototype(_prototypeID, _levelID, prototypename, pArg);
}

unordered_map<_wstring, class Base*>* Engine::GameInstance::Get_Prototypes()
{
    return m_pPrototypeManager->Get_Prototypes();
}

HRESULT Engine::GameInstance::Clear_Level_Prototype(_uint _levelID)
{
    return m_pPrototypeManager->Clear_Level(_levelID);
}
/******************************************************* PrototypeManager *******************************************************/



//////////////////////////////////////////////////////// GameObjectManager ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Add_GameObject_To_Layer(const _uint _prototypeLevelID, const _wstring& _prototypename, const _uint _levelID, const _wstring& _layername,
    GameObject** _outobject, void* pArg)
{
	return m_pGameObjectManager->Add_GameObject(_prototypeLevelID, _prototypename, _levelID, _layername, _outobject, pArg);
}

HRESULT Engine::GameInstance::Add_GameObject(GameObject* pGameObject, const _uint _levelID, const _wstring& _layername)
{
    return m_pGameObjectManager->Add_GameObject(pGameObject,  _levelID, _layername);
}


GameObject* Engine::GameInstance::Get_GameObject(const _uint _levelID, const _wstring& _layername, const _wstring& _gameobjectname) const
{
	return m_pGameObjectManager->Get_GameObject(_levelID, _layername, _gameobjectname);
}

unordered_map<_wstring, class Layer*>& Engine::GameInstance::Get_Current_Layers()
{
        return m_pGameObjectManager->Get_Current_Layers();
}

Layer* Engine::GameInstance::Get_Layer(const _wstring& _layerName)
{
    return m_pGameObjectManager->Get_Layer(_layerName);
}

HRESULT Engine::GameInstance::Clear_Level_GameObject(_uint _levelID)
{
    return m_pGameObjectManager->Clear_Level(_levelID);
}

void Engine::GameInstance::Set_Parallel_Dirty()
{
    m_pGameObjectManager->Set_Parallel_Dirty();
}
/******************************************************* GameObjectManager *******************************************************/





//////////////////////////////////////////////////////// Renderer ////////////////////////////////////////////////////////
#ifdef _DEBUG
void Engine::GameInstance::Render_Grid()
{
    m_pRenderer->Render_Grid();
}
#endif // _DEBUG

HRESULT Engine::GameInstance::Add_RenderObject(RENDER_GROUP _rendergroup, GameObject* _gameobject)
{
	return m_pRenderer->Add_RenderObject(_rendergroup, _gameobject);
}

HRESULT Engine::GameInstance::Clear_Renderer()
{
	return m_pRenderer->Clear_Renderer();
}

#ifdef _DEBUG
void Engine::GameInstance::Add_Debug_Sphere(BoundingSphere _sphere, _float3 _color)
{
    m_pRenderer->Add_Debug_Sphere(_sphere, _color);
}

void Engine::GameInstance::Add_Debug_Capsule(CAPSULE_DESC _capsule)
{
    m_pRenderer->Add_Debug_Capsule(_capsule);
}
    
void Engine::GameInstance::Add_Debug_Capsule(_float3& _center, _float _radius, _float _halfHeight, _float4& _quaternion, _float3& _color)
{
    m_pRenderer->Add_Debug_Capsule(_center, _radius, _halfHeight, _quaternion, _color);
}

void Engine::GameInstance::Add_Debug_LookLine(_float3& _pos, _float3& _target, _float4& _color)
{
    m_pRenderer->Add_Debug_LookLine(_pos, _target, _color);
}

void Engine::GameInstance::Add_Debug_Fan(const DebugFan& _fan)
{
    m_pRenderer->Add_Debug_Fan(_fan);
}
#endif // _DEBUG

HRESULT Engine::GameInstance::Build_OcTree(const _float3& _worldCenter, _float _worldHalfSize, _uint _maxDepth)
{
    return m_pRenderer->Build_OcTree(_worldCenter, _worldHalfSize, _maxDepth);
}

HRESULT Engine::GameInstance::Add_StaticObject(RENDER_GROUP _group, GameObject* _staticobject, const BoundingBox& _worldAABB)
{
    return m_pRenderer->Add_StaticObject(_group, _staticobject, _worldAABB);
}

HRESULT Engine::GameInstance::Add_Object_To_Octree(RENDER_GROUP _group, GameObject* _staticobject, const BoundingBox& _worldAABB)
{
    return m_pRenderer->Add_StaticObject(_group, _staticobject, _worldAABB);
}

HRESULT Engine::GameInstance::Remove_StaticObject(GameObject* _staticObject)
{
    return m_pRenderer->Remove_StaticObject(_staticObject);
}

void Engine::GameInstance::Query_AABB(const BoundingBox& _aabb, vector<const STATIC_OBJ_INFO*>& _outInfos) const 
{
    return m_pRenderer->Query_AABB(_aabb, _outInfos);
}

void Engine::GameInstance::Clear_OcTree()
{
    m_pRenderer->Clear_OcTree();
}

void Engine::GameInstance::Set_HDROption(_float4 _vHDROption)
{
    m_pRenderer->Set_HDROption(_vHDROption);
}

void Engine::GameInstance::Set_SSAOOption(_float4 _vSSAOOption)
{
    m_pRenderer->Set_SSAOOption(_vSSAOOption);
}

void Engine::GameInstance::Set_ShaderDesc(ShaderDesc _ColorGradingDesc)
{
    m_pRenderer->Set_ShaderDesc(_ColorGradingDesc);
}


void Engine::GameInstance::ChangeMap(_float _t)
{
    m_pRenderer->ChangeMap(_t);
}

void  Engine::GameInstance::ChangeBaseMap()
{
    m_pRenderer->ChangeBaseMap();
}
void  Engine::GameInstance::ChangeCaveMap(_float _t)
{
    m_pRenderer->ChangeCaveMap(_t);
}
void Engine::GameInstance::Set_Capture(_bool _bCapture)
{
    m_pRenderer->Set_Capture(_bCapture);
}
void Engine::GameInstance::ChangeCustomize()
{
    m_pRenderer->ChangeCustomize();
}
void Engine::GameInstance::ChangeMainMap()
{
    m_pRenderer->ChangeMainMap();
}
void Engine::GameInstance::ChangeChurch()
{
    m_pRenderer->ChangeChurch();
}
void Engine::GameInstance::ChangeChurchBoss()
{
    m_pRenderer->ChangeChurchBoss();
}
void Engine::GameInstance::ChangeEnd()
{
    m_pRenderer->ChangeEnd();
}

_float4 Engine::GameInstance::Get_PlayerSkinColor()
{
    return m_pRenderer->Get_PlayerSkinColor();
}

void Engine::GameInstance::Set_PlayerSkinColor(_float3 vColor)
{
    return m_pRenderer->Set_PlayerSkinColor(vColor);
}
/******************************************************* Renderer *******************************************************/



//////////////////////////////////////////////////////// PipeLine ////////////////////////////////////////////////////////
void Engine::GameInstance::Set_Transform(D3DTRANSFORM eD3DTransform, _matrix matrix)
{
	m_pPipeLine->Set_Transform(eD3DTransform, matrix);
}

HRESULT Engine::GameInstance::Bind_PipeLineMatrix(Shader* shader, const _string& constant, D3DTRANSFORM transform)
{
	return m_pPipeLine->Bind_PipeLineMatrix(shader, constant, transform);
}

HRESULT Engine::GameInstance::Bind_PipeLineMatrix_View(Shader* _shader)
{
    return m_pPipeLine->Bind_PipeLineMatrix_View(_shader);
}

HRESULT Engine::GameInstance::Bind_PipeLineMatrix_Proj(Shader* _shader)
{
    return m_pPipeLine->Bind_PipeLineMatrix_Proj(_shader);
}

HRESULT Engine::GameInstance::Bind_PipeLineInverseMatrix(Shader* shader, const _string& constant, D3DTRANSFORM transform)
{
	return m_pPipeLine->Bind_PipeLineInverseMatrix(shader, constant, transform);
}

HRESULT Engine::GameInstance::Bind_PipeLineInverseMatrix_View(Shader* _shader)
{
    return m_pPipeLine->Bind_PipeLineInverseMatrix_View(_shader);
}

HRESULT Engine::GameInstance::Bind_PipeLineInverseMatrix_Proj(Shader* _shader)
{
    return m_pPipeLine->Bind_PipeLineInverseMatrix_Proj(_shader);
}

HRESULT Engine::GameInstance::Bind_CameraPosition(Shader* shader, const _string& constant)
{
	return m_pPipeLine->Bind_CameraPosition(shader);
}

HRESULT Engine::GameInstance::Bind_PipeLine_All(Shader* _shader)
{
    return m_pPipeLine->Bind_PipeLine_All(_shader);
}

void Engine::GameInstance::Set_CamNearFar(_float _near, _float _far)
{
    m_pPipeLine->Set_CamNearFar(_near, _far);
}
void Engine::GameInstance::Update_PrevMatrices()
{
    m_pPipeLine->Update_PrevMatrices();
}
_float4 Engine::GameInstance::Get_CameraPosition()
{
    return m_pPipeLine->Get_CameraPosition();
}

_float4x4 Engine::GameInstance::Get_PipeLineMatrix(D3DTRANSFORM _transform)
{
    return m_pPipeLine->Get_PipeLineMatrix(_transform);
}

_float4x4 Engine::GameInstance::Get_PrevPipeLineMatrix(D3DTRANSFORM _transform)
{
    return m_pPipeLine->Get_PrevPipeLineMatrix(_transform);
}

_float4x4 Engine::GameInstance::Get_PipeLineInversMatrix(D3DTRANSFORM _transfrom)
{
    return m_pPipeLine->Get_PipeLineInversMatrix(_transfrom);
}
/******************************************************* PipeLine *******************************************************/



//////////////////////////////////////////////////////// InputManager ////////////////////////////////////////////////////////
_byte Engine::GameInstance::Get_DIKeyState(_ubyte byKeyID) const
{
	return m_pInputManager->Get_DIKeyState(byKeyID);
}
_bool Engine::GameInstance::KeyPress(_ubyte byKeyID) const
{
	return m_pInputManager->KeyPress(byKeyID);
}
_bool Engine::GameInstance::KeyDown(_ubyte byKeyID) const
{
	return m_pInputManager->KeyDown(byKeyID);
}
_bool Engine::GameInstance::KeyUp(_ubyte byKeyID) const
{
	return m_pInputManager->KeyUp(byKeyID);
}
_byte Engine::GameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse) const
{
	return m_pInputManager->Get_DIMouseState(eMouse);
}
_bool Engine::GameInstance::MousePress(MOUSEKEYSTATE eMouse) const
{
	return m_pInputManager->MousePress(eMouse);
}
_bool Engine::GameInstance::MouseDown(MOUSEKEYSTATE eMouse) const
{
	return m_pInputManager->MouseDown(eMouse);
}
_bool Engine::GameInstance::MouseUp(MOUSEKEYSTATE eMouse) const
{
	return m_pInputManager->MouseUp(eMouse);
}
_long Engine::GameInstance::MouseMove(MOUSEMOVESTATE eMouseState) const
{
	return m_pInputManager->Get_DIMouseMove(eMouseState);
}

HRESULT Engine::GameInstance::Register_HotKey(_ubyte Key, bool bCtrl, bool bShift, bool bAlt, HOTKEY_MODE eMode, HotKeyCallback Func)
{
    return m_pInputManager->Register_HotKey(Key,bCtrl,bShift,bAlt, Func, eMode);
}

_bool Engine::GameInstance::IsAnyKeyPressed()
{
    return m_pInputManager->IsAnyKeyPressed();
}

/******************************************************* InputManager *******************************************************/



//////////////////////////////////////////////////////// LightManager ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Add_Light(const _uint _lightindex, const LIGHT_DESC& _lightdesc)
{
	return m_pLightManager->Add_Light(_lightindex, _lightdesc);
}

HRESULT Engine::GameInstance::Delete_Light(const _uint _lightindex)
{
	return m_pLightManager->Delete_Light(_lightindex);
}

LIGHT_DESC* Engine::GameInstance::Get_LightDesc(const _uint _lightindex)
{
	return m_pLightManager->Get_LightDesc(_lightindex);
}

HRESULT Engine::GameInstance::Set_LightDesc(const _uint _lightindex, const LIGHT_DESC& _lightdesc)
{
	return m_pLightManager->Set_LightDesc(_lightindex, _lightdesc);
}

void Engine::GameInstance::Render_Lights(Shader* _shader, VIBuffer_Rect* _buffer, const _float fTimeDelta)
{
    return m_pLightManager->Render(_shader, _buffer, fTimeDelta);
}

unordered_map<_uint, Light*>* Engine::GameInstance::Get_LightuMapPtr()
{
    return m_pLightManager->Get_LightuMapPtr();
}
_int Engine::GameInstance::Get_LightCnt()
{
    return m_pLightManager->Get_LightCnt();
}
Light* Engine::GameInstance::Get_LightPtr(const _uint _lightindex)
{
    return m_pLightManager->Get_LightPtr(_lightindex);
}
/******************************************************* LightManager *******************************************************/



//////////////////////////////////////////////////////// SamplerManager ////////////////////////////////////////////////////////
unordered_map<_string, class Sampler*> Engine::GameInstance::Get_Sampler_String()
{
	return m_pSamplerManager->Get_Sampler_String();
}

unordered_map<_uint, Sampler*> Engine::GameInstance::Get_Sampler_Slot()
{
	return m_pSamplerManager->Get_Sampler_Slot();
}

Sampler* Engine::GameInstance::Get_Sampler(_uint _slot)
{
	return m_pSamplerManager->Get_Sampler(_slot);
}

Sampler* Engine::GameInstance::Get_Sampler(const _string& _name)
{
	return m_pSamplerManager->Get_Sampler(_name);
}

HRESULT Engine::GameInstance::Bind_Sampler(_uint _slot, _uint _stageMask)
{
	return m_pSamplerManager->Bind_Sampler(_slot, _stageMask);
}

HRESULT Engine::GameInstance::Bind_Sampler(const _string& _name, _uint _stageMask)
{
	return m_pSamplerManager->Bind_Sampler(_name, _stageMask);
}

SamplerManager* Engine::GameInstance::Get_SamplerManager()
{
	return m_pSamplerManager->Get_SamplerManager();
}
/******************************************************* SamplerManager *******************************************************/



//////////////////////////////////////////////////////// Calculator ////////////////////////////////////////////////////////
_float Engine::GameInstance::RandomValue(_float _min, _float _max)
{
    return m_pCalculator->RandomValue(_min, _max);
}

_uint Engine::GameInstance::RandomValue_int(_uint _min, _uint _max)
{
    return m_pCalculator->RandomValue_int(_min, _max);
}

_vector Engine::GameInstance::QuaternionToDegrees(_fvector _quaternion)
{
    return m_pCalculator->QuaternionToDegrees(_quaternion);
}
/******************************************************* Calculator *******************************************************/



//////////////////////////////////////////////////////// FontManager ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Add_BitmapFont(const _wstring& _BitmapFontName, const wstring& strFntPath, _string FontTextureName)
{
    return m_pFontManager->Add_BitmapFont(_BitmapFontName, strFntPath, FontTextureName);
}

BitmapFont* Engine::GameInstance::Get_BitmapFont(const _wstring _bitmapName)
{
    return m_pFontManager->Get_BitmapFont(_bitmapName);
}

HRESULT Engine::GameInstance::Load_Font(const _wstring& _fontName, const _wstring& _fontPath)
{
    return m_pFontManager->Load_Font(_fontName, _fontPath);
}

void Engine::GameInstance::Draw_Text(const _wstring& _fontName, const _wstring& _text, const _float2& _position, const _float& _Rotation, _fvector _color,  const _float2& _size)
{
    m_pFontManager->Draw_Text(_fontName, _text, _position, _color, _Rotation,_size);
}

void Engine::GameInstance::Render_Texts_Stacked()
{
    m_pFontManager->Render_Texts_Stacked();
}



void Engine::GameInstance::Draw_Text_Begin()
{
    m_pFontManager->Draw_Text_Begin();
}

void Engine::GameInstance::Draw_Text_Mask_Begin()
{
    m_pFontManager->Draw_Text_Mask_Begin();
}

void Engine::GameInstance::Draw_Text_Alpha_Begin()
{
    m_pFontManager->Draw_Text_Alpha_Begin();
}

HRESULT Engine::GameInstance::Set_PsOutLineShader()
{
    return m_pFontManager->Set_PsOutLineShader();
}

void Engine::GameInstance::Draw_BitMapText_Begin()
{
    m_pFontManager->Draw_BitMapText_Begin();
}

void Engine::GameInstance::Draw_Text_End()
{
    m_pFontManager->Draw_Text_End();
}

void Engine::GameInstance::Draw_Text_Outline4(const _wstring& _fontName, const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor)
{
    m_pFontManager->Draw_Text_Outline4(_fontName, _text, _position, _color, _outlinecolor);
}

void Engine::GameInstance::Draw_Text_Outline8(const _wstring& _fontName, const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor)
{
    m_pFontManager->Draw_Text_Outline8(_fontName, _text, _position, _color, _outlinecolor);
}

void Engine::GameInstance::Draw_Text_Stack(const TEXT_DESC& _textdesc)
{
    m_pFontManager->Add_Text_Stack(_textdesc);
}

void Engine::GameInstance::Add_Text_Stack(const TEXT_DESC& _textdesc)
{
    /* 위에랑 똑같은 함수인데 함수명만 다름 */
    m_pFontManager->Add_Text_Stack(_textdesc);
}


MyFont* Engine::GameInstance::Get_Font(const _wstring& _fontName)
{
    return m_pFontManager->Get_Font(_fontName);
}

SpriteBatch* Engine::GameInstance::Get_Batch()
{
    return m_pFontManager->Get_Batch();
}


void Engine::GameInstance::Clear_FontManager()
{
    return m_pFontManager->Clear_FontManager();
}


void Engine::GameInstance::Draw_Text_Outline_Begin(ID3D11ShaderResourceView* pFont, ID3D11Texture2D* pTex2D, float fThickness, _float4 OutLineColor, _float4 Color)
{
    return m_pFontManager->Draw_Text_Outline_Begin(pFont,pTex2D,fThickness, OutLineColor, Color);
}

void Engine::GameInstance::Set_OutlineShader(class Shader* pShader)
{
    return m_pFontManager->Set_OutlineShader(pShader);
}


#ifdef _DEBUG
vector<string>      Engine::GameInstance::Get_All_FontName()
{
    return m_pFontManager->Get_All_FontName();
}

vector<string>      Engine::GameInstance::Get_All_BitmapFontName()
{
    return m_pFontManager->Get_All_BitmapFontName();
}

#endif // _DEBUG

/******************************************************* FontManager *******************************************************/



//////////////////////////////////////////////////////// CameraManager ////////////////////////////////////////////////////////
void Engine::GameInstance::CameraManager_Reset(_uint _maxCameraCount)
{
    m_pCameraManager->CameraManager_Reset(_maxCameraCount);
}

HRESULT Engine::GameInstance::Add_Camera(_uint _prototypeLevelID, const _wstring& _cameraName, _uint _levelID, _uint _cameraNum, GameObject** _outCamera, void* arg)
{
    return m_pCameraManager->Add_Camera(_prototypeLevelID, _cameraName, _levelID, _cameraNum, _outCamera, arg);
}

void Engine::GameInstance::Camera_Change(_uint _cameraNum)
{
    m_pCameraManager->Camera_Change(_cameraNum);
}

void Engine::GameInstance::Camera_Active(_uint _cameraNum, _bool _isActive)
{
    m_pCameraManager->Camera_Active(_cameraNum, _isActive);
}

void Engine::GameInstance::Update_Cameras(const _float fTimeDelta)
{
    m_pCameraManager->Update_Cameras(fTimeDelta);
}

vector<class Camera*> Engine::GameInstance::Get_CameraVector()
{
    return m_pCameraManager->Get_CameraVector();
}

inline Camera* Engine::GameInstance::Get_Camera(_uint _cameraNum)
{
    return m_pCameraManager->Get_Camera(_cameraNum);
}

inline Camera* Engine::GameInstance::Get_CurrentCamera()
{
    return m_pCameraManager->Get_CurrentCamera();
}

const _float3 Engine::GameInstance::Get_PrevCameraPos_Float3()
{
    return m_pCameraManager->Get_PrevCameraPos_Float3();
}

const _vector Engine::GameInstance::Get_PrevCameraPos_Vector()
{
    return m_pCameraManager->Get_PrevCameraPos_Vector();
}

const _float4x4 Engine::GameInstance::Get_PrevCamera_Float4x4()
{
    return m_pCameraManager->Get_PrevCamera_Float4x4();
}

const _matrix Engine::GameInstance::Get_PrevCamera_Matrix()
{
    return m_pCameraManager->Get_PrevCamera_Matrix();
}

void Engine::GameInstance::Set_Aspect(_float _aspect)
{
    m_pCameraManager->Set_Aspect(_aspect);
}

void Engine::GameInstance::Camera_LookAt(_vector _targetPosition)
{
    m_pCameraManager->Camera_LookAt(_targetPosition);
}
/******************************************************* CameraManager *******************************************************/





//////////////////////////////////////////////////////// RenderStateManager ////////////////////////////////////////////////////////
void Engine::GameInstance::Set_RasterizerState(RSTATE _rasterrizerState)
{
    m_pRenderStateManager->Set_RasterizerState(_rasterrizerState);
}

void Engine::GameInstance::Set_RasterizerState(const _string& _name)
{
    m_pRenderStateManager->Set_RasterizerState(_name);
}

void Engine::GameInstance::Set_DepthStencilState(DSSTATE _depthStencilState, _uint _stencilRef)
{
    m_pRenderStateManager->Set_DepthStencilState(_depthStencilState, _stencilRef);
}

void Engine::GameInstance::Set_DepthStencilState(const _string& _name, _uint _stencilRef)
{
    m_pRenderStateManager->Set_DepthStencilState(_name, _stencilRef);
}

void Engine::GameInstance::Set_BlendState(BSTATE _blendState)
{
    m_pRenderStateManager->Set_BlendState(_blendState);
}

void Engine::GameInstance::Set_BlendState(const _string& _name)
{
    m_pRenderStateManager->Set_BlendState(_name);
}

ID3D11RasterizerState* Engine::GameInstance::Get_RS(const _string& _name)
{
    return m_pRenderStateManager->Get_RS(_name);

}

ID3D11DepthStencilState* Engine::GameInstance::Get_DSS(const _string& _name)
{
    return m_pRenderStateManager->Get_DSS(_name);

}

ID3D11BlendState* Engine::GameInstance::Get_BS(const _string& _name)
{
    return m_pRenderStateManager->Get_BS(_name);
}

ID3D11RasterizerState* Engine::GameInstance::Get_RS(RSSET _rasterrizerState)
{
    return m_pRenderStateManager->Get_RS(_rasterrizerState);
}

ID3D11DepthStencilState* Engine::GameInstance::Get_DSS(DSSSET _depthStencilState)
{
    return m_pRenderStateManager->Get_DSS(_depthStencilState);
}

ID3D11BlendState* Engine::GameInstance::Get_BS(BSSET _blendState)
{
    return m_pRenderStateManager->Get_BS(_blendState);
}
/******************************************************* RenderStateManager *******************************************************/



//////////////////////////////////////////////////////// RenderTargetManager ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Add_RenderTarget(RenderTargets _targetName, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor)
{
    return m_pRenderTargetManager->Add_RenderTarget(_targetName, _width, _height, _textureformat, _clearcolor);
}
HRESULT Engine::GameInstance::Add_RenderTarget_Shadow(RenderTargets _targetName, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor)
{
    return m_pRenderTargetManager->Add_RenderTarget_Shadow(_targetName, _width, _height, _textureformat, _clearcolor);
}
HRESULT Engine::GameInstance::Add_MultiRenderTarget(MRT _mrtName, RenderTargets _rtvName)
{
    return m_pRenderTargetManager->Add_MultiRenderTarget(_mrtName, _rtvName);
}

HRESULT Engine::GameInstance::Bind_RT_ShaderResource_FullSlot(RenderTargets _rtvName, Shader* _shader, _uint _slotNum, _uint _stageMask)
{
    return m_pRenderTargetManager->Bind_ShaderResource_FullSlot(_rtvName, _shader, _slotNum, _stageMask);
}

HRESULT Engine::GameInstance::Begin_MRT(MRT _mrtName, _bool DSV, _bool _Clear)
{
    return m_pRenderTargetManager->Begin_MRT(_mrtName, DSV, _Clear);
}

HRESULT Engine::GameInstance::Begin_MRT_HDR(MRT _mrtName, ID3D11DepthStencilView* _DSV, _bool _Clear)
{
    return m_pRenderTargetManager->Begin_MRT_HDR(_mrtName, _DSV, _Clear);
}

HRESULT Engine::GameInstance::Begin_RT(RenderTargets _targetName, ID3D11DepthStencilView* _DSV)
{
    return m_pRenderTargetManager->Begin_RT(_targetName, _DSV);
}

HRESULT Engine::GameInstance::Begin_RT_HDR(RenderTargets _targetName, ID3D11DepthStencilView* _DSV, _bool _Clear)
{
    return m_pRenderTargetManager->Begin_RT_HDR(_targetName, _DSV, _Clear);
}

ID3D11DepthStencilView** Engine::GameInstance::Begin_MRT_ShadowCascade(MRT _mrtName, vector<GameObject*> m_vecObjects[], _float _fTimeDelta, _int iNumDSV, RENDER_GROUP _eRGroup)
{
    return m_pRenderTargetManager->Begin_MRT_ShadowCascade(_mrtName, m_vecObjects, _fTimeDelta, iNumDSV, _eRGroup);
}

HRESULT Engine::GameInstance::End_MRT()
{
    return m_pRenderTargetManager->End_MRT();
}

HRESULT Engine::GameInstance::Reset_MRT()
{
    return m_pRenderTargetManager->Reset_MRT();
}

void Engine::GameInstance::Copy_Resource(RenderTargets _rtvName, ID3D11Resource* pDest)
{
    return m_pRenderTargetManager->Copy_Resource(_rtvName, pDest);
}

ID3D11ShaderResourceView* Engine::GameInstance::Get_ShaderResourceView(RenderTargets _target)
{
    return m_pRenderTargetManager->Get_ShaderResourceView(_target);
}

ID3D11RenderTargetView* Engine::GameInstance::Get_RenderTargetView(RenderTargets _target)
{
    return m_pRenderTargetManager->Get_RenderTargetView(_target);
}

void Engine::GameInstance::Clear_RTV(MRT _mrtName)
{
    m_pRenderTargetManager->Clear_RTV(_mrtName);
}

#ifdef _DEBUG
HRESULT Engine::GameInstance::Ready_RT_Debug(RenderTargets _RTName, _float _fX, _float _fY, _float _sizeX, _float _sizeY)
{
    return m_pRenderTargetManager->Ready_Debug(_RTName, _fX, _fY, _sizeX, _sizeY);
}

HRESULT Engine::GameInstance::Render_RT(MRT _MRTName, class Shader* _shader, class VIBuffer_Rect* _buffer, const _float fTimeDelta)
{
    return m_pRenderTargetManager->Render(_MRTName, _shader, _buffer, fTimeDelta);
}

HRESULT Engine::GameInstance::Render_RT(RenderTargets _targetName, Shader* _shader, VIBuffer_Rect* _buffer, const _float fTimeDelta)
{
    return m_pRenderTargetManager->Render(_targetName, _shader, _buffer, fTimeDelta);;
}

PrimitiveBatch<VertexPositionColor>* Engine::GameInstance::Get_DebugBatch()
{
    return m_pRenderer->Get_DebugBatch();
}

void Engine::GameInstance::Add_DebugComponent(Component* _component)
{
    m_pRenderer->Add_DebugComponent(_component);
}
#endif // _DEBUG
/******************************************************* RenderTargetManager *******************************************************/



//////////////////////////////////////////////////////// Shadow ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Add_ShadowLight(const SHADOW_DESC& _shadowDesc)
{
    return m_pShadow->Add_ShadowLight(_shadowDesc);
}

inline _float4x4 Engine::GameInstance::Get_LightMatrix(D3DTRANSFORM _transform)
{
    return m_pShadow->Get_LightMatrix(_transform);
}
    
HRESULT Engine::GameInstance::Add_ShadowCascadeLight(_int _iNumCascade, const SHADOW_DESC& _shadowDesc, _float _Bias[])
{
    return m_pShadow->Add_ShadowCascadeLight(_iNumCascade, _shadowDesc, _Bias);
}

inline _float4x4* Engine::GameInstance::Get_ShadowCascade_Matrix(D3DTRANSFORM _transform)
{
    return m_pShadow->Get_ShadowCascade_Matrix(_transform);
}

_float4& Engine::GameInstance::Get_ShadowCascade_Split()
{
    return m_pShadow->Get_ShadowCascade_Split();
}

_float* Engine::GameInstance::Get_ShadowCascade_Bias()
{
    return m_pShadow->Get_ShadowCascade_Bias();
}

_int Engine::GameInstance::Get_ShadowCascade_Num()
{
    return m_pShadow->Get_ShadowCascade_Num();
}
_float4& Engine::GameInstance::Get_ShadowLightDir()
{
    return m_pShadow->Get_ShadowLightDir();
}
void Engine::GameInstance::Follow_ShadowCascade_Matrix(_vector _at)
{
    return m_pShadow->Follow_ShadowCascade_Matrix(_at);
}
/******************************************************* Shadow *******************************************************/



//////////////////////////////////////////////////////// UIManager ////////////////////////////////////////////////////////
const unordered_map<size_t, UIObject*>&  Engine::GameInstance::Get_AllUIObjects()
{
    return m_pUIManager->Get_AllUIObjects();
}

HRESULT Engine::GameInstance::Add_UIObject(UIObject* _ui, bool bReAdd)
{
    return m_pUIManager->Add_UIObject(_ui, bReAdd);
}

void Engine::GameInstance::Clear_UIManager()
{
    m_pUIManager->Clear_UIManager();
}

void Engine::GameInstance::Delete_DeadUI()
{
    m_pUIManager->Delete_DeadUI();
}
void Engine::GameInstance::Set_Visible_All_UI(_bool _isVisible)
{
    m_pUIManager->Visible_All_UI(_isVisible);
}

void Engine::GameInstance::Process_UI_Picking(const POINT& _mousePos, _bool _bClicked)
{
    m_pUIManager->Process_Picking(_mousePos, _bClicked);
}

void Engine::GameInstance::Set_UIManager_Active(_bool _isActive)
{
    m_pUIManager->Set_Active(_isActive);
}

bool Engine::GameInstance::Get_UIManager_Active()
{
    return m_pUIManager->Get_Active();
}



HRESULT Engine::GameInstance::Remove_UIObject(class UIObject* _ui)
{
    return m_pUIManager->Remove_UIObject(_ui);
}

HRESULT Engine::GameInstance::Remove_UIObject(_wstring UIName)
{
    return m_pUIManager->Remove_UIObject(UIName);
}

void Engine::GameInstance::Rename_Object(wstring OldName,UIObject* pObj)
{
    return m_pUIManager->Rename_Object(OldName,pObj);
}

UIObject* Engine::GameInstance::Find_UI_ByName(const _wstring& _uiName)
{
    return m_pUIManager->Find_UI_ByName(_uiName);
}

UIObject* Engine::GameInstance::Get_ClickedUI()
{
    return m_pUIManager->Get_ClickedUI();
}
void Engine::GameInstance::Clear_ClickedUI()
{
    return m_pUIManager->Clear_ClickedUI();
}

UIObject* Engine::GameInstance::Get_HoveredUI()
{
    return m_pUIManager->Get_HoveredUI();
}
void Engine::GameInstance::Clear_HoveredUI()
{
    return m_pUIManager->Clear_HoveredUI();
}

void Engine::GameInstance::Push_Window(wstring _windowName)
{
    return m_pUIManager->Push_Window(_windowName);
}

void Engine::GameInstance::Close_Window(wstring _windowName)
{
    return m_pUIManager->Close_Window(_windowName);
}
void Engine::GameInstance::Close_Window()
{
    return m_pUIManager->Close_Window();
}

UIObject* Engine::GameInstance::Get_CurrentWindow()
{
    return m_pUIManager->Get_CurrentWindow();
}

bool Engine::GameInstance::Is_Empty_WindowStack()
{
    return m_pUIManager->Is_Empty_WindowStack();
}

void    Engine::GameInstance::Set_Force_HoverUI(UIObject* pObj)
{
    return m_pUIManager->Set_Force_HoverUI(pObj);
}

size_t    Engine::GameInstance::Get_WindowStackCount()
{
    return m_pUIManager->Get_WindowStackCount();
}

UIObject* Engine::GameInstance::Get_Top_At_WindowStack()
{
    return m_pUIManager->Get_Top_At_WindowStack();
}

void Engine::GameInstance::Set_AllLoadPersistents(bool b)
{
    return m_pUIManager->Set_AllLoadPersistents(b);
}

bool Engine::GameInstance::Get_AllLoadPersistents()
{
    return m_pUIManager->Get_AllLoadPersistents();
}

void        Engine::GameInstance::Change_UIMode(UI_MODE eMode, bool bUseActiveAnim)
{
    return m_pUIManager->Change_UIMode(eMode,bUseActiveAnim);
}

#ifdef _DEBUG
void Engine::GameInstance::Render_UIHierarchy()
{
    return m_pUIManager->Render_Hierarchy();
}

void Engine::GameInstance::Render_Search()
{
    return m_pUIManager->Render_Search();
}

void Engine::GameInstance::Set_SelectObject(UIObject* pObj)
{
    return m_pUIManager->Set_SelectObject(pObj);
}

UIObject* Engine::GameInstance::Get_UISelectObject()
{
    return m_pUIManager->Get_SelectObject();
}
#endif // _DEBUG

void Engine::GameInstance::Close_All_Menu()
{
    return m_pUIManager->Close_All_Menu();
}

void Engine::GameInstance::Register_Pooling_Func(UIPOOLINGFUNC         Func)
{
    return m_pUIManager->Register_Pooling_Func(Func);
}

UIObject* Engine::GameInstance::Get_PoolObject(_uint eType)
{
    return m_pUIManager->Get_PoolObject(eType);
}

UIObject* Engine::GameInstance::Find_PersistentUI_ByName(const _wstring& _uiName)
{
    return m_pUIManager->Find_PersistentUI_ByName(_uiName);
}

void Engine::GameInstance::Register_Factory(_uint eType, UIObjectInfo Info)
{
    return m_pUIManager->Register_Factory(eType, Info);
}

void Engine::GameInstance::Add_PoolObject(_uint eType, _uint Size)
{
    return m_pUIManager->Add_PoolObject(eType, Size);
}

void Engine::GameInstance::Return_PoolObject(_uint eType, UIObject* pTarget)
{
    return m_pUIManager->Return_PoolObject(eType, pTarget);
}

UIObjectInfo* Engine::GameInstance::Get_Proto_At_UIPool(_uint eType)
{
    return m_pUIManager->Get_Proto_At_UIPool(eType);
}

HRESULT        Engine::GameInstance::Register_PersistentUI(class UIObject* _ui)
{
    return m_pUIManager->Register_PersistentUI(_ui);
}

void        Engine::GameInstance::Clear_Scene_UI()
{
    return m_pUIManager->Clear_Scene_UI();
}

/******************************************************* UIManager *******************************************************/


//////////////////////////////////////////////////////// SoundManager ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::LoadSound(const string& _name, const std::wstring& _filePath, SoundID* _outID)
{
    return m_pSoundManager->LoadSound(_name, _filePath, _outID);
}

HRESULT Engine::GameInstance::LoadSound_AddGroup(const string& _name, const string& _groupName, const std::wstring& _filePath, SoundID* _outID)
{
    return m_pSoundManager->LoadSound_AddGroup(_name, _groupName, _filePath, _outID);
}

HRESULT Engine::GameInstance::LoadSoundFolder(const wstring& _folderPath)
{
    return m_pSoundManager->LoadSoundFolder(_folderPath);
}

HRESULT Engine::GameInstance::LoadSoundFolder_Recursive(const wstring& _folderPath)
{
    return m_pSoundManager->LoadSoundFolder_Recursive(_folderPath);
}

void Engine::GameInstance::Delete_Sound(const _string& _soundName)
{
    m_pSoundManager->Delete_Sound(_soundName);
}

void Engine::GameInstance::Delete_Sound(SoundID _ID)
{
    m_pSoundManager->Delete_Sound(_ID);
}

void Engine::GameInstance::Play_Sound(const string& _name, float _volume, bool _loop)
{
    m_pSoundManager->Play_Sound(_name, _volume, _loop);
}

void Engine::GameInstance::Play_Sound_Unique(const string& _name, float _volume, bool _loop)
{
    m_pSoundManager->Play_Sound_Unique(_name, _volume, _loop);
}

void Engine::GameInstance::Play_Sound_InGroup(const string& _name, const string& _groupName, float _volume, bool _loop)
{
    m_pSoundManager->Play_Sound_InGroup(_name, _groupName, _volume, _loop);
}

void Engine::GameInstance::StopSound(const string& _name)
{
    m_pSoundManager->StopSound(_name);
}

void Engine::GameInstance::StopGroup(const string& _groupName)
{
    m_pSoundManager->StopGroup(_groupName);
}

void Engine::GameInstance::StopAllSound()
{
    m_pSoundManager->StopAll();
}

void Engine::GameInstance::FadeOutSound(const string& _name, _float _fadeDuration)
{
    m_pSoundManager->FadeOutSound(_name, _fadeDuration);
}

void Engine::GameInstance::SetVolume(const string& _name, float _volume)
{
    m_pSoundManager->SetVolume(_name, _volume);
}

void Engine::GameInstance::SetGroupVolume(const string& _groupName, float _volume)
{
    m_pSoundManager->SetGroupVolume(_groupName, _volume);
}

void Engine::GameInstance::SetMasterVolume(float _volume)
{
    m_pSoundManager->SetMasterVolume(_volume);
}

_bool Engine::GameInstance::IsPlaying(const string& _name)
{
    return m_pSoundManager->IsPlaying(_name);
}

_bool Engine::GameInstance::IsGroupPlaying(const string& _groupName)
{
    return m_pSoundManager->IsGroupFinished(_groupName);
}

_bool Engine::GameInstance::IsGroupFinished(const string& _groupName)
{
    return m_pSoundManager->IsGroupFinished(_groupName);
}

void Engine::GameInstance::AddSoundToGroup(const string& _soundName, const string& _groupName)
{
    m_pSoundManager->AddSoundToGroup(_soundName, _groupName);
}

void Engine::GameInstance::RemoveSoundFromGroup(const string& _soundName, const string& _groupName)
{
    m_pSoundManager->RemoveSoundFromGroup(_soundName, _groupName);
}

void Engine::GameInstance::PlayGroup(const string& _groupName, float _volume, bool _loop)
{
    m_pSoundManager->PlayGroup(_groupName, _volume, _loop);
}

_float Engine::GameInstance::GetMasterVolume() const
{
    return m_pSoundManager->GetMasterVolume();
}

_uint Engine::GameInstance::GetGroupPlayingCount(const string& _groupName)
{
    return m_pSoundManager->GetGroupPlayingCount(_groupName);
}

void Engine::GameInstance::ClearGroup(const string& _groupName)
{
    m_pSoundManager->ClearGroup(_groupName);
}

void Engine::GameInstance::Clear_SoundManager()
{
    m_pSoundManager->Clear_SoundManager();
}

void Engine::GameInstance::Play_Sound_RandomInGroup(const string& _groupName, _float _volume, _bool _loop)
{
    m_pSoundManager->PlayRandomInGroup(_groupName, _volume, _loop);
}

void Engine::GameInstance::Play_Sound(SoundID _id, _float _volume, _bool _loop)
{
    m_pSoundManager->Play_Sound(_id, _volume, _loop);
}

void Engine::GameInstance::Play_Sound_InGroup(SoundID _id, SoundID _groupID, _float _volume, _bool _loop)
{
    m_pSoundManager->Play_Sound_InGroup(_id, _groupID, _volume, _loop);
}

void Engine::GameInstance::StopSound(SoundID _id)
{
    m_pSoundManager->StopSound(_id);
}

void Engine::GameInstance::StopGroup(SoundID _groupID)
{
    m_pSoundManager->StopGroup(_groupID);
}

_bool Engine::GameInstance::IsPlaying(SoundID _id)
{
    return m_pSoundManager->IsPlaying(_id);
}

_bool Engine::GameInstance::IsGroupPlaying(SoundID _groupID)
{
    return m_pSoundManager->IsGroupPlaying(_groupID);
}

_bool Engine::GameInstance::IsGroupFinished(SoundID _groupID)
{
    return m_pSoundManager->IsGroupFinished(_groupID);
}

void Engine::GameInstance::SetVolume(SoundID _id, _float _volume)
{
    m_pSoundManager->SetVolume(_id, _volume);
}

void Engine::GameInstance::SetGroupVolume(SoundID _groupID, _float _volume)
{
    m_pSoundManager->SetGroupVolume(_groupID, _volume);
}
/******************************************************* SoundManager *******************************************************/



//////////////////////////////////////////////////////// EventManager ////////////////////////////////////////////////////////
void Engine::GameInstance::UnsubScribe(EventHandle _handlenum)
{
    m_pEventManager->UnsubScribe(_handlenum);
}
/******************************************************* EventManager *******************************************************/



//////////////////////////////////////////////////////// Picking ////////////////////////////////////////////////////////
_bool Engine::GameInstance::PickingObject(_float4* pOut)
{
    return m_pPicking->PickingObject(pOut);
}
/******************************************************* Picking *******************************************************/



//////////////////////////////////////////////////////// FileManager ////////////////////////////////////////////////////////
_bool Engine::GameInstance::SaveAsJson(const _string& _filePath, Serializable& _data, const TOOLTYPE _toolType, const _string& _version)
{
    return m_pFileManagaer->SaveAsJson(_filePath, _data, _toolType, _version);
}
/******************************************************* FileManager *******************************************************/



//////////////////////////////////////////////////////// ResouceManager ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Add_Shader(const _wstring& _ProtoName, const _wstring& _shaderPath, const D3D11_INPUT_ELEMENT_DESC* _elementsDesc, _uint _numElements, SHADERENTRIES _entries)
{
    return m_pResourceManager->Add_Shader(_ProtoName, _shaderPath, _elementsDesc, _numElements, _entries);
}

Shader* Engine::GameInstance::Get_Shader_Prototype(const _wstring& _shaderName)
{
    return m_pResourceManager->Get_Shader_Prototype(_shaderName);
}

Buffer* Engine::GameInstance::Register_Buffer(_uint _slot, Buffer* _pBuffer)
{
    return m_pResourceManager->Register_Buffer(_slot, _pBuffer);
}

Buffer* Engine::GameInstance::Find_Buffer(_uint _slot)
{
    return m_pResourceManager->Find_Buffer(_slot);
}

Buffer* Engine::GameInstance::Register_LocalBuffer(const _string& _cbName, _uint _slot, Buffer* _pBuffer)
{
    return m_pResourceManager->Register_LocalBuffer(_cbName, _slot, _pBuffer);
}

Buffer* Engine::GameInstance::Find_LocalBuffer(const _string& _cbName)
{
    return m_pResourceManager->Find_LocalBuffer(_cbName);
}

void Engine::GameInstance::Register_ShaderHandle(const _string& _varName, SHADERHANDLE* _pHandle)
{
    m_pResourceManager->Register_ShaderHandle(_varName, _pHandle);
}

void Engine::GameInstance::Update_Handles_From_Buffer(Buffer* _pBuffer, _uint _slot, _bool _bOverwrite)
{
    m_pResourceManager->Update_Handles_From_Buffer(_pBuffer, _slot, _bOverwrite);
}

void Engine::GameInstance::Add_Player(GameObject* _player)
{
    m_pResourceManager->Add_Player(_player);
}

GameObject* Engine::GameInstance::Get_Player()
{
    return m_pResourceManager->Get_Player();
}

void Engine::GameInstance::Release_Player()
{
    m_pResourceManager->Release_Player();
}

_vector Engine::GameInstance::Get_PlayerPos_Vector()
{
    return m_pResourceManager->Get_PlayerPos_Vector();
}

_float3 Engine::GameInstance::Get_PlayerPos_Float3()
{
    return m_pResourceManager->Get_PlayerPos_Float3();
}

_float4 Engine::GameInstance::Get_PlayerPos_Float4()
{
    return m_pResourceManager->Get_PlayerPos_Float4();
}

const _float4x4* Engine::GameInstance::Get_Player_Matrix_Ptr()
{
    return m_pResourceManager->Get_Player_Matrix_Ptr();
}

ID3D11ShaderResourceView* Engine::GameInstance::Load_Texture(const wstring& _filePath, _bool _bLinear)
{
    return m_pResourceManager->Load_Texture(_filePath, _bLinear);
}

void Engine::GameInstance::Release_Texture(const _wstring& _filePath)
{
    m_pResourceManager->Release_Texture(_filePath);
}

_bool Engine::GameInstance::Release_Texture_Safe(const _wstring& _filePath)
{
    return m_pResourceManager->Release_Texture_Safe(_filePath);
}

GameObject* Engine::GameInstance::Get_Companion()
{
    return m_pResourceManager->Get_Companion();;
}

void GameInstance::Add_Companion(GameObject* _companion)
{
    m_pResourceManager->Add_Companion(_companion);
}

void GameInstance::Release_Companion()
{
    m_pResourceManager->Release_Companion();
}

_vector GameInstance::Get_CompanionPos_Vector()
{
    return m_pResourceManager->Get_CompanionPos_Vector();
}

_float3 GameInstance::Get_CompanionPos_Float3()
{
    return m_pResourceManager->Get_CompanionPos_Float3();
}

_float4 GameInstance::Get_CompanionPos_Float4()
{
    return m_pResourceManager->Get_CompanionPos_Float4();
}
const _float4x4* Engine::GameInstance::Get_Companion_Matrix_Ptr()
{
    return m_pResourceManager->Get_Companion_Matrix_Ptr();
}
/******************************************************* ResouceManager *******************************************************/



//////////////////////////////////////////////////////// Frustum ////////////////////////////////////////////////////////
_bool Engine::GameInstance::IsIn_InWorldSpace(_fvector vWorldPos, _float fRange)
{
    return m_pFrustum->IsIn_InWorldSpace(vWorldPos, fRange);
}

_bool Engine::GameInstance::IsIn_InLocalSpace(_fvector vLocalPos, _float fRange)
{
    return m_pFrustum->IsIn_InLocalSpace(vLocalPos, fRange);
}

_bool Engine::GameInstance::IsIn_Frustum(_fvector _worldPos, _float _range)
{
    return m_pFrustum->IsIn_InWorldSpace(_worldPos, _range);
}

_bool Engine::GameInstance::IsIn_Frustum_AABB(const BoundingBox& _aabb)
{
    return m_pFrustum->IsIn_AABB_InWorldSpace(_aabb);
}

void Engine::GameInstance::Get_Frustum_Planes(_float4* _outPlanes) const
{
    m_pFrustum->Get_Planes(_outPlanes);
}
/******************************************************* Frustum *******************************************************/



//////////////////////////////////////////////////////// ObjectFactory ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Spawn_LevelData(const _string& _folderPath, _uint _level_for_create, _bool _useOnlyJson)
{
    return m_pObjectFactory->Spawn_LevelData(_folderPath, _level_for_create, _useOnlyJson);
}

HRESULT Engine::GameInstance::Spawn_LevelData(LevelLoadResult _result, _uint _level_for_create)
{
    return m_pObjectFactory->Spawn_LevelData(_result, _level_for_create);
}
/******************************************************* ObjectFactory *******************************************************/



//////////////////////////////////////////////////////// Instancing ////////////////////////////////////////////////////////
void Engine::GameInstance::Add_Instance(const wstring& strProtoTag, class Model* pModel, const _float4x4& WorldMatrix)
{
    if (nullptr == m_pRenderer)
        return;

    m_pRenderer->Add_InstanceBatch(strProtoTag, pModel, WorldMatrix);
}
/******************************************************* Instancing *******************************************************/



//////////////////////////////////////////////////////// PhysX ////////////////////////////////////////////////////////
void Engine::GameInstance::Update_PhysX(const _float fTimeDelta)
{
    m_pPhysXManager->Update(fTimeDelta);
}

HRESULT Engine::GameInstance::PhysX_Cooking_Static_Model(PHYSX_ACTOR_DESC& _actorDesc)
{
    return m_pPhysXManager->PhysX_Cooking_Static_Model(_actorDesc);
}

PxRigidStatic* Engine::GameInstance::PhysX_Load_Static_Actor(PHYSX_ACTOR_DESC& _actorDesc)
{
    return m_pPhysXManager->PhysX_Load_Static_Actor(_actorDesc);
}

PxRigidStatic* Engine::GameInstance::PhysX_Load_Static_Actor_ConvexMesh(PHYSX_ACTOR_DESC& _actorDesc)
{
    return m_pPhysXManager->PhysX_Load_Static_Actor_ConvexMesh(_actorDesc);
}

PxRigidStatic* Engine::GameInstance::PhysX_Load_Static_Actor_Auto(PHYSX_ACTOR_DESC& _actorDesc)
{
    return m_pPhysXManager->PhysX_Load_Static_Actor_Auto(_actorDesc);
}

PxController* Engine::GameInstance::PhysX_Create_Controller(const PHYSX_CONTROLLER_DESC& _controllerDesc)
{
    return m_pPhysXManager->PhysX_Create_Controller(_controllerDesc);
}

PHYSX_RAYCAST_RESULT Engine::GameInstance::PhysX_Raycast(const PxVec3& _origin, const PxVec3& _direction, _float _maxDistance)
{
    return m_pPhysXManager->PhysX_Raycast(_origin, _direction, _maxDistance);
}

PHYSX_SWEEP_RESULT Engine::GameInstance::PhysX_Sweep_Capsule(_float _radius, _float _halfHeight, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance)
{
    return m_pPhysXManager->PhysX_Sweep_Capsule(_radius, _halfHeight, _pose, _direction, _maxDistance);
}

PHYSX_MULTI_SWEEP_RESULT Engine::GameInstance::PhysX_Sweep_Capsule_Multi(_float _radius, _float _halfHeight, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance)
{
    return m_pPhysXManager->PhysX_Sweep_Capsule_Multi(_radius, _halfHeight, _pose, _direction, _maxDistance);
}

PHYSX_MULTI_SWEEP_RESULT Engine::GameInstance::PhysX_Sweep_Capsule_Multi_ExceptStatic(_float _radius, _float _halfHeight, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance)
{
    return m_pPhysXManager->PhysX_Sweep_Capsule_Multi_ExceptStatic(_radius, _halfHeight, _pose, _direction, _maxDistance);
}

PHYSX_SWEEP_RESULT Engine::GameInstance::PhysX_Sweep_Sphere(_float _radius, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance)
{
    return m_pPhysXManager->PhysX_Sweep_Sphere(_radius, _pose, _direction, _maxDistance);
}

PHYSX_OVERLAP_RESULT Engine::GameInstance::PhysX_Overlap_Capsule(_float _radius, _float _halfHeight, const PxTransform& _pose)
{
    return m_pPhysXManager->PhysX_Overlap_Capsule(_radius, _halfHeight, _pose);
}

PHYSX_MULTI_SWEEP_RESULT Engine::GameInstance::PhysX_Sweep_Sphere_Multi(_float _radius, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance)
{
    return m_pPhysXManager->PhysX_Sweep_Sphere_Multi(_radius, _pose, _direction, _maxDistance);
}

PHYSX_OVERLAP_RESULT Engine::GameInstance::PhysX_Overlap_Sphere(_float _radius, const PxTransform& _pose)
{
    return m_pPhysXManager->PhysX_Overlap_Sphere(_radius, _pose);
}

PxPhysics* GameInstance::Get_Physics() const
{
    return m_pPhysXManager->Get_Physics();
}

PxScene* GameInstance::Get_Scene() const
{
    return m_pPhysXManager->Get_Scene();
}

PxControllerManager* GameInstance::Get_ControllerManager() const
{
    return m_pPhysXManager->Get_ControllerManager();
}

void Engine::GameInstance::PhysX_Remove_Actor(PxRigidActor* _pActor)
{
    m_pPhysXManager->PhysX_Remove_Actor(_pActor);
}

void Engine::GameInstance::PhysX_Remove_Controller(PxController* _pController)
{
    m_pPhysXManager->PhysX_Remove_Controller(_pController);
}

void Engine::GameInstance::PhysX_Disable_Controller(PxController* _pController)
{
    m_pPhysXManager->Disable_Controller(_pController);
}

void Engine::GameInstance::PhysX_Enable_Controller(PxController* _pController, const PxExtendedVec3& _spawnPos, PX_ACTOR_TYPE _eType)
{
    m_pPhysXManager->Enable_Controller(_pController, _spawnPos, _eType);
}

HRESULT Engine::GameInstance::PhysX_Create_BoneChain(const PHYSX_BONE_CHAIN_DESC& _desc, Model* _pMasterModel, const _float4x4& _objectWorldMatrix)
{
    return m_pPhysXManager->PhysX_Create_BoneChain(_desc, _pMasterModel, _objectWorldMatrix);
}

void Engine::GameInstance::PhysX_Update_BoneChain_Kinematic(_uint _iChainID, const _float4x4& _rootCombinedMatrix, const _float4x4& _objectWorldMatrix)
{
    m_pPhysXManager->PhysX_Update_BoneChain_Kinematic(_iChainID, _rootCombinedMatrix, _objectWorldMatrix);
}

void Engine::GameInstance::PhysX_Fetch_BoneChain_Results(_uint _iChainID, const _float4x4& _objectWorldMatrix, vector<pair<_int, _float4x4>>& _outBoneMatrices)
{
    m_pPhysXManager->PhysX_Fetch_BoneChain_Results(_iChainID, _objectWorldMatrix, _outBoneMatrices);
}

void Engine::GameInstance::PhysX_Remove_BoneChain(_uint _iChainID)
{
    m_pPhysXManager->PhysX_Remove_BoneChain(_iChainID);
}

void Engine::GameInstance::PhysX_Remove_All_BoneChains()
{
    m_pPhysXManager->PhysX_Remove_All_BoneChains();
}


#ifdef _DEBUG
void Engine::GameInstance::Enable_DebugVisualization()
{
    m_pPhysXManager->Enable_DebugVisualization();
}

void Engine::GameInstance::Disable_DebugVisualization()
{
    m_pPhysXManager->Disable_DebugVisualization();
}

void Engine::GameInstance::Toggle_DebugVisualization()
{
    m_pPhysXManager->Toggle_DebugVisualization();
}

_bool Engine::GameInstance::Is_DebugVisualization()
{
    return m_pPhysXManager->Is_DebugVisualization();
}

const PxRenderBuffer* Engine::GameInstance::Get_PhysXRenderBuffer() const
{
    return m_pPhysXManager->Get_PhysXRenderBuffer();
}

void Engine::GameInstance::Set_VisualizationParam(PxVisualizationParameter::Enum _eParam, _bool _bEnable)
{
    m_pPhysXManager->Set_VisualizationParam(_eParam, _bEnable);
}

_bool Engine::GameInstance::Get_VisualizationParam(PxVisualizationParameter::Enum _eParam) const
{
    return m_pPhysXManager->Get_VisualizationParam(_eParam);
}
#endif // _DEBUG
/******************************************************* PhysX *******************************************************/




//////////////////////////////////////////////////////// EffectManager ////////////////////////////////////////////////////////
vector<_string> const Engine::GameInstance::Get_EffectNames()
{
    return m_pEffectManager->Get_EffectNames();
}

HRESULT Engine::GameInstance::Add_Effect(const _wstring& wstrEffectName, ParticleSystem* pParticleSystem)
{
    return m_pEffectManager->Add_Effect(wstrEffectName, pParticleSystem);
}

ParticleSystem* Engine::GameInstance::Find_Effect(const _wstring& wstrEffectName)
{
    return m_pEffectManager->Find_Effect(wstrEffectName);
}

void Engine::GameInstance::Play_Effect_Position(const _wstring& wstrEffectName, const _float3& vPosition)
{
    return m_pEffectManager->Play_Effect_Position(wstrEffectName, vPosition);
}

void Engine::GameInstance::Play_Effect_Matrix(const _wstring& wstrEffectName, const _float4x4& matWorld)
{
    return m_pEffectManager->Play_Effect_Matrix(wstrEffectName, matWorld);
}

void Engine::GameInstance::Stop_Effect(const _wstring& wstrEffectName)
{
    m_pEffectManager->Stop_Effect(wstrEffectName);
}
/******************************************************* EffectManager *******************************************************/




//////////////////////////////////////////////////////// DecalManager ////////////////////////////////////////////////////////
HRESULT Engine::GameInstance::Initialize_Pool(_uint iPrototypeLevelID, const _wstring wstrPrototypeTag, _uint iLevelID, _uint iPoolSize)
{
    return m_pDecalManager->Initialize_Pool(iPrototypeLevelID, wstrPrototypeTag, iLevelID, iPoolSize);
}

void Engine::GameInstance::Spawn_Decal(const _float3& vPosition, const _float3& vNormal, _float fScale, _float fLifeTime)
{
    return m_pDecalManager->Spawn_Decal(vPosition, vNormal, fScale, fLifeTime);
}
/******************************************************* DecalManager *******************************************************/




//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::GameInstance::Register_Collider(COLLISION_GROUP _groupTag, Collider* _pCollider)
{
    m_pColliderManager->Register(_groupTag, _pCollider);
}

void Engine::GameInstance::Unregister_Collider(COLLISION_GROUP _groupTag, Collider* _pCollider)
{
    m_pColliderManager->Unregister(_groupTag, _pCollider);
}

void Engine::GameInstance::Add_CollisionPair(COLLISION_GROUP _srcGroup, COLLISION_GROUP _dstGroup)
{
    m_pColliderManager->Add_CollisionPair(_srcGroup, _dstGroup);
}

void Engine::GameInstance::Remove_CollisionPair(COLLISION_GROUP _srcGroup, COLLISION_GROUP _dstGroup)
{
    m_pColliderManager->Remove_CollisionPair(_srcGroup, _dstGroup);
}

void Engine::GameInstance::Clear_ColliderManager()
{
    m_pColliderManager->Clear();
}
/******************************************************* EffectManager *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
_uint Engine::GameInstance::Release_Engine()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& manager : m_vecCleanupTargets)
	{
		Safe_Release(manager);
	}
	m_vecCleanupTargets.clear();

    Mouse::DestroyInstance();

	DestroyInstance();

    return m_pInstance ? 1 : 0;
}

void Engine::GameInstance::Free()
{
	__super::Free();

	Safe_Release(m_pGraphic_Device);
}
/******************************************************* 객체 반환 함수 *******************************************************/
