#pragma once
#include "MState.h"
#include "Monster_Hit_Defines.h"


NS_BEGIN(Client)
class MState_Hit :
    public MState
{


private:
	explicit MState_Hit();
	explicit MState_Hit(const MState_Hit& Prototype) = delete;
	virtual ~MState_Hit() = default;

public:
	virtual HRESULT		Initialize(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);

	virtual void Enter_State() override;
	virtual void Update_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

	// 여기서만 쓸 애니메이션 데이터
	struct Animation_Data {
		_bool bIsLoop = false;
		_uint iAnimationIndex = {};
		_float fLerpTime = {};
		_float fAnimationSpeed = {};
	};

	// 보조 애니메이션 추가
	virtual HRESULT Add_Additional_Anim(_uint _Index, _float _speed, _int _slotNum, _bool _isLoop = false, _float _lerpTime = 0.2f) override;

public:
	static MState_Hit* Create(class GameObject* pOwner, _uint iAnimIdx, _float fSpeed);
	virtual void Free() override;

private:
	UMAP<_int, Animation_Data> m_umapAnimationData;

	HIT_LEVEL	m_eHitLevel;
	HIT_DIR	m_eHitDir;

};

NS_END
