#include "Client_Define.h"
#include "CustomizingManager.h"
#include "GameInstance.h"

#include "UIObj_CustomizingInfo.h"
#include "UIObj_CustomMenu.h"

#include "UIObj_CustomizeGrid.h"
#include "UIObj_CustomMenuButton.h"
#include "UIObj_CustomizeSelector.h"

#include "UIObj_Window_Palette.h"
#include "UIObj_PaletteArea.h"
#include "UIObj_VerticalSlider.h"

#include "Player_Head.h"
#include "Player_Hair.h"
#include "Player_Body.h"
#include "Player_Outer.h"

#include "UIObj_TextureSelector.h"
#include "UIObj_MeshSelector.h"

#include "UIObj_ItemGrid.h"
#include "UIObj_ColorSelector.h"
#include "UIObj_ValueSelector.h"

#include "Camera.h"
#include "UISoundUtil.h"




IMPLEMENT_SINGLETON(CustomizingManager);

Client::CustomizingManager::CustomizingManager()
{
}

Client::CustomizingManager::~CustomizingManager()
{
}
HRESULT Client::CustomizingManager::Initialize()
{
    m_pGameInstance = GameInstance::GetInstance();
    
    m_pMainPlayer = dynamic_cast<Player*>(m_pGameInstance->Get_Player());
    CHECK_NULLPTR(m_pMainPlayer);

    Cache_Pickers();

    //ㅈ ㅏ주 사용할 구조체들 캐싱
    pPlayer_Shader_Desc = m_pMainPlayer->Get_PlayerShaderDescPtr();
    Player_Head* pPlayerHead = dynamic_cast<Player_Head*>(m_pMainPlayer->Find_PartObject(TEXT("Part_Head")));
    CHECK_NULLPTR(pPlayerHead);
    pHead_Texture_Desc = pPlayerHead->Get_HeadTexturePtr();

    //파트오브젝트캐싱
    m_pPlayerHair = dynamic_cast<Player_Hair*>(m_pMainPlayer->Find_PartObject(TEXT("Part_Hair")));
    CHECK_NULLPTR(m_pPlayerHair);

    m_pPlayerHead = dynamic_cast<Player_Head*>(m_pMainPlayer->Find_PartObject(TEXT("Part_Head")));
    CHECK_NULLPTR(pPlayerHead);


    m_pPlayerBody = dynamic_cast<Player_Body*>(m_pMainPlayer->Find_PartObject(TEXT("Part_Body")));
    CHECK_NULLPTR(m_pPlayerBody);

    m_pPalette = dynamic_cast<UIObj_Window_Palette*>(m_pActivePickers[0]);

 
    CHECK_NULLPTR(m_pPalette);


    CHECK_FAILED(Ready_SkinData(), E_FAIL);
    CHECK_FAILED(Ready_HairData(), E_FAIL);
    CHECK_FAILED(Ready_EyeData(), E_FAIL);
    CHECK_FAILED(Ready_EyebrowData(), E_FAIL);
    CHECK_FAILED(Ready_MakeUpData(), E_FAIL);
    CHECK_FAILED(Ready_ClothesData(), E_FAIL);
    CHECK_FAILED(Ready_FadcePaintData(), E_FAIL);

    //모든 colorselector 들이 default값으로 미리보기 색깔 지정할수있또록하기
    UIObj_CustomizeSelector::CustomizeSelectorUIEvent Event;
    Event.eType = UIObj_CustomizeSelector::EventType::SET_DEFAULTCOLOR;
    m_pGameInstance->Publish(Event);


    return S_OK;

}
HRESULT Client::CustomizingManager::Cache_Pickers()
{
    m_pActivePickers.reserve(_UINT(CUSTOMIZING_VALUE_TYPE::END));

    //COLOR TYPE : 팔레트열어라.
    m_pActivePickers.push_back(m_pGameInstance->Find_UI_ByName(L"ColorPalette"));

    //Texturetype: itemgrid열어라.
    m_pActivePickers.push_back(m_pGameInstance->Find_UI_ByName(L"ItemGrid"));


    //Meshtype: itemgrid열어라.
    m_pActivePickers.push_back(m_pGameInstance->Find_UI_ByName(L"ItemGrid"));

    return S_OK;
}
void Client::CustomizingManager::Update(const _float fTimeDelta)
{
    CHECK_JUST_NULL(m_pGameInstance);
    CHECK_FALSE(m_bActive);

    if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::RB))
    {
       //창이있을때 하나씩닫기
        if (m_pActivePicker || 
            m_bOpenValueType)
        {
            PlayCancleSound();
            

            Request_Close_ActivePicker();
            Set_UIState(CustomMizingstate::FOCUS);
        }
           

        else
        {
            //TPYE변경(호버가능한 ㅅ아태로 만들기)
            On_FocusMenuExit();
            //m_eFocusType = CUSTOMIZING_TYPE::END;
        }
 
    }

#ifdef _DEBUG
    if (m_pGameInstance->KeyDown(DIK_P))
        Debug_Current();
#endif // _DEBUG

    
}
void Client::CustomizingManager::Set_FocusType(CUSTOMIZING_TYPE eType)
{
   

    if (m_eFocusType != eType)
    {
        m_ePreFocusType = m_eFocusType;
        m_eFocusType = eType;

        //1.맞는 메뉴 visible시키도록 이벤트보내기!!(상점 slotgrid처럼)
        UIObj_CustomizeGrid::CustomizeGridUIEvent GridEvent;
        GridEvent.eventType = UIObj_CustomizeGrid::CustomizeGridEventType::UPDATE_FOCUSCATEGORY;
        GridEvent.eTargetCategory = eType;
        m_pGameInstance->Publish(GridEvent);



        //2. Info 갱신하기!!
        UIObj_CustomizingInfo::CustomizingInfoUIEvent InfoEvent;
        InfoEvent.m_eType = m_eFocusType;
        m_pGameInstance->Publish(InfoEvent);


        //3.selector 갱신하기!!
        UIObj_CustomMenu::CustomMenuUIEvent MenuEvent;
        MenuEvent.eType = UIObj_CustomMenu::CustomMenuUIEventType::UPDATE_SELECTOR;
        MenuEvent.m_eCustomType = m_eFocusType;
        m_pGameInstance->Publish(MenuEvent);


    }
  
    
 
}

void Client::CustomizingManager::On_FocusMenuEnter()
{
	Set_UIState(CustomizingManager::CustomMizingstate::FOCUS);

	

}

void Client::CustomizingManager::On_FocusMenuExit()
{
    HRESULT hr = Set_UIState(CustomMizingstate::READY);
    if (hr == S_OK)
        PlayCancleSound();


    
}

