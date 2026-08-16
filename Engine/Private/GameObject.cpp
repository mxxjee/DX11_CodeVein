#include "Engine_Define.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Mesh.h"

#include "StructuredBuffer.h"
#include "Renderer.h"

_uint GameObject::m_iTotalID = 0;

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::GameObject::GameObject()
{
}

Engine::GameObject::GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

Engine::GameObject::GameObject(const GameObject& original)
	: m_pDevice(original.m_pDevice), m_pContext(original.m_pContext), m_pGameInstance(original.m_pGameInstance), m_iLevel(original.m_iLevel)
	, m_wstrPrototypeName(original.m_wstrPrototypeName), m_wstrModelName(original.m_wstrModelName)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
	m_bIsClone = true;
	m_iObjectID = ++m_iTotalID;
	m_pGameInstance->Set_Parallel_Dirty();
}

Engine::GameObject::~GameObject()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::GameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::GameObject::Initialize(void* arg)
{
	m_pTransformCom = Transform::Create(m_pDevice, m_pContext);
	CHECK_NULLPTR(m_pTransformCom);
	m_umapComponents.emplace(Com_Transform, m_pTransformCom);

	if(arg != nullptr)
	{
		GAMEOBJECT_DESC* desc = static_cast<GAMEOBJECT_DESC*>(arg);

		m_wstrName = desc->wstrName;
		m_bIsActive = desc->bIsActive;
		m_bIsVisible = desc->bIsVisible;
		m_bIsDead = desc->bIsDead;
		m_wstrShaderName = desc->wstrShaderName;
		m_wstrModelName = desc->wstrModelName;
		m_vecObjPass = desc->vecPasses;

		m_pTransformCom->Initialize(arg);
	}

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Engine::GameObject::Add_Component(_uint prototypelevelID, const _wstring& prototypename, const _wstring& componentname, Component** outcomponent, void* arg)
{
	if (m_umapComponents.contains(componentname))
	{
		_wstring message = componentname + L"컴포넌트가 이미 존재합니다.";
		_wstring caption = Get_Name() + L"오브젝트 컴포넌트 추가 실패";
		MSG_ON(message.c_str(), caption.c_str());
		BREAK;
		return E_FAIL;
	}

	*outcomponent = CAST(Component*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, prototypelevelID, prototypename, arg));

	MSG_NULL(*outcomponent, L"컴포넌트가 nullptr입니다.", L"오류!!!", E_FAIL);

	m_umapComponents.emplace(componentname, *outcomponent);

	// 쉐이더라면 쉐이더 이름을 저장
	if (DCAST(Shader*)(*outcomponent) != nullptr)
	{
		m_wstrShaderName = prototypename;
	}

	return S_OK;
}

HRESULT Engine::GameObject::Add_StructuredBuffer(StructuredBuffer*& _buffer, SBUSAGE _usage, _uint _numElements, _uint _stride, const void* _initialData)
{
	_buffer = StructuredBuffer::Create(m_pDevice, m_pContext, _usage, _numElements, _stride, _initialData);
	m_vecCleanUpTargets.push_back(_buffer);
	return S_OK;
}

HRESULT Engine::GameObject::Add_StructuredBuffer(SB_DESC& _desc)
{
	return Add_StructuredBuffer(_desc.pBuffer, _desc.eUsage, _desc.iNumElements, _desc.iStride, _desc.initialData);
}

HRESULT Engine::GameObject::Add_Collider(COLLISION_GROUP _groupTag, COLLIDER _type, Collider** _outCollider, void* _initialData)
{
	_wstring checkName = Com_Collider;
	_wstring componentName = checkName;
	_uint nameNum = 0;
	// Collider의 이름을 Com_Collider, Com_Collider0, Com_Collider1, 이런식으로 없는게 나올때까지 찾음
	while (m_umapComponents.contains(componentName))
	{
		componentName = checkName + to_wstring(nameNum);
		++nameNum;
	}

	_wstring colliderType = L"";
	switch (_type)
	{
	case COLLIDER::AABB:
		colliderType = L"Prototype_Component_Collider_AABB";
		break;
	case COLLIDER::OBB:
		colliderType = L"Prototype_Component_Collider_OBB";
		break;
	case COLLIDER::SPHERE:
		colliderType = L"Prototype_Component_Collider_Sphere";
		break;
	}

	*_outCollider = CAST(Collider*)(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, colliderType, _initialData));

	MSG_NULL(*_outCollider, L"컴포넌트가 nullptr입니다.", L"오류!!!", E_FAIL);

	m_pGameInstance->Register_Collider(_groupTag, *_outCollider);

	m_umapComponents.emplace(componentName, *_outCollider);

	return S_OK;
}

