#pragma once
#include "Base.h"
namespace Engine
{
    class GameInstance;
    class GameObject;
}

/*현재 있는 레벨의 Interaction객체들을 한데 모아서 관리한다.*/
NS_BEGIN(Client)
class IInteractable;


class InteractionManager :
    public Base
{
    DECLARE_SINGLETON(InteractionManager);
private:
    explicit InteractionManager();
    virtual ~InteractionManager();

public:
    HRESULT Initialize();
    
                    //관리하기 위한 인터렉션 추가
    void        Register_InteractableObject(IInteractable* pObj);
    void        Set_Player(GameObject* pPlayer) { m_pMainPlayer = pPlayer;}

                //특정 오브젝트 관리대상에서 제외
    void        UnRegisterInteractable(IInteractable* pObj);

                    //매프레임 interaction확인
    void        Update(const _float& fTimeDelta);
    
                //인터렉션 키 눌렀을때 상호작용호출
    bool        OnPressedInteractionKey();

            //씬 바꿀 시 호출
    void        Clear_InteractionManager();

    IInteractable* Get_Current_Interaction_Target() { return m_pCurrent; }
    
    void        Set_Enable(bool b);
    void        Set_Lock(bool b) { m_bLock = b; }
 
    /// ///////SavePointData
    void            Set_LastSavePointInfo(LEVEL eLevel, int iIdx);
    void            Reset_LastSavePointInfo() { m_LastSaveInfo = nullptr; }
    SAVE_POINT_INFO*            Get_LastSavePointInfo() { return m_LastSaveInfo; }
    
    SAVE_POINT_INFO*     Get_SavePointInfo(LEVEL eLevel, int iIdx);


    void                Finishi_Interaction();
    void                Reset_CurrentInteraction() { m_pCurrent = nullptr; }
	void                Reset_BestPriority() { iBestPriority = (int)INTERACTION_TYPE::END; }

    LEVEL               Get_Level_By_MapType(MAP_TYPE eType);
private:

    HRESULT         Load_SavePointData(string path);

public:
    void Free() override final;


private:
    unordered_map< INTERACTION_TYPE, vector<IInteractable*>>     m_InteractionMap;
    GameObject*             m_pMainPlayer = nullptr;
    float                   fMinDistSq = FLT_MAX;
    IInteractable*          m_pCurrent = nullptr;
    int        iBestPriority = (int)INTERACTION_TYPE::END;


private:
    map<LEVEL, map<int, SAVE_POINT_INFO*>> m_SavePoints;
    SAVE_POINT_INFO*        m_LastSaveInfo=nullptr; //버튼을 누를시 갱신되는 가장 최근에 이동했떤 세이브포인트정보
                                          //씬 진입시 이를 읽는다.

private:
    GameInstance* m_pGameInstance = nullptr;
    bool        m_bEnable = true;

public:
    void Set_SceneChanging(bool bState) { m_bIsSceneChanging = bState; }
    bool Is_SceneChanging() const { return m_bIsSceneChanging; }

private:
    bool        m_bLock = false;//락이 걸려있으면, enable도안먹는다.
    bool        m_bIsSceneChanging = false; // 씬 이동 혹은 텔레포트 중인지 여부

};
NS_END
