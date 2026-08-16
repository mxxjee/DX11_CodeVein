#pragma once
#include "ShaderTool_Define.h"
#include "ContainerObject.h"


NS_BEGIN(Engine)
class Model;
class TrailEffect;
class StateMachine;
class Stat;
NS_END

NS_BEGIN(ShaderTool)
class MinimapRenderComponent;


class Player : public ContainerObject
{
public:
	typedef struct tagPlayerDesc : public GameObject::GAMEOBJECT_DESC
	{
		PHYSX_ACTOR_DESC tActorDesc;
		PHYSX_CONTROLLER_DESC tControllerDesc;

	}PLAYER_DESC;

public:
	enum PLAYERSTATE
	{
		IDLE,			//기본 상태(무기별) + 거점의 경우에는 기본 Idle 
		WALK,			//걷기 (락온 or 막기)
		RUN,			//기본 키입력이동
		DASH,			//RUN + Shift 일때
		ROLL,			//키입력 + Space 구르기
		BACKSTEP,		//키입력x + Space 구르기
		ATTACKNORMAL,	//기본공격
		ATTACKSTRONG,	//강공격
		ATTACKDODGEF,	//대쉬에서 마우스 클릭시 공격 모션
		ATTACKDODGEB,	//제자리 백스텝 후 마우스 클릭시 공격모션
		SPECIALATTACK,	// 무기별 고유 액션
		GUARDSTART,		//Alt 시작 가드 상태
		GUARDLOOP,		//Alt 루프 가드 상태
		GUARDEND,		//Alt 끝날때 가드 상태
		GUARDWALK,		//가드 전용 걷기모션
		GUARDHIT,		//가드시에 피격
		END
	};

	enum INPUT_MASK
	{
		INPUT_W = 1 << 0, //0001
		INPUT_A = 1 << 1, //0010
		INPUT_S = 1 << 2, //0100
		INPUT_D = 1 << 3, //1000
	};

	typedef struct tagPlayerInput
	{
		_vector			vMoveDir = {};
		INPUT_DIR		InputDir = INPUT_DIR::INPUT_END;

		MOUSEKEYSTATE	eLastMouseKeyState = {};
		_bool			bShiftPressed = { false };
		_bool			bAltPressed = { false };
		_bool			bSpaceDown = { false };
		_bool			bAltDown = { false };
		_bool			bAltUp = { false };

	}INPUT_DESC;

	typedef struct tagPlayerAnimRunTimeEvent
	{
		_int iInputAreaCount = 0;
		_int iCanMoveCount = 0;
		_int iCanComboCount = 0;
		_int iCanEscapeCount = 0;

		_bool bInputArea() const { return iInputAreaCount > 0; }
		_bool bCanMove()  const { return iCanMoveCount > 0; }
		_bool bCanCombo() const { return iCanComboCount > 0; }
		_bool bCanEscape()const { return iCanEscapeCount > 0; }

		_float fStaminaCost = { 0.f };
		_bool bBlockRegenStamina = { false };

	}PLAYERRUNTIMEEVENT_DESC;

	typedef struct tagUIEvent
	{

	};
private:
	explicit Player();
	explicit Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player(const Player& original);
	virtual ~Player();

public:
	class Player_Weapon* Get_ActivePlayerWeapon() { return m_pActiveWeapon; }
	void						Set_CanTurn(bool bTurn) { m_bTurn = bTurn; }
	INPUT_DESC& Get_PlayerInputDesc() { return m_tInputDesc; }
	PxController* Get_PlayerPxControllder() { return m_pController; }
	PLAYERRUNTIMEEVENT_DESC& Get_PlayerAnimEventDesc() { return m_tPlayerRunTimeEvent; }
	void						Reset_PlayerAnimEventDesc(); //이벤트 애니메이션 바뀔때 리셋해야할것들
	void						Reset_PlayerMoveInputDesc(); //이동 관련 입력값만 리셋용 매프레임
	void						Reset_PlayerPreesedInputDesc(); //지속형 Pressed 관련  매 프레임 리셋
	void						Reset_PlayerConsumeDesc(); //단발(소비)형 Dowm , Up 과 같은것들 
	void						Set_ApplyTranslation(_bool bApplyTranslation) { m_bApplyTranslation = bApplyTranslation; }

	void						Teleport(_fvector vTargetPos);

public:
	virtual HRESULT		Initialize_Prototype(LEVEL _level);
	virtual HRESULT		Initialize(void* arg) override;
	virtual _int		Update_Priority(const _float fTimeDelta) override;
	virtual _int		Update_Parallel(const _float fTimeDelta) override;
	virtual _int		Update(const _float fTimeDelta) override;
	virtual _int		Update_Late(const _float fTimeDelta) override;
	virtual HRESULT		Render(const _float fTimeDelta) override;



private:
	class Stat* m_pStatCom = { nullptr };
	class StateMachine* m_pUpperStateMachineCom = { nullptr }; //상체용 스테이트머신, 기존 사용하던건 Character에있음
	class Player_MasterRig* m_pMasterRig = { nullptr };
	class Player_Weapon* m_pActiveWeapon = { nullptr };
	class Player_Weapon* m_pWeapons[_UINT(WEAPON_TYPE::WP_END)] = { nullptr };