Component* Engine::GameObject::Get_Component_FromName(const _wstring& _componentName)
{
    if (!m_umapComponents.contains(_componentName))
    {
        //MSG_ON((_componentName + L" 컴포넌트가 없습니다.").c_str(), L"컴포넌트 검색 실패");
        //BREAK;
        return nullptr;
    }

    return m_umapComponents[_componentName];
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 변수 겟 함수 ////////////////////////////////////////////////////////
_float3 Engine::GameObject::Get_Scale()
{
    return m_pTransformCom->Get_Scale();
}

_float Engine::GameObject::Get_Speed()
{
    return m_pTransformCom->Get_Speed();
}
/******************************************************* 변수 겟 함수 *******************************************************/



//////////////////////////////////////////////////////// 변수 세팅 함수 ////////////////////////////////////////////////////////
void Engine::GameObject::Set_State(DIRECTION _direction, _fvector _value)
{
    m_pTransformCom->Set_State(_direction, _value);
}

void Engine::GameObject::Set_State(DIRECTION _direction, _float4& _value)
{
    m_pTransformCom->Set_State(_direction, _value);
}

void Engine::GameObject::Set_Scale(const _float ScaleX, const _float ScaleY, const _float ScaleZ)
{
    m_pTransformCom->Set_Scale(ScaleX, ScaleY, ScaleZ);
}

void Engine::GameObject::Set_Dead(_bool _isalive)
{
	m_bIsDead = _isalive;
	if(m_bIsDead)
	{
		DEAD_EVENT event;
		event.bDeadObject = true;
		Publish(event);
	}
}

void Engine::GameObject::Set_Speed(const _float _speed)
{
    m_pTransformCom->Set_Speed(_speed);
}

void Engine::GameObject::Rotation(_fvector _axis, const _float _radian)
{
    m_pTransformCom->Rotation(_axis, _radian);
}

void Engine::GameObject::Rotation(_float _fRadianX, _float _fRadianY, _float _fRadianZ)
{
    m_pTransformCom->Rotation(_fRadianX, _fRadianY, _fRadianZ);
}

void Engine::GameObject::Add_Object_To_Octree(RENDER_GROUP _group)
{
	BoundingBox boundingaabb = Calculate_WorldAABB();
	m_pGameInstance->Add_Object_To_Octree(_group, this, boundingaabb);
	Component* model = Get_Component_FromName(Com_Model);
	if (model)
	{
		CAST(Model*)(model)->LocalAABB_To_WorldAABB(boundingaabb);
	}
}

BoundingBox Engine::GameObject::Calculate_WorldAABB()
{
	BoundingBox worldAABB = {};

	// 모델이 있을 경우
	Model* pModel = Get_Model();
	if (pModel)
	{
		vector<Mesh*>& meshes = pModel->Get_Meshes();

		// Mesh가 있는지 확인
		if (meshes.empty())
		{
			// Mesh가 없으면 기본 크기 사용
			worldAABB.Center = {};
			worldAABB.Extents = _float3(0.5f, 0.5f, 0.5f);
		}
		else
		{
			worldAABB = pModel->Get_LocalAABB();
		}
	}
	else
	{
		// Model이 없으면 기본 크기 사용
		worldAABB.Center = {};
		worldAABB.Extents = _float3(0.5f, 0.5f, 0.5f);
	}

	// Transform 적용 (World Matrix)
	_matrix worldMatrix = m_pTransformCom->Get_WorldMatrix();
	worldAABB.Transform(worldAABB, worldMatrix);

	return worldAABB;
}

#ifdef _DEBUG
void Engine::GameObject::Add_Debug_Capsule(_fvector _vStart, _fvector _vEnd, _float _radius, _float3 _color)
{
	// 중심점
	_vector vCenter = (_vStart + _vEnd) * 0.5f;
	// 방향/거리
	_vector vDir = _vEnd - _vStart;
	float fLength = {};
	XMStoreFloat(&fLength, XMVector3Length(vDir));
	if (fLength < 0.001f)
		return;
	// 기본 축(Y-up)에서 실제 방향으로 회전
	_vector vDefaultAxis = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vector vDirNorm = XMVector3Normalize(vDir);
	float fDot = {};
	XMStoreFloat(&fDot, XMVector3Dot(vDefaultAxis, vDirNorm));
	_vector qOrientation = {};
	// 내적값이 너무 크면 정규화
	if (fDot > 0.9999f)
		qOrientation = XMQuaternionIdentity();
	else if (fDot < -0.9999f) // 너무 작
		qOrientation = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XM_PI);
	else
	{
		_vector vCross = XMVector3Cross(vDefaultAxis, vDirNorm);
		_float fAngle = acosf(fDot);
		qOrientation = XMQuaternionRotationAxis(XMVector3Normalize(vCross), fAngle);
	}
	CAPSULE_DESC desc = {};
	XMStoreFloat3(&desc.vCenter, vCenter);
	XMStoreFloat4(&desc.qOrientation, qOrientation);
	desc.fHalfHeight = fLength * 0.5f;
	desc.fRadius = _radius;
	desc.vColor = _color;
	m_pGameInstance->Add_Debug_Capsule(desc);
}

void Engine::GameObject::Add_Debug_Capsule(const _float3& _startPosition, const _float3& _endPosition, _float _radius, _float3 _color)
{
	_vector start = XMVectorSetW(XMLoadFloat3(&_startPosition), 1.f);
	_vector end = XMVectorSetW(XMLoadFloat3(&_endPosition), 1.f);

	Add_Debug_Capsule(start, end, _radius, _color);
}

