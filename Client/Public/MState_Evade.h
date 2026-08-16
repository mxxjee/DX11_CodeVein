#pragma once
#include "MState.h"

NS_BEGIN(Client)
class MState_Evade :
    public MState
{
private:
	explicit MState_Evade();
	explicit MState_Evade(const MState_Evade& Prototype) = delete;
	virtual ~MState_Evade() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

	virtual HRESULT Add_Additional_Anim(_uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop = false, _float _lerpTime = 0.2f) override;

	struct Animation_Data {
		_bool bIsLoop = false;
		_bool bIsSet = false;	// 벡터의 이 애니메이션 인덱스가 채워졌는지
		_uint iAnimationIndex = {};
		_float fLerpTime = {};
		_float fAnimationSpeed = {};
	};

private:
	vector<Animation_Data> m_vecEvadeData;

public:
	static MState_Evade* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;
};

NS_END