HRESULT Client::CustomizingManager::Set_UIState(CustomMizingstate eState)
{
    if (m_eUIState != eState)
    {
        m_ePreState = m_eUIState;
        m_eUIState = eState;

#ifdef _DEBUG
        string Str = "";
        switch (eState)
        {
        case Client::CustomizingManager::READY:
            Str = "READY";
            break;
        case Client::CustomizingManager::FOCUS:
            Str = "FOCUS";
            break;
        case Client::CustomizingManager::ACTIVE_PICKER:
            Str = "ACTIVE_PICKER";
            break;
        case Client::CustomizingManager::END:
            break;
        default:
            break;
        }
#endif // _DEBUG

        Enter_State(m_eUIState);

        return S_OK;
    }

    return E_FAIL;
}

void Client::CustomizingManager::Enter_State(CustomMizingstate eState)
{
    switch (eState)
    {
    case Client::CustomizingManager::READY:
    {

        //메뉴 나감처리(알파값되돌리고, HOVER EXIT호출)
        UIObj_CustomizeGrid::CustomizeGridUIEvent GridEvent;
        GridEvent.eTargetCategory = m_eFocusType;
        GridEvent.eventType = UIObj_CustomizeGrid::CustomizeGridEventType::EXIT_MENU;
        m_pGameInstance->Publish(GridEvent);

        //6.메뉴 들어왔으니까 메뉴버튼은 어둡게 처리하자
        UIObj_CustomMenuButton::CustomMenuButtonUIEvent ButtonUIEvent;
        ButtonUIEvent.eType = UIObj_CustomMenuButton::CustomMenuButtonUIType::EXIT_MENU;
        ButtonUIEvent.m_eCustomType = m_eFocusType;
        m_pGameInstance->Publish(ButtonUIEvent);

        m_pSelectedTargetField = nullptr;
        m_iSelectFocusIdx = 0;

    }
        break;
    case Client::CustomizingManager::FOCUS:
    {
        if (m_ePreState == CustomizingManager::READY)
        {
            //가장첫번째 요소를 hover상태로 유지시켜라! (Grid에게 openMenu알림)
            UIObj_CustomizeGrid::CustomizeGridUIEvent GridEvent;
            GridEvent.eTargetCategory = m_eFocusType;
            GridEvent.eventType = UIObj_CustomizeGrid::CustomizeGridEventType::OPEN_MENU;
            m_pGameInstance->Publish(GridEvent);   
            
            //FocusIdx초기화
            m_iFocusIdx = 0;

            PlayHoverSound();

        }


     

        //들어왔으니까 메뉴버튼은 어둡게 처리하자
        UIObj_CustomMenuButton::CustomMenuButtonUIEvent ButtonUIEvent;
        ButtonUIEvent.eType = UIObj_CustomMenuButton::CustomMenuButtonUIType::ENTER_MENU;
        ButtonUIEvent.m_eCustomType = m_eFocusType;
        m_pGameInstance->Publish(ButtonUIEvent);
    }
        break;
    case Client::CustomizingManager::ACTIVE_PICKER:
    {
        if (m_ePreState != CustomizingManager::FOCUS)
        {
            //들어왔으니까 m_pCurrentTargetField메뉴버튼은 어둡게 처리하자
            UIObj_CustomMenuButton::CustomMenuButtonUIEvent ButtonUIEvent;
            ButtonUIEvent.eType = UIObj_CustomMenuButton::CustomMenuButtonUIType::ENTER_MENU;
            ButtonUIEvent.m_eCustomType = m_eFocusType;
            m_pGameInstance->Publish(ButtonUIEvent);

            if (m_pHoverTargetField)
                m_pHoverTargetField->OnHoverEnter_Force();

        }
       

        //선택할 수 없는것들은 어둡게처리
        UIObj_CustomizeGrid::CustomizeGridUIEvent GridEvent;
        GridEvent.eventType = UIObj_CustomizeGrid::CustomizeGridEventType::OPEN_SELECTOR;
        GridEvent.eTargetCategory = m_eFocusType;

        if (m_pSelectedTargetField)
        {
            int idx= *m_pSelectedTargetField->Get_Idx();
            GridEvent.idx = idx;
        }
            

        m_pGameInstance->Publish(GridEvent);
    }
        break;
    case Client::CustomizingManager::END:
        break;
    default:
        break;
    }
}



