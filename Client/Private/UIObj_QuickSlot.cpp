#include "Client_Define.h"
#include "UIObj_QuickSlot.h"
#include "UIObj_SlotGrid.h"
#include "InventoryManager.h"
#include "UI_Progress.h"




Client::UIObj_QuickSlot::UIObj_QuickSlot()
{
}

Client::UIObj_QuickSlot::UIObj_QuickSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObj_Slot(pDevice,pContext)
{
}

Client::UIObj_QuickSlot::UIObj_QuickSlot(const UIObj_QuickSlot& original)
    :UIObj_Slot(original)
{
}

Client::UIObj_QuickSlot::~UIObj_QuickSlot()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_QuickSlot::Initialize_Prototype(_uint iLevel)
{
    CHECK_FAILED(__super::Initialize_Prototype(iLevel), E_FAIL);
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_QuickSlot::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);


    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<UIObj_QuickSlot::QuickSlotEvent>([this](const UIObj_QuickSlot::QuickSlotEvent& Event)
    {
            if (m_pParentGrid)
            {

               

                switch (Event.m_eEventType)
                {
                case QuickSlotEventType::EQUIP:
                {
                    CHECK_TRUE(_UINT(m_eQuickSlotType) != Event.TargetSlotGrid);
                    CHECK_TRUE(m_iSlotIdx != Event.iIdx);

                    /*스킬정보 세팅*/
                    m_pSkill = Event.m_pSkillBase;
                    
                    Equip(Event.m_pItemInfo);
                }
                break;

                case QuickSlotEventType::START_ACTIVE:
                {
                    //해당되는 스킬의 슬롯만 해라!
                    CHECK_TRUE(Event.m_pSkillBase != m_pSkill);
                    Change_State(ESlotState::ACTIVE);
                }
                break;

                /*플레이어에게 쿨타임이벤트 시작 받아서처리*/
                case QuickSlotEventType::START_COOLDOWN:
                {
                    //해당되는 스킬의 슬롯만 해라!
                    CHECK_TRUE(Event.m_pSkillBase != m_pSkill);
                    Change_State(ESlotState::COOLDOWN);

                }
                break;


				case QuickSlotEventType::END_COOLTIME:
				{
                    CHECK_JUST_NULL(m_pEquipItemInfo);
                    CHECK_JUST_NULL(Event.m_pSkillBase);

                    CHECK_TRUE(m_pSkill != Event.m_pSkillBase);
                    Change_State(ESlotState::COOLDOWN_END);

                 
                    
				}
              break;

                case QuickSlotEventType::WEAPON_CHANGE :
                {
                    //모든슬롯이 처리함
                    Change_Color_By_WeaponType();
                }
                break;


                case QuickSlotEventType::CANCEL_ACTIVE:
                {
                    CHECK_JUST_NULL(m_pEquipItemInfo);
                    CHECK_TRUE(m_pSkill != Event.m_pSkillBase);
                    Change_State(ESlotState::CANCLE_ACTIVE);
                }
                break;
                }
        
               

            }
    }));


    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////



_int Client::UIObj_QuickSlot::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_QuickSlot::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    /////슬롯상태가 READY일때만 입력가능!!
    ////근데 현재는 디버깅을 위해 키누를때 토글처리함
    ///*if (m_eSlotState == ESlotState::READY && m_bEquip)
    //{*/
    //    if (m_pGameInstance->KeyDown(m_bActionKey))
    //    {
    //        /*여기가 스킬슬롯에 연결된 키를 입력하는순간*/
    //        {
    //            //스킬이껴져있을때만 이벤트발송..
    //            if (m_bEquip)
    //            {
    //                //여기서 이벤트쏘면됨..근데 여기는 메인슬롯판단이안되므로 이런식으로처리함
    //                PlayerSkillInfo SkillOnPressedEvent;
    //                SkillOnPressedEvent.idx = (m_eQuickSlotType == MAINUISLOT::SKILL_1) ? m_iSlotIdx : m_iSlotIdx + 4;
    //                SkillOnPressedEvent.m_ItemInfo = m_pEquipItemInfo;
    //                m_pGameInstance->Publish(SkillOnPressedEvent);


    //              /*  if (m_eQuickSlotType == MAINUISLOT::SKILL_1)
    //                    WCOUT("Idx:" << m_iSlotIdx<<"Name:"<<m_pEquipItemInfo->ItemName);

    //                if (m_eQuickSlotType == MAINUISLOT::SKILL_2)
    //                    WCOUT("Idx:" << m_iSlotIdx+4 << "Name:" << m_pEquipItemInfo->ItemName);*/

    //            }
    //            
    //        }
    //        if (m_eSlotState == ESlotState::READY && m_bEquip)
    //        {
    //            //플레이어가 낀 무기랑 현재 발동하려는 무기타입이 같아야만 change_state가능

    //            Change_State(ESlotState::ACTIVE);
    //        }
    //           

    //        /*이벤트 받아서 쿨다운처리하기!*/
    //    /*    else if (m_eSlotState == ESlotState::ACTIVE && m_bEquip)
    //            Change_State(ESlotState::COOLDOWN);*/
    //    }

    //}

    return 0;
}

