#pragma once

#include "Engine_Define.h"
#include "Base.h"

/*이벤트들을 등록한다.
Emit ->실행시킬 이벤트의 이름과 각종 값들을 등록 (실행예약)
RegisterListener -> 이벤트 이름과 어떤 일을 수행할건지 function을 저장
 Dispatch-> 라이프사이클의 마지막 update에서 매프레임 함수를 실행하고 이벤트 큐를 clear하는작업
        -> RegisterLisnter에도 등록되어있고,  Emit한 상태인 경우 그 이름에 맞는 이벤트를 수행

 Emit = 이벤트 발생 (실행 요청)

 “지금 이 이벤트 실행하고 싶어요!”
 즉, 실행 요청을 큐에 넣는 것
 Update 중 언제든 여러 번 호출됨
 */



NS_BEGIN(Engine)

class CEventBus_Manager :
    public Base
{

private:
    explicit CEventBus_Manager();
    virtual ~CEventBus_Manager() = default;


public:
    HRESULT        Initialize();

public:
            //이번프레임에 수행할 이벤트들
    void Emit(const GameEvent& Event);

        
    //키값에 유효한 핸들을 반환한다.
    _uint RegisterListners(const string& CBName, EventCallBack Callback);
    void    UnRegisterListenrs(const string& CBName, _uint iHandle);
    
public:
    void        DisPatch(_float fTimeDelta);// 매 라이프사이클의 update마지막에 수행될 , 예약열을 보며 이벤트를 수행하는 함수
                            //이후 비워진다.

public:
    static CEventBus_Manager* Create();
    virtual void    Free();
private:
    vector<GameEvent>       m_GameEventQue;       //이번프레임에 실행할 이벤트들 ,GameEvent=이벤트실행시 넘겨줄 데이터들을 담는구조체,,
    UMap<string, map<_uint,EventCallBack>>         m_EventCB;      
                    //m_GAmeEvnet의 이름과 비교하여 실제로 실행할 콜백함수들
                    //하나의 이벤트 당 여러 콜백함수를 등록할 수 있도록 하기위해.
                    //ex) m_EventCB[NPCFOCUS]=CameraZoom();
                      //    m_EventCB[NPCFOCUS] = UIDialog();
};  

NS_END
