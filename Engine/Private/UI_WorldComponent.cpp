#include "Engine_Define.h"
#include "UI_WorldComponent.h"
#include "UIObject.h"
#include "Camera.h"


Engine::UI_WorldComponent::UI_WorldComponent()
{
}

Engine::UI_WorldComponent::UI_WorldComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIComponent(pDevice,pContext)
{
}

Engine::UI_WorldComponent::UI_WorldComponent(const UI_WorldComponent& original)
	:UIComponent(original)
{
}

Engine::UI_WorldComponent::~UI_WorldComponent()
{
}

HRESULT Engine::UI_WorldComponent::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::UI_WorldComponent::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);
	if (arg)
	{
		WorldUICompDesc* pDesc = static_cast<WorldUICompDesc*>(arg);
		m_pTarget = pDesc->pTarget;
		m_vWorldPos = pDesc->vWorldPos;
		m_vOffSet = pDesc->vOffset;

		m_fMaxVisibleDist = pDesc->fMaxVisibleDist;
		m_bDistanceScaling = pDesc->bDistanceScaling;
		m_MinScale = pDesc->MinScale;

                                                                                      		m_TargetCameraIdx = pDesc->TargetCameraIdx;

	}

	if (m_pOwner)
		m_pOwner->Set_RenderGroup(RENDER_GROUP::WORLD_UI);

	return S_OK;
}


_int	Engine::UI_WorldComponent::Update(const _float fTimeDelta)
{
	CHECK_NULL_RESULT(m_pOwner, 0);

	//타겟존재할경우: 타겟의 오프셋만큼 
	if (m_pTarget)
	{
		if (m_pTarget->Is_Dead())
			m_pOwner->Set_Active(false);


		XMStoreFloat3(&m_vWorldPos, m_pTarget->Get_Transform()->Get_State(DIRECTION::POSITION));
		Transform* pTargetTransform = m_pTarget->Get_Transform();

		_float fTargetYaw = pTargetTransform->Get_Rotation_Yaw();
		//m_pOwner->Set_Rotation(XMConvertToDegrees(fTargetYaw));

	}


	//없을 경우, 생성했던 desc->m_vWorldpos
	m_vBasePos = XMLoadFloat3(&m_vWorldPos);
	m_vBasePos += XMLoadFloat3(&m_vOffSet);

	
	//카메라와의 거리계산
	//1:CAMERA::PLAYER_FOCUS_00
	Camera* pMainCam = m_pGameInstance->Get_Camera(m_TargetCameraIdx);
	if (pMainCam)
	{
		_vector CameraPos = pMainCam->Get_Position();
		_float fDistance = XMVectorGetX(XMVector3Length(m_vBasePos - CameraPos));

		if (fDistance > m_fMaxVisibleDist)
		{
			m_pOwner->Set_Visible(false);
			return 0;
		}

		else
			m_pOwner->Set_Visible(true);


		//////거리기반 스케일링
		if (m_bDistanceScaling)
		{
			_float fScale = 10.f / fDistance;
			fScale = max(m_MinScale, min(fScale, 1.2f));//1.2 -> 최소

			_float2 Scale = _float2(m_pOwner->Get_LocalTransform().m_fSizeX, m_pOwner->Get_LocalTransform().m_fSizeY);

			m_pOwner->Set_Size(Scale.x * fScale,
				Scale.y * fScale);



		}

	}

	_float3 vFinalPos;
	XMStoreFloat3(&vFinalPos, m_vBasePos);
	m_pOwner->Set_Combined_Poistion(vFinalPos.x, vFinalPos.y,vFinalPos.z);
	
	return 0;
}

_int	Engine::UI_WorldComponent::Update_Late(const _float fTimeDelta)
{

	return 0;
}

void Engine::UI_WorldComponent::Render_Imgui()
{

	if (ImGui::Button("SetTarget"))
	{
		//nonblend에있는 타겟하나 설정(2:main씬)
		GameObject* pFind =m_pGameInstance->Get_GameObject(2, L"Model_Layer", L"TestModel");
		if (pFind)
			m_pTarget = pFind;

	}

	if (ImGui::Button("ClearTarget"))
	{
		Clear_Target();

	}


	_float3 vCurrentPost = m_vWorldPos;

	if (ImGui::DragFloat3("UI WorldPos", (_float*)&vCurrentPost,0.01f))
	{
		m_vWorldPos = vCurrentPost;

	}

	ImGui::DragFloat3("UI OffSet", (_float*)&m_vOffSet, 0.01f);

	

	ImGui::DragFloat("MaxVisibleDist", &m_fMaxVisibleDist);

	ImGui::DragFloat("MinScale", &m_MinScale);


	ImGui::DragFloat3("MaxVisibleDist", &m_fMaxVisibleDist);

	ImGui::Checkbox("Use Distance Scalling", &m_bDistanceScaling);




	
}
UI_WorldComponent* Engine::UI_WorldComponent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	UI_WorldComponent* pInstance = new UI_WorldComponent(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"UI_WorldComponent 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::UI_WorldComponent::Clone(void* arg)
{
	UI_WorldComponent* pInstance = new UI_WorldComponent(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"UI_WorldComponent 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}



void Engine::UI_WorldComponent::Free()
{

	__super::Free();
}

#pragma region parsing
void	Engine::UI_WorldComponent::Save_Data(ordered_json& pJson)
{
	pJson["WorldPos"] = { m_vWorldPos.x,m_vWorldPos.y,m_vWorldPos.z };
	pJson["OffSet"] = { m_vOffSet.x,m_vOffSet.y,m_vOffSet.z };

	pJson["MaxVisibleDist"] = m_fMaxVisibleDist;
	pJson["DistanceScaling"] = m_bDistanceScaling;
	pJson["MinScale"] = m_MinScale;



}
void Engine::UI_WorldComponent::Load_Data(GameObject* pOwner, ComponentData& Data, UITYPE* eType)
{

	__super::Load_Data(pOwner, Data, eType);

	CHECK_JUST_NULL(pOwner);
	ordered_json root = Data.ComJson;

	if (root.contains("WorldPos"))
		m_vWorldPos = _float3(root["WorldPos"][0], root["WorldPos"][1], root["WorldPos"][2]);

	if (root.contains("OffSet"))
		m_vOffSet = _float3(root["OffSet"][0], root["OffSet"][1], root["OffSet"][2]);

	m_fMaxVisibleDist = root.value("MaxVisibleDist", 0.f);

	m_bDistanceScaling = root.value("DistanceScaling", false);
	m_MinScale = root.value("MinScale", 1.f);


	*eType = UITYPE::WORLDUI;


}
#pragma endregion


/////////////////////그 외 함수/////////////////////
void Engine::UI_WorldComponent::Clear_Target()
{
	if(m_pTarget)
		m_vWorldPos = m_pTarget->Get_Position() + m_vOffSet;
	m_pTarget = nullptr;

	

}