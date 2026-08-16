#pragma once

#include "Client_Define.h"
#include "MState.h"

NS_BEGIN(Client)

class MState_Stun final : public MState
{
private:
	explicit MState_Stun();
	explicit MState_Stun(const MState_Stun& Prototype) = delete;
	virtual ~MState_Stun() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

	HRESULT Add_Additional_Anim(_uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime);

	struct Animation_Data {
		_bool bIsLoop = false;
		_bool bIsSet = false;	// 벡터의 이 애니메이션 인덱스가 채워졌는지
		_uint iAnimationIndex = {};
		_float fLerpTime = {};
		_float fAnimationSpeed = {};
	};

private:
	vector<Animation_Data> m_vecStunAnimIndexData;	// 추가 애니메이션 데이터

public:
	static MState_Stun* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

};

NS_END

