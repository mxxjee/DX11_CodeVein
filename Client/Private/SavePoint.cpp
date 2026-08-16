#include "Client_Define.h"
#include "SavePoint.h"
#include "GameInstance.h"

#include "InteractionManager.h"
#include "MinimapRenderComponent.h"
#include "Player_Stat.h"
#include "PoolingManager.h"
#include "ParticleSystem.h"
#include "SkillManager.h"

#include "InventoryManager.h"
#include "ItemManager.h"



SavePoint::SavePoint(ID3D11Device* pD, ID3D11DeviceContext* pC)
    : MapObject(pD, pC)
{
}

SavePoint::SavePoint(const SavePoint& original)
    : MapObject(original)
{
}

HRESULT SavePoint::Initialize_Prototype(LEVEL _level)
{
    m_iLevel = _UINT(_level);

    return S_OK;
}

HRESULT SavePoint::Initialize(void* arg)
{
    if (FAILED(__super::Initialize(arg)))
        return E_FAIL;

    if (nullptr != arg)
    {
        MAPOBJECT_DESC* pDesc = (MAPOBJECT_DESC*)arg;
        if (pDesc->jExtraData.contains("MyMapType"))
        {
            m_eSPMapType = (MAP_TYPE)pDesc->jExtraData["MyMapType"].get<int>();
            m_iSPIndex = pDesc->jExtraData["MyIndex"].get<int>();
        }
    }
    else
    {
        m_eSPMapType = MAP_TYPE::ST00_BASE;
        m_iSPIndex = 0;
    }

    if (FAILED(Ready_Components()))
        return E_FAIL;

    Vector4 vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

    static _uint namenum = 0;
    m_wstrName = L"SavePoint_" + to_wstring(namenum++);

    m_strSoundGroupName = string(m_wstrName.begin(), m_wstrName.end());

    m_pTransformCom->Set_Scale(1.2f, 1.2f, 1.2f);
    m_eObjType = OBJTYPE::TYPE_MAP;

    m_SavePointInfo.iIndex = m_iSPIndex;
    m_SavePointInfo.mapType = m_eSPMapType;
    m_SavePointInfo.spawnPosition = _float4(vPos.x, vPos.y, vPos.z, 1.f);
    m_SavePointInfo.wsName = m_wstrName;

    Set_Value();

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
    {
        m_vecObjPass.push_back(0);
    }

    Set_AllPass_VecObjPass(5);

    m_eInteractionType = INTERACTION_TYPE::SAVEPOINT;
   InteractionManager::GetInstance()->Register_InteractableObject(this);
    
   LIGHT_DESC loadDesc;

   loadDesc.eType = LIGHT::POINT;

   loadDesc.vDiffuse = { 3.f, 3.f, 6.19f, 1.0f };
   loadDesc.vAmbient = { 0.f, 0.f, 0.f, 0.f };
   loadDesc.vSpecular = { 0.f, 0.f, 0.f, 0.f };
   loadDesc.vDirection = { 0.f, 0.f, 0.f, 0.f };

   loadDesc.vPosition = { vPos.x, vPos.y + 1.5f, vPos.z, 1.0f };

   loadDesc.fRange = 4.0f;

   _uint iNewLightIndex = m_pGameInstance->Get_LightCnt();

   if (SUCCEEDED(Add_Component(_UINT(LEVEL::STATIC), L"Proto_Component_Point_Light",
       TEXT("Com_Light" + to_wstring(iNewLightIndex + 1)),
       reinterpret_cast<Component**>(&m_pLightComponent),
       &loadDesc)))
   {
       _vector vLoadedPos = XMLoadFloat4(&loadDesc.vPosition);
       m_pLightComponent->Update(0, vLoadedPos);
   }

   _float3 fPos;
   XMStoreFloat3(&fPos, Get_Position());
    
   m_pEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_MISTLE));
   if (m_pEffect == nullptr)
       return S_OK;

   m_pEffect->Set_WorldPosition(fPos);
   m_pEffect->OnSpawn(nullptr);

   m_pInteractionManager = InteractionManager::GetInstance();
   m_pInventoryManager = InventoryManager::GetInstance();


    return S_OK;
}

_int SavePoint::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

