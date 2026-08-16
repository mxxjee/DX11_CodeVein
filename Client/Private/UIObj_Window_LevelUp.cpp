#include "Client_Define.h"
#include "UIObj_Window_LevelUp.h"
#include "UIObj_Text.h"
#include "InventoryManager.h"
#include "Player.h"
#include "Player_Stat.h"
#include "UISoundUtil.h"



Client::UIObj_Window_LevelUp::UIObj_Window_LevelUp()
{
}

Client::UIObj_Window_LevelUp::UIObj_Window_LevelUp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_Window_LevelUp::UIObj_Window_LevelUp(const UIObj_Window_LevelUp& original)
    :UIObject(original)
{
}

Client::UIObj_Window_LevelUp::~UIObj_Window_LevelUp()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Window_LevelUp::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Window_LevelUp::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<Window_LevelUpUIEvent>([this](const Window_LevelUpUIEvent& e)
        {
            m_bFocus = e.m_bFocus;

        }));

    vector<UIObj_Text*>     m_LevelTexts;
    m_LevelTexts.reserve(1);
    TextObjs[UIPARENT::LEVELINFO] = m_LevelTexts;

    vector<UIObj_Text*>     m_PlayerTexts;
    m_PlayerTexts.reserve((int)PlayerDataSequnce::END);
    TextObjs[UIPARENT::PLAYERINFO] = m_PlayerTexts;

    vector<UIObj_Text*>     m_WeaponTexts;
    m_WeaponTexts.reserve((int)WeaponDataSequnce::END);
    TextObjs[UIPARENT::WEAPONINFO] = m_WeaponTexts;

    vector<UIObj_Text*>     m_ShieldTexts;
    m_ShieldTexts.reserve((int)ShieldnDataSequnce::END);
    TextObjs[UIPARENT::TYPE] = m_ShieldTexts;


    CHECK_FAILED(Load_LevelData(), E_FAIL);

    m_pInventoryManager = InventoryManager::GetInstance();
 
    WeaponDatas.push_back(LevelUpInfo_Weapon());
    WeaponDatas.push_back(LevelUpInfo_Weapon());


    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/


_int Client::UIObj_Window_LevelUp::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_LevelUp::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_LevelUp::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    /*키보드 입력*/
    if (m_pGameInstance->KeyDown(DIK_LEFTARROW))
    {
        --m_iShowLevel;

        m_iShowLevel = clamp<_uint>(m_iShowLevel, 1, iMaxLevel);
        Set_LevelData(m_iShowLevel);
    }

    if (m_pGameInstance->KeyDown(DIK_RIGHTARROW))
    {
        ++m_iShowLevel;

        m_iShowLevel = clamp<_uint>(m_iShowLevel, 1, iMaxLevel);
        Set_LevelData(m_iShowLevel);
    }
    CHECK_FALSE_RESULT(m_bAbleKeyInput, 0);
    CHECK_FALSE_RESULT(m_bFocus, 0);

    if (m_pGameInstance->KeyDown(DIK_E))
    {
        CHECK_NULL_RESULT(m_pTargetData,0);
        //헤이즈 감소
        InventoryManager::GetInstance()->Reduce_Haze(m_pTargetData->PlayerData.NeedHaze);
        
        //레벨업ui띄우기
        UI_MasterEvent Event;
        Event.m_bFlag = true;
        Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
        Event.m_ActionName = "ActiveUI";
        Event.m_Text = L"LevelUp_Popup";
        m_pGameInstance->Publish(Event);
        PlayLevelUpSound();



        //레벨업호출
        m_pMainPlayerStatComp->LevelUp(m_pTargetData->iLevel, m_pTargetData->MaxHp, m_pTargetData->MaxST, m_pTargetData->MaxAttack);

        /*갱신!!*/
        Set_CurrentPlayerData();    //가져올수있는 플레이어데이터가져오기
        Set_LevelData(m_iLevel);    //일단 현재데이터꺼 보여주기
        m_iShowLevel = m_iLevel;

        m_bFocus = false;   //포커스는 이제 다른 popup이..

        ESCLOCKEVENT LockEvent;
        LockEvent.m_bLock = true;
        m_pGameInstance->Publish(LockEvent);


        //레벨 3/7스킬얻음
        if (m_iShowLevel >= 3 && !m_bGetItem_Level3)
        {
            InventoryManager::GetInstance()->Get_Skill_ByLevel(3);
            m_bGetItem_Level3 = true;
        }

        if (m_iShowLevel >= 7 && !m_bGetItem_Level7)
        {
            InventoryManager::GetInstance()->Get_Skill_ByLevel(7);
        }
    }
    return 0;
}

