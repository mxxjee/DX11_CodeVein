#pragma once

#include "Client_Define.h"
#include "Skill_Base.h"


NS_BEGIN(Client)
class Skill_ShadowAssault final : public Skill_Base
{
private:
	explicit Skill_ShadowAssault();
	explicit Skill_ShadowAssault(const Skill_ShadowAssault& Prototype) = delete;
	virtual ~Skill_ShadowAssault() = default;

public:
	virtual HRESULT Initialize(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);

	virtual void Enter_Skill() override;
	virtual void Update_Skill(_float fTimeDelta) override;
	virtual void Exit_Skill() override;

	virtual void On_StartKetsugi() override;
public:
	static Skill_ShadowAssault* Create(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);
	virtual void Free() override;
};
NS_END
