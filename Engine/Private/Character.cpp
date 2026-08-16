#include "Engine_Define.h"
#include "Character.h"

//#include "GameInstance.h"
#include "PhysX_Function.h"
#include "Bone.h"
#include "Mesh.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Character::Character()
{
}

Engine::Character::Character(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext)
{
	m_eObjType = OBJTYPE::TYPE_CHARACTER;
}

Engine::Character::Character(const Character& original)
	: GameObject(original)
{
	m_eObjType = OBJTYPE::TYPE_CHARACTER;
}

Engine::Character::~Character()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Character::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::Character::Initialize(void* arg)
{
	CHECK_FAILED(GameObject::Initialize(arg), E_FAIL);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Character::Ready_Components()
{
	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Character::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Character::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Character::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Character::Render(const _float fTimeDelta)
{
	MSG_FAIL(Bind_ShaderResources(), (m_wstrName + L"\n객체의 바인딩 과정중 오류가 발생했습니다.").c_str(), L"렌더 실패", E_FAIL);


	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 루트모션 업데이트 ////////////////////////////////////////////////////////
void Engine::Character::Update_RootPos(_bool _reverseX, _bool _reverseY, _bool _reverseZ, Transform* _containerObjectTransform, Model* _masterModel)
{
	Transform* transform = nullptr;
	Model* model = nullptr;

	// 트랜스폼, 모델이 매개변수로 들어오는건지(파트 오브젝트라서 부모의 트랜스폼, 모델이 필요한지)
	// StandAlone형식(통모델)이라 그냥 써도 되는건지 선택
	transform = (_containerObjectTransform != nullptr) ? _containerObjectTransform : m_pTransformCom;
	model = (_masterModel != nullptr) ? _masterModel : m_pModelCom;

	//  루트 모션 회전 델타를 먼저 적용 (이동보다 선행해야 방향이 갱신됨)
	_vector qRotationDelta = model->Get_RootMotionRotationDelta();
	transform->Rotate_ByQuaternionDelta(qRotationDelta);

	// 모델에서 루트모션델타 가져오기
	_vector vRootMotionDelta = model->Get_RootMotionDelta();

	//cout << "RootDelta x,y,z = " << vDelta.x << ", " << vDelta.y << ", " << vDelta.z << endl;

	vRootMotionDelta = vRootMotionDelta / 100.f; //이거 언리얼 엔진은 cm 단위인데 블렌더는 m단위라서 블렌더로 뽑은거라서 다시 /100 해줘야함.

	//로컬 공간의 루트 모션 델타를 월드 공간의 벡터로 바꿔줘야 내가 바꾼 회전이 적용
	_vector vRight	= transform->Get_State(DIRECTION::RIGHT);
	_vector vUp		= transform->Get_State(DIRECTION::UP);
	_vector vLook	= transform->Get_State(DIRECTION::LOOK);

	// 라 업 룩 방향으로 얼마나 이동했는지 Position에 더해주기
	_vector worldDelta = {};
	// 모델 생성할때 Y축 반대로 했으면 reverse해줘야함
	_float fSignX = _reverseX ? -1.f : 1.f;
	_float fSignY = _reverseY ? -1.f : 1.f;
	_float fSignZ = _reverseZ ? -1.f : 1.f;

	worldDelta = vRight * (fSignX * XMVectorGetX(vRootMotionDelta))
				+ vUp	* (fSignZ * XMVectorGetZ(vRootMotionDelta))
				+ vLook * (fSignY * XMVectorGetY(vRootMotionDelta));

	// transform을 worldDelta만큼 이동
	transform->Translate(worldDelta);
}

_vector Engine::Character::Calculate_RootPos(_bool _reverseX, _bool _reverseY, _bool _reverseZ, Transform* _containerObjectTransform, Model* _masterModel)
{
	Transform* transform = nullptr;
	Model* model = nullptr;

	// 트랜스폼, 모델이 매개변수로 들어오는건지(파트 오브젝트라서 부모의 트랜스폼, 모델이 필요한지)
	// StandAlone형식(통모델)이라 그냥 써도 되는건지 선택
	transform = (_containerObjectTransform != nullptr) ? _containerObjectTransform : m_pTransformCom;
	model = (_masterModel != nullptr) ? _masterModel : m_pModelCom;

	//  루트 모션 회전 델타를 먼저 적용 (이동보다 선행해야 방향이 갱신됨)
	_vector qRotationDelta = model->Get_RootMotionRotationDelta();
	transform->Rotate_ByQuaternionDelta(qRotationDelta);

	// 모델에서 루트모션델타 가져오기
	_vector vRootMotionDelta = model->Get_RootMotionDelta();

	//cout << "RootDelta x,y,z = " << vDelta.x << ", " << vDelta.y << ", " << vDelta.z << endl;

	vRootMotionDelta = vRootMotionDelta / 100.f; //이거 언리얼 엔진은 cm 단위인데 블렌더는 m단위라서 블렌더로 뽑은거라서 다시 /100 해줘야함.

	//로컬 공간의 루트 모션 델타를 월드 공간의 벡터로 바꿔줘야 내가 바꾼 회전이 적용
	_vector vRight = transform->Get_State(DIRECTION::RIGHT);
	_vector vUp = transform->Get_State(DIRECTION::UP);
	_vector vLook = transform->Get_State(DIRECTION::LOOK);

	// 라 업 룩 방향으로 얼마나 이동했는지 Position에 더해주기
	_vector worldDelta = {};
	// 모델 생성할때 Y축 반대로 했으면 reverse해줘야함
	_float fSignX = _reverseX ? -1.f : 1.f;
	_float fSignY = _reverseY ? -1.f : 1.f;
	_float fSignZ = _reverseZ ? -1.f : 1.f;

	worldDelta = vRight * (fSignX * XMVectorGetX(vRootMotionDelta))
		+ vUp * (fSignZ * XMVectorGetZ(vRootMotionDelta))
		+ vLook * (fSignY * XMVectorGetY(vRootMotionDelta));

	return worldDelta;
}
/******************************************************* 루트모션 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 컴포넌트 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Character::Add_Model(const _wstring& _modelName)
{
	MSG_FAIL(Add_Component(m_iLevel, _modelName, Com_Model, RCAST(Component**)(&m_pModelCom)), L"모델 추가 실패!", L"실패!!!", E_FAIL);
	m_pModelCom->Set_OwnerId(m_iObjectID);
	return S_OK;
}

PxCapsuleController* Engine::Character::Create_Controller(PHYSX_CONTROLLER_DESC& _controllerDesc)
{
	PxCapsuleController* capsuleController = CAST(PxCapsuleController*)(m_pGameInstance->PhysX_Create_Controller(_controllerDesc));
	return capsuleController;
}
/******************************************************* 컴포넌트 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 디버그 함수 ////////////////////////////////////////////////////////
#ifdef _DEBUG
void Engine::Character::Add_Debug_Controller_Capsule(const _float fTimeDelta)
{
	CHECK_NULL_RESULT(m_pController, );

	CAPSULE_DESC desc;
	desc.vCenter = To_float3(m_pController->getPosition());
	desc.fRadius = m_pController->getRadius();
	desc.fHalfHeight = m_pController->getHeight();

	if (m_bHit)
	{
		m_fHitViewTime += fTimeDelta;
		if (m_fHitViewTime >= 0.15f)
		{
			m_fHitViewTime = 0.f;
			m_bHit = false;
		}

		desc.vColor = _float3(1.f, 0.1f, 0.1f);
	}

	m_pGameInstance->Add_Debug_Capsule(desc);
}
#endif // _DEBUG
/******************************************************* 디버그 함수 *******************************************************/



//////////////////////////////////////////////////////// PhysX Bone Chain 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Character::BoneChain_Build(const BONE_CHAIN_BUILD_DESC& _buildDesc, Model* _masterModel)
{
	m_vecBoneChains.clear();
	m_tBoneChainBuildDesc = _buildDesc;
	m_pBoneChainMasterModel = _masterModel;

	// 마스터 모델이나 본 소스 모델이 없으면 빌드 불가
	if (_masterModel == nullptr || _buildDesc.pBoneSourceModel == nullptr)
		return E_FAIL;

	// 소켓 등록 (벡터 재할당 완료시킴)
	for (const BONE_CHAIN_DEFINE& define : _buildDesc.vecChainDefines)
	{
		// 체인의 부모 본을 소켓으로 등록 (나중에 행렬 포인터를 가져오기 위함)
		_masterModel->Register_SocketBoneName(define.strParentBone);

		// 체인의 첫 번째 본이 마스터 모델에 존재하면 소켓으로 추가 등록 (키네마틱 본용)
		if (!define.vecBones.empty())
		{
			if (_masterModel->Get_BoneIndex(define.vecBones[0].strName.c_str()) != -1)
				_masterModel->Register_SocketBoneName(define.vecBones[0].strName);
		}
	}

	// 체인 빌드
	const vector<Bone*>& sourceBones = _buildDesc.pBoneSourceModel->Get_Bones();

	// 사용할 본 체인 개수만큼 미리 할당
	m_vecBoneChains.reserve(_buildDesc.vecChainDefines.size());

	// 본 체인 만들어주기
	for (const BONE_CHAIN_DEFINE& define : _buildDesc.vecChainDefines)
	{
		CHAIN_BONE_DESC tChain = {};
		tChain.strParentBoneName = define.strParentBone;

		// 등록해둔 소켓에서 부모 본의 Combined 행렬 포인터를 가져옴
		tChain.iParentSocketNum = _masterModel->Get_SocketBoneIndex(tChain.strParentBoneName);

		// 부모 본 체인이 없으면(해당 본이 부모라면) 컨티뉴
		if (tChain.iParentSocketNum == -1)
			continue;

		// 본 유효성 검사
		_bool chainValid = true;
		tChain.vecBones.reserve(define.vecBones.size());

		for (const BONE_CHAIN_DEFINE::BONE_ENTRY& tBoneDef : define.vecBones)
		{
			CHAIN_BONE_INFO tInfo = {};
			tInfo.strBoneName = tBoneDef.strName;
			tInfo.iChainParentIndex = tBoneDef.iChainParent;

			// 마스터 모델에서 본 인덱스를 찾음 (나중에 Combined 행렬 덮어쓰기에 사용)
			tInfo.iMasterBoneIndex = _masterModel->Get_BoneIndex(tBoneDef.strName.c_str());

			// 마스터 모델에 해당 본이 없으면 체인 무효 처리
			if (tInfo.iMasterBoneIndex == -1)
			{
				chainValid = false;
				break;
			}

			// 본 소스 모델에서도 인덱스를 찾음 (레스트포즈 로컬 행렬 추출용)
			_int iBoneIndex = _buildDesc.pBoneSourceModel->Get_BoneIndex(tBoneDef.strName.c_str());
			if (iBoneIndex == -1)
			{
				chainValid = false;
				break;
			}

			// 소스 모델의 로컬 변환 행렬을 레스트포즈로 저장
			tInfo.matRestPoseLocal = sourceBones[iBoneIndex]->Get_TransformationMatrix();
			tChain.vecBones.push_back(tInfo);
		}

		// 모든 본이 유효하고 비어있지 않을 때만 체인 등록
		if (chainValid && !tChain.vecBones.empty())
			m_vecBoneChains.push_back(tChain);
	}

	COUT("[BoneChain] 유효 체인 수: " << m_vecBoneChains.size());

	return S_OK;
}

HRESULT Engine::Character::BoneChain_CreatePhysX(const _float4x4& _worldMatrix)
{
	if (m_pBoneChainMasterModel == nullptr)
		return E_FAIL;

	for (_uint index = 0; index < (_uint)m_vecBoneChains.size(); ++index)
	{
		const CHAIN_BONE_DESC& tChain = m_vecBoneChains[index];
		if (tChain.vecBones.empty())
			continue;

		// 체인의 첫 번째 본 = PhysX에서 키네마틱으로 고정될 루트 본
		const _string& strKinematicBone = tChain.vecBones[0].strBoneName;

		const _float4x4* parentMatrix = m_pBoneChainMasterModel->Get_SocketBoneMatrixPtr(tChain.iParentSocketNum);
		_matrix matParentCombined = XMLoadFloat4x4(parentMatrix);

		// 레스트포즈 Combined 계산 (로컬 행렬을 부모부터 순차적으로 누적)
		vector<_float4x4> vecCombinedCache = {};
		vecCombinedCache.resize(tChain.vecBones.size());

		for (_uint i = 0; i < (_uint)tChain.vecBones.size(); ++i)
		{
			const CHAIN_BONE_INFO& tInfo = tChain.vecBones[i];

			_matrix matParent = {};

			// 체인 내 부모가 없으면(-1) 소켓 부모 행렬 사용, 있으면 체인 내 부모의 Combined 사용
			if (tInfo.iChainParentIndex == -1)
				matParent = matParentCombined;
			else
				matParent = XMLoadFloat4x4(&vecCombinedCache[tInfo.iChainParentIndex]);

			// 로컬 * 부모 = Combined (월드 기준 레스트포즈)
			_matrix matCombined = XMLoadFloat4x4(&tInfo.matRestPoseLocal) * matParent;
			XMStoreFloat4x4(&vecCombinedCache[i], matCombined);
		}

		// PhysX 본 체인 생성에 필요한 DESC 구성
		vector<_string>   vecBoneNames = {};
		vector<_float4x4> vecInitialPoses = {};
		vector<_int>      vecParentIndices = {};

		for (_uint i = 0; i < (_uint)tChain.vecBones.size(); ++i)
		{
			// PhysX에 넘길 포즈에서 스케일 제거 (위치/회전만 유지)
			_matrix matScaleFree = XMLoadFloat4x4(&vecCombinedCache[i]);
			matScaleFree.r[0] = XMVector3Normalize(matScaleFree.r[0]);
			matScaleFree.r[1] = XMVector3Normalize(matScaleFree.r[1]);
			matScaleFree.r[2] = XMVector3Normalize(matScaleFree.r[2]);
			// r[3] (position)은 그대로 유지

			_float4x4 matNormalized = {};
			XMStoreFloat4x4(&matNormalized, matScaleFree);

			vecBoneNames.push_back(tChain.vecBones[i].strBoneName);
			vecInitialPoses.push_back(matNormalized); // 스케일 제거된 포즈
			vecParentIndices.push_back(tChain.vecBones[i].iChainParentIndex);
		}

		// 오프셋 + 루프 인덱스로 고유한 체인 ID 부여
		_uint iChainID = m_tBoneChainBuildDesc.iChainID_Offset + index;

		// 기본 파라미터를 복사하고 체인별 데이터만 덮어쓰기
		PHYSX_BONE_CHAIN_DESC desc = m_tBoneChainBuildDesc.tDefaultPhysXDesc;
		desc.iChainID = iChainID;
		desc.vecBoneNames = vecBoneNames;
		desc.vecInitialPoses = vecInitialPoses;
		desc.vecParentIndices = vecParentIndices;

		// PhysX 씬에 본 체인 액터 생성
		CHECK_FAILED(m_pGameInstance->PhysX_Create_BoneChain(
			desc, m_pBoneChainMasterModel, _worldMatrix), E_FAIL);

		// 생성된 PhysX 체인 정보를 관리 벡터에 저장
		PHYSX_CHAIN_INFO tPhysXInfo = {};
		tPhysXInfo.iChainID = iChainID;
		tPhysXInfo.strKinematicBoneName = strKinematicBone;

		m_vecBoneChainPhysX.push_back(tPhysXInfo);
	}

	// 소켓 번호 캐싱 (매 프레임 키네마틱 업데이트 시 빠르게 접근하기 위함)
	for (PHYSX_CHAIN_INFO& tInfo : m_vecBoneChainPhysX)
	{
		tInfo.iKinematicSocketNum = m_pBoneChainMasterModel->Get_SocketBoneIndex(tInfo.strKinematicBoneName);
		if (tInfo.iKinematicSocketNum == -1)
		{
			COUT("[BoneChain] Kinematic Socket 캐싱 실패 : " << tInfo.strKinematicBoneName);
			return E_FAIL;
		}
	}

	return S_OK;
}

void Engine::Character::BoneChain_UpdateRestPose()
{
	if (m_vecBoneChains.empty() || m_pBoneChainMasterModel == nullptr)
		return;

	// 마스터 모델의 Combined 행렬을 레스트포즈로 덮어쓸 목록
	vector<pair<_int, _float4x4>> vecOverwrite = {};

	for (const CHAIN_BONE_DESC& tChain : m_vecBoneChains)
	{
		if (tChain.iParentSocketNum == -1 || tChain.vecBones.empty())
			continue;

		// 소켓 부모의 현재 Combined 행렬을 루트 기준으로 사용
		const _float4x4* parentMatrix = m_pBoneChainMasterModel->Get_SocketBoneMatrixPtr(tChain.iParentSocketNum);
		_matrix matParentCombined = XMLoadFloat4x4(parentMatrix);

		// 체인 내 본들의 Combined 행렬을 임시 캐싱 (자식이 부모 결과를 참조해야 하므로)
		vector<_float4x4> vecCombinedCache = {};
		vecCombinedCache.resize(tChain.vecBones.size());

		for (_uint i = 0; i < (_uint)tChain.vecBones.size(); ++i)
		{
			const CHAIN_BONE_INFO& tInfo = tChain.vecBones[i];

			_matrix matParent = {};
			if (tInfo.iChainParentIndex == -1)
				matParent = matParentCombined;
			else
				matParent = XMLoadFloat4x4(&vecCombinedCache[tInfo.iChainParentIndex]);

			_matrix matCombined = XMLoadFloat4x4(&tInfo.matRestPoseLocal) * matParent;
			XMStoreFloat4x4(&vecCombinedCache[i], matCombined);

			_float4x4 matResult = {};
			XMStoreFloat4x4(&matResult, matCombined);

			// (마스터 본 인덱스, 계산된 Combined 행렬) 쌍으로 덮어쓰기 목록에 추가
			vecOverwrite.push_back({ tInfo.iMasterBoneIndex, matResult });
		}
	}

	// 한 번에 모아서 마스터 모델의 Combined 행렬을 레스트포즈로 덮어씀
	if (!vecOverwrite.empty())
		m_pBoneChainMasterModel->Overwrite_CombinedMatrices(vecOverwrite);
}

void Engine::Character::BoneChain_UpdateKinematic(const _float4x4& _worldMatrix)
{
	for (_uint index = 0; index < (_uint)m_vecBoneChains.size(); ++index)
	{
		if (index >= (_uint)m_vecBoneChainPhysX.size())
			break;
		const CHAIN_BONE_DESC& tChain = m_vecBoneChains[index];
		const PHYSX_CHAIN_INFO& tInfo = m_vecBoneChainPhysX[index];
		if (tChain.vecBones.empty())
			continue;

		const _float4x4* parentMatrix = m_pBoneChainMasterModel->Get_SocketBoneMatrixPtr(tChain.iParentSocketNum);
		if (parentMatrix == nullptr)
			continue;

		_matrix matParentCombined = XMLoadFloat4x4(parentMatrix);
		_matrix matKinematic = XMLoadFloat4x4(&tChain.vecBones[0].matRestPoseLocal) * matParentCombined;

		// CreatePhysX와 동일하게 스케일 제거
		matKinematic.r[0] = XMVector3Normalize(matKinematic.r[0]);
		matKinematic.r[1] = XMVector3Normalize(matKinematic.r[1]);
		matKinematic.r[2] = XMVector3Normalize(matKinematic.r[2]);

		_float4x4 matKinResult = {};
		XMStoreFloat4x4(&matKinResult, matKinematic);

		m_pGameInstance->PhysX_Update_BoneChain_Kinematic(
			tInfo.iChainID,
			matKinResult,
			_worldMatrix
		);
	}
}

void Engine::Character::BoneChain_FetchResults(const _float4x4& _worldMatrix, _float _fDesiredScale)
{
	if (m_pBoneChainMasterModel == nullptr)
		return;

	for (_uint chainIndex = 0; chainIndex < (_uint)m_vecBoneChainPhysX.size(); ++chainIndex)
	{
		const PHYSX_CHAIN_INFO& tInfo = m_vecBoneChainPhysX[chainIndex];

		m_vecBoneChainFetchScratch.clear();
		m_pGameInstance->PhysX_Fetch_BoneChain_Results(
			tInfo.iChainID, _worldMatrix, m_vecBoneChainFetchScratch);

		// RestPose 위치 고정 모드가 아니면 기존 방식 사용
		if (!m_tBoneChainBuildDesc.bUseRestPosePosition)
		{
			for (auto& result : m_vecBoneChainFetchScratch)
			{
				_matrix matBone = XMLoadFloat4x4(&result.second);
				matBone.r[0] = XMVector3Normalize(matBone.r[0]) * _fDesiredScale;
				matBone.r[1] = XMVector3Normalize(matBone.r[1]) * _fDesiredScale;
				matBone.r[2] = XMVector3Normalize(matBone.r[2]) * _fDesiredScale;
				XMStoreFloat4x4(&result.second, matBone);
			}

			if (!m_vecBoneChainFetchScratch.empty())
				m_pBoneChainMasterModel->Overwrite_CombinedMatrices(m_vecBoneChainFetchScratch);

			continue;
		}

		if (chainIndex >= (_uint)m_vecBoneChains.size())
			continue;

		const CHAIN_BONE_DESC& tChain = m_vecBoneChains[chainIndex];

		const _float4x4* parentMatrix = m_pBoneChainMasterModel->Get_SocketBoneMatrixPtr(tChain.iParentSocketNum);
		if (parentMatrix == nullptr)
			continue;

		// RestPose Combined 재계산
		_matrix matParentCombined = XMLoadFloat4x4(parentMatrix);
		vector<_float4x4> vecRestCache = {};
		vecRestCache.resize(tChain.vecBones.size());

		for (_uint i = 0; i < (_uint)tChain.vecBones.size(); ++i)
		{
			const CHAIN_BONE_INFO& boneInfo = tChain.vecBones[i];
			_matrix matParent = {};
			if (boneInfo.iChainParentIndex == -1)
				matParent = matParentCombined;
			else
				matParent = XMLoadFloat4x4(&vecRestCache[boneInfo.iChainParentIndex]);

			_matrix matCombined = XMLoadFloat4x4(&boneInfo.matRestPoseLocal) * matParent;
			XMStoreFloat4x4(&vecRestCache[i], matCombined);
		}

		// PhysX 회전 + RestPose 위치/스케일로 재조합
		vector<pair<_int, _float4x4>> vecFinal = {};

		for (auto& result : m_vecBoneChainFetchScratch)
		{
			_int iChainBoneIndex = -1;
			for (_uint i = 0; i < (_uint)tChain.vecBones.size(); ++i)
			{
				if (tChain.vecBones[i].iMasterBoneIndex == result.first)
				{
					iChainBoneIndex = (_int)i;
					break;
				}
			}

			if (iChainBoneIndex == -1)
				continue;

			// PhysX 결과에서 회전 추출
			_matrix matPhysX = XMLoadFloat4x4(&result.second);
			_vector vPhysXScale = {}, vPhysXRot = {}, vPhysXPos = {};
			XMMatrixDecompose(&vPhysXScale, &vPhysXRot, &vPhysXPos, matPhysX);

			// RestPose에서 위치/스케일 추출
			_matrix matRest = XMLoadFloat4x4(&vecRestCache[iChainBoneIndex]);
			_vector vRestScale = {}, vRestRot = {}, vRestPos = {};
			XMMatrixDecompose(&vRestScale, &vRestRot, &vRestPos, matRest);

			// RestPose 스케일 + PhysX 회전 + RestPose 위치
			_matrix matResult = XMMatrixAffineTransformation(
				vRestScale,
				XMVectorZero(),
				vPhysXRot,
				vRestPos
			);

			_float4x4 matStore = {};
			XMStoreFloat4x4(&matStore, matResult);
			vecFinal.push_back({ result.first, matStore });
		}

		if (!vecFinal.empty())
			m_pBoneChainMasterModel->Overwrite_CombinedMatrices(vecFinal);
	}
}

void Engine::Character::BoneChain_UpdateKinematic_Before(const _float4x4& _worldMatrix)
{
	// 각 PhysX 체인의 키네마틱 본 위치를 현재 애니메이션 결과로 동기화
	for (const PHYSX_CHAIN_INFO& tInfo : m_vecBoneChainPhysX)
	{
		if (tInfo.iKinematicSocketNum == -1)
			continue;

		const _float4x4* parentMatrix = m_pBoneChainMasterModel->Get_SocketBoneMatrixPtr(tInfo.iKinematicSocketNum);
		if (parentMatrix == nullptr)
			continue;

		// PhysX 쪽 키네마틱 액터의 트랜스폼을 소켓 행렬 * 월드 행렬로 갱신
		m_pGameInstance->PhysX_Update_BoneChain_Kinematic(
			tInfo.iChainID,
			*parentMatrix,
			_worldMatrix
		);
	}
}

void Engine::Character::BoneChain_FetchResults_Before(const _float4x4& _worldMatrix, _float _fDesiredScale)
{
	if (m_pBoneChainMasterModel == nullptr)
		return;

	for (const PHYSX_CHAIN_INFO& tInfo : m_vecBoneChainPhysX)
	{
		// PhysX 시뮬레이션 결과를 (본이름, 행렬) 쌍으로 가져옴
		m_vecBoneChainFetchScratch.clear();
		m_pGameInstance->PhysX_Fetch_BoneChain_Results(
			tInfo.iChainID, _worldMatrix, m_vecBoneChainFetchScratch);

		// PhysX 결과 행렬의 스케일을 원하는 크기로 정규화 (시뮬레이션 중 스케일이 틀어질 수 있으므로)
		for (auto& result : m_vecBoneChainFetchScratch)
		{
			_matrix matBone = XMLoadFloat4x4(&result.second);

			matBone.r[0] = XMVector3Normalize(matBone.r[0]) * _fDesiredScale;
			matBone.r[1] = XMVector3Normalize(matBone.r[1]) * _fDesiredScale;
			matBone.r[2] = XMVector3Normalize(matBone.r[2]) * _fDesiredScale;

			XMStoreFloat4x4(&result.second, matBone);
		}

		// 정규화된 결과로 마스터 모델의 Combined 행렬을 덮어씀 (최종 렌더링에 반영)
		if (!m_vecBoneChainFetchScratch.empty())
			m_pBoneChainMasterModel->Overwrite_CombinedMatrices(m_vecBoneChainFetchScratch);
	}
}

void Engine::Character::BoneChain_AutoBuildDefines(
	Model* _pBoneSourceModel,
	const _string& _strParentBoneName,
	vector<BONE_CHAIN_DEFINE>& _outDefines)
{
	_outDefines.clear();

	if (_pBoneSourceModel == nullptr)
		return;

	// 1단계 : 모든 메시에서 실제 스키닝 본 인덱스 수집
	set<_uint> activeSet = {};
	for (Mesh* pMesh : _pBoneSourceModel->Get_Meshes())
	{
		set<_uint> meshActive = pMesh->Get_ActiveBoneIndices();
		activeSet.insert(meshActive.begin(), meshActive.end());
	}

	// 2단계 : 부모 본 인덱스 찾기
	const vector<Bone*>& vecBones = _pBoneSourceModel->Get_Bones();
	_int iParentBoneIndex = _pBoneSourceModel->Get_BoneIndex(_strParentBoneName.c_str());
	if (iParentBoneIndex == -1)
		return;

	// activeSet에서 부모 본 자체는 제외
	activeSet.erase((_uint)iParentBoneIndex);

	// 3단계 : 부모 본의 직속 자식 중 active인 본을 체인 루트로 식별
	// 각 active 본에서 부모를 타고 올라가서, 부모 본 바로 아래 자식을 찾음
	// 같은 직속 자식을 공유하는 본들은 같은 체인
	map<_uint, vector<_uint>> mapChainRootToMembers = {}; // 직속자식 인덱스 -> 해당 체인에 속하는 본 인덱스들

	for (_uint iBoneIndex : activeSet)
	{
		// 이 본에서 부모를 타고 올라가서 _strParentBoneName 직속 자식을 찾기
		_int iCurrent = (_int)iBoneIndex;
		_int iDirectChild = -1;

		while (iCurrent != -1)
		{
			_int iParent = vecBones[iCurrent]->Get_ParentBoneIndex();
			if (iParent == iParentBoneIndex)
			{
				// 현재 본이 부모 본의 직속 자식
				iDirectChild = iCurrent;
				break;
			}
			iCurrent = iParent;
		}

		if (iDirectChild == -1)
			continue; // 이 본은 부모 본의 하위가 아님

		mapChainRootToMembers[(_uint)iDirectChild].push_back(iBoneIndex);
	}

	// 4단계 : 각 체인 루트별로 체인 정의 생성
	for (auto& [iRootIndex, vecMembers] : mapChainRootToMembers)
	{
		// 체인 멤버를 계층 순서로 정렬 (부모가 먼저 오도록)
		// 각 본의 depth를 계산해서 정렬
		struct BoneDepthInfo
		{
			_uint iBoneIndex = {};
			_int  iDepth = {};
		};

		vector<BoneDepthInfo> vecSorted = {};
		for (_uint idx : vecMembers)
		{
			_int iDepth = 0;
			_int iCur = (_int)idx;
			while (iCur != iParentBoneIndex && iCur != -1)
			{
				++iDepth;
				iCur = vecBones[iCur]->Get_ParentBoneIndex();
			}
			vecSorted.push_back({ idx, iDepth });
		}

		sort(vecSorted.begin(), vecSorted.end(),
			[](const BoneDepthInfo& _a, const BoneDepthInfo& _b) { return _a.iDepth < _b.iDepth; });

		// 체인 내 인덱스 매핑 (본 인덱스 -> 체인 내 순서)
		map<_uint, _int> mapBoneToChainIndex = {};

		BONE_CHAIN_DEFINE tDefine = {};
		tDefine.strParentBone = _strParentBoneName;

		for (_uint i = 0; i < (_uint)vecSorted.size(); ++i)
		{
			_uint iBoneIdx = vecSorted[i].iBoneIndex;
			const _string& strName = vecBones[iBoneIdx]->Get_BoneName();

			// 체인 내 부모 인덱스 결정
			_int iChainParent = -1;
			_int iBoneParent = vecBones[iBoneIdx]->Get_ParentBoneIndex();

			// 부모가 체인 내에 있으면 그 인덱스, 없으면 -1 (체인 루트)
			if (iBoneParent != iParentBoneIndex)
			{
				auto it = mapBoneToChainIndex.find((_uint)iBoneParent);
				if (it != mapBoneToChainIndex.end())
					iChainParent = it->second;
				else
					iChainParent = -1; // 부모가 active가 아닌 경우 (루트 취급)
			}

			BONE_CHAIN_DEFINE::BONE_ENTRY tEntry = {};
			tEntry.strName = strName;
			tEntry.iChainParent = iChainParent;

			tDefine.vecBones.push_back(tEntry);
			mapBoneToChainIndex[iBoneIdx] = (_int)i;
		}

		if (!tDefine.vecBones.empty())
			_outDefines.push_back(tDefine);
	}

	COUT("[BoneChain] AutoBuild - 부모: " << _strParentBoneName
		<< " 체인 수: " << _outDefines.size()
		<< " (active 본: " << activeSet.size() << ")");
}

void Engine::Character::BoneChain_Cleanup()
{
	// PhysX 씬에서 모든 본 체인 액터 제거
	for (const PHYSX_CHAIN_INFO& tInfo : m_vecBoneChainPhysX)
	{
		m_pGameInstance->PhysX_Remove_BoneChain(tInfo.iChainID);
	}

	m_vecBoneChainPhysX.clear();
	m_vecBoneChains.clear();
	m_iBoneChainWaitFrames = 0;
}

void Engine::Character::BoneChain_UpdateAll(const _float4x4& _worldMatrix, _float _fDesiredScale)
{
	if (m_vecBoneChains.empty())
		return;

	// 프레임 0~1 : PhysX 생성 전이므로 레스트포즈만 적용 (본이 안정된 상태를 확보)
	if (m_iBoneChainWaitFrames < 2)
	{
		++m_iBoneChainWaitFrames;
		BoneChain_UpdateRestPose();
	}
	// 프레임 2 : 레스트포즈 적용 후 PhysX 액터를 생성 (안정된 초기 포즈 기반)
	else if (m_iBoneChainWaitFrames == 2)
	{
		BoneChain_UpdateRestPose();
		if (SUCCEEDED(BoneChain_CreatePhysX(_worldMatrix)))
			m_iBoneChainWaitFrames = 3;
		else
			BoneChain_UpdateRestPose();
	}

	// 프레임 3~ : PhysX 시뮬레이션 루프 (키네마틱 동기화 -> 결과 페치)
	if (m_iBoneChainWaitFrames == 3)
	{
		BoneChain_UpdateRestPose();
		BoneChain_UpdateKinematic(_worldMatrix);
		BoneChain_FetchResults(_worldMatrix, _fDesiredScale);
	}
}

void Engine::Character::BoneChain_UpdateAll_Before(const _float4x4& _worldMatrix, _float _fDesiredScale)
{
	if (m_vecBoneChains.empty())
		return;

	// 프레임 0~1 : PhysX 생성 전이므로 레스트포즈만 적용 (본이 안정된 상태를 확보)
	if (m_iBoneChainWaitFrames < 2)
	{
		++m_iBoneChainWaitFrames;
		BoneChain_UpdateRestPose();
	}
	// 프레임 2 : 레스트포즈 적용 후 PhysX 액터를 생성 (안정된 초기 포즈 기반)
	else if (m_iBoneChainWaitFrames == 2)
	{
		BoneChain_UpdateRestPose();
		if (SUCCEEDED(BoneChain_CreatePhysX(_worldMatrix)))
			m_iBoneChainWaitFrames = 3;
		else
			BoneChain_UpdateRestPose();
	}

	// 프레임 3~ : PhysX 시뮬레이션 루프 (키네마틱 동기화 -> 결과 페치)
	if (m_iBoneChainWaitFrames == 3)
	{
		//BoneChain_UpdateRestPose();
		BoneChain_UpdateKinematic_Before(_worldMatrix);
		BoneChain_FetchResults_Before(_worldMatrix, _fDesiredScale);
	}
}
/******************************************************* PhysX Bone Chain 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Character::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 텔레포토 ////////////////////////////////////////////////////////
void Engine::Character::Teleport(_fvector vTargetPos)
{
	m_pTransformCom->Set_State(DIRECTION::POSITION, vTargetPos);

	if (m_pController)
	{
		m_pController->setFootPosition(ToPxExtendedVec3(vTargetPos));
	}
}

void Engine::Character::Teleport(_fvector _targetPosition, _float4 _Rotation)
{
	m_pTransformCom->Set_State(DIRECTION::POSITION, _targetPosition);
	m_pTransformCom->Rotation(XMConvertToRadians(_Rotation.x), XMConvertToRadians(_Rotation.y), XMConvertToRadians(_Rotation.z));
	if (m_pController)
	{
		m_pController->setFootPosition(ToPxExtendedVec3(_targetPosition));
	}
}

void Engine::Character::Teleport(_float4 _targetPosition)
{
	m_pTransformCom->Set_State(DIRECTION::POSITION, _targetPosition);

	if (m_pController)
	{
		m_pController->setFootPosition(physx::PxExtendedVec3(_targetPosition.x, _targetPosition.y, _targetPosition.z));
	}
}

void Engine::Character::Teleport(_float4 _targetPosition, _float4 _Rotation)
{

	m_pTransformCom->Set_State(DIRECTION::POSITION, _targetPosition);
	m_pTransformCom->Rotation(XMConvertToRadians(_Rotation.x), XMConvertToRadians(_Rotation.y), XMConvertToRadians(_Rotation.z));
	if (m_pController)
	{
		m_pController->setFootPosition(physx::PxExtendedVec3(_targetPosition.x, _targetPosition.y, _targetPosition.z));
	}
}
/******************************************************* 텔레포토 *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 세팅 ////////////////////////////////////////////////////////
void Engine::Character::Set_Shader(const _wstring& _shaderName)
{
	m_pShaderCom = m_pGameInstance->Get_Shader_Prototype(_shaderName);
}
/******************************************************* 쉐이더 세팅 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Character::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