void Client::CustomizingManager::Request_Open_ActivePicker(CUSTOMIZING_TYPE CustomType, CUSTOMIZING_VALUE_TYPE eType, UIObj_CustomizeSelector* Caller)
{
    switch (eType)
    {
    case CUSTOMIZING_VALUE_TYPE::COLOR:
    {
        m_pActivePicker = m_pActivePickers[_UINT(CUSTOMIZING_VALUE_TYPE::COLOR)];
        m_pSelectedTargetField = Caller;
        m_iSelectFocusIdx= Caller->Get_Value();
        m_eEditingType = CustomType;

        m_iFocusIdx = Caller->Get_Value();


        //상태변경
        Set_UIState(CustomMizingstate::ACTIVE_PICKER);


        //창 열기
        UI_MasterEvent Event;
        Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
        Event.m_bFlag = false;
        Event.m_bPersistent = false;
        Event.m_ActionName = "OpenWindow";
        Event.m_Text = L"ColorPalette";
        m_pGameInstance->Publish(Event);


        //값세팅
        /*팔레트 이벤트*/

        _uint iTargetIdx;

        if (CustomType == CUSTOMIZING_TYPE::CLOTHES)
            iTargetIdx = m_iClothesIdx * 6 + m_iSelectFocusIdx;

        else
            iTargetIdx = m_iSelectFocusIdx;

        UIObj_PaletteArea::PaletteAreaUIEvent   UIEvent;
        UIEvent.eType = UIObj_PaletteArea::PaletteAreaUIEventType::OPEN_PALETTE;
        auto iter = m_CustomDatas.find(m_eFocusType);
        if (iter != m_CustomDatas.end())
        {
            CustomData& Data = iter->second;

            unordered_map<CustomKey, CustomColorInfo> ColorDatas = Data.m_ColorInfos;
            auto ColorIter = find_if(ColorDatas.begin(), ColorDatas.end(), [iTargetIdx,this](const auto& pair)
                {
                    return pair.first.idx == iTargetIdx;
                });

            if (ColorIter != ColorDatas.end())
                UIEvent.ColorInfo = &ColorIter->second;

            //("선택한 selector" << ColorIter->first.Name << "idx:" << ColorIter->first.idx);

            /*슬라이더이벤트*/
            UIObj_VerticalSlider::VerticalSliderUIEvent SliderEvent;
            SliderEvent.eType = UIObj_VerticalSlider::VerticalSliderUIEventType::OPEN_PALETTE;
            SliderEvent.ColorInfo = &ColorIter->second;
            m_pGameInstance->Publish(SliderEvent);

            //슬라이더이후,paleete
            m_pGameInstance->Publish(UIEvent);

            //display색 동기화
            Update_CurrentTargetField(&ColorIter->second.vColor);
        }

       

       
    }
       

        break;
    case CUSTOMIZING_VALUE_TYPE::TEXTURE:
    {
        m_pActivePicker = m_pActivePickers[_UINT(CUSTOMIZING_VALUE_TYPE::MESH)];
        m_pSelectedTargetField = Caller;
        m_iSelectFocusIdx = (int)Caller->Get_Value();
        m_eEditingType = CustomType;

        m_iFocusIdx = (int)Caller->Get_Value();


        //상태변경
        Set_UIState(CustomMizingstate::ACTIVE_PICKER);


        //창 열기
        UI_MasterEvent Event;
        Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
        Event.m_bFlag = false;
        Event.m_bPersistent = false;
        Event.m_ActionName = "OpenWindow";
        Event.m_Text = L"ItemGrid";
        m_pGameInstance->Publish(Event);


        //값세팅
        /*textureselector event*/
        auto iter = m_CustomDatas.find(m_eFocusType);
        if (iter != m_CustomDatas.end())
        {
            CustomData& Data = iter->second;

            unordered_map<CustomKey, CustomItemGridInfo> TexDatas = Data.m_Texs;
            auto TexIter = find_if(TexDatas.begin(), TexDatas.end(), [this](const auto& pair)
                {
                    return pair.first.idx == m_iSelectFocusIdx;
                });

            if (TexIter != TexDatas.end())
            {
                UIObj_TextureSelector* pSelector = dynamic_cast<UIObj_TextureSelector*>(Caller);
                if (pSelector)
                {
                    //itemgrid한테 값세팅한다(저장된값으로 세팅)
                    UIObj_ItemGrid::ItemGridUIEventEvent GridEvent;
                    GridEvent.NewTexKey = pSelector->Get_NewTexKey();
                    GridEvent.Desc = pSelector->Get_Desc();
                    GridEvent.eType = UIObj_ItemGrid::ItemGridUIEventType::OPEN_TEXTURESELECTOR;
                    GridEvent.iFocusTexIdx =  TexIter->second.TexIdx;
                    GridEvent.iTotalTex = pSelector->Get_TotalTex();

                    GridEvent.iFocusSlotIdx = m_iFocusIdx;
                    GridEvent.CustomizingType = m_eEditingType;
                    GridEvent.CustomizingValueType = pSelector->Get_Custom_ValueType();
                    
                    m_pGameInstance->Publish(GridEvent);

                    //display 미리보기 동기화
                    Update_CurrentTargetField(&TexIter->second);
                }
            }

        }
        

    }

        break;
    case CUSTOMIZING_VALUE_TYPE::MESH:
    {
        m_pActivePicker = m_pActivePickers[_UINT(CUSTOMIZING_VALUE_TYPE::MESH)];
        m_pSelectedTargetField = Caller;
        m_iSelectFocusIdx = (int)Caller->Get_Value();
        m_eEditingType = CustomType;

        m_iFocusIdx = (int)Caller->Get_Value();


        //상태변경
        Set_UIState(CustomMizingstate::ACTIVE_PICKER);


        //창 열기
        UI_MasterEvent Event;
        Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
        Event.m_bFlag = false;
        Event.m_bPersistent = false;
        Event.m_ActionName = "OpenWindow";
        Event.m_Text = L"ItemGrid";
        m_pGameInstance->Publish(Event);


        //값세팅
        /*textureselector event*/
        auto iter = m_CustomDatas.find(m_eFocusType);
        if (iter != m_CustomDatas.end())
        {
            CustomData& Data = iter->second;

            unordered_map<CustomKey, CustomItemGridInfo> MeshDatas = Data.m_Meshs;
            auto MeshIter = find_if(MeshDatas.begin(), MeshDatas.end(), [this](const auto& pair)
                {
                    return pair.first.idx == m_iSelectFocusIdx;
                });

            if (MeshIter != MeshDatas.end())
            {
                UIObj_MeshSelector* pSelector = dynamic_cast<UIObj_MeshSelector*>(Caller);
                if (pSelector)
                {
                    //itemgrid한테 값세팅한다(저장된값으로 세팅)
                    UIObj_ItemGrid::ItemGridUIEventEvent GridEvent;
                    GridEvent.NewTexKey = pSelector->Get_NewTexKey();
                    GridEvent.Desc = pSelector->Get_Desc();
                    GridEvent.eType = UIObj_ItemGrid::ItemGridUIEventType::OPEN_TEXTURESELECTOR;
                    GridEvent.iFocusTexIdx = MeshIter->second.TexIdx;
                    GridEvent.iTotalTex = pSelector->Get_TotalTex();

                    GridEvent.iFocusSlotIdx = m_iFocusIdx;
                    GridEvent.CustomizingType = m_eEditingType;
                    GridEvent.CustomizingValueType = pSelector->Get_Custom_ValueType();

                    m_pGameInstance->Publish(GridEvent);

                    //display 미리보기 동기화
                    Update_CurrentTargetField(&MeshIter->second);
                }
            }

        }


    }
    break;

    case CUSTOMIZING_VALUE_TYPE::VALUE:
    {
        //창열건없음
        m_pActivePicker = nullptr;
        m_pSelectedTargetField = Caller;
        m_iSelectFocusIdx = (int)Caller->Get_Value();
        m_eEditingType = CustomType;

        m_iFocusIdx = (int)Caller->Get_Value();


        //상태변경
        Set_UIState(CustomMizingstate::ACTIVE_PICKER);

        UIObj_ValueSelector* pSelector = dynamic_cast<UIObj_ValueSelector*>(m_pSelectedTargetField);
        if (pSelector)
        {
            UIObj_ValueSelector::ValueSelectorEvent Event;
            Event.ownerID = pSelector->Get_ObjectID();
            Event.bValue = true;
            m_pGameInstance->Publish_Stack(Event);

        }

        m_bOpenValueType = true;
    }
        break;
    default:
        break;

    }

}

  