HRESULT SavePoint::Ready_Components()
{
    CHECK_FAILED(Add_Component((_uint)LEVEL::STATIC, L"Prototype_Component_Shader_VTXAnimMesh", Com_Shader, RCAST(Component**)(&m_pShaderCom)), E_FAIL);
    CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_Model_SavePoint", Com_Model, RCAST(Component**)(&m_pModelCom)), E_FAIL);

    m_pModelCom->Set_Animation_CS(1);

    if (m_pModelCom != nullptr)
    {
        m_vecObjPass.resize(m_pModelCom->Get_NumMeshes(), 0);
    }

    m_pModelCom->LocalAABB_To_WorldAABB(m_pTransformCom->Get_WorldMatrix());

    MinimapRenderComponent::MINIMAPDESC MinimapDesc;
    MinimapDesc.pOwner = this;


    MinimapDesc.eType = MinimapRenderComponent::ICON_TYPE::SAVEPOINT;
    MinimapDesc.m_bIsTrace = false;	//발자국남김

    CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Com_Minimap, Com_Minimap, RCAST(Component**)(&m_pMinimapRenderCom), &MinimapDesc), E_FAIL);

    return S_OK;
}

_int SavePoint::Update_Parallel(const _float fTimeDelta)
{
    __super::Update_Parallel(fTimeDelta);

    return 0;
}

_int SavePoint::Update(const _float fDT)
{
    m_pModelCom->Play_Animation_CS(fDT);


    GameObject* pPlayer = m_pGameInstance->Get_Player();
    if (pPlayer)
    {
        Vector4 vPlayerPos = pPlayer->Get_Transform()->Get_State(DIRECTION::POSITION);
        Vector4 vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

        _float fDistance = Vector3::Distance(
            Vector3(vPlayerPos.x, vPlayerPos.y, vPlayerPos.z),
            Vector3(vMyPos.x, vMyPos.y, vMyPos.z)
        );

        if (fDistance < m_fMaxSoundDistance)
        {
            if (!m_bSoundStarted)
            {
                m_pGameInstance->Play_Sound_InGroup("SavePoint_Idle_Roop", m_strSoundGroupName, m_fMaxVolume, true);
                m_bSoundStarted = true;
            }

            _float fDistanceRatio = fDistance / m_fMaxSoundDistance;
            _float fRatio = 1.0f - (fDistanceRatio * fDistanceRatio);
            _float fTargetVolume = m_fMaxVolume * fRatio;

            if (fTargetVolume < 0.01f) fTargetVolume = 0.01f;

            m_pGameInstance->SetGroupVolume(m_strSoundGroupName, fTargetVolume);
        }
        else
        {
            if (m_bSoundStarted)
            {
                m_pGameInstance->StopGroup(m_strSoundGroupName);
                m_bSoundStarted = false;
            }
        }
    }

    return __super::Update(fDT);
}

_int SavePoint::Update_Late(const _float fDT)
{
    Add_RenderGroup(RENDER_GROUP::NONBLEND);
    Add_RenderGroup(RENDER_GROUP::SHADOW);
    __super::Update_Late(fDT);

    m_pMinimapRenderCom->Update_Late(fDT);

    return 0;
}

HRESULT SavePoint::Render(const _float fDT)
{
    CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        _uint bitflag = 0;
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 1, i, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &bitflag), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_SHININESS, 0, &bitflag), E_FAIL);
        //AO나중에 처리
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ShadCTexture", i, aiTextureType_EMISSIVE), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);
        //CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);
        m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));

        CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);
        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);
        CHECK_FAILED(m_pModelCom->Render(fDT, i), E_FAIL);
    }

    return S_OK;
}

HRESULT SavePoint::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
    CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Begin(4, true), E_FAIL);

    for (size_t i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);

        CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);

        CHECK_FAILED(m_pShaderCom->Bind_Resources(4), E_FAIL);

        CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
    }
    return S_OK;
}

