#pragma once

#include "Client_Define.h"
#include "MState.h"

NS_BEGIN(Client)

class MState_Turn final : public MState
{
private:
	explicit MState_Turn();
	explicit MState_Turn(const MState_Turn& Prototype) = delete;
	virtual ~MState_Turn() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

	// 보조 애니메이션 추가
	virtual HRESULT Add_Additional_Anim(_uint _Index, _float _speed, _int _slotNum, _bool _isLoop = false, _float _lerpTime = 0.2f) override;

	// 여기서만 쓸 애니메이션 데이터
	struct Animation_Data {
		_bool bIsLoop = false;
		_bool bIsSet = false;	// 벡터의 이 애니메이션 인덱스가 채워졌는지
		_uint iAnimationIndex = {};
		_float fLerpTime = {};
		_float fAnimationSpeed = {};
	};
	enum TurnAnimation { TURN_LEFT, TURN_RIGHT, TURN_LEFT180, TURN_RIGHT180, TURN_ANIMATION_END };

private:
	UMAP<_int, Animation_Data> m_umapAnimationData;
	vector<Animation_Data> m_vecAnimationData;

public:
	static MState_Turn* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

};

NS_END

