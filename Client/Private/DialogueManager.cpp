#include "Client_Define.h"
#include "DialogueManager.h"
#include "GameInstance.h"
#include "UIObj_NpcDialogue.h"
#include "UIObj_Cursor.h"
#include "NPC.h"

#include "UIObj_Npc_ChoiceMenu.h"
#include "UIObj_Focus_NPCMenu.h"
#include "UIObj_Cursor.h"




IMPLEMENT_SINGLETON(DialogueManager);

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////

Client::DialogueManager::DialogueManager()
{
}

Client::DialogueManager::~DialogueManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/
HRESULT Client::DialogueManager::Initialize()
{
    m_pGameInstance = GameInstance::GetInstance();
    return S_OK;

}

NPCData* Client::DialogueManager::Get_DialogueData(const string& jsonPath)
{
    /*검색해서 있으면 캐싱한거 반환.*/
    size_t FindHash = hash<string>{}(jsonPath);
    auto iter = m_DialogueCache.find(FindHash);
    if (iter != m_DialogueCache.end())
        return iter->second;


    /*없으면. 새로읽기*/
    NPCData* pNewData = Load_And_Parse(jsonPath);
    if (pNewData)
    {
        m_DialogueCache[FindHash] = pNewData;
    }
    return pNewData;

}

void Client::DialogueManager::Start_Dialogue(NPC* npc, NPCData* Desc)
{
    CHECK_JUST_NULL(npc);
    CHECK_JUST_NULL(Desc);

    m_pTargetNPC = npc;
    m_TargetNPCData = Desc;
    m_iCurrentStep = 0;
    m_bIsDialogueActive = true;

    /*NPCMENU 먼저 연결해주기*/


    //INPUT_LOCK_EVENT InputEvent;
    //InputEvent.bLock = true;
    //m_pGameInstance->Publish(InputEvent);



    ////커서끄기
    //UIObj_Cursor::CursorEvent cursorEvent;
    //cursorEvent.bEnable = false;
    //m_pGameInstance->Publish(cursorEvent);


    UIObj_Npc_ChoiceMenu::NPCMENUEVENT MenuEvent;
    MenuEvent.eType = UIObj_Npc_ChoiceMenu::NPCMENUEVENTTYPE::INITIALIZE;
    MenuEvent.m_pTarget = npc;
    MenuEvent.TargetOffSet = Desc->OffSet;
    m_pGameInstance->Publish(MenuEvent);


    //첫 대화부터시작.
    Move_To_Step(0);
}

void Client::DialogueManager::Next_Dialogue()
{
    if (m_eState == DIALOGUE_STATE::TEXT)
    {
        auto& line = m_TargetNPCData->dialogueMap[m_iCurrentStep];
        //이번 step안에 선택지가 있다면 상태변경, UI띄우기.
        if (!line.vecChoices.empty())
        {
            m_eState = DIALOGUE_STATE::CHOICE;
            Publish_Active_FocusMenu_UI(line.vecChoices);
            //Move_To_Step(-1);
        }

        else
        {
            //선택지가없으면? 그냥 다음대사 호출..
            Move_To_Step(m_iCurrentStep + 1);
        }
            
    }
}

void Client::DialogueManager::Move_To_Step(int iNextStep)
{
    //-1이면 대화종료를 의미
    if (iNextStep == -1)
    {
        Exit_Dialogue();
        return;
    }


    m_iCurrentStep = iNextStep;
    m_eState = DIALOGUE_STATE::TEXT;

   
    auto& line = m_TargetNPCData->dialogueMap[m_iCurrentStep];

    //NPC에게 대사전달하는 UI이벤트 작성
    Publish_DialogueText_UI(m_TargetNPCData->npcName,line.wstrText);
   

}

void Client::DialogueManager::Select_Choice(int iChoiceIdx)
{
    CHECK_TRUE(m_eState != DIALOGUE_STATE::CHOICE);
    CHECK_TRUE(m_pTargetNPC == nullptr);

    auto& choice = m_TargetNPCData->dialogueMap[m_iCurrentStep].vecChoices[iChoiceIdx];

    //액션수행..(수행할 액션이있다면 true를반환한다)
    if (m_pTargetNPC->DoAction(choice.action))
        return;



    //만약 액션이없다면,, 다음step이동( 보통 ui띄우면 다음은 -1임)
    Move_To_Step(choice.nextStep);

}