void Client::CustomizingManager::Request_Close_ActivePicker()
{
    //열린창이없으면 return
    //CHECK_JUST_NULL(m_pActivePicker);

    // 호출한 UI에게 값 전달..(
    //m_pCurrentTargetField->

    ////창닫으라고시키기
    if (m_pActivePicker)
    {
        UI_MasterEvent Event;
        Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
        Event.m_ActionName = "CloseWindow";
        Event.m_Text = m_pActivePicker->Get_Name();
        m_pGameInstance->Publish(Event);

    }


    //다시 밝게..
    UIObj_CustomizeGrid::CustomizeGridUIEvent GridEvent;
    GridEvent.eventType = UIObj_CustomizeGrid::CustomizeGridEventType::EXIT_SELECTOR;
    GridEvent.eTargetCategory = m_eFocusType;
    GridEvent.m_pCaller = m_pCurrentTargetField;


    m_pGameInstance->Publish(GridEvent);

    m_pCurrentTargetField->On_Close();

    //값 초기화
    m_pActivePicker = nullptr;

    m_pPreTargetField = m_pCurrentTargetField;
//    m_pCurrentTargetField = nullptr;
    m_bOpenValueType = false;

    m_pGameInstance->Get_CurrentCamera()->Camera_Lock(false);

}

void Client::CustomizingManager::Hover_CustomizeSelector(UIObject* pObj)
{
    m_pPreTargetField = m_pCurrentTargetField;
    m_pCurrentTargetField = dynamic_cast<UIObj_CustomizeSelector*>(pObj);

    CHECK_JUST_NULL(m_pPreTargetField);

    //이전거를 hover exit해준다
    if (m_pPreTargetField != pObj)
        m_pPreTargetField->OnHoverExit();

}

void Client::CustomizingManager::Update_CustomData(CUSTOMIZING_VALUE_TYPE ValueType, void* pArg)
{
    Update_CurrentTargetField(pArg);


    //타입에맞춰서 데이터저장

    switch (ValueType)
    {
    case CUSTOMIZING_VALUE_TYPE::COLOR:
    {
        if (m_eEditingType != CUSTOMIZING_TYPE::CLOTHES)
        {
            Export_ColorData* pData = static_cast<Export_ColorData*>(pArg);
            if (FAILED(Update_ColorData(pData->vColor, pData->fSaturate)))
                return;
            //플레이어연동
            Connet_Player(m_eEditingType, ValueType, m_iSelectFocusIdx);
        }
    
        else
        {
            //clostheㄴ일땐 현재 targetIdx에맞춰서 저장
            Export_ColorData* pData = static_cast<Export_ColorData*>(pArg);
            m_iSelectClothesIdx = m_iClothesIdx * 6 + m_iSelectFocusIdx ;
            if (FAILED(Update_ClothesData(pData->vColor, pData->fSaturate)))
                return;

            //플레이어연동
            Connet_Player(m_eEditingType, ValueType, m_iSelectClothesIdx);
        }

    }
        
        break;
    case CUSTOMIZING_VALUE_TYPE::TEXTURE:
        //클릭해야만 진짜저장
    {
        _uint* FocusIdx = static_cast<_uint*>(pArg);
        if (FAILED(Update_TexData(*FocusIdx)))
            return;
        //플레이어연동
        Connet_Player(m_eEditingType, ValueType, m_iSelectFocusIdx);

    }
        break;
    case CUSTOMIZING_VALUE_TYPE::MESH:
        //클릭해야만 진짜저장
    {
        _uint* FocusIdx = static_cast<_uint*>(pArg);
        if (FAILED(Update_MeshData(*FocusIdx)))
            return;
        //플레이어연동
        Connet_Player(m_eEditingType, ValueType, m_iSelectFocusIdx);

    }
        break;

    case CUSTOMIZING_VALUE_TYPE::VALUE:
        //클릭해야만 진짜저장
    {
        _float* fRatio = static_cast<_float*>(pArg);
        if (FAILED(Update_ValueData(*fRatio)))
            return;
        //플레이어연동
        Connet_Player(m_eEditingType, ValueType, m_iSelectFocusIdx);

    }
    break;

    case CUSTOMIZING_VALUE_TYPE::END:
        break;
    default:
        break;
    }
   
}

void Client::CustomizingManager::Update_CurrentTargetField(void* pArg)
{
    CHECK_JUST_NULL(m_pSelectedTargetField);

    m_pSelectedTargetField->Update_Display(pArg);

}

HRESULT Client::CustomizingManager::Ready_SkinData()
{
    
    /*대상-skincolor*/
    CustomData SkinData;
    _float4 vDefaultSkinColor=m_pGameInstance->Get_PlayerSkinColor();

    CustomColorInfo ColorInfo;

    ColorInfo.vColor = vDefaultSkinColor;
    ColorInfo.fSaturation = m_pPalette->Get_SaturationFromRGB(ColorInfo.vColor);

    ColorInfo.vCursorPos = m_pPalette->Get_CursorPosFromRGB(vDefaultSkinColor);
    ColorInfo.SliderCursorPosY = m_pPalette->Get_CursorPosYFromValue(ColorInfo.fSaturation);
    ColorInfo.m_UpdateFunc = [this](void* pArg)
        {
            _float4* vColor = static_cast<_float4*>(pArg);
            m_pGameInstance->Set_PlayerSkinColor(_float3(vColor->x,vColor->y,vColor->z));
        };


    SkinData.m_ColorInfos[CustomKey("SkinColor",0)] = ColorInfo;

    m_CustomDatas.emplace(CUSTOMIZING_TYPE::SKIN, SkinData);

    return S_OK;
}

HRESULT Client::CustomizingManager::Ready_HairData()
{




    /*참고할대상*/
    PLAYER_HAIR_SHADER_DESC* Desc = m_pPlayerHair->Get_PlayerHairShaderDescPtr();

    CustomData HairData;
    CustomColorInfo ColorInfo;

    ColorInfo.vColor = Desc->vPlayerHairColor;
    ColorInfo.fSaturation = m_pPalette->Get_SaturationFromRGB(ColorInfo.vColor);


    ColorInfo.vCursorPos = m_pPalette->Get_CursorPosFromRGB(ColorInfo.vColor);
    ColorInfo.SliderCursorPosY = m_pPalette->Get_CursorPosYFromValue(ColorInfo.fSaturation);
    ColorInfo.m_UpdateFunc = [this](void* pArg)
        {
            _float4* vColor = static_cast<_float4*>(pArg);
            m_pMainPlayer->Get_PlayerShaderDesc().m_tHairDesc.vPlayerHairColor = *vColor;
        };


    HairData.m_ColorInfos[CustomKey("HairColor", 0)] = ColorInfo;

    /* 헤어 정보 변경 */
    CustomItemGridInfo HairMeshInfo;
    HairMeshInfo.TexIdx = *m_pPlayerHair->Get_HairMeshNumPrt();
    HairMeshInfo.m_UpdateFunc = [this](void* pArg)
        {

            ////메시번호변경
            _uint* TargetIdx = static_cast<_uint*>(pArg);

            //_int* pTarget = m_pPlayerHair->Get_HairMeshNumPrt();
            //*pTarget = (int)*TargetIdx;

            m_pPlayerHair->Switch_HairMesh(*TargetIdx);
        };

    HairData.m_Meshs[CustomKey("HairMeshNum", 0)] = HairMeshInfo;

    m_CustomDatas.emplace(CUSTOMIZING_TYPE::HAIR, HairData);

    return S_OK;
}

