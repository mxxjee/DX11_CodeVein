#pragma once

#include"Base.h"

NS_BEGIN(Engine)
class GameInstance;
NS_END

NS_BEGIN(Client)
class Player;
class SkillManager final : public Base
{
	DECLARE_SINGLETON(SkillManager);

private:
	explicit SkillManager();
	virtual ~SkillManager();

public:
	HRESULT				Initialize();

public:
	HRESULT				Register_Skill(_uint iSkillId, class Skill_Base* pSkill); //스킬 아이디 , 스킬 등록
	class Skill_Base*	Find_Skill(_uint iSkillId); //스킬아이디로 스킬 찾기
	
	HRESULT				Equip_SkillToSlot(_uint iSlotId, _uint iSkillId); //슬롯 등록
	class Skill_Base*	Get_SkillToSlot(_uint iSlotId); //슬롯으로 스킬 찾아오기

	void				Update_CooldownAndDuration(_float fTimeDelta); //스킬들 쿨 돌리기

	class Skill_Base*	Requset_Skill(_uint iSlotId);

	void				Debug_SkillReset(); //스킬 쿨타임,지속시간 등 리셋 Debug용 아닙니다

private:
	UMAP<_uint, class Skill_Base*> m_umapSkills;
	UMAP<_uint, _uint> m_umapEquip; //장착 슬롯으로 몇번 슬롯에 스킬아이디를 등록할것인지 (0~7번슬롯)01234567 사용 예정

	GameInstance* m_pGameInstance = { nullptr };


public:
	void Free() override final;


private:
	Player* m_pMainPlayer = nullptr;

};
NS_END