void Engine::GameObject::Add_Debug_Capsule(const _float4& _startPosition, const _float4& _endPosition, _float _radius, _float3 _color)
{
	_vector start = XMLoadFloat4(&_startPosition);
	_vector end = XMLoadFloat4(&_endPosition);

	Add_Debug_Capsule(start, end, _radius, _color);
}

void Engine::GameObject::Add_Debug_Capsule(_fmatrix _startMatrix, _fmatrix _endMatrix, _float _radius, _float3 _color)
{
	_vector vStart = _startMatrix.r[3];
	_vector vEnd = _endMatrix.r[3];

	Add_Debug_Capsule(vStart, vEnd, _radius, _color);
}

void Engine::GameObject::Add_Debug_Capsule(const _float4x4& _startMatrix, const _float4x4& _endMatrix, _float _radius, _float3 _color)
{
	Add_Debug_Capsule(XMLoadFloat4x4(&_startMatrix).r[3], XMLoadFloat4x4(&_endMatrix).r[3], _radius, _color);
}

void Engine::GameObject::Add_Debug_Sphere(BoundingSphere _sphere, _float3 _color)
{
	m_pGameInstance->Add_Debug_Sphere(_sphere, _color);
}

void Engine::GameObject::Add_Debug_LookLine()
{
	_vector pos = Get_Position();
	_vector target = m_pTransformCom->Get_State(DIRECTION::LOOK);
	_float3 realpos = m_pTransformCom->Get_Position_Float3();
	_float3 finaltarget{};
	XMStoreFloat3(&finaltarget, pos + target);
	_float4 color = _float4(1.f, 1.f, 1.f, 1.f);

	m_pGameInstance->Add_Debug_LookLine(realpos, finaltarget, color);
}

void Engine::GameObject::Add_Debug_Fan(_float3 _center, _float3 _look, _float _radius, _float _halfAngle, _float3 _color)
{
	DebugFan fan;
	fan.vCenter = _center;
	fan.vLook = _look;
	fan.fRadius = _radius;
	fan.fHalfAngle = _halfAngle;
	fan.vColor = _color;
	m_pGameInstance->Add_Debug_Fan(fan);
}
#endif // _DEBUG


void Engine::GameObject::Unsubscribe_Event(EventHandle _handlenum)
{
	m_pGameInstance->UnsubScribe(_handlenum);

	for (auto iter = m_vecSubscribeNumbers.begin(); iter != m_vecSubscribeNumbers.end(); ++iter)
	{
		if ((*iter) == _handlenum)
		{
			m_vecSubscribeNumbers.erase(iter);
			break;
		}
	}
}

void Engine::GameObject::Set_AllPass_ByModel(Model* _model, _int _passNum)
{
	m_vecObjPass.resize(_model->Get_NumMeshes());
	for (_int i = 0; i < m_vecObjPass.size(); ++i)
	{
		m_vecObjPass[i] = _passNum;
	}
}

HRESULT Engine::GameObject::Bind_ProjectionMatrices(Shader* _shader)
{
	//쉐도우인데 왜 이거 넣지?
    CHECK_FAILED(m_pGameInstance->Bind_PipeLineMatrix_View(_shader), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Bind_PipeLineMatrix_Proj(_shader), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Bind_CameraPosition(_shader), E_FAIL);

    return S_OK;
}

_float Engine::GameObject::RandomDamage(_float _damage, _uint _range)
{
	_float damageRatio = _damage / _range;
	return m_pGameInstance->RandomValue(_damage - damageRatio, _damage + damageRatio);
}
/******************************************************* 변수 세팅 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::GameObject::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::GameObject::Update(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::GameObject::Update_Late(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::GameObject::Render(const _float fTimeDelta)
{
	return S_OK;
}

void Engine::GameObject::Add_RenderGroup(RENDER_GROUP _rendergroup)
{
    m_pGameInstance->Add_RenderObject(_rendergroup, this);
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
GameObject* Engine::GameObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

GameObject* Engine::GameObject::Clone(void* arg)
{
	return nullptr;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::GameObject::Free()
{
	m_pGameInstance->Set_Parallel_Dirty();
	__super::Free();

	// 컴포넌트 전부 해제
	for (auto& [Name, component] : m_umapComponents)
	{
		if (DCAST(Collider*)(component) != nullptr)
		{
			m_pGameInstance->Unregister_Collider(CAST(Collider*)(component)->Get_Group(), CAST(Collider*)(component));
		}

		Safe_Release(component);
	}
	m_umapComponents.clear();

	// 아무튼 등록된거 다 해제
	for (auto& base : m_vecCleanUpTargets)
	{
		Safe_Release(base);
	}
	m_vecCleanUpTargets.clear();

	// 구독목록 전부 해제
	for (auto& handle : m_vecSubscribeNumbers)
	{
		m_pGameInstance->UnsubScribe(handle);
	}
	m_vecSubscribeNumbers.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

