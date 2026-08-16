#pragma once

#include "Client_Define.h"
#include "Skill_Base.h"


NS_BEGIN(Client)
class Skill_StrikeRiser final : public Skill_Base
{
private:
	explicit Skill_StrikeRiser();
	explicit Skill_StrikeRiser(const Skill_StrikeRiser& Prototype) = delete;
	virtual ~Skill_StrikeRiser() = default;

public:
	virtual HRESULT Initialize(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);

	virtual void Enter_Skill() override;
	virtual void Update_Skill(_float fTimeDelta) override;
	virtual void Exit_Skill() override;

	virtual void On_StartKetsugi() override;
	virtual void On_DurationEnd() override;
public:
	static Skill_StrikeRiser* Create(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);
	virtual void Free() override;
};
NS_END
