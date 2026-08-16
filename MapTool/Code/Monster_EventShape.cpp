#include "MT_Defines.h"
#include "Monster_EventShape.h"
#include "Collider.h"
#include "Monster.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Monster_EventShape::Monster_EventShape()
{
}

Monster_EventShape::Monster_EventShape(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:EventShape(pDevice,pContext)
{
}

Monster_EventShape::Monster_EventShape(const Monster_EventShape& original)
	:EventShape(original)
{
}

Monster_EventShape::~Monster_EventShape()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Monster_EventShape::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}
HRESULT Monster_EventShape::Initialize(void* arg)
{
	MonsterEventShapeDesc* pDesc = static_cast<MonsterEventShapeDesc*>(arg);

	if (arg != nullptr)
	{
		m_pOwner = pDesc->pOwner;
		m_ColliderType = pDesc->eColliderType;
		m_vOffSet = pDesc->vOffSet;
		m_Extents = pDesc->Extents;

		if (!pDesc->jExtraData.empty())
			Set_ExtraData(pDesc->jExtraData);

		CHECK_FAILED(__super::Initialize(arg), E_FAIL);

		m_pTransformCom->Set_Scale(m_Extents.x * 2.f, m_Extents.y * 2.f, m_Extents.z * 2.f);

		_vector vPos = XMLoadFloat3(&m_vOffSet);
		vPos = XMVectorSetW(vPos, 1.f);
		m_pTransformCom->Set_State(DIRECTION::POSITION, vPos);
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	static _uint namenum = 0;
	m_wstrName = L"TriggerBox_" + to_wstring(namenum++);

	m_eObjType = OBJTYPE::TYPE_MAP;

	if (m_pColliderCom)
		m_pColliderCom->Set_Group(COLLISION_GROUP::EVENT_POINT);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



bool Monster_EventShape::Is_Collision()
{
	CHECK_NULL_RESULT(m_pColliderCom, false);

	return m_pColliderCom->Is_Collision();
}

/******************************************************* 생성자, 소멸자 *******************************************************/


HRESULT Monster_EventShape::Ready_Components()
{
	Bounding_AABB::BOUNDAABB_DESC Desc;

	Desc.vCenter = _float3(0.f, 0.f, 0.f);
	Desc.vExtents = _float3(0.5f, 0.5f, 0.5f);

	CHECK_FAILED(Add_Component(_UINT(LEVEL::TOOL), L"Prototype_Component_Collider_AABB",
		Com_Collider, RCAST(Component**)(&m_pColliderCom), &Desc), E_FAIL);

	return S_OK;
}


//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Monster_EventShape::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Monster_EventShape::Update(const _float fTimeDelta)
{
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Monster_EventShape::Update_Late(const _float fTimeDelta)
{
	CHECK_NULL_RESULT(m_pColliderCom, 0);

#ifdef _DEBUG
	m_pColliderCom->Add_DebugRender();
#endif // _DEBUG

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Monster_EventShape::Render(const _float fTimeDelta)
{
	CHECK_NULL_RESULT(m_pColliderCom, 0);

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Monster_EventShape* Monster_EventShape::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Monster_EventShape* pInstance = new Monster_EventShape(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"Monster_EventShape 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Monster_EventShape* Monster_EventShape::Clone(void* arg)
{
	Monster_EventShape* pInstance = new Monster_EventShape(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"Monster_EventShape 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/

ordered_json Monster_EventShape::Get_ExtraData()
{
	ordered_json j;
	ordered_json jArray = ordered_json::array();

	for (const auto& spawn : m_vecSpawnList)
	{
		ordered_json jNode;
		jNode["MonsterID"] = spawn.iMonsterID;
		jNode["Position"] = { spawn.vPosition.x, spawn.vPosition.y, spawn.vPosition.z };
		jNode["Rotation"] = { spawn.vRotation.x, spawn.vRotation.y, spawn.vRotation.z };

		jArray.push_back(jNode);
	}

	if (!jArray.empty())
	{
		j["SpawnList"] = jArray;
	}

	return j;
}

void Monster_EventShape::Set_ExtraData(const ordered_json& j)
{
	if (j.contains("SpawnList"))
	{
		m_vecSpawnList.clear();

		for (const auto& jNode : j["SpawnList"])
		{
			MonsterSpawnInfo info;
			info.iMonsterID = jNode["MonsterID"];

			info.vPosition.x = jNode["Position"][0];
			info.vPosition.y = jNode["Position"][1];
			info.vPosition.z = jNode["Position"][2];

			info.vRotation.x = jNode["Rotation"][0];
			info.vRotation.y = jNode["Rotation"][1];
			info.vRotation.z = jNode["Rotation"][2];

			m_vecSpawnList.push_back(info);
		}
	}
}


//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Monster_EventShape::Free()
{
	m_vecSpawnList.clear();
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/