HRESULT Client::CustomizingManager::Ready_EyeData()
{

    /*참고할대상*/
    CustomData EyeData;

    /*홍채 색1*/
    CustomColorInfo EyeBaseColorInfo;
    EyeBaseColorInfo.vColor = m_pPlayerHead->Get_PlayerEyeShaderDescPtr()->vPlayerEyeBaseColor;
    EyeBaseColorInfo.fSaturation = m_pPalette->Get_SaturationFromRGB(EyeBaseColorInfo.vColor);

    EyeBaseColorInfo.vCursorPos = m_pPalette->Get_CursorPosFromRGB(EyeBaseColorInfo.vColor);
    EyeBaseColorInfo.SliderCursorPosY = m_pPalette->Get_CursorPosYFromValue(EyeBaseColorInfo.fSaturation);
    EyeBaseColorInfo.m_UpdateFunc = [this](void* pArg)
        {
            _float4* vColor = static_cast<_float4*>(pArg);
            m_pMainPlayer->Get_PlayerShaderDesc().m_tEyeDesc.vPlayerEyeBaseColor = *vColor;
        };



    EyeData.m_ColorInfos[CustomKey("EyeBaseColor", 0)] = EyeBaseColorInfo;

    /*홍채 색2*/
    CustomColorInfo EyeAppendColorInfo;
    EyeAppendColorInfo.vColor = m_pPlayerHead->Get_PlayerEyeShaderDescPtr()->vPlayerEyeAppendColor;
    EyeAppendColorInfo.fSaturation = m_pPalette->Get_SaturationFromRGB(EyeAppendColorInfo.vColor);

    EyeAppendColorInfo.vCursorPos = m_pPalette->Get_CursorPosFromRGB(EyeAppendColorInfo.vColor);
    EyeAppendColorInfo.SliderCursorPosY = m_pPalette->Get_CursorPosYFromValue(EyeAppendColorInfo.fSaturation);
    EyeAppendColorInfo.m_UpdateFunc = [this](void* pArg)
        {
            _float4* vColor = static_cast<_float4*>(pArg);
            m_pMainPlayer->Get_PlayerShaderDesc().m_tEyeDesc.vPlayerEyeAppendColor = *vColor;
        };

    EyeData.m_ColorInfos[CustomKey("EyeAppendColor", 1)] = EyeAppendColorInfo;


    //텍스처(눈동자1텍스처/눈동자2텍스처/하이라이트텍스처)
    EyeData.m_Texs[CustomKey("EyeDetailNum", 0)] = CustomItemGridInfo(pHead_Texture_Desc->iEyeDetailNum,
        [this](void* pArg)
        {
            _uint* m_TexIdx = static_cast<_uint*>(pArg);
            if (m_TexIdx)
            {
                pHead_Texture_Desc->iEyeDetailNum = (int)*m_TexIdx;
            }
        });//눈동자 1텍스처


    EyeData.m_Texs[CustomKey("EyeWhiteNum", 1)] = CustomItemGridInfo(pHead_Texture_Desc->iEyeWhiteNum,
        [this](void* pArg)
        {
            _uint* m_TexIdx = static_cast<_uint*>(pArg);
            if (m_TexIdx)
            {
                pHead_Texture_Desc->iEyeWhiteNum = (int)*m_TexIdx;
            }
        });//눈동자2텍스처

    EyeData.m_Texs[CustomKey("EyeHighlightNum", 2)] = CustomItemGridInfo(pHead_Texture_Desc->iEyeHighlightNum,
        [this](void* pArg)
        {
            _uint* m_TexIdx = static_cast<_uint*>(pArg);
            if (m_TexIdx)
            {
                pHead_Texture_Desc->iEyeHighlightNum = (int)*m_TexIdx;
            }
        });//눈동자3텍스처


    /*하이라이트 강도*/
    EyeData.m_Values[CustomKey("EyeHighlightStrength", 0)] = CustomAlphaValueInfo(m_pPlayerHead->Get_PlayerEyeShaderDescPtr()->fPlayerEyeHighlightStrength,
        [this](void* pArg)
        {
            _float* m_fValue = static_cast<_float*>(pArg);
            if (m_fValue)
            {
                m_pMainPlayer->Get_PlayerShaderDesc().m_tEyeDesc.fPlayerEyeHighlightStrength = *m_fValue;

            }
        });


    m_CustomDatas.emplace(CUSTOMIZING_TYPE::EYE, EyeData);

    return S_OK;
}

HRESULT Client::CustomizingManager::Ready_EyebrowData()
{
    

    /*대상이 되는 partobject 가져오기*/
    Player_Head* pPlayerHead = dynamic_cast<Player_Head*>(m_pMainPlayer->Find_PartObject(TEXT("Part_Head")));
    CHECK_NULLPTR(pPlayerHead);

    /*참고할대상*/
    PLAYER_BROW_SHADER_DESC Desc = pPlayerHead->Get_PlayerBrowShaderDesc();
    HEAD_TEXTURE_CHANGE* HeadTextureChange = pPlayerHead->Get_HeadTexturePtr();
    CustomData EyeBrowData;

    /*눈썹 색깔*/
    CustomColorInfo EyeBrowColorInfo;

    EyeBrowColorInfo.vColor = Desc.vPlayerBrowColor;
    EyeBrowColorInfo.fSaturation = m_pPalette->Get_SaturationFromRGB(EyeBrowColorInfo.vColor);


    EyeBrowColorInfo.vCursorPos = m_pPalette->Get_CursorPosFromRGB(EyeBrowColorInfo.vColor);
    EyeBrowColorInfo.SliderCursorPosY = m_pPalette->Get_CursorPosYFromValue(EyeBrowColorInfo.fSaturation);
    EyeBrowColorInfo.m_UpdateFunc = [this](void* pArg)
        {
            _float4* vColor = static_cast<_float4*>(pArg);
            pPlayer_Shader_Desc->m_tBrowDesc.vPlayerBrowColor = *vColor;

        };

    EyeBrowData.m_ColorInfos[CustomKey("BrowColor",0)] = EyeBrowColorInfo;

    //텍스처
    EyeBrowData.m_Texs[CustomKey("BrowNum", 0)] = CustomItemGridInfo(pHead_Texture_Desc->iBrowNum, 
        [this](void* pArg)
        {
            _uint* m_TexIdx = static_cast<_uint*>(pArg);
            if (m_TexIdx)
            {
                pHead_Texture_Desc->iBrowNum = (int)*m_TexIdx;
            }
        });


    m_CustomDatas.emplace(CUSTOMIZING_TYPE::EYEBROW, EyeBrowData);

    return S_OK;
}


