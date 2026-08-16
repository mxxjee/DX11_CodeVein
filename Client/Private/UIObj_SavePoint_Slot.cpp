#include "Client_Define.h"
#include "UIObj_SavePoint_Slot.h"
#include "UIObj_FadeScreen.h"
#include "InteractionManager.h"
#include "Character.h"
#include "IInteractable.h"
#include "UISoundUtil.h"


Client::UIObj_SavePoint_Slot::UIObj_SavePoint_Slot()
{
}

Client::UIObj_SavePoint_Slot::UIObj_SavePoint_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObj_GlowButton(pDevice,pContext)
{
}

Client::UIObj_SavePoint_Slot::UIObj_SavePoint_Slot(const UIObj_SavePoint_Slot& original)
    :UIObj_GlowButton(original)
{
}

Client::UIObj_SavePoint_Slot::~UIObj_SavePoint_Slot()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_SavePoint_Slot::Initialize_Prototype(_uint iLevel)
{
    __super::Initialize_Prototype(iLevel);
    return S_OK;
}

HRESULT Client::UIObj_SavePoint_Slot::Initialize(void* arg)
{
    __super::Initialize(arg);
    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/

UIObj_SavePoint_Slot* Client::UIObj_SavePoint_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_SavePoint_Slot* pInstance = new UIObj_SavePoint_Slot(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_SavePoint_Slot 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
   
}
GameObject* Client::UIObj_SavePoint_Slot::Clone(void* pArg)
{
    UIObj_SavePoint_Slot* pInstance = new UIObj_SavePoint_Slot(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_SavePoint_Slot 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_SavePoint_Slot::After_ApplyData()
{
    //자식캐싱
    __super::After_ApplyData();

    if (m_UIType == "LEVEL_MAIN")
        m_eLevel = LEVEL::MAIN;

    if (m_UIType == "LEVEL_SAMPLE")
        m_eLevel = LEVEL::SAMPLE;

    if (m_UIType == "LEVEL_BASE")
        m_eLevel = LEVEL::BASE;

    if (m_UIType == "LEVEL_CHURCH")
        m_eLevel = LEVEL::CHURCH;

  
}

void Client::UIObj_SavePoint_Slot::Execute_By_Event(const string& strActionName, void* pArg)
{
    __super::Execute_By_Event(strActionName, pArg);

    UI_MasterEvent* pMasterEvent = static_cast<UI_MasterEvent*>(pArg);

    //이 슬롯이 점유하고있는 씬과 세이브포인트로 이동한다.
     if (strActionName == "SceneChange")
     {
         PlayClickSound();

         //씬이동 플래그 활성화
         InteractionManager::GetInstance()->Set_SceneChanging(true);

         PlayerCheckPointEvnet CheckPointEvent;
         CheckPointEvent.eCheckPointType = CheckPointEventType::START;
         CheckPointEvent.iLevel = _UINT(m_eLevel);
         CheckPointEvent.iIdx = m_iIdx;
         m_pGameInstance->Publish(CheckPointEvent);


         //이 창은이제끄자..
         m_pGameInstance->Close_All_Menu();//여ㅑ기서
        // m_pGameInstance->Close_Window();

       //  CHECK_JUST_NULL(m_SaveInfo);
       //  LEVEL CurrentLevel = LEVEL(m_pGameInstance->Get_Current_LevelID());

       //  //여기서 플레이어한테 이벤트 애니메이션 재생을 시켜 Start를 이제 Start가 끝나면 
       //  // playerSavePointEVFent Event
       //  // Event.ilevel=이동할레벨
       //  // Event.idx= 여기서몇번쨰 버튼인지
       //  

       //  //void  Start_AFter()
       //  // {
       //  // SAvepoint* point = InteractionManager::GetInstance()->Get_SavePointInfo(m_eLevel, m_iIdx);
       //  // 
       //  // }
       //  //FadeIn을 시작하면서 씬전환이 되는거겠지

       //  //현재레벨과 다른 씬으로 이동하려면..
       //  if (CurrentLevel != m_eLevel)
       //  {
       //      wstring Text = pMasterEvent->m_Text;
       //      UIObj_FadeScreen::FadeScreenEvent Event;
       //      Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
       //      Event.m_iNextLevel = m_eLevel;
       //      m_pGameInstance->Publish(Event);

       //      
       //  }

       //  //같은씬이라면, 단지 텔레포트
       //  else
       //  {
       //      //마지막 위치랑 현재 누른애랑 같으면 이동X(뭐야 처리했는데 게임에선 가지넴..)
       ///*      SAVE_POINT_INFO* pLastSavePointData = InteractionManager::GetInstance()->Get_LastSavePointInfo();
       //      if (pLastSavePointData)
       //      {
       //          if (pLastSavePointData->iIndex == m_iIdx)
       //              return;
       //      }*/

       //      wstring Text = pMasterEvent->m_Text;
       //      UIObj_FadeScreen::FadeScreenEvent Event;
       //      Event.eType = UIObj_FadeScreen::FadeScreenEventType::AUTO_FADE;
       //      Event.m_iNextLevel = m_eLevel;
       //      Event.m_fSecond = 2.f;
       //      Event.m_EndFunc = [CurrentLevel,this]()
       //          {

       //              GameObject* pPlayer = m_pGameInstance->Get_Player();
       //              Character* pCharacter = dynamic_cast<Character*>(pPlayer);
       //              if (pCharacter)
       //              {
       //                  pCharacter->Teleport(m_SaveInfo->spawnPosition,m_SaveInfo->spawnRotation);
       //                  m_pGameInstance->Close_All_Menu();

       //                  INPUT_LOCK_EVENT LockEvent;
       //                  LockEvent.bLock = false;
       //                  m_pGameInstance->Publish(LockEvent);

       //                  IInteractable* pObj = InteractionManager::GetInstance()->Get_Current_Interaction_Target();
       //                  if (pObj)
       //                      pObj->Exit_Interaction(pPlayer);
       //            
       //                  //각씬에 맞도록 창 되돌리기
       //                  switch (CurrentLevel)
       //                  {
       //                  case LEVEL::BASE:
       //                      m_pGameInstance->Change_UIMode(UI_MODE::BASE);
       //                      break;

       //                  default:
       //                      m_pGameInstance->Change_UIMode(UI_MODE::DEFAULT);
       //                      break;
       //                  }           
       //              }

       //          };
       //      m_pGameInstance->Publish(Event);
       //  }
       
         InteractionManager::GetInstance()->Set_LastSavePointInfo(m_eLevel, m_iIdx);
    }

}

void        Client::UIObj_SavePoint_Slot::Set_Idx(_uint i)
{
    m_iIdx = i;
    m_SaveInfo = InteractionManager::GetInstance()->Get_SavePointInfo(m_eLevel, m_iIdx);

}
void Client::UIObj_SavePoint_Slot::Free()
{
    __super::Free();
}
