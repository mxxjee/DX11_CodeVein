#include "Client_Define.h"
#include "Cinematic_EventShape.h"
#include "Collider.h"
#include "Monster.h"
#include "PoolingManager.h"
#include "UIObj_Minimap.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Cinematic_EventShape::Cinematic_EventShape()
{
}

Client::Cinematic_EventShape::Cinematic_EventShape(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:EventShape(pDevice, pContext)
{
}

Client::Cinematic_EventShape::Cinematic_EventShape(const Cinematic_EventShape& original)
	:EventShape(original)
{
}

Client::Cinematic_EventShape::~Cinematic_EventShape()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Cinematic_EventShape::Initialize_Prototype(LEVEL _level)
{
	__super::Initialize_Prototype();

	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Cinematic_EventShape::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	if (arg != nullptr)
	{
		CinematicEventShapeDesc* desc = CAST(CinematicEventShapeDesc*)(arg);

		m_fRadius = desc->fRadius;
	}


	m_bIsTriggered = false;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	static _uint namenum = 0;
	m_wstrName = L"Cinematic_EventShape" + to_wstring(namenum++);

	m_eObjType = OBJTYPE::TYPE_MAP;

	if (m_pColliderCom)
		m_pColliderCom->Set_Group(COLLISION_GROUP::EVENT_POINT);

	switch (m_iLevel)
	{
	case _UINT(LEVEL::MAIN):
		m_vecCinematicKeyFrames = CinematicPreset::Load("../../DataFiles/CinematicPreset/Oliver2.json");
		break;

	case _UINT(LEVEL::CHURCH):
		m_vecCinematicKeyFrames = CinematicPreset::Load("../../DataFiles/CinematicPreset/WolfGhost2.json");
		break;
	}

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



bool Client::Cinematic_EventShape::Is_Collision()
{
	CHECK_NULL_RESULT(m_pColliderCom, false);

	return m_pColliderCom->Is_Collision();
}


HRESULT Client::Cinematic_EventShape::Ready_Components()
{
	Bounding_Sphere::BOUNDSPHERE_DESC desc;
	desc.fRadius = m_fRadius;

	Add_Collider(COLLISION_GROUP::EVENT_POINT, COLLIDER::SPHERE, &m_pColliderCom, &desc);

	return S_OK;
}



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Cinematic_EventShape::Update_Priority(const _float fTimeDelta)
{
	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Cinematic_EventShape::Update(const _float fTimeDelta)
{
	if (m_pColliderCom == nullptr) return 0;
	if (m_bIsTriggered == true) return -1;

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (!m_bIsTriggered && m_pColliderCom->Is_Collision())
	{
		COUT("시네마틱 쉐이프 트리거 박스 충돌 감지");
		m_bIsTriggered = true;
		// 시네마틱용 카메라 이벤트
		CameraEvent event;
		event.eCameraState = CAMERA_STATE::CINEMATIC;
		event.bCinematicAutoReturn = true;
		event.vecKeyframes = m_vecCinematicKeyFrames;
		m_pGameInstance->Publish(event);

		//미니맵꺼
		UIObject* pMinimap = m_pGameInstance->Find_UI_ByName(L"Minimap_Center");
		if (pMinimap)
			pMinimap->Set_Visible(false);
	}

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Cinematic_EventShape::Update_Late(const _float fTimeDelta)
{
	if (m_bIsTriggered == true) return -1;
	CHECK_NULL_RESULT(m_pColliderCom, 0);

#ifdef _DEBUG
	m_pColliderCom->Add_DebugRender();
#endif

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Cinematic_EventShape::Render(const _float fTimeDelta)
{
	CHECK_NULL_RESULT(m_pColliderCom, 0);

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Cinematic_EventShape* Client::Cinematic_EventShape::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Cinematic_EventShape* pInstance = new Cinematic_EventShape(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Cinematic_EventShape 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Cinematic_EventShape* Client::Cinematic_EventShape::Clone(void* arg)
{
	Cinematic_EventShape* pInstance = new Cinematic_EventShape(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"Cinematic_EventShape 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Cinematic_EventShape::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/