HRESULT Client::CustomizingManager::Ready_MakeUpData()
{


    /*대상이 되는 partobject 가져오기*/
    Player_Head* pPlayerHead = dynamic_cast<Player_Head*>(m_pMainPlayer->Find_PartObject(TEXT("Part_Head")));
    CHECK_NULLPTR(pPlayerHead);

    /*참고할대상*/
    PLAYER_BROW_SHADER_DESC Desc = pPlayerHead->Get_PlayerBrowShaderDesc();
    HEAD_TEXTURE_CHANGE* HeadTextureChange = pPlayerHead->Get_HeadTexturePtr();
    CustomData MakeUpData;

    /*아이라인색깔*/
    CustomColorInfo EyeLineColorInfo;

    EyeLineColorInfo.vColor = Desc.vPlayerEyeLineColor;
    EyeLineColorInfo.fSaturation = m_pPalette->Get_SaturationFromRGB(EyeLineColorInfo.vColor);


    EyeLineColorInfo.vCursorPos = m_pPalette->Get_CursorPosFromRGB(EyeLineColorInfo.vColor);
    EyeLineColorInfo.SliderCursorPosY = m_pPalette->Get_CursorPosYFromValue(EyeLineColorInfo.fSaturation);
    EyeLineColorInfo.m_UpdateFunc = [this](void* pArg)
        {
            _float4* vColor = static_cast<_float4*>(pArg);

            m_pMainPlayer->Get_PlayerShaderDesc().m_tBrowDesc.vPlayerEyeLineColor = *vColor;

           // m_pPlayerHead->Get_PlayerBrowShaderDescPtr()->vPlayerEyeLineColor = *vColor;
        };
    MakeUpData.m_ColorInfos[CustomKey("EyeLineColor", 0)] = EyeLineColorInfo;


    /*립색깔*/
    CustomColorInfo LipColorInfo;
    LipColorInfo.fSaturation = 0.f;
    LipColorInfo.SliderCursorPosY = -45.f;

    LipColorInfo.vColor = Desc.vPlayerLipColor;
    LipColorInfo.vCursorPos = m_pPalette->Get_CursorPosFromRGB(LipColorInfo.vColor);
    LipColorInfo.m_UpdateFunc = [this](void* pArg)
        {
            _float4* vColor = static_cast<_float4*>(pArg);
            m_pMainPlayer->Get_PlayerShaderDesc().m_tBrowDesc.vPlayerLipColor = *vColor;
            //m_pPlayerHead->Get_PlayerBrowShaderDescPtr()->vPlayerLipColor = *vColor;
        };

    MakeUpData.m_ColorInfos[CustomKey("LipColor", 1)] = LipColorInfo;


    //텍스처
    MakeUpData.m_Texs[CustomKey("EyelashNum", 0)] = CustomItemGridInfo(pHead_Texture_Desc->iEyelashNum,
        [this](void* pArg)
        {
            _uint* m_TexIdx = static_cast<_uint*>(pArg);
            if (m_TexIdx)
            {
                pHead_Texture_Desc->iEyelashNum = (int)*m_TexIdx;
            }
        });


    //농도값들
    MakeUpData.m_Values[CustomKey("EyeLineAlpha", 0)] = CustomAlphaValueInfo(Desc.fEyeLineAlpha,
        [this](void* pArg)
        {
            _float* m_fValue = static_cast<_float*>(pArg);
            if (m_fValue)
            {
                m_pMainPlayer->Get_PlayerShaderDesc().m_tBrowDesc.fEyeLineAlpha = *m_fValue;
 
            }
        });
    MakeUpData.m_Values[CustomKey("LipAlpha",1)] = CustomAlphaValueInfo(Desc.fLipAlpha,
        [this](void* pArg)
        {
            _float* m_fValue = static_cast<_float*>(pArg);
            if (m_fValue)
            {
                m_pMainPlayer->Get_PlayerShaderDesc().m_tBrowDesc.fLipAlpha = *m_fValue;
            }
        });
    
    m_CustomDatas.emplace(CUSTOMIZING_TYPE::MAKEUP, MakeUpData);

    return S_OK;
}

HRESULT Client::CustomizingManager::Ready_ClothesData()
{

    /*대상이 되는 partobject 가져오기*/
    Player_Body* pPlayerBody = dynamic_cast<Player_Body*>(m_pMainPlayer->Find_PartObject(TEXT("Part_Body")));
    CHECK_NULLPTR(pPlayerBody);

    /*참고할대상*/
    PLAYER_BODY_SHADER_DESC Desc = pPlayerBody->Get_PlayerBodyShaderDesc();
    _int* BodyMeshNum = pPlayerBody->Get_BodyMeshNumPtr();

    CustomData ClothesData;

    /*옷색깔들(6개)*/
    vector < vector < _float4 >> Colors = m_pPlayerBody->Get_BodyColorSetUp();


    for (int i = 0; i < 7; ++i)
    {

      
        for (int j = 0; j < 6; ++j)
        {
            int iIdx = i * 6 + j;

            string Key = "InnerColor" + to_string(iIdx);

            CustomColorInfo ClothesColorInfo;
            ClothesColorInfo.vColor = Colors[i][j];
            ClothesColorInfo.fSaturation = m_pPalette->Get_SaturationFromRGB(Colors[i][j]);

            ClothesColorInfo.SliderCursorPosY = m_pPalette->Get_CursorPosYFromValue(ClothesColorInfo.fSaturation);
            ClothesColorInfo.vCursorPos = m_pPalette->Get_CursorPosFromRGB(ClothesColorInfo.vColor);
            ClothesColorInfo.m_UpdateFunc = [this, j](void* pArg)
                {
                    _float4* vColor = static_cast<_float4*>(pArg);
                    pPlayer_Shader_Desc->m_tBodyDesc.vInnerColor[j] = *vColor;

                };
            ClothesData.m_ColorInfos[CustomKey(Key, iIdx)] = ClothesColorInfo;
        }





    }

    for (int i = 0; i < 7; ++i)
    {
        string Key = "BodyMeshNum" + to_string(i);
        ClothesData.m_Meshs[CustomKey(Key, i)] = CustomItemGridInfo(*BodyMeshNum, [this](void* pArg)
            {
                _uint* m_TexIdx = static_cast<_uint*>(pArg);
                if (m_TexIdx)
                {
                    int* pPtr = m_pPlayerBody->Get_BodyMeshNumPtr();
                    *pPtr = (int)*m_TexIdx;
                }
            });
    }


    m_CustomDatas.emplace(CUSTOMIZING_TYPE::CLOTHES, ClothesData);

    return S_OK;
}

