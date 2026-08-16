#pragma once

#include "Base.h"

NS_BEGIN(Engine)

static constexpr PxU32 FILTER_DISABLED = (1 << 31);

class DisabledActorQueryFilter : public PxQueryFilterCallback
{
public:
	_bool m_bMultiMode = {};

	PxQueryHitType::Enum preFilter(
		const PxFilterData& _filterData,
		const PxShape* _pShape,
		const PxRigidActor* _pActor,
		PxHitFlags& _queryFlags) override
	{
		if (_filterData.word3 & FILTER_DISABLED)
			return PxQueryHitType::eNONE;

		return m_bMultiMode ? PxQueryHitType::eTOUCH : PxQueryHitType::eBLOCK;
	}

	PxQueryHitType::Enum postFilter(
		const PxFilterData& _filterData,
		const PxQueryHit& _hit,
		const PxShape* _pShape,
		const PxRigidActor* _pActor) override
	{
		return m_bMultiMode ? PxQueryHitType::eTOUCH : PxQueryHitType::eBLOCK;
	}
};



class PhysXManager final : public Base, public PxControllerBehaviorCallback
{
private:
	explicit PhysXManager();
	virtual ~PhysXManager();

public:
	HRESULT Initialize();
	_int	Update_Priority(const _float fTimeDelta);
	void    Update(const _float fTimeDelta);
	_int	Update_Late(const _float fTimeDelta);
	HRESULT Render(const _float fTimeDelta);

public:
	// 맵툴에서 사용할 쿠킹만 되는 함수
	HRESULT PhysX_Cooking_Static_Model(PHYSX_ACTOR_DESC& _actorDesc);
	// 클라이언트나 다른 툴에서 사용할 쿠킹된 Actor 불러오기
	PxRigidStatic* PhysX_Load_Static_Actor(PHYSX_ACTOR_DESC& _actorDesc);
	PxRigidStatic* PhysX_Load_Static_Actor_ConvexMesh(PHYSX_ACTOR_DESC& _actorDesc);
	PxRigidStatic* PhysX_Load_Static_Actor_Auto(PHYSX_ACTOR_DESC& _actorDesc);
	// 캐릭터를 움직일 Controller 만들기
	PxController* PhysX_Create_Controller(const PHYSX_CONTROLLER_DESC& desc);
	// 캐릭터 제거
	void PhysX_Remove_Actor(PxRigidActor* _pActor);
	// 컨트롤러 제거(액터랑 연결됨)
	void PhysX_Remove_Controller(PxController* _pController);

	void Disable_Controller(PxController* _pController);

	void Enable_Controller(PxController* _pController, const PxExtendedVec3& _spawnPos, PX_ACTOR_TYPE _eType);

public:
	/* 충돌 검사 함수 */
	// 광선 발사 함수(총알판정, 마우스 피킹, 바닥 감지 등)
	PHYSX_RAYCAST_RESULT    PhysX_Raycast(const PxVec3& _origin, const PxVec3& _direction, _float _maxDistance);
	// 캡슐 검사 함수(캡슐을 밀어 처음 부딪히는 물체 검출)
	PHYSX_SWEEP_RESULT      PhysX_Sweep_Capsule(_float _radius, _float _halfHeight, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance);
	// 위와 동일(구로 바뀜, 검사 판정이 조금 더 단순{연산량이 적음})
	PHYSX_SWEEP_RESULT      PhysX_Sweep_Sphere(_float _radius, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance);
	// 다중 히트 Sweep
	PHYSX_MULTI_SWEEP_RESULT PhysX_Sweep_Sphere_Multi(_float _radius, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance);
	// 구 범위 안의 객체들 검출(범위 스킬같은거 검사용)
	PHYSX_OVERLAP_RESULT    PhysX_Overlap_Sphere(_float _radius, const PxTransform& _pose);
	PHYSX_OVERLAP_RESULT	PhysX_Overlap_Capsule(_float radius, _float halfHeight, const PxTransform& pose);
	// 캡슐 범위 안의 객체들 다단히트
	PHYSX_MULTI_SWEEP_RESULT PhysX_Sweep_Capsule_Multi(_float _radius, _float _halfHeight, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance);

	PHYSX_MULTI_SWEEP_RESULT PhysX_Sweep_Capsule_Multi_ExceptStatic(_float _radius, _float _halfHeight, const PxTransform& _pose, const PxVec3& _direction, _float _maxDistance);

#ifdef _DEBUG
public:
	void Enable_DebugVisualization();	// 시각화 파라미터 ON
	void Disable_DebugVisualization();	// 시각화 파라미터 OFF
	void Toggle_DebugVisualization();	// 시각화 토글
	_bool Is_DebugVisualization() const { return m_bDebugVisualization; }	// 시각화 상태 확인

