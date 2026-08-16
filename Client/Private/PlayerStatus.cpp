#include "Client_Define.h"
#include "PlayerStatus.h"
#include "GameInstance.h"

////////////////////////임시//////////////////
void PlayerStatus::Update(const _float fTimeDelta)
{
	if (fCurrentGuard == fMaxGuard)
		m_bFocusState = true;

	if (m_bFocusState)
	{
		/*집중상태=집중게이지 줄어들기*/
		fCurrentGuard -= fTimeDelta * 3.f;//임시값
		fCurrentGuard = clamp<float>(fCurrentGuard, 0.f, fMaxGuard);
		if (fCurrentGuard == 0)
			m_bFocusState = false;
	}

	//한번 Stamina가 부족하다고 감지가되었을때, 일정값 이상이면 이벤트발송한다(다시 빨간색fill을꺼라) 
	if (m_bEnterLessStaminaState)
	{
		if (fCurrentStamina >= fLowStamina)
		{
			m_bEnterLessStaminaState = false;
			UI_MasterEvent	Event;
			Event.m_ActionName = "OnStaminaRecovered";
			Event.m_EventTarget = UI_EVENT_TARGET::UIOBJECT;
			Event.m_ActionStrHash = hash<string>{}(Event.m_ActionName);
			GameInstance::GetInstance()->Publish(Event);

		}
	}
	if (!m_bUseStamina)
	{
		//스테미나를 안쓰고있을때,대기상태 이후로 값을 채워준다
		fStaminaGenTime += fTimeDelta;
		if (fStaminaGenTime >= 0.5f)
		{
			if (fCurrentStamina < fMaxStamina)
				fCurrentStamina += fTimeDelta * 20.f;
			//키누르면 다시 fStminaGenTime=0
		}

	}




}

void PlayerStatus::Walk(const _float fTimeDelta)
{
	fCurrentStamina -= fTimeDelta * 10.f;
	fCurrentStamina = clamp<float>(fCurrentStamina, 0.f, fMaxStamina);
	fStaminaGenTime = 0.f;

	m_bUseStamina = true;

	//일정스테미나 이하면 빨간색깜빡깜빢스 실행 ㄱㄱ
	if (fCurrentStamina < fLowStamina)
	{
		m_bEnterLessStaminaState = true;

		UI_MasterEvent	Event;
		Event.m_ActionName = "OnStaminaLow";
		Event.m_EventTarget = UI_EVENT_TARGET::UIOBJECT;
		Event.m_ActionStrHash = hash<string>{}(Event.m_ActionName);
		GameInstance::GetInstance()->Publish(Event);

	}
}