_int Client::UIObj_QuickSlot::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    Update_State(fTimeDelta);


    return 0;
}

HRESULT Client::UIObj_QuickSlot::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

void Client::UIObj_QuickSlot::Free()
{
    __super::Free();
}

UIObj_QuickSlot* Client::UIObj_QuickSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_QuickSlot* pInstance = new UIObj_QuickSlot(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_QuickSlot 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_QuickSlot::Clone(void* pArg)
{
    UIObj_QuickSlot* pInstance = new UIObj_QuickSlot(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_QuickSlot 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_QuickSlot::Change_State(ESlotState eNewState)
{
    if (eNewState != m_eSlotState)
    {

        m_ePreSlotState = m_eSlotState;
        m_eSlotState = eNewState;
        Apply_State_UI(eNewState);

        
       
    }
}

void Client::UIObj_QuickSlot::Exit_State(ESlotState ePrevState)
{
}

void Client::UIObj_QuickSlot::Update_State(const _float fTimeDelta)
{
    CHECK_JUST_NULL(m_pEquipItemInfo);
    CHECK_JUST_NULL(m_pSkill);

    switch (m_eSlotState)
    {
    case Client::UIObj_QuickSlot::ESlotState::READY:

        break;
    case Client::UIObj_QuickSlot::ESlotState::ACTIVE:
        if (m_pBack_Progress)
        {
            _float fRatio = m_pSkill->Get_SkillDurationRatio();
            if (fRatio == 0)
                fRatio = 1;
            m_pBack_Progress->Set_Ratio(fRatio);
            m_pBack_Progress->Set_TargetRatio(fRatio);
            m_pBack_Progress->Set_RenderRato(fRatio);
        }
        break;
    case Client::UIObj_QuickSlot::ESlotState::COOLDOWN:
    {
        if (m_pSlotFill_Progress)
        {
            _float fRatio = m_pSkill->Get_CoolTimeRatio();
            m_pSlotFill_Progress->Set_Ratio(fRatio);
            m_pSlotFill_Progress->Set_TargetRatio(fRatio);
            m_pSlotFill_Progress->Set_RenderRato(fRatio);
        }
    }
        break;

    case Client::UIObj_QuickSlot::ESlotState::COOLDOWN_END:
    {
        m_fEndTime += fTimeDelta;
        if (m_fEndTime >= 1.f)
        {
            m_fEndTime = 0.f;
            Change_State(ESlotState::READY);
       }
    }
    break;

    default:
        break;
    }
}

void Client::UIObj_QuickSlot::Set_BackImgColor(_float4 vColor)
{
    Component* pImage = m_pBackImg->Get_Component_FromName(Proto_UIImage);
    if (pImage)
    {
        UI_Image* ppImage = dynamic_cast<UI_Image*>(pImage);
        if (ppImage)
            ppImage->Set_Color(vColor);

    }

}

void Client::UIObj_QuickSlot::Set_IconImgColor(_float4 vColor)
{
    Component* pImage = m_pIcon->Get_Component_FromName(Proto_UIImage);
    if (m_pIcon)
    {
        UI_Image* ppImage = dynamic_cast<UI_Image*>(pImage);
        if (ppImage)
            ppImage->Set_Color(vColor);

    }

    if (m_pBack_Progress)
    {
        m_pBack_Progress->Set_RenderRato(1.f);
        m_pBack_Progress->Set_Ratio(1.f);
        m_pBack_Progress->Set_TargetRatio(1.f);
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT Client::UIObj_QuickSlot::Equip(ItemInfo* pInfo)
{

    if (FAILED(__super::Equip(pInfo)))
        return E_FAIL;

    //색변경
    CHECK_NULLPTR(m_pBackImg);

    //wepaontype과 비교해서 색깔처리
    _bool bActiveSkill = m_pSkill->Can_Active_Skill();
    if (bActiveSkill)
    {
        vTargetColor_Slot = ACTIVE_SLOT_COLOR;
        vTargetColor_Icon = ACTIVE_ICON_COLOR;
      
    }
    else
    {
        vTargetColor_Slot = DEACTIVE_SLOT_COLOR;
        vTargetColor_Icon = DEACTIVE_ICON_COLOR;
    }

    Set_BackImgColor(vTargetColor_Slot);
    Set_IconImgColor(vTargetColor_Icon);

    return S_OK;

}
/******************************************************* 객체 준비 함수 *******************************************************/

void        Client::UIObj_QuickSlot::After_ApplyData()
{

    __super::After_ApplyData();
    if (m_wstrName.find(L"QuickSlot_1") != wstring::npos)
    {
        m_eQuickSlotType = MAINUISLOT::SKILL_1;
      

    }

    else
    {
        m_eQuickSlotType = MAINUISLOT::SKILL_2;
       
    }
    UIObject* pImg = Get_Child(L"Slot_Img");
    if (pImg)
    {
        m_pBackImg = pImg;
        Component* pComp = m_pBackImg->Get_Component_FromName(Proto_UIProgress);
        m_pBack_Progress = dynamic_cast<UI_Progress*>(pComp);

    }

    UIObject* pSlotFill = Get_Child(L"Slot_Fill");
    if (pSlotFill)
    {
        m_pSlot_Fill = pSlotFill;

        Component* pComp = m_pSlot_Fill->Get_Component_FromName(Proto_UIProgress);
        if (pComp)
            m_pSlotFill_Progress = dynamic_cast<UI_Progress*>(pComp);

    }

    UIObject* pEffect1 = Get_Child(L"Active_Effect1");
    if (pEffect1)
    {
        m_pEffect1 = pEffect1;
    }


    UIObject* pEffectBlur = Get_Child(L"Acitve_Effect_Blur");
    if (pEffectBlur)
    {
        m_pEffect_Blur = pEffectBlur;
    }

    Change_State(ESlotState::READY);



}

void Client::UIObj_QuickSlot::Apply_State_UI(ESlotState eState)
{
    switch (eState)
    {
    case ESlotState::READY:
    {
        /*m_pEffect1->Stop_Animation();
        m_pEffect1->Set_Visible(false);
        m_pEffect1->Set_Active_Force(false);


        m_pEffect_Blur->Stop_Animation();
        m_pEffect_Blur->Set_Visible(false);
        m_pEffect_Blur->Set_Active_Force(false);*/


        if (m_pBack_Progress)
        {


            m_pBack_Progress->Set_RenderRato(1.f);
            m_pBack_Progress->Set_Ratio(1.f);
            m_pBack_Progress->Set_TargetRatio(1.f);
        }

        if (m_pSlot_Fill)
            m_pSlot_Fill->Set_Active(false);

        if (m_pEffect1)
        {
            m_pEffect1->Stop_Animation();
            m_pEffect1->Set_Visible(false);
            m_pEffect1->Set_Active_Force(false);
        }
            


        if (m_pEffect_Blur)
        {
            m_pEffect_Blur->Stop_Animation();
            m_pEffect_Blur->Set_Visible(false);
            m_pEffect_Blur->Set_Active_Force(false);
        }


    }
    break;

    case ESlotState::ACTIVE:
    {

	

        Set_BackImgColor(vTargetColor_Slot);
        if (m_pBackImg)
            m_pBackImg->Play_Animation(SkillUseAnimKey);

        if (m_pBack_Progress)
        {

			_float fRatio = m_pSkill->Get_SkillDurationRatio();
            m_pBack_Progress->Set_TargetRatio(fRatio);
            m_pBack_Progress->Set_RenderRato(fRatio);

        }

        if (m_pEffect1)
        {
            m_pEffect1->Stop_Animation();
            m_pEffect1->Set_Visible(false);
            m_pEffect1->Set_Active_Force(false);
        }



        if (m_pEffect_Blur)
        {
            m_pEffect_Blur->Stop_Animation();
            m_pEffect_Blur->Set_Visible(false);
            m_pEffect_Blur->Set_Active_Force(false);
        }
    }
    break;

    case ESlotState::COOLDOWN:
    {
       


        //진행중인 m_pBackImage의 색상을 원래대로바꾼다.
        //애니메이션멈추기
        Set_IconImgColor(_float4(0.5f, 0.5f, 0.5f, 1.f));
        Set_BackImgColor(_float4(0.f, 0.f, 0.f, 0.5f));
        m_pBackImg->Stop_Animation();

        //fill 쿨다운 표시
        if (m_pSlot_Fill)
        {
            m_pSlot_Fill->Set_Visible(true);
            m_pSlot_Fill->Set_Active(true);
            if (m_pSlotFill_Progress)
            {
                _float fRatio = m_pSkill->Get_CoolTimeRatio();
                m_pSlotFill_Progress->Set_TargetRatio(fRatio);
                m_pSlotFill_Progress->Set_RenderRato(fRatio);
            }
        }

        if (m_pEffect1)
        {
            m_pEffect1->Stop_Animation();
            m_pEffect1->Set_Visible(false);
            m_pEffect1->Set_Active_Force(false);
        }



        if (m_pEffect_Blur)
        {
            m_pEffect_Blur->Stop_Animation();
            m_pEffect_Blur->Set_Visible(false);
            m_pEffect_Blur->Set_Active_Force(false);
        }
    }
    break;

    case ESlotState::COOLDOWN_END:
    {
        if (m_pEffect1)
        {
            //내가켜져있으면실행
            if (m_bIsActive)
            {
                
				m_pEffect1->Set_Active(true);
				m_pEffect1->Play_Animation(SkillReactiveAnimKey);

                if (m_pEffect_Blur)
                {
                    m_pEffect_Blur->Set_Visible(true);
                    m_pEffect_Blur->Set_Active(true);
                    string Key = "OnReactive";
                    m_pEffect_Blur->Play_Animation(Key);
                }
            }

            else
            {
                m_pEffect1->Stop_Animation();
                m_pEffect1->Set_Visible(false);
                m_pEffect1->Set_Active_Force(false);

                m_pEffect_Blur->Stop_Animation();
                m_pEffect_Blur->Set_Visible(false);
                m_pEffect_Blur->Set_Active_Force(false);
            }

        }

        //끝났으니까 이펙트활성화.및 색돌리기
        Set_IconImgColor(vTargetColor_Icon);
        Set_BackImgColor(vTargetColor_Slot);

        m_pSlot_Fill->Set_Visible(false);
        m_pSlot_Fill->Set_Active(false);
      
    }
    break;

    case ESlotState::CANCLE_ACTIVE:
    {
   
        if (m_pBack_Progress)
        {


            m_pBack_Progress->Set_RenderRato(1.f);
            m_pBack_Progress->Set_Ratio(1.f);
            m_pBack_Progress->Set_TargetRatio(1.f);

            m_pBackImg->Stop_Animation();

        }

        Set_IconImgColor(vTargetColor_Icon);
        Set_BackImgColor(vTargetColor_Slot);

        m_pSlot_Fill->Set_Visible(false);
        m_pSlot_Fill->Set_Active(false);

        Change_State(ESlotState::READY);

    }
    break;

    }
}

void Client::UIObj_QuickSlot::Change_Color_By_WeaponType()
{
    CHECK_JUST_NULL(m_pSkill);

   
    /*현재 타겟웨폰이랑 지금 저장된 스킬에 허용되는 타입인지확인*/
    _bool bActiveSkill = m_pSkill->Can_Active_Skill();
    if (bActiveSkill)
    {
        vTargetColor_Slot = ACTIVE_SLOT_COLOR;
        vTargetColor_Icon = ACTIVE_ICON_COLOR;

    }
    else
    {
        vTargetColor_Slot = DEACTIVE_SLOT_COLOR;
        vTargetColor_Icon = DEACTIVE_ICON_COLOR;
    }

    CHECK_TRUE(m_eSlotState == ESlotState::COOLDOWN);//쿨타운중에는 바꾸지마라.알아서 끝나고바꿀끼다


    Set_BackImgColor(vTargetColor_Slot);
    Set_IconImgColor(vTargetColor_Icon);
}

void    Client::UIObj_QuickSlot::Ready_Slot()
{
    if (m_eQuickSlotType == MAINUISLOT::SKILL_1)
    {
        switch (m_iSlotIdx)
        {
        case 0:
            m_bActionKey = DIK_1;
            break;

        case 1:
            m_bActionKey = DIK_2;
            break;

        case 2:
            m_bActionKey = DIK_3;
            break;

        case 3:
            m_bActionKey = DIK_4;
            break;


        }
    }

    else
    {
        switch (m_iSlotIdx)
        {
        case 0:
            m_bActionKey = DIK_5;
            break;

        case 1:
            m_bActionKey = DIK_6;
            break;

        case 2:
            m_bActionKey = DIK_7;
            break;

        case 3:
            m_bActionKey = DIK_8;
            break;


        }
    }
}

void Client::UIObj_QuickSlot::Set_Active(_bool _isActive)
{
    __super::Set_Active(_isActive);

    //활성화되었을때 할일..(상태갱신)
    Apply_State_UI(m_eSlotState);

    if (m_pEffect_Blur)
    {
        if (m_pEffect_Blur->Is_Active())
        {
            if (m_eSlotState != ESlotState::COOLDOWN_END)
            {
                //왜안꺼져.. 강제적으로꺼주기
                m_pEffect_Blur->Stop_Animation();
                m_pEffect_Blur->Set_Alpha(0.f);
                m_pEffect_Blur->Set_Active_Force(false);
                m_pEffect_Blur->Set_Visible(false);

                m_pEffect1->Stop_Animation();
                m_pEffect1->Set_Alpha(0.f);
                m_pEffect1->Set_Active_Force(false);
                m_pEffect1->Set_Visible(false);

            }
        }
    }

}