HRESULT Client::UIObj_Window_LevelUp::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

void Client::UIObj_Window_LevelUp::Free()
{
    __super::Free();
}

HRESULT Client::UIObj_Window_LevelUp::Load_LevelData()
{

    wstring filePath = L"../../DataFiles/LevelUp_Data/LevelUpData.json";

    std::ifstream file(filePath);
    if (!file.is_open())
        return E_FAIL;

    nlohmann::json rootData;
    file >> rootData;
    file.close();

    if (rootData.find("LevelTable") == rootData.end())
        return E_FAIL;

    auto& levelTable = rootData["LevelTable"];

    // 기존 데이터 초기화
    m_LevelUpDatas.clear();

    // 3. 배열 순회
    for (auto& levelEntry : levelTable)
    {
        LevelUpData tLevelData;

   
     
        int iCurrentLevel = levelEntry.value("Level", 0);
        if (iCurrentLevel == 0) continue;

        tLevelData.iLevel = iCurrentLevel;

        // PlayerData 및 하위 Stats 접근
        auto& playerData = levelEntry["PlayerData"];
        auto& stats = playerData["Stats"];
        auto& defense = levelEntry["DefenseStats"];

        // 기본 정보 할당
        tLevelData.MaxAttack = levelEntry.value("AttackPower", 0);
        tLevelData.MaxHp = levelEntry.value("MaxHp", 0);
        tLevelData.MaxST = levelEntry.value("MaxST", 0);

        tLevelData.PlayerData.NeedHaze = playerData.value("Info_NeedHaze", 0);
        tLevelData.PlayerData.EvadeType = stringToWstring(playerData.value("Info_Evade", "Step"));

        // 스탯(랭크) 정보 할당
        tLevelData.PlayerData.Type1 = stringToWstring(stats.value("Info_Type1_Strength", "C"));
        tLevelData.PlayerData.Type2 = stringToWstring(stats.value("Info_Type2_Dexterity", "C"));
        tLevelData.PlayerData.Type3 = stringToWstring(stats.value("Info_Type3_Mind", "D"));
        tLevelData.PlayerData.Type4 = stringToWstring(stats.value("Info_Type4_Will", "D"));
        tLevelData.PlayerData.Type5 = stringToWstring(stats.value("Info_Type5_Vitality", "C"));
        tLevelData.PlayerData.Type6 = stringToWstring(stats.value("Info_Type6_Fortitude", "C"));

        // 방어력 정보 (vector)
        tLevelData.ShieldData.vecInts.clear();
        tLevelData.ShieldData.vecInts.push_back(defense.value("Info_Type1_Physical", 0));
        tLevelData.ShieldData.vecInts.push_back(defense.value("Info_Type2_Fire", 0));
        tLevelData.ShieldData.vecInts.push_back(defense.value("Info_Type3_Ice", 0));
        tLevelData.ShieldData.vecInts.push_back(defense.value("Info_Type4_Lightning", 0));
        tLevelData.ShieldData.vecInts.push_back(defense.value("Info_Type5_Blood", 0));

        m_LevelUpDatas.emplace(iCurrentLevel, tLevelData);
    }

    return S_OK;
}


