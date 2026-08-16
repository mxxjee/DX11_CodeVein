#pragma once

#include "Client_Define.h"
#include "Skill_Base.h"


NS_BEGIN(Client)
class Skill_RadiantBarrel final : public Skill_Base
{
private:
	explicit Skill_RadiantBarrel();
	explicit Skill_RadiantBarrel(const Skill_RadiantBarrel& Prototype) = delete;
	virtual ~Skill_RadiantBarrel() = default;

public:
	virtual HRESULT Initialize(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);

	virtual void Enter_Skill() override;
	virtual void Update_Skill(_float fTimeDelta) override;
	virtual void Exit_Skill() override;

	virtual void On_StartKetsugi() override;
	virtual void On_SkillProjectileEvent() override; //발사체 생성
public:
	static Skill_RadiantBarrel* Create(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);
	virtual void Free() override;
};
NS_END
