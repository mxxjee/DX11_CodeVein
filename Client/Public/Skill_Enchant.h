#pragma once

#include "Client_Define.h"
#include "Skill_Base.h"


NS_BEGIN(Client)
class Skill_Enchant final : public Skill_Base
{
private:
	explicit Skill_Enchant();
	explicit Skill_Enchant(const Skill_Enchant& Prototype) = delete;
	virtual ~Skill_Enchant() = default;

public:
	virtual HRESULT Initialize(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);

	virtual void Enter_Skill() override;
	virtual void Update_Skill(_float fTimeDelta) override;
	virtual void Exit_Skill() override;

	virtual void On_StartKetsugi() override;
	virtual void On_DurationEnd() override;
public:
	static Skill_Enchant* Create(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);
	virtual void Free() override;
};
NS_END
