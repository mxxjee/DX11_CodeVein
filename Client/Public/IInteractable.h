#pragma once

namespace Engine
{
	class GameObject;
}


NS_BEGIN(Client)
/*상호작용 가능한 물체에게 상속하는 클래스.*/
/*인터렉션 키는 E로 고정*/

class IInteractable
{
public:
	virtual ~IInteractable() {};

public:
	virtual bool		IsInteractable() = 0;	
				//상호작용가능한지 물어보는 함수
				//매니저에선 일단 이거가 true여야 비교를하므로, 거리비교 전에 체크할대상이 잇을경우 정의
				//ex) 플레이어가 공격중이아닌지 체크? 이런것들.
				//딱히없으면 true반환하도록 하기.

	virtual _float		Get_InteractionRange() = 0;		//상호작용하기 위한 최소거리

	/*각 함수내에서 플레이어의 값이 필요할수있으므로 매개변수  추가*/
	virtual void		Enter_InteractionRange(GameObject* pPlayer) = 0;		//상호작용 가능 범위에들어왔을때 호출되는함수
	virtual void		Stay_InteractionRange(GameObject* pPlayer,const _float& fTimeDelta) = 0;
	virtual void		Exit_InteractionRange(GameObject* pPlayer) = 0;		//범위에 들어왔다가 나갔을때 호출되는 함수

	virtual void		Enter_Interaction(GameObject* pPlayer) = 0;		//키를 눌러서 실제로 인터렉션실행(대부분 여기서 진짜 상호작용 행동을 정의)
	virtual void		Stay_Interaction(GameObject* pPlayer,const _float& fTimeDelta) = 0;
	virtual void		Exit_Interaction(GameObject* pPlayer) = 0;			//객체가 판단하여 알아서 exit_Interaction()을 수행해야한다. 혹은 NPC들은 대화끝났을때.


	virtual _vector		Get_Position() = 0;

public:
	INTERACTION_TYPE		Get_InteractionType() { return m_eInteractionType; }
protected:
	INTERACTION_TYPE		m_eInteractionType;


public:
	/*enter/exit 판단하기위함*/
	bool		m_bPrevRange = false;	//이전프레임에 상호작용 범위에들어왔느지?
	bool		m_bPreInteraction = false;		//이전프레임의 상호작용 여부 
};
NS_END
