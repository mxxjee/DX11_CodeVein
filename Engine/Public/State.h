#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL State abstract : public Base
{
public:
	typedef struct tagAnimationDesc {
		_float fAnimspeed = {};
		_uint iAnimIndex = {};

	}STATE_ANIM_DESC;

protected:
	explicit State();
	explicit State(const State& original) = delete;
	virtual ~State() = default;

public:
	class GameObject* Get_Owner() { return m_pOwner; }
	_uint Get_StateType() const { return m_iStateType; }
	void Set_StateType(_uint iStateType) { m_iStateType = iStateType; }
	_bool Get_HasAnimEvents() const { return m_bHasAnimEvents; }
	void Set_HasAnimEvents(_bool bHasAnimEvents) { m_bHasAnimEvents = bHasAnimEvents; }
	
	void Set_DamagePower(DAMAGEPOWER eDamagePower) { m_eDamagePower = eDamagePower; } //공격상태들은 파워 설정해주기 (플레이어,몬스터 공통)
	DAMAGEPOWER Get_DamagePower() const { return m_eDamagePower; }

	void Set_LerpTime(_float _lerpTime) { m_fLerpTime = _lerpTime; }

public:
	virtual HRESULT Initialize(class GameObject* pOwner);

	virtual void Enter_State() = 0;
	virtual void Update_State(_float fTimeDelta) = 0;
	virtual void Exit_State() = 0;

	virtual void On_ProjectileEvent(); //투사체 발사 이벤트 

	// 하위 상태중 추가적인 애니메이션이 필요할 경우 이거 사용
	virtual HRESULT Add_Additional_Anim(_uint _animIndex, _float _speed, _int _slotNum, _bool _isLoop = false, _float _lerpTime = 0.2f)
	{
		// 잘못 들어왔음을 알리는 DebugBreak
		// 상태 번호를 잘못 줬거나 자식 클래스에서 이거 오버라이딩 안 한것
		BREAK;
		return E_FAIL;
	}
	
	/// <summary>
	/// 알람 범위 지정 
	/// </summary>
	/// <param name="_first"> 알람 최소 시간 </param>
	/// <param name="_last"> 알람 최대 시간 </param>
	/// <param name="_alamNum"> 알람 번호(알람이 여러개일 경우) </param>
	virtual void Set_Alarm_Range(_float _first, _float _last, _uint _alamNum = 0) { BREAK; return; }

	virtual void Set_AnimSpeed(_float _speed) {}
	virtual void Set_AnimIndex(_uint _index) {}
	virtual void Set_AnimStruct(_uint _index) {}

protected:
	class GameInstance*		m_pGameInstance = { nullptr };
	class GameObject*		m_pOwner = { nullptr };		// 상태의 소유자
	_uint					m_iStateType = {};			// 상태 타입
	_bool					m_bHasAnimEvents = { false };
	DAMAGEPOWER				m_eDamagePower = { DAMAGEPOWER::POWER_END };
	_float					m_fLerpTime = 0.2f;

public:
	virtual void Free() override;
};

NS_END