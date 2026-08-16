#pragma once

#include "Client_Define.h"
#include "Skill_Base.h"


NS_BEGIN(Client)
class Skill_Yeomgeom final : public Skill_Base
{
private:
	explicit Skill_Yeomgeom();
	explicit Skill_Yeomgeom(const Skill_Yeomgeom& Prototype) = delete;
	virtual ~Skill_Yeomgeom() = default;

public:
	virtual HRESULT Initialize(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);

	virtual void Enter_Skill() override;
	virtual void Update_Skill(_float fTimeDelta) override;
	virtual void Exit_Skill() override;

	virtual void On_StartKetsugi() override;
	virtual void On_DurationEnd() override;
public:
	static Skill_Yeomgeom* Create(class Player* pPlayer, class Player_MasterRig* pMasterRig, void* pArg);
	virtual void Free() override;
};
NS_END