	const PxRenderBuffer* Get_PhysXRenderBuffer() const;

	void Set_VisualizationParam(PxVisualizationParameter::Enum _eParam, _bool _bEnable);
	_bool Get_VisualizationParam(PxVisualizationParameter::Enum _eParam) const;
private:
	_bool m_bDebugVisualization = { false };	// 시각화 활성화 여부

	_bool m_bVis_CollisionShapes = { false };
	_bool m_bVis_CollisionAABBs = { true };
	_bool m_bVis_ActorAxes = { true };
	_bool m_bVis_ContactPoint = { false };
	_bool m_bVis_ContactNormal = { false };
	_bool m_bVis_BodyLinVelocity = { false };
	_bool m_bVis_BodyAngVelocity = { false };
	_bool m_bVis_JointLocalFrames = { false };
	_bool m_bVis_JointLimits = { false };
#endif

private:
	// 메쉬 쿠킹 및 저장
	physx::PxTriangleMesh* Cook_And_Save_Mesh(class Mesh* _mesh, const _wstring& _savePath);
	// 쿠킹된 메쉬 불러오기
	physx::PxTriangleMesh* Load_Cooked_Mesh(const _wstring& strPath);

	PxConvexMesh* Cook_And_Save_ConvexMesh(class Mesh* _mesh, const _wstring& _savePath);
	PxConvexMesh* Load_Cooked_ConvexMesh(const _wstring& strPath);

	PxShape* Load_Shape_Auto(const _wstring& _cachePath, const PxMeshScale& _scale, PxMaterial* _material);


public:
#pragma region Get함수들
	PxPhysics* Get_Physics() const { return m_pPhysics; }
	PxScene* Get_Scene() const { return m_pScene; }
	PxControllerManager* Get_ControllerManager() const { return m_pControllerManager; }
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& _shape, const PxActor& _actor) override
	{
		return PxControllerBehaviorFlags(0);
	}

	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& _controller) override
	{
		return PxControllerBehaviorFlag::eCCT_SLIDE;
	}

	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& _obstacle) override
	{
		return PxControllerBehaviorFlags(0);
	}
#pragma endregion Get함수들

private:
	_wstring Make_Cache_Path(const _wstring& strModelName, _uint meshIndex);

#pragma region PhysXCloth
	public:
		HRESULT PhysX_Create_BoneChain(const PHYSX_BONE_CHAIN_DESC& _desc, class Model* _pMasterModel, const _float4x4& _objectWorldMatrix);
		void PhysX_Update_BoneChain_Kinematic(_uint _iChainID, const _float4x4& _rootCombinedMatrix, const _float4x4& _objectWorldMatrix);
		void PhysX_Fetch_BoneChain_Results(_uint _iChainID, const _float4x4& _objectWorldMatrix, vector<pair<_int, _float4x4>>& _outBoneMatrices);
		void PhysX_Remove_BoneChain(_uint _iChainID);
		void PhysX_Remove_All_BoneChains();


	private:
		UMAP<_uint, PHYSX_BONE_CHAIN> m_umapBoneChains = {};
		_uint m_iBoneChainIDCounter = {};
#pragma endregion PhysXCloth



private:
	PxFoundation* m_pFoundation = { nullptr };
	PxPhysics* m_pPhysics = { nullptr };
	PxDefaultCpuDispatcher* m_pDispatcher = { nullptr };
	PxScene* m_pScene = { nullptr };
	PxControllerManager* m_pControllerManager = { nullptr };
	PxPvd* m_pPvd = { nullptr };
	PxOmniPvd* m_pOmniPvd = { nullptr };

	/* 쿠킹용 변수 */
	_wstring m_strCacheDir = L"../../DataFiles/PhysXCache/";

	_float m_fPhysXAccumulator = {};
	_float m_fPhysXFixedStep = 1.f / 60.f; // 60Hz 고정 스텝

private:
	// 메모리 할당기 & 에러 콜백 (PhysX 필수)
	PxDefaultAllocator      m_Allocator;
	PxDefaultErrorCallback  m_ErrorCallback;
	DisabledActorQueryFilter m_QueryFilter = {};

private:
	class GameInstance* m_pGameInstance = { nullptr };

public:
	static PhysXManager* Create();

public:
	void Free() override final;

	
};

NS_END