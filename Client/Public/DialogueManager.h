#pragma once
#include "Base.h"

//NPC의 대화 관련된 함수정의, NPC들은 각자 desc를 소유하고있고
//Interaction이 들어올 경우 이 매니저의 함수를 호출한다.

//NPC대화 이후 e를 누르면 같은 step안에서 UI선택지를 띄울수있는 구조임
namespace Engine
{
    class GameInstance;

}
NS_BEGIN(Client)
class NPC;
class DialogueManager final:
    public Base
{
    DECLARE_SINGLETON(DialogueManager);

private:
    explicit DialogueManager();
    virtual ~DialogueManager();
    
public:
    HRESULT         Initialize();

public:
            //각 npc들 initliaze()시 자신의 dialogue경로넣어주기
    NPCData* Get_DialogueData(const string& jsonPath);


/////////////////////////대화 관련//////////////////////////////////
    /// 대화시작할때 호출 (대화창 UI이벤트//////////////////
    void    Start_Dialogue(NPC* npc, NPCData* Desc);

    void    Next_Dialogue();


            //각 메뉴의 버튼을 누르면 호출
    void    Select_Choice(int iChoiceIdx);
                //대화 완전 종료
    void    Exit_Dialogue();

    void    Close_Dialogue();   //대화창닫기
    void    Close_SelectionMenu();  //선택지 닫기


private:
    //내부적으로 다음대화로 넘어갈때호출
    void    Move_To_Step(int iNextStep);
    /////////////////////////대화 관련//////////////////////////////////

    /////////////////////UI이벤트 쏴주는함수들////////////////////////////
private:
    void        Publish_DialogueText_UI(wstring npcName,wstring Text);
    void        Publish_Active_FocusMenu_UI(vector<Choice>  _vecChoices);

    /////////////////////UI이벤트 쏴주는함수들////////////////////////////
private:
                    //json로딩
    NPCData* Load_And_Parse(const string& jsonPath);

public:
    virtual void        Free() override;

private:
    NPC*        m_pTargetNPC = nullptr;
    NPCData*        m_TargetNPCData; //현재활성화 되어있는 npc 대화정보


    int         m_iCurrentStep = 0;
    bool         m_bIsDialogueActive = false;       //현재 대화중인지 플래그값


private:
    map<size_t, NPCData*>     m_DialogueCache;       //Key:json파일경로를 hash화한 결과/ VAlue:NPCData
    GameInstance*               m_pGameInstance = nullptr;
    DIALOGUE_STATE              m_eState = DIALOGUE_STATE::END;


};
NS_END