HRESULT Client::CustomizingManager::Ready_FadcePaintData()
{
    CustomData FadePaintData;
    //텍스처
    FadePaintData.m_Texs[CustomKey("TattoPaintNum", 0)] = CustomItemGridInfo(pHead_Texture_Desc->iEyelashNum,
        [this](void* pArg)
        {
            _uint* m_TexIdx = static_cast<_uint*>(pArg);
            if (m_TexIdx)
            {
                //m_pMainPlayer->Get_PlayerShaderDesc().m_tEyeDesc.fPlayerTatooOnOff = true;
                pHead_Texture_Desc->iTattoPaintNum = (int)*m_TexIdx;
            }
        });


    FadePaintData.m_Values[CustomKey("TattoMoveY", 0)] = CustomAlphaValueInfo(m_pPlayerHead->Get_PlayerEyeShaderDesc().fPlayerTattoOffsetV,
        [this](void* pArg)
        {
            _float* m_fRatio = static_cast<_float*>(pArg);
            if (m_fRatio)
            {
                m_pMainPlayer->Get_PlayerShaderDesc().m_tEyeDesc.fPlayerTattoOffsetV = *m_fRatio;
                
            }
        });

    FadePaintData.m_Values[CustomKey("TattoMoveX", 1)] = CustomAlphaValueInfo(m_pPlayerHead->Get_PlayerEyeShaderDesc().fPlayerTattoOffsetU,
        [this](void* pArg)
        {
            _float* m_fRatio = static_cast<_float*>(pArg);
            if (m_fRatio)
            {
                m_pMainPlayer->Get_PlayerShaderDesc().m_tEyeDesc.fPlayerTattoOffsetU = *m_fRatio;

            }
        });

    FadePaintData.m_Values[CustomKey("TattoScale", 2)] = CustomAlphaValueInfo(m_pPlayerHead->Get_PlayerEyeShaderDesc().fPlayerTattoScale,
        [this](void* pArg)
        {
            _float* m_fRatio = static_cast<_float*>(pArg);
            if (m_fRatio)
            {
                m_pMainPlayer->Get_PlayerShaderDesc().m_tEyeDesc.fPlayerTattoScale = (*m_fRatio);

            }
        });
    m_CustomDatas.emplace(CUSTOMIZING_TYPE::FACEPAINT, FadePaintData);
    return S_OK;
}

HRESULT Client::CustomizingManager::Update_ColorData(_float4 vColor, _float Saturate)
{
    auto iter = m_CustomDatas.find(m_eEditingType);
    if (iter != m_CustomDatas.end())
    {
        auto& colorMap = iter->second.m_ColorInfos;
        auto colorIter = find_if(colorMap.begin(), colorMap.end(), [this](const auto& pair) {
            return pair.first.idx == m_iSelectFocusIdx;
            });

        if (colorIter != colorMap.end())
        {
            colorIter->second.vColor = vColor;
            colorIter->second.fSaturation = Saturate;

            colorIter->second.SliderCursorPosY = m_pPalette->Get_CursorPosYFromValue(Saturate);
            colorIter->second.vCursorPos = m_pPalette->Get_CursorPosFromRGB(vColor);


            COUT("업데이트정보-" << "커스터마이징타입:" << Get_CustomType_Str(m_eEditingType) << "인덱스:" << m_iSelectFocusIdx);
        }
    }

    return S_OK;
}

HRESULT Client::CustomizingManager::Update_TexData(_uint iTex)
{
    auto iter = m_CustomDatas.find(m_eEditingType);
    if (iter != m_CustomDatas.end())
    {
        auto& TexrMap = iter->second.m_Texs;
        auto TexIter = find_if(TexrMap.begin(), TexrMap.end(), [this](const auto& pair) {
            return pair.first.idx == m_iSelectFocusIdx;
            });

        if (TexIter != TexrMap.end())
        {
            TexIter->second.TexIdx= iTex;
            COUT("업데이트정보-" << "커스터마이징타입:" << Get_CustomType_Str(m_eEditingType) << "인덱스:" << m_iSelectFocusIdx);
        }
    }

    return S_OK;
}

HRESULT Client::CustomizingManager::Update_MeshData(_uint iTex)
{
    auto iter = m_CustomDatas.find(m_eEditingType);
    if (iter != m_CustomDatas.end())
    {
        auto& MeshMap = iter->second.m_Meshs;
        auto TexIter = find_if(MeshMap.begin(), MeshMap.end(), [this](const auto& pair) {
            return pair.first.idx == m_iSelectFocusIdx;
            });

        if (TexIter != MeshMap.end())
        {
            TexIter->second.TexIdx = iTex;
            COUT("업데이트정보-" << "커스터마이징타입:" << Get_CustomType_Str(m_eEditingType) << "인덱스:" << m_iSelectFocusIdx);
        }
    }

    return S_OK;
}

HRESULT Client::CustomizingManager::Update_ValueData(_float fValue)
{

    auto iter = m_CustomDatas.find(m_eEditingType);
    if (iter != m_CustomDatas.end())
    {
        auto& ValueMap = iter->second.m_Values;
        auto ValueIter = find_if(ValueMap.begin(), ValueMap.end(), [this](const auto& pair) {
            return pair.first.idx == m_iSelectFocusIdx;
            });

        if (ValueIter != ValueMap.end())
        {
            ValueIter->second.fRatio = fValue;
            COUT("업데이트정보-" << "커스터마이징타입:" << Get_CustomType_Str(m_eEditingType) << "인덱스:" << m_iSelectFocusIdx);
        }
    }

    return S_OK;
}

HRESULT Client::CustomizingManager::Update_ClothesData(_float4 vColor, _float Saturate)
{
    auto iter = m_CustomDatas.find(CUSTOMIZING_TYPE::CLOTHES);
    if (iter != m_CustomDatas.end())
    {
        auto& colorMap = iter->second.m_ColorInfos;
        auto colorIter = find_if(colorMap.begin(), colorMap.end(), [this](const auto& pair) {
            return pair.first.idx == m_iSelectClothesIdx;
            });

        if (colorIter != colorMap.end())
        {
            colorIter->second.vColor = vColor;
            colorIter->second.fSaturation = Saturate;

            colorIter->second.SliderCursorPosY = m_pPalette->Get_CursorPosYFromValue(Saturate);
            colorIter->second.vCursorPos = m_pPalette->Get_CursorPosFromRGB(vColor);


            COUT("업데이트정보-" << "커스터마이징타입:" << Get_CustomType_Str(m_eEditingType) << "인덱스:" << m_iSelectClothesIdx);
        }
    }

    return S_OK;
}