HRESULT SavePoint::Player_Teleport(const _float fDT)
{
    //if (m_SavePointInfo.targetLevel == LEVEL::END)
    //{
    //    MSG_BOX("목표 레벨이 설정되지 않았습니다!");
    //    return E_FAIL;
    //}

    //GameObject* pPlayer = m_pGameInstance->Get_Player();
    //if (nullptr == pPlayer)
    //{
    //    MSG_BOX("플레이어를 찾을 수 없습니다!");
    //    return E_FAIL;
    //}

    //m_pGameInstance->Add_Level(_UINT(LEVEL::LOADING),
    //    Level_Load::Create(m_pDevice, m_pContext, m_SavePointInfo.targetLevel)
    //);

    //m_pGameInstance->Change_Level(_UINT(LEVEL::LOADING));

    //pPlayer->Set_State(DIRECTION::POSITION, m_SavePointInfo.spawnPosition);

    return S_OK;
}

ordered_json SavePoint::Get_ExtraData()
{
    ordered_json j;
    j["MyMapType"] = (_int)m_eSPMapType;
    j["MyIndex"] = m_iSPIndex;
    return j;
}

void SavePoint::Set_Value()
{
    //if (m_eSPMapType == MAP_TYPE::ST00_BASE)
    //{
    //    m_SavePointInfo.targetLevel = LEVEL::BASE;

    //    switch (m_iSPIndex)
    //    {
    //    case 0:
    //        m_SavePointInfo.spawnPosition = _float4(10.0f, 2.0f, 15.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"활동 거점";
    //        break;
    //    }
    //}

    //else if (m_eSPMapType == MAP_TYPE::ST01_UNDER)
    //{
    //    m_SavePointInfo.targetLevel = LEVEL::MAIN;

    //    switch (m_iSPIndex)
    //    {
    //    case 0:
    //        m_SavePointInfo.spawnPosition = _float4(15.0f, 3.0f, 10.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"자연 동굴";
    //        break;
    //    case 1:
    //        m_SavePointInfo.spawnPosition = _float4(35.0f, 3.0f, 25.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"자연 동굴 심부";
    //        break;
    //    case 2:
    //        m_SavePointInfo.spawnPosition = _float4(55.0f, 5.0f, 15.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"실외의 십자로";
    //        break;
    //    }
    //}

    //else if (m_eSPMapType == MAP_TYPE::ST02_CHURCH)
    //{
    //    m_SavePointInfo.targetLevel = LEVEL::CHURCH;

    //    switch (m_iSPIndex)
    //    {
    //    case 0:
    //        m_SavePointInfo.spawnPosition = _float4(20.0f, 1.0f, 30.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"C맵_입구";
    //        break;
    //    case 1:
    //        m_SavePointInfo.spawnPosition = _float4(40.0f, 3.0f, 20.0f, 1.0f);
    //        m_SavePointInfo.wsName = L"C맵_중간";
    //        break;
    //    }
    //}

    //m_SavePointInfo.spawnRotation = _float4(0.0f, 0.0f, 0.0f, 1.0f);
}

