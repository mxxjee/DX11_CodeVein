#pragma once

#include "Client_Define.h"
#include "Skill_Base.h"


NS_BEGIN(Client)
class Skill_IndraCoil final : public Skill_Base
{
private:
	explicit Skill_IndraCoil();
	explicit Skill_IndraCoil(const Skill_IndraCoil& Prototype) = delete;
	virtual ~Skill_IndraCoil() = default;

public:
	virtual HRESULT Initialize(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);

	virtual void Enter_Skill() override;
	virtual void Update_Skill(_float fTimeDelta) override;
	virtual void Exit_Skill() override;

	virtual void On_StartKetsugi() override;
	virtual void On_SkillProjectileEvent() override; //발사체 생성

private:
	void		Create_IndraCoilThunder();

private:
	_bool	m_bFire = { false };
	_float	m_fElapsedTime = { 0.f };
	_uint	m_iFireCount = { 0 };

	_float m_fDelayTime[3] = { 0.0f,0.1f,0.1f };

public:
	static Skill_IndraCoil* Create(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);
	virtual void Free() override;
};
NS_END
