#include "Engine_Define.h"
#include "Mouse.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "Model.h"
#include "Mesh.h"

IMPLEMENT_SINGLETON(Mouse);

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Mouse::Mouse()
{
}

Engine::Mouse::~Mouse()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/




//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Mouse::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (!m_pDevice)
    {
        m_pDevice = pDevice;
        m_pContext = pContext;
        m_pGameInstance = GameInstance::GetInstance();

        Safe_AddRef(m_pDevice);
        Safe_AddRef(m_pContext);

        Safe_AddRef(m_pGameInstance);

        // 마우스 잠금 이제 마우스 클래스에서
        EventHandle handle = m_pGameInstance->Subscribe<MouseLockEvent>([this](const MouseLockEvent& _event)
            {
                m_bMouseLock = _event.bLock;
            });
        m_vecEvent.push_back(handle);
    }



    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Mouse::Update_Priority(const _float fTimeDelta)
{
    m_bMouseClickedInWindow = false;
    m_bMouseDraggingInWindow = false;

    GetCursorPos(&m_MousePos);
    ScreenToClient(eg_hWnd, &m_MousePos);

    RECT Engine{};
    GetClientRect(eg_hWnd, &Engine);
    if (PtInRect(&Engine, m_MousePos))
    {
        if(m_pGameInstance->MouseDown(MOUSEKEYSTATE::LB))
            m_bMouseClickedInWindow = true;

        if (m_pGameInstance->MousePress(MOUSEKEYSTATE::LB))
            m_bMouseDraggingInWindow = true;

    }

    if (m_bMouseLock)
        Lock_Mouse();

    m_pGameInstance->Process_UI_Picking(m_MousePos, m_bMouseClickedInWindow);

    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Mouse::Update(const _float fTimeDelta)
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(eg_hWnd, &pt);

    m_MouseDelta.x = pt.x - m_MousePos.x;
    m_MouseDelta.y = pt.y - m_MousePos.y;


    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Mouse::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Mouse::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Mouse::Bind_ShaderResources()
{
    return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 마우스 피킹 함수 ////////////////////////////////////////////////////////
void Engine::Mouse::Picking_Object_All()
{
    //_float distance{}, outdistance{};
    //_uint faceIndex{};
    //_vector outworldvector{}, outworldvector2;
    //GameObject* gameobject{};
    //unordered_map<_wstring, Layer*>& layers = m_pGameInstance->Get_Current_Layers();
    //for (auto& layer : layers)
    //{
    //    if (layer.second->Get_Name() == L"Camera_Layer") continue;
    //    unordered_map<_wstring, GameObject*>& objects = layer.second->Get_GameObjects();
    //    for (auto& object : objects)
    //    {
    //        VIBuffer* buffer{};
    //        if (layer.second->Get_Name() == L"Enviroment_Layer")
    //            buffer = CAST(VIBuffer*)(object.second->Get_Component_FromName(Com_VIBuffer));
    //        //else
    //        //    buffer = CAST(VIBuffer*)(object.second->Get_Component_FromName(Com_Model));

    //        if (!buffer) continue;

    //        _matrix worldmatrix = object.second->Get_WorldMatrix();
    //        MyRay localray = m_pGameInstance->Get_LocalRay(worldmatrix);
    //        m_pGameInstance->IntersectBuffer(localray, buffer, worldmatrix, outworldvector, outdistance);
    //        if (distance < outdistance)
    //        {
    //            distance = outdistance;
    //            gameobject = object.second;
    //            outworldvector2 = outworldvector;
    //        }
    //    }
    //}

    //if (gameobject)
    //{
    //    _float3 worldpos{};
    //    XMStoreFloat3(&worldpos, outworldvector2);
    //    WCOUT(gameobject->Get_Name());
    //    _matrix world = gameobject->Get_WorldMatrix();
    //    COUT("x : " << worldpos.x << ",\ty : " << worldpos.y << ",\tz : " << worldpos.z);
    //    COUT("");
    //}

}

_bool Engine::Mouse::Picking_Object_Terrain(PICKING_DESC& _desc)
{
    _float distance{}, outdistance{};
    _uint faceIndex{};
    _vector outworldvector{}, outworldvector2;
    GameObject* gameobject{};
    unordered_map<_wstring, Layer*>& layers = m_pGameInstance->Get_Current_Layers();
    for (auto& layer : layers)
    {
        if (layer.second->Get_Name() != L"Layer_Enviroment") continue;
        unordered_map<_wstring, GameObject*>& objects = layer.second->Get_GameObjects();
        for (auto& object : objects)
        {
            VIBuffer* buffer = CAST(VIBuffer*)(object.second->Get_Component_FromName(Com_VIBuffer));
            if (!buffer) continue;

            _matrix worldmatrix = object.second->Get_WorldMatrix();
            MyRay localray = m_pGameInstance->Get_LocalRay(worldmatrix);
            //m_pGameInstance->IntersectBuffer(localray, buffer, worldmatrix, outworldvector, outdistance);
            if (distance < outdistance)
            {
                distance = outdistance;
                gameobject = object.second;
                outworldvector2 = outworldvector;
            }
        }
    }

    if (gameobject)
    {
        _float3 worldpos{};
        XMStoreFloat3(&worldpos, outworldvector2);
        _desc.fDistance = distance;
        _desc.iMeshIndex = faceIndex;
        _desc.pGameObject = gameobject;
        _desc.vWorldPos = worldpos;
        //WCOUT(gameobject->Get_Name());
        //_matrix world = gameobject->Get_WorldMatrix();
        //COUT("x : " << worldpos.x << ",\ty : " << worldpos.y << ",\tz : " << worldpos.z);
        //COUT("distance : " << distance);
        //COUT("");
        return true;
    }

    return false;
}

_bool Engine::Mouse::Picking_Object_GameObj(PICKING_DESC& _desc)
{
    _float distance{}, outdistance{};
    _uint faceIndex{};
    _vector outworldvector{}, outworldvector2;
    GameObject* gameobject{};
    unordered_map<_wstring, Layer*>& layers = m_pGameInstance->Get_Current_Layers();
    for (auto& layer : layers)
    {   //오타수정
        if (layer.second->Get_Name() == L"Layer_Camera") continue;
        if (layer.second->Get_Name() == L"Layer_Enviroment") continue;
        unordered_map<_wstring, GameObject*>& objects = layer.second->Get_GameObjects();
        for (auto& object : objects)
        {
            Model* model = CAST(Model*)(object.second->Get_Component_FromName(Com_Model));
            if (!model) continue;

            vector<Mesh*> Meshes = model->Get_Meshes();
            _int numMeshes = model->Get_NumMeshes();

            _matrix worldmatrix = object.second->Get_WorldMatrix();
            MyRay localray = m_pGameInstance->Get_LocalRay(worldmatrix);
            _bool isupdated = { false };
            for (_int i = 0; i < numMeshes; ++i)
            {
                VIBuffer* buffer = CAST(VIBuffer*)(Meshes[i]);
                if (!buffer) continue;
                if (!isupdated && buffer->IsAnimation()) model->Update_SkinnedMeshes();

                //m_pGameInstance->IntersectBuffer(localray, buffer, worldmatrix, outworldvector, outdistance);
                if (distance < outdistance)
                {
                    distance = outdistance;
                    gameobject = object.second;
                    outworldvector2 = outworldvector;
                }
            }
        }
    }

    if (gameobject)
    {
        _float3 worldpos{};
        XMStoreFloat3(&worldpos, outworldvector2);
        _desc.fDistance = distance;
        _desc.iMeshIndex = faceIndex;
        _desc.pGameObject = gameobject;
        _desc.vWorldPos = worldpos;
        //WCOUT(gameobject->Get_Name());
        //_matrix world = gameobject->Get_WorldMatrix();
        //COUT("x : " << worldpos.x << ",\ty : " << worldpos.y << ",\tz : " << worldpos.z);
        //COUT("distance : " << distance);
        //COUT("");
        return true;
    }


    return false;
}

_bool Engine::Mouse::Picking_Object_GameObject_Pixel(GameObject*& pGameObject)
{

    _float4 fOut = {};
    unordered_map<_wstring, Layer*>& layers = m_pGameInstance->Get_Current_Layers();

    m_pGameInstance->PickingObject(&fOut);
    for (auto& layer : layers)
    {
        if (layer.second->Get_Name() == L"Layer_Camera") continue;
        //if (layer.second->Get_Name() == L"Layer_Enviroment") continue;
        unordered_map<_wstring, GameObject*>& objects = layer.second->Get_GameObjects();
        for (auto& object : objects)
        {
            if (fOut.w == object.second->Get_ObjectID())
            {
                pGameObject = object.second;
                return true;
            }
        }
    }

    return false;
}
void Engine::Mouse::Get_MouseRay(_vector& outRaypos, _vector& outRayDir)
{
    POINT ptMouse;
    GetCursorPos(&ptMouse);
    ScreenToClient(eg_hWnd, &ptMouse);

    D3D11_VIEWPORT Viewport;
    _uint iNumViewports = 1;
    m_pContext->RSGetViewports(&iNumViewports, &Viewport);


    _float3 vMousepos;
    vMousepos.x = (ptMouse.x / (Viewport.Width * 0.5f)) - 1.f;
    vMousepos.y = (ptMouse.y / (Viewport.Height * -0.5f)) + 1.f; // Height로 수정!!
    vMousepos.z = 0.f; // Near Plane 지점

    _float4x4 fInverseProj = m_pGameInstance->Get_PipeLineInversMatrix(D3DTS_PROJ);
    _float4x4 fInverseView = m_pGameInstance->Get_PipeLineInversMatrix(D3DTS_VIEW);


    _matrix InProj = XMLoadFloat4x4(&fInverseProj);
    _matrix InView = XMLoadFloat4x4(&fInverseView);


    outRaypos = InView.r[3];


    _vector vTargetPos = XMVector3TransformCoord(XMLoadFloat3(&vMousepos), InProj);
    vTargetPos = XMVector3TransformCoord(vTargetPos, InView);

    outRayDir = XMVector3Normalize(vTargetPos - outRaypos);
}
/******************************************************* 마우스 피킹 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Mouse::Lock_Mouse()
{
    POINT	ptMouse{ LONG(Engine_WINCX * 0.5f), LONG(Engine_WINCY * 0.5f) };

    ClientToScreen(eg_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);
}
/******************************************************* 마우스 피킹 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Mouse::Free()
{
    __super::Free();

    for (auto& handle : m_vecEvent)
    {
        m_pGameInstance->UnsubScribe(handle);
    }

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/