void Client::DialogueManager::Exit_Dialogue()
{
    m_eState = DIALOGUE_STATE::END;

    CHECK_JUST_NULL(m_pTargetNPC);
    m_pTargetNPC->Exit_Interaction(m_pGameInstance->Get_Player());

    Close_Dialogue();
    Close_SelectionMenu();


    //플레이어입력 다시풀기
    INPUT_LOCK_EVENT InputEvent;
    InputEvent.bLock = false;
    m_pGameInstance->Publish(InputEvent);
}

void Client::DialogueManager::Close_Dialogue()
{
    UIObj_NpcDialogue::DialogueUIEvent Event;
    Event.eType = UIObj_NpcDialogue::DIALOGUEUIEVENT_TYPE::EXIT;
    m_pGameInstance->Publish(Event);
}

void Client::DialogueManager::Close_SelectionMenu()
{
    m_pGameInstance->Close_Window();

}

void Client::DialogueManager::Publish_DialogueText_UI(wstring npcName, wstring Text)
{

    UIObj_NpcDialogue::DialogueUIEvent Event;
    Event.eType = UIObj_NpcDialogue::DIALOGUEUIEVENT_TYPE::UPDATE_NAME;
    Event.m_Text = npcName;
    m_pGameInstance->Publish(Event);


    Event.eType = UIObj_NpcDialogue::DIALOGUEUIEVENT_TYPE::UPDATE_DESC;
    Event.m_Text = Text;
    m_pGameInstance->Publish(Event);
}

void Client::DialogueManager::Publish_Active_FocusMenu_UI(vector<Choice> _vecChoices)
{
    //대화창이 나왔으면,,
        //플레이어입력을 막자.
    INPUT_LOCK_EVENT LockEvent;
    LockEvent.bLock = true;
    m_pGameInstance->Publish(LockEvent);

    //마우스커서 보여라!
    UIObj_Cursor::CursorEvent cursorEvent;
    cursorEvent.bEnable = true;
    m_pGameInstance->Publish(cursorEvent);



    // 마우스 락 풀어
    MouseLockEvent MouseEvent;
    MouseEvent.bLock = false;
    m_pGameInstance->Publish(MouseEvent);

    
    //원래있떤 대화창꺼주고.선택?으로나둘까
    UIObj_NpcDialogue::DialogueUIEvent Event;
    Event.eType = UIObj_NpcDialogue::DIALOGUEUIEVENT_TYPE::EXIT;
    m_pGameInstance->Publish(Event);


    //NPc포커스메뉴들에게 text갱신해주고 idx갱신?
    //NPC메뉴활성화.
    UI_MasterEvent MasterEvent;
    MasterEvent.m_ActionName = "OpenWindow";
    MasterEvent.m_bFlag = true;
    MasterEvent.m_Text = L"NPC_Menu";
    MasterEvent.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
    m_pGameInstance->Publish(MasterEvent);


    //NPC포커스 메뉴한테 선택지 텍스트 전달
    for (int i = 0; i < _vecChoices.size(); ++i)
    {
        UIObj_Focus_NPCMenu::FocusNPCMenuUIEvent FocusEvent;
        FocusEvent.eType = UIObj_Focus_NPCMenu::FocusNPCMenuEventType::UPDATE_TEXT;
        FocusEvent.m_iIdx = i;
        FocusEvent.Text = _vecChoices[i].text;
        m_pGameInstance->Publish(FocusEvent);
    }


    //이때여기서 카메라연출도시작하면될듯
    m_pTargetNPC->Start_NPCCamera();

}

NPCData* Client::DialogueManager::Load_And_Parse(const string& jsonPath)
{
    ifstream file(jsonPath);
    if (!file.is_open())
        return nullptr;


    json j;
    file >> j;

    NPCData* pData = new NPCData();
    pData->npcName = stringToWstring(j["Name"]);
    if (j.contains("OffSet") && j["OffSet"].is_array())
    {
        pData->OffSet.x = j["OffSet"][0];
        pData->OffSet.y = j["OffSet"][1];
        pData->OffSet.z = j["OffSet"][2];
    }
    
    for (auto& item : j["Dialogues"]) {
        DialogueLine line;
        line.step = item["Step"];
        line.wstrText = stringToWstring(item["Text"]);

        for (auto& c : item["Choices"]) {
            Choice choice;
            choice.text = stringToWstring(c["Text"]);
            choice.nextStep = c["NextStep"];
            choice.action = c.value("Action", "NONE"); // 없으면 NONE
            line.vecChoices.push_back(choice);
        }
        pData->dialogueMap[line.step] = line;
    }
    return pData;
}

void Client::DialogueManager::Free()
{
    for (auto& pair : m_DialogueCache)
    {
        if (pair.second)
            Safe_Delete(pair.second);

    }

    m_DialogueCache.clear();

}
