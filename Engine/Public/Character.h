#pragma once
#include "Engine_Define.h"
#include "GameObject.h"
#include "ParticleSystem.h"

NS_BEGIN(Engine)

class Shader;
class Model;
class Component;
class LightComponent;
class StateMachine;

// 자식 클래스가 체인 정의를 넘길 때 사용하는 구조체
struct BONE_CHAIN_DEFINE
{
	struct BONE_ENTRY
	{
		_string strName = {};
		_int    iChainParent = {};
	};

	_string              strParentBone = {};
	vector<BONE_ENTRY>   vecBones = {};
};

struct BONE_CHAIN_BUILD_DESC
{
	vector<BONE_CHAIN_DEFINE> vecChainDefines = {};
	Model* pBoneSourceModel = {};  // 레스트포즈를 가져올 모델 (Hair: m_pModelComVec[n], Outer: m_pModelCom)
	_uint                     iChainID_Offset = {};   // 체인 ID 시작 오프셋 (Hair: 100, Outer: 0)
	PHYSX_BONE_CHAIN_DESC     tDefaultPhysXDesc = {}; // 기본 PhysX 파라미터
	_bool                     bUseRestPosePosition = { false };
};

/* 순수 상속용 클래스 */
class ENGINE_DLL Character abstract : public GameObject
{
protected:
	explicit Character();
	explicit Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Character(const Character& original);
	virtual ~Character();

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* _arg) override;
	_int Update_Priority(const _float fTimeDelta) override;
	_int Update(const _float fTimeDelta) override;
	_int Update_Late(const _float fTimeDelta) override;
	HRESULT Render(const _float fTimeDelta) override;

public:
	// 중간부모 클래스에 만들어둬서 자식 클래스에서는 선언 안 해도 됨
	// 굳이굳이 final 해뒀음
	Shader* Get_Shader() override final { return m_pShaderCom; }
	Model* Get_Model() override final { return m_pModelCom; }
	LightComponent*& Get_LightPtr() { return m_pLightComponent; }

	void Set_Shader(Shader* _shader) override final { m_pShaderCom = _shader; }
	void Set_Shader(const _wstring& _shaderName) override final;
	void Set_Model(Model* _model) override final { m_pModelCom = _model; }
	void Set_ElapsedTime(_float fTimeElapsed) { m_fTimeElapsed = fTimeElapsed; }

	/* Transform과 PhysX를 둘 다 옮기는 함수 */
	void Teleport(_fvector vTargetPos);
	void Teleport(_fvector _targetPosition, _float4 _Rotation);
	void Teleport(_float4 _targetPosition);
	void Teleport(_float4 _targetPosition, _float4 _Rotation);

protected:
	inline HRESULT Add_Shader(const _wstring& _shaderName) {
		return Add_Component(0, _shaderName, Com_Shader, RCAST(Component**)(&m_pShaderCom));
	}
	HRESULT Add_Model(const _wstring& _modelName);

	PxCapsuleController* Create_Controller(PHYSX_CONTROLLER_DESC& _controllerDesc);

	/// <summary>
	/// 루트 모션 업데이트
	/// </summary>
	/// <param name="_reverseY">Y축 반전 객체인지</param>
	/// <param name="_containerObjectTransform">업데이트 할 트랜스폼 컴포넌트</param>
	/// <param name="_masterModel">애니메이션 업데이트 되는 모델 컴포넌트</param>
	void Update_RootPos(_bool _reverseX = false, _bool _reverseY = false, _bool _reverseZ = false, class Transform* _containerObjectTransform = nullptr, class Model* _masterModel = nullptr);
	_vector Calculate_RootPos(_bool _reverseX = false, _bool _reverseY = false, _bool _reverseZ = false, Transform* _containerObjectTransform = nullptr, Model* _masterModel = nullptr);

#ifdef _DEBUG
	void Add_Debug_Controller_Capsule(const _float fTimeDelta);
#endif // _DEBUG

#pragma region PhysX Bone Chain
	HRESULT BoneChain_Build(const BONE_CHAIN_BUILD_DESC& _buildDesc, Model* _masterModel);
	HRESULT BoneChain_CreatePhysX(const _float4x4& _worldMatrix);
	void    BoneChain_UpdateRestPose();
	void    BoneChain_UpdateKinematic(const _float4x4& _worldMatrix);
	void    BoneChain_FetchResults(const _float4x4& _worldMatrix, _float _fDesiredScale = 0.01f);
	void BoneChain_UpdateKinematic_Before(const _float4x4& _worldMatrix);
	void BoneChain_FetchResults_Before(const _float4x4& _worldMatrix, _float _fDesiredScale);
	void BoneChain_AutoBuildDefines(Model* _pBoneSourceModel, const _string& _strParentBoneName, vector<BONE_CHAIN_DEFINE>& _outDefines);
	void    BoneChain_Cleanup();

	void    BoneChain_UpdateAll(const _float4x4& _worldMatrix, _float _fDesiredScale = 0.01f);

	void BoneChain_UpdateAll_Before(const _float4x4& _worldMatrix, _float _fDesiredScale = 0.01f);

#pragma endregion PhysX Bone Chain

	
protected:
	//이거상속해서제대로 쓰려면 자식에 같은 변수명이 없어야함
	Model* m_pModelCom = { nullptr };
	vector<Model*> m_pModelComVec;
	Shader* m_pShaderCom = { nullptr };
	Texture* m_pNoiseTextureCom = { nullptr };
	Texture* m_pRimNoiseTextureCom = { nullptr };
	LightComponent* m_pLightComponent = { nullptr };
	StateMachine* m_pStateMachineCom = { nullptr };

	physx::PxCapsuleController* m_pController = { nullptr };	// 피직스 컨트롤러

	_float              m_fGravity = 0.f;              // 중-력
	_bool               m_bIsGrounded = false;         // 땅에 있는지
	_bool               m_bEnablePhysics = false;      // 중력 적용
	_bool				m_bHit = false;
	_float				m_fHitViewTime = 0.f;
	_float				m_fTimeElapsed = {};
	_float m_fRimNoiseTime = {};

	// 추가됨 : PhysX BoneChain 공용 멤버
	vector<CHAIN_BONE_DESC>         m_vecBoneChains = {};
	vector<PHYSX_CHAIN_INFO>        m_vecBoneChainPhysX = {};
	vector<pair<_int, _float4x4>>   m_vecBoneChainFetchScratch = {};
	_uint                           m_iBoneChainWaitFrames = {};
	BONE_CHAIN_BUILD_DESC           m_tBoneChainBuildDesc = {};  // 추가됨 : PhysX 생성 시 참조용 캐싱
	Model* m_pBoneChainMasterModel = {}; // 추가됨 : 체인이 참조하는 마스터 모델

protected:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	void Free() override;

};

NS_END