SavePoint* SavePoint::Create(ID3D11Device* pD, ID3D11DeviceContext* pC, LEVEL _level)
{
    SavePoint* pInstance = new SavePoint(pD, pC);
    if (FAILED(pInstance->Initialize_Prototype(_level)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

GameObject* SavePoint::Clone(void* arg)
{
    SavePoint* pInstance = new SavePoint(*this);
    if (FAILED(pInstance->Initialize(arg)))
    {
        Safe_Release(pInstance);
        return nullptr;
    }
    return pInstance;
}

#pragma region IInteractable
bool SavePoint::IsInteractable()
{
    /*씬이동중에는 ui안뜨게 그냥 인터렉션조차!!!*/
    CHECK_TRUE_RESULT(InteractionManager::GetInstance()->Is_SceneChanging(),false);


    GameObject* pPlayer = m_pGameInstance->Get_Player();
    if (pPlayer)
    {
        _vector vPlayerPos = pPlayer->Get_Position();
        _vector vItemPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

        _float fPlayerY = XMVectorGetY(vPlayerPos);
        _float fItemY = XMVectorGetY(vItemPos);
        _float fDistY = fabs(fPlayerY - fItemY);

        _float fMaxYOffSet = 1.5f;

        if (fDistY <= fMaxYOffSet)
        {
            return true;
        }
    }

    return false;
}

void SavePoint::Enter_InteractionRange(GameObject* pPlayer)
{
    //UI띄우깅
    //WCOUT(L"현재 " << m_wstrName << L" 의 interaction 범위에 들어왔습니다. Enter_InteractionRange ");
    UI_MasterEvent Event;
    Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
    Event.m_ActionName = "ActiveUI";
    Event.m_Text = L"PopUp_SavePoint";
    Event.m_bPersistent = true;
    Event.m_bFlag = true;
    m_pGameInstance->Publish(Event);


    //COUT("Index: " << m_SavePointInfo.iIndex
    //    << " Pos: " << m_SavePointInfo.spawnPosition.x
    //    << ", " << m_SavePointInfo.spawnPosition.y
    //    << ", " << m_SavePointInfo.spawnPosition.z);


}

void SavePoint::Stay_InteractionRange(GameObject* pPlayer,const _float& fTimeDelta)
{
  //  
  //  WCOUT(L"현재 " << m_wstrName << L" 의 interaction 중입니다.Stay_Interaction ");
  //CHECK_TRUE(m_bPreInteraction);

  ////들어와있느데 창이 비활성화되어있다면 켜주는명령
  //UI_MasterEvent Event;
  //Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
  //Event.m_ActionName = "ActiveUI";e
  //Event.m_Text = L"PopUp_NPC";
  //Event.m_bPersistent = true;
  //Event.m_bFlag = true;
  //m_pGameInstance->Publish(Event);

}

void SavePoint::Exit_InteractionRange(GameObject* pPlayer)
{
    //UI없애깅
    //WCOUT(L"현재 " << m_wstrName << L" 의 interaction 범위에서 나갔습니다.Exit_InteractionRange ");

    //Interaction안한상태로 exit할때만..
   
   UI_MasterEvent Event;
   Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
   Event.m_ActionName = "ActiveUI";
   Event.m_Text = L"PopUp_SavePoint";
   Event.m_bPersistent = true;
   Event.m_bFlag = false;
   m_pGameInstance->Publish(Event);
   
   m_bPrevRange = false;

}

void SavePoint::Enter_Interaction(GameObject* pPlayer)
{
   //HP랑 스태미나랑 명혈 이렇게3개 
    if (m_pPlayerStat == nullptr)
    {
        Stat* pStat = CAST(Stat*)(pPlayer->Get_Component_FromName(Com_Stat));
        m_pPlayerStat = DCAST(Player_Stat*)(pStat);
        if (!m_pPlayerStat) return;
    }


    m_pPlayerStat->Heal_Hp(m_pPlayerStat->Get_MaxHp());
    m_pPlayerStat->FullRestore_Stamina();
    m_pPlayerStat->Apply_Myeonghyeol(m_pPlayerStat->Get_MaxMyeonghyeol());
    
    SkillManager::GetInstance()->Debug_SkillReset();


    if (m_pGameInstance->Get_Current_LevelID() != static_cast<_uint>(LEVEL::BASE))
    {
        YAKUMO_ACTIVE_EVENT YakumoDesc; //야쿠모 활성화 이벤트
        YakumoDesc.bSuccess = true;
        m_pGameInstance->Publish(YakumoDesc);
    }

    m_pInventoryManager->Reset_HillItem();

    m_pInteractionManager->Set_LastSavePointInfo(m_pInteractionManager->Get_Level_By_MapType(m_SavePointInfo.mapType), m_SavePointInfo.iIndex);
}

void SavePoint::Stay_Interaction(GameObject* pPlayer,const _float& fTimeDelta)
{


}

void SavePoint::Exit_Interaction(GameObject* pPlayer)
{
    //머ㅜ......나중에....플레이어상태도려녹ㅎ기..??머..
    //WCOUT(L"현재 " << m_wstrName << L" 의 Exit_Interaction ");

    m_pGameInstance->StopGroup(m_strSoundGroupName);
    //메뉴끄면 이거 호출하자.
    m_bPreInteraction = false;


    Exit_InteractionRange(pPlayer);
    InteractionManager::GetInstance()->Reset_BestPriority();
}


#pragma endregion
void SavePoint::Free()
{
    if(m_bIsClone)
        InteractionManager::GetInstance()->UnRegisterInteractable(this);

    if (m_pEffect != nullptr)
    {
        PoolingManager::Get_Instance()->Despawn_Object(m_pEffect);
        m_pEffect = nullptr;
    }

    __super::Free();
}