UIObj_Window_LevelUp* Client::UIObj_Window_LevelUp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Window_LevelUp* pInstance = new UIObj_Window_LevelUp(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Window_LevelUp 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_Window_LevelUp::Clone(void* pArg)
{
    UIObj_Window_LevelUp* pInstance = new UIObj_Window_LevelUp(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Window_LevelUp 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_Window_LevelUp::Cache_LevelInfoText()
{
    

    UIObject* pLevelInfo_Back = Get_Child(L"LevelInfo_Back");
    if (pLevelInfo_Back)
    {
        UIObject* pLevelInfo = pLevelInfo_Back->Get_Child(L"LevelInfo");
        if (pLevelInfo)
        {
            UIObj_Text* pLevelText = dynamic_cast<UIObj_Text*>(pLevelInfo->Get_Child(L"Text")); //레벨 표시 텍스트
            if (pLevelText)
                TextObjs[UIPARENT::LEVELINFO].push_back(pLevelText);
        }
    }
}

void Client::UIObj_Window_LevelUp::Cache_PlayerInfoText()
{

    UIObject* pPlayerInformation = Get_Child(L"Player_Information");
    wstring Informations[] = { L"Info_Haze",L"Info_LevelUpHaze",L"Info_BloodCode",L"Info_HP",L"Info_SP"
        ,L"Info_Myunghyeol",L"Info_Evade",L"Info_Type1",L"Info_Type2",L"Info_Type3",L"Info_Type4",L"Info_Type5",L"Info_Type6" };

    size_t count = std::size(Informations);

    if (pPlayerInformation)
    {
        for (int i = 0; i < count; ++i)
        {
            UIObject* pParent = pPlayerInformation->Get_Child(Informations[i]);//부모를먼저찾고
            if (pParent)
            {
                UIObj_Text* Text = dynamic_cast<UIObj_Text*>(pParent->Get_Child(L"Text"));
                if (Text)
                    TextObjs[UIPARENT::PLAYERINFO].push_back(Text);
            }
        }
    }

}

void Client::UIObj_Window_LevelUp::Cache_WeaponInfoText()
{
    UIObject* pWeaponInformation = Get_Child(L"Weapon_Information");
    wstring Informations[] = { L"Info_Attack",L"Info_Rolling",
        L"Info_Attack2",L"Info_Rolling2",
        L"Info_Physics",L"Info_Type"};

    size_t count = std::size(Informations);

    if (pWeaponInformation)
    {
        for (int i = 0; i < count; ++i)
        {
            UIObject* pParent = pWeaponInformation->Get_Child(Informations[i]);//부모를먼저찾고
            if (pParent)
            {
                UIObj_Text* Text = dynamic_cast<UIObj_Text*>(pParent->Get_Child(L"Text"));
                if (Text)
                    TextObjs[UIPARENT::WEAPONINFO].push_back(Text);
            }
        }
    }
}

void Client::UIObj_Window_LevelUp::Cache_TypeText()
{
    UIObject* pTypeInformation = Get_Child(L"Type_Infomation");
    wstring Informations[] = { L"Info_Type1",L"Info_Type2",
        L"Info_Type3",L"Info_Type4",
        L"Info_Type5"};

    size_t count = std::size(Informations);

    if (pTypeInformation)
    {
        for (int i = 0; i < count; ++i)
        {
            UIObject* pParent = pTypeInformation->Get_Child(Informations[i]);//부모를먼저찾고
            if (pParent)
            {
                UIObj_Text* Text = dynamic_cast<UIObj_Text*>(pParent->Get_Child(L"Text"));
                if (Text)
                    TextObjs[UIPARENT::TYPE].push_back(Text);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Window_LevelUp::After_ApplyData()
{
    __super::After_ApplyData();

    //자식캐싱
    //LevelInfo_Back의 자식들캐싱
    Cache_LevelInfoText();
    Cache_PlayerInfoText();
    Cache_WeaponInfoText();
    Cache_TypeText();

   
    UIObject* pLeftArrow = Get_Child(L"LeftArrow");
    if (pLeftArrow)
    {
        pLeftArrow->Bind_OnClickEvent([this]()
            {
                --m_iShowLevel;

                m_iShowLevel = clamp<_uint>(m_iShowLevel, 1, iMaxLevel);
                Set_LevelData(m_iShowLevel);

                PlayArrowSound();
            });

        m_Arrows.push_back(pLeftArrow);
    }


    UIObject* pRightArrow = Get_Child(L"RightArrow");
    if (pRightArrow)
    {
        pRightArrow->Bind_OnClickEvent([this]()
            {
                ++m_iShowLevel;

                m_iShowLevel = clamp<_uint>(m_iShowLevel, 1, iMaxLevel);
                Set_LevelData(m_iShowLevel);
                PlayArrowSound();

            });

        m_Arrows.push_back(pRightArrow);
    }

    UIObject* pObj = Get_Child(L"Key_E");
    if (pObj)
        Interactions.push_back(pObj);

    pObj = Get_Child(L"InteractionText_Desc");
    if (pObj)
        Interactions.push_back(pObj);


}
/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_Window_LevelUp::Set_CurrentPlayerData()
{
    if (!m_pMainPlayer)
        m_pMainPlayer = dynamic_cast<Player*>(m_pGameInstance->Get_Player());

    CHECK_JUST_NULL(m_pMainPlayer);

    //소지헤이즈 가져오기
    m_PlayerData.CurrentHaze = m_pInventoryManager->Get_Haze();

 


    if(!m_pMainPlayerStatComp)
        m_pMainPlayerStatComp = dynamic_cast<Player_Stat*>(m_pMainPlayer->Get_PlayerStatCom());
    
    CHECK_JUST_NULL(m_pMainPlayerStatComp);

    //현재 hp가져오기
    m_PlayerData.iHp = m_pMainPlayerStatComp->Get_Hp();

    //현재 ST가져오기
    m_PlayerData.iST = m_pMainPlayerStatComp->Get_Stamina();

    //현재명혈가져오기
    m_PlayerData.Meyonghyul = (int)m_pMainPlayerStatComp->Get_Myeonghyeol();
    m_PlayerData.MaxMeyonghyul =(int)m_pMainPlayerStatComp->Get_MaxMyeonghyeol();


    //블러드코드 이름가져오기
    ItemInfo* pBloodCodeItem = m_pInventoryManager->Get_MainSlotItemInfo(_UINT(MAINUISLOT::BLOODCODE), 0);
    if (pBloodCodeItem)
        m_PlayerData.BloodCodeName = pBloodCodeItem->ItemName;

    //웨폰데이터
    //무기에 대한 데이터
    {
        for (int i = 0; i < 2; ++i)
        {
            ItemInfo* Weapon = m_pInventoryManager->Get_MainSlotItemInfo(_UINT(MAINUISLOT::EQUIP_WEAPON), i);
            if (Weapon)
            {
                WeaponDatas[i].Attack = Weapon->Get_Value(ITEM_VALUE::PHYSICS_DMG);
                WeaponDatas[i].Type = Weapon->m_AttackType;
                WeaponDatas[i].m_bEquip = true;
            }
            else
                WeaponDatas[i].m_bEquip = false;
        }



    }

    //clothes데이터
    
	ItemInfo* Clothes = m_pInventoryManager->Get_MainSlotItemInfo(_UINT(MAINUISLOT::EQUIP_ARMOR), 0);
    if (Clothes)
    {
        ClothesData.Attack = Clothes->Get_Value(ITEM_VALUE::PHYSICS_DMG);
        ClothesData.Type = Clothes->m_AttackType;

    }

    //레벨
    m_iLevel = m_pMainPlayerStatComp->Get_Level();

}

void Client::UIObj_Window_LevelUp::Set_LevelData(_uint iLevel)
{
    CHECK_TRUE(m_iLevel > iLevel);  //들어온매개변수의 레벨이 현재플레이어 레벨보다 작을수없음


    auto LevelDataiter = m_LevelUpDatas.find(iLevel);
    CHECK_TRUE(LevelDataiter == m_LevelUpDatas.end());
    //현재레벨 과 같은지 판단하고, 아니라면 파란색표시필요함(변경사항)
    m_pTargetData = &LevelDataiter->second;
    int iStep = iLevel - m_iLevel;
    Set_Different_Effect(LevelDataiter->second,iLevel);


    //LevelDatatext 표시
    //레벨표시
    auto LevelIter = TextObjs.find(UIPARENT::LEVELINFO);
    CHECK_TRUE(LevelIter == TextObjs.end());
    LevelIter->second.at(0)->Set_Text(to_wstring(iLevel));

    ////////////////////PlayerData표시/////////////////////////
    auto PlayerDataIter = TextObjs.find(UIPARENT::PLAYERINFO);
    CHECK_TRUE(PlayerDataIter == TextObjs.end());
    auto PlayerDataVec = PlayerDataIter->second;

    PlayerDataVec[_UINT(PlayerDataSequnce::CURRENTHAZE)]->Set_Text(to_wstring(m_PlayerData.CurrentHaze));//소지헤이즈
    PlayerDataVec[_UINT(PlayerDataSequnce::NEEDHAZE)]->Set_Text(to_wstring(LevelDataiter->second.PlayerData.NeedHaze));//레벨업 필요량
    PlayerDataVec[_UINT(PlayerDataSequnce::INPUT_BLOODCODE)]->Set_Text(m_PlayerData.BloodCodeName);//레벨업 필요량

    PlayerDataVec[_UINT(PlayerDataSequnce::HP)]->Set_Text(to_wstring(LevelDataiter->second.MaxHp));//최대 HP
    PlayerDataVec[_UINT(PlayerDataSequnce::SP)]->Set_Text(to_wstring(LevelDataiter->second.MaxST));//최대 ST
    PlayerDataVec[_UINT(PlayerDataSequnce::MYUNGHYEOL)]->Set_Text(L"10(30)");//명혈스톡(고정)
    PlayerDataVec[_UINT(PlayerDataSequnce::EVADE)]->Set_Text(LevelDataiter->second.PlayerData.EvadeType);//기본회피성능

    PlayerDataVec[_UINT(PlayerDataSequnce::TYPE1)]->Set_Text(LevelDataiter->second.PlayerData.Type1);//완력
    PlayerDataVec[_UINT(PlayerDataSequnce::TYPE2)]->Set_Text(LevelDataiter->second.PlayerData.Type2);//재주
    PlayerDataVec[_UINT(PlayerDataSequnce::TYPE3)]->Set_Text(LevelDataiter->second.PlayerData.Type3);//정신
    PlayerDataVec[_UINT(PlayerDataSequnce::TYPE4)]->Set_Text(LevelDataiter->second.PlayerData.Type4);//의지
    PlayerDataVec[_UINT(PlayerDataSequnce::TYPE5)]->Set_Text(LevelDataiter->second.PlayerData.Type5);//활력
    PlayerDataVec[_UINT(PlayerDataSequnce::TYPE6)]->Set_Text(LevelDataiter->second.PlayerData.Type6);//활력


    /////WeaponData표시////////
    auto WeaponDataIter = TextObjs.find(UIPARENT::WEAPONINFO);
    CHECK_TRUE(WeaponDataIter == TextObjs.end());
    auto WeaponDataVec = WeaponDataIter->second;

    //장착여부에 따라서 표시처리
    WeaponDataVec[_UINT(WeaponDataSequnce::ATTACK1)]->Set_Text(to_wstring(WeaponDatas[0].Attack + (iStep*2)));
    WeaponDataVec[_UINT(WeaponDataSequnce::ROLLING1)]->Set_Text(WeaponDatas[0].Type);

    WeaponDataVec[_UINT(WeaponDataSequnce::ATTACK2)]->Set_Text(to_wstring(WeaponDatas[1].Attack+ (iStep * 2)));
    WeaponDataVec[_UINT(WeaponDataSequnce::ROLLING2)]->Set_Text(WeaponDatas[1].Type);


    WeaponDataVec[_UINT(WeaponDataSequnce::ATTACK1)]->Set_Active(WeaponDatas[0].m_bEquip);
    WeaponDataVec[_UINT(WeaponDataSequnce::ROLLING1)]->Set_Active(WeaponDatas[0].m_bEquip);

    WeaponDataVec[_UINT(WeaponDataSequnce::ATTACK2)]->Set_Active(WeaponDatas[1].m_bEquip);
    WeaponDataVec[_UINT(WeaponDataSequnce::ROLLING2)]->Set_Active(WeaponDatas[1].m_bEquip);

   
   


    /////ClothesData표시////////
    WeaponDataVec[_UINT(WeaponDataSequnce::ATTACK3)]->Set_Text(to_wstring(ClothesData.Attack+ (iStep * 2)));
    WeaponDataVec[_UINT(WeaponDataSequnce::ROLLING3)]->Set_Text(ClothesData.Type);

    //방어력표시//
    auto DefenceDataIter = TextObjs.find(UIPARENT::TYPE);
    CHECK_TRUE(DefenceDataIter == TextObjs.end());
    auto DefenceDataVec = DefenceDataIter->second;

    DefenceDataVec[_UINT(ShieldnDataSequnce::TYPE1)]->Set_Text(to_wstring(LevelDataiter->second.ShieldData.vecInts[0]));//완력
    DefenceDataVec[_UINT(ShieldnDataSequnce::TYPE2)]->Set_Text(to_wstring(LevelDataiter->second.ShieldData.vecInts[1]));//재주
    DefenceDataVec[_UINT(ShieldnDataSequnce::TYPE3)]->Set_Text(to_wstring(LevelDataiter->second.ShieldData.vecInts[2]));//정신
    DefenceDataVec[_UINT(ShieldnDataSequnce::TYPE4)]->Set_Text(to_wstring(LevelDataiter->second.ShieldData.vecInts[3]));//의지
    DefenceDataVec[_UINT(ShieldnDataSequnce::TYPE5)]->Set_Text(to_wstring(LevelDataiter->second.ShieldData.vecInts[4]));//활력
  

}

void Client::UIObj_Window_LevelUp::Set_Different_Effect(LevelUpData& Data,_uint iLevel)
{
    bool bDiff = m_iLevel != iLevel;
    if (bDiff)
    {
        m_Arrows[0]->Set_Color(_float4(0.f, 0.f, 0.f, 1.f));
        /*MaxLevel에 도달했을경우 더이상못감*/
        if (iLevel != iMaxLevel)
            m_Arrows[1]->Set_Color(_float4(0.f, 0.f, 0.f, 1.f));
        else
            m_Arrows[1]->Set_Color(DEFAULTCOLOR);


        auto PlayerDataIter = TextObjs.find(UIPARENT::PLAYERINFO);
        CHECK_TRUE(PlayerDataIter == TextObjs.end());
        auto PlayerDataVec = PlayerDataIter->second;

        PlayerDataVec[_UINT(PlayerDataSequnce::HP)]->Set_Color(DIFFERCOLOR);//최대 HP
        PlayerDataVec[_UINT(PlayerDataSequnce::SP)]->Set_Color(DIFFERCOLOR);//최대 ST


        auto WeaponDataIter = TextObjs.find(UIPARENT::WEAPONINFO);
        CHECK_TRUE(WeaponDataIter == TextObjs.end());
        auto WeaponDataVec = WeaponDataIter->second;

        WeaponDataVec[_UINT(WeaponDataSequnce::ATTACK1)]->Set_Color(DIFFERCOLOR);
        WeaponDataVec[_UINT(WeaponDataSequnce::ATTACK2)]->Set_Color(DIFFERCOLOR);
        WeaponDataVec[_UINT(WeaponDataSequnce::ATTACK3)]->Set_Color(DIFFERCOLOR);


        //헤이즈가 많은경우에만 활성화색처리
        _float4 TargetColor = _float4(1.f, 1.f, 1.f, 1.f);
        m_bAbleKeyInput = true;
        PlayerDataVec[_UINT(PlayerDataSequnce::CURRENTHAZE)]->Set_Color(DEFAULTCOLOR);

        if (m_PlayerData.CurrentHaze < Data.PlayerData.NeedHaze)
        {
            TargetColor = _float4(0.3f, 0.3f, 0.3f, 1.f);
            PlayerDataVec[_UINT(PlayerDataSequnce::CURRENTHAZE)]->Set_Color(LESSCOLOR);            //색깔빨갛게.

            m_bAbleKeyInput = false;
        }


      
        for (auto& pInteraction : Interactions)
            pInteraction->Set_Color(TargetColor);

    }

    else
    {
        m_Arrows[0]->Set_Color(DEFAULTCOLOR);

        /*MaxLevel에 도달했을경우 더이상못감*/
        if(iLevel !=iMaxLevel)
            m_Arrows[1]->Set_Color(_float4(0.f, 0.f, 0.f, 1.f));
        else
            m_Arrows[1]->Set_Color(DEFAULTCOLOR);


 


        auto PlayerDataIter = TextObjs.find(UIPARENT::PLAYERINFO);
        CHECK_TRUE(PlayerDataIter == TextObjs.end());
        auto PlayerDataVec = PlayerDataIter->second;

        PlayerDataVec[_UINT(PlayerDataSequnce::CURRENTHAZE)]->Set_Color(DEFAULTCOLOR);            //색깔빨갛게.

        PlayerDataVec[_UINT(PlayerDataSequnce::HP)]->Set_Color(DEFAULTCOLOR);//최대 HP
        PlayerDataVec[_UINT(PlayerDataSequnce::SP)]->Set_Color(DEFAULTCOLOR);//최대 ST


        auto WeaponDataIter = TextObjs.find(UIPARENT::WEAPONINFO);
        CHECK_TRUE(WeaponDataIter == TextObjs.end());
        auto WeaponDataVec = WeaponDataIter->second;

        WeaponDataVec[_UINT(WeaponDataSequnce::ATTACK1)]->Set_Color(DEFAULTCOLOR);
        WeaponDataVec[_UINT(WeaponDataSequnce::ATTACK2)]->Set_Color(DEFAULTCOLOR);
        WeaponDataVec[_UINT(WeaponDataSequnce::ATTACK3)]->Set_Color(DEFAULTCOLOR);


        for (auto& pInteraction : Interactions)
            pInteraction->Set_Color(_float4(0.3f, 0.3f, 0.3, 1.f));

        m_bAbleKeyInput = false;
    }


}

void Client::UIObj_Window_LevelUp::Set_Active(_bool _isActive)
{
    __super::Set_Active(_isActive);

    if (_isActive)
    {
        m_bFocus = true;

        Set_CurrentPlayerData();    //가져올수있는 플레이어데이터가져오기
        Set_LevelData(m_iLevel);    //키면 일단 현재데이터꺼 보여주기
        m_iShowLevel = m_iLevel;
        
    }

    

}