CustomColorInfo* Client::CustomizingManager::Get_InitialColor(CUSTOMIZING_TYPE eType, _int iIdx)
{
    auto iter = m_CustomDatas.find(eType);
    if (iter == m_CustomDatas.end()) return nullptr;

    // CustomData 내부의 m_ColorInfos 순회 (find_if 사용)
    auto& colorMap = iter->second.m_ColorInfos;
    auto colorIter = find_if(colorMap.begin(), colorMap.end(), [iIdx](const auto& pair) {
        return pair.first.idx == iIdx;
        });

    if (colorIter != colorMap.end())
        return &(colorIter->second);

    return nullptr;
}

CustomItemGridInfo* Client::CustomizingManager::Get_InitialTexture(CUSTOMIZING_TYPE eType, _int iIdx)
{
    auto iter = m_CustomDatas.find(eType);
    if (iter == m_CustomDatas.end()) return nullptr;

    // CustomData 내부의 m_ColorInfos 순회 (find_if 사용)
    auto& TextureMap = iter->second.m_Texs;
    auto TexIter = find_if(TextureMap.begin(), TextureMap.end(), [iIdx](const auto& pair) {
        return pair.first.idx == iIdx;
        });

    if (TexIter != TextureMap.end())
        return &(TexIter->second);

    return nullptr;
}

CustomItemGridInfo* Client::CustomizingManager::Get_InitialMesh(CUSTOMIZING_TYPE eType, _int iIdx)
{
    auto iter = m_CustomDatas.find(eType);
    if (iter == m_CustomDatas.end()) return nullptr;

    // CustomData 내부의 m_ColorInfos 순회 (find_if 사용)
    auto& MeshMap = iter->second.m_Meshs;
    auto MeshIter = find_if(MeshMap.begin(), MeshMap.end(), [iIdx](const auto& pair) {
        return pair.first.idx == iIdx;
        });

    if (MeshIter != MeshMap.end())
        return &(MeshIter->second);

    return nullptr;
}

CustomAlphaValueInfo* Client::CustomizingManager::Get_InitialValue(CUSTOMIZING_TYPE eType, _int iIdx)
{
    auto iter = m_CustomDatas.find(eType);
    if (iter == m_CustomDatas.end()) return nullptr;

    // CustomData 내부의 m_ColorInfos 순회 (find_if 사용)
    auto& ValueMap = iter->second.m_Values;
    auto ValueIter = find_if(ValueMap.begin(), ValueMap.end(), [iIdx](const auto& pair) {
        return pair.first.idx == iIdx;
        });

    if (ValueIter != ValueMap.end())
        return &(ValueIter->second);

    return nullptr;
}

void Client::CustomizingManager::Update_Clothes_Idx_Preview(_uint ClothesIdx)
{ //옷인덱스가 0이라면 0~5 인덱스로 세팅
    //옷인덱스가 1이라면
//    clothesIDx를바꾸지않는 미리보기용 함수
// 
    int iTargetIdx = ClothesIdx * 6;


    for (int i = 0; i < 6; ++i)
    {
        int Idx = iTargetIdx + i;
        UIObj_ColorSelector::ColorSelectorUIEvent Event;
        Event.iSlotIdx = i;
        Event.eType = UIObj_ColorSelector::EventType::CHANGE_CLOTHESIDX;
        Event.Idx = Idx;

        m_pGameInstance->Publish(Event);

        Connet_Player(CUSTOMIZING_TYPE::CLOTHES, CUSTOMIZING_VALUE_TYPE::COLOR, Idx);
    }
}

void Client::CustomizingManager::Update_Clothes_Idx(_uint ClothesIdx)
{
    //옷인덱스가 0이라면 0~5 인덱스로 세팅
    //옷인덱스가 1이라면
//     
    m_iClothesIdx = ClothesIdx;
    int iTargetIdx = ClothesIdx * 6 ;


    for (int i = 0; i < 6; ++i)
    {
        int Idx = iTargetIdx + i;
        UIObj_ColorSelector::ColorSelectorUIEvent Event;
        Event.iSlotIdx = i;
        Event.eType = UIObj_ColorSelector::EventType::CHANGE_CLOTHESIDX;
        Event.Idx = Idx;

        m_pGameInstance->Publish(Event);

        Connet_Player(CUSTOMIZING_TYPE::CLOTHES, CUSTOMIZING_VALUE_TYPE::COLOR, Idx);
    }

    

}

void Client::CustomizingManager::Free()
{
	__super::Free();
}

void Client::CustomizingManager::Connet_Player(CUSTOMIZING_TYPE eCustomType, CUSTOMIZING_VALUE_TYPE eCustomValueType, _uint iDataIdx, void* pData)
{
    switch (eCustomValueType)
    {
    case CUSTOMIZING_VALUE_TYPE::COLOR:
    {
        //지정할색깔을 저장
        _float4 Targetcolor = {};

        CustomColorInfo* pInfo = Get_InitialColor(eCustomType, iDataIdx);
        if (pInfo)
        {
            if (pData == nullptr)
                Targetcolor = pInfo->vColor;

            else
                Targetcolor = *static_cast<_float4*>(pData);

            pInfo->m_UpdateFunc(&Targetcolor);
        }
    }

    break;
    case CUSTOMIZING_VALUE_TYPE::TEXTURE:
    {
        _uint iIdx = 0;
        CustomItemGridInfo* pInfo = Get_InitialTexture(eCustomType, iDataIdx);
        if (pInfo)
        {
            if (pData == nullptr)
                iIdx = pInfo->TexIdx;

            else
                iIdx = *static_cast<_uint*>(pData);

            pInfo->m_UpdateFunc(&iIdx);
        }
    }
    break;

    case CUSTOMIZING_VALUE_TYPE::MESH:
    {
        _uint iIdx = 0;
        CustomItemGridInfo* pInfo = Get_InitialMesh(eCustomType, iDataIdx);
        if (pInfo)
        {
            if (pData == nullptr)
                iIdx = pInfo->TexIdx;

            else
                iIdx = *static_cast<_uint*>(pData);

            pInfo->m_UpdateFunc(&iIdx);
        }
    }
       
        break;
    case CUSTOMIZING_VALUE_TYPE::VALUE:
    {
        _float fRatio = 0.f;
        CustomAlphaValueInfo* pInfo = Get_InitialValue(eCustomType, iDataIdx);
        if (pInfo)
        {
            if (pData == nullptr)
                fRatio = pInfo->fRatio;

            else
                fRatio = *static_cast<_float*>(pData);

            pInfo->m_UpdateFunc(&fRatio);
        }
    }

    break;

    }
}




#ifdef _DEBUG
//void Client::CustomizingManager::Debug_Current()
//{
//    string strType = Get_CustomType_Str(m_eFocusType);
//    COUT("현재 focus type:" << strType << "\nFocusIdx:" << m_iFocusIdx << "\n");
//}

#endif // _DEBUG