	class Player_Hair* m_pActiveHair = { nullptr };
	class Player_Hair* m_pHairs[_UINT(WEAPON_TYPE::WP_END)] = { nullptr };

	_float m_fSpeed = 0.f;

	/* PhysX 캐릭터 충돌체용 */
	physx::PxController* m_pController = nullptr;
	_float               m_fGravity = 0.f;      // 누적될 중력 값
	_bool                m_bIsGrounded = false;
	_bool				 m_bEnablePhysics = false;

	_bool				m_bAttackSweepActive = { false };	// 공격중인지
	_float				m_fAttackRadius = { 0.5f };			// 공격 범위
	_float				m_fAttackDamage = { 10.f };			// 공격 데미지
	_float				m_fAttackHalfHeight = { 0.f };		// 공격 반높이
	_float3				m_vPrevWeaponPos = {};				// 이 전 프레임 무기 위치
	_float3				m_vCurrentWeaponPos = {};			// 현재 프레임 무기 위치
	set<_uint>			m_setHitTargets = {};				// 이미 공격한 몬스터 저장
	_uint				m_iColliderEventHandle = {};		// 
	_float				m_fKnockbackForce = {};
	_float4x4* m_pWeaponBoneMatrix = {};			// 무기 본 위치
	_float4x4* m_pWeaponTrailTip = {};
	_float4x4* m_pWeaponTrailRoot = {};

	vector<_float4x4*>	m_vecWeaponBoneMatrices;		// 무기들 본 위치

	Model* m_pMasterRigModel = { nullptr };
	Model* m_pMasterModel = { nullptr };

	//플레이어 입력 관리
	INPUT_DESC				m_tInputDesc = {};
	_uint					m_iPlayerAnimEventHandle = {};
	PLAYERRUNTIMEEVENT_DESC	m_tPlayerRunTimeEvent = {};
	_bool					m_bApplyTranslation = { true };
	_bool m_bFix = {};
	//Guard 상태 일때 제어
	_bool					m_bOnGuard = { false };
	_vector					m_vSaveCameraLook = {}; //가드 시점에 카메라의 마지막 Look 방향벡터를 저장하기 위해
	_float					m_fRegenPerSecond = { 20.f }; //리젠속도

public:
	void			Gather_Input(); //키입력 모으기 + 키입력 상태제어
	void			Apply_MoveAndRotation(_float fTimeDelta); //키입력 모은걸 토대로 실제 이동 및 회전
	void			Start_Dir_RotationLerp(); //시작해야하는 방향 알려주고
	void			Update_RotationLerp(_float fTimeDelta); //회전 행렬 보간 시작
	void			Apply_AnimEventCount(const PLAYERANIM_EVENT& PlayerAnimEvent, _int iSign); //이벤트 수집해서 카운트 증가로 정수형으로 _bool 변수 판단
	void			Update_Playerstat(const PLAYERANIM_EVENT& PlayerAnimEvent);
	// Sword Trail 관련 변수
	vector<TrailEffect*>		m_vecTrailEffects;
	_bool						m_bTrailActive = {};

	void			ChangeWeapon(); //임시 무기 체인지용
	PLAYER_SHADER_DESC Get_PlayerShaderDesc() { return m_tPlayerShaderDesc; }
	PLAYER_MESHTEX_NUM* Get_PlayerMeshTexNumPtr() { return &m_tPlayerMeshTex; }
	void Set_PlayerShaderDesc(PLAYER_SHADER_DESC _tPlayerShaderDesc) { m_tPlayerShaderDesc = _tPlayerShaderDesc; }

private:
	HRESULT			Ready_Components();
	HRESULT			Ready_PartObjects();
	HRESULT			Ready_States();
	HRESULT			Ready_Event();
	HRESULT			Ready_PhysXEvent();
	HRESULT			Bind_ShaderResources();

#pragma region PhysX 이동, 전투
	class Model* Get_MasterRigModel();
	void			Update_WeaponPosition();
	void			Process_AttackSweep();
#pragma endregion PhysX 이동, 전투

	void Update_PlayerCustomDesc();
public:
	static Player* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

private:
	bool		m_bTurn = true;
	MinimapRenderComponent* m_pMinimapRendercom = nullptr;
	PLAYER_SHADER_DESC m_tPlayerShaderDesc;
	_bool m_bOnetime = false;
	PLAYER_MESHTEX_NUM m_tPlayerMeshTex;
	vector<vector<_float4>> m_vInnerColor;
	_int m_iPrevBodyNum = 0;
};
NS_END