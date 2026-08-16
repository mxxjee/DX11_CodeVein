#include "Engine_Define.h"
#include "UI_Render.h"
#include "GameObject.h"
#include "Shader.h"
#include "UIObject.h"
#include "GameInstance.h"



Engine::UI_Render::UI_Render()
	:UIComponent()
{
}

Engine::UI_Render::UI_Render(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIComponent(pDevice,pContext)
{
	m_iSavePriority = 0;

}

Engine::UI_Render::UI_Render(const UI_Render& original)
	:UIComponent(original)
{
}

Engine::UI_Render::~UI_Render()
{
}

HRESULT Engine::UI_Render::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::UI_Render::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	/*Transform 참조시 레퍼런스 카운트 증가*/
	if (m_pOwner!=nullptr)
	{
		m_pTransformCom = m_pOwner->Get_Transform();
		if (m_pTransformCom)
			Safe_AddRef(m_pTransformCom);

		//m_pOwner->Set_RenderComponent(this);



	}
#pragma region 디버그 버퍼
#ifdef _DEBUG
			//LT
	vertices[0].vPosition = { -0.5f,  0.5f,    0.f };
	vertices[0].vColor = _float4(0.f, 1.f, 0.f, 1.f);

//RT
	vertices[1].vPosition = { 0.5f, 0.5f,    0.f };
	vertices[1].vColor = _float4(0.f, 1.f, 0.f, 1.f);

	//RB
	vertices[2].vPosition = { 0.5f, -0.5f, 0.f };
	vertices[2].vColor = _float4(0.f, 1.f, 0.f, 1.f);

	//LB
	vertices[3].vPosition = { -0.5f,  -0.5f, 0.f };
	vertices[3].vColor = _float4(0.f, 1.f, 0.f, 1.f);

	//LT(다시연결)
	vertices[4].vPosition = { -0.5f,  0.5f, 0.f };
	vertices[4].vColor = _float4(0.f, 1.f, 0.f, 1.f);

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VTXPOSCOR) *5;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA VertexInitialData{};
	VertexInitialData.pSysMem = vertices;

	m_pDevice->CreateBuffer(&desc, &VertexInitialData, &m_pDebugVB);

	Base* pShader = GameInstance::GetInstance()->Clone_Prototype(PROTOTYPE::COMPONENT, 0, L"Prototype_Component_Shader_VtxPosColor", nullptr);

	if (pShader)
		m_pLineShader = dynamic_cast<Shader*>(pShader);


	D3D11_VIEWPORT viewportdesc = {};
	UINT inumviewport = { 1 };
	m_pContext->RSGetViewports(&inumviewport, &viewportdesc);

	m_fWindowX = viewportdesc.Width;
	m_fWindowY = viewportdesc.Height;




#endif // _DEBUG
#pragma endregion





	return S_OK;
}


HRESULT Engine::UI_Render::Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket)
{


	CHECK_NULLPTR(m_pShaderCom);
	CHECK_NULLPTR(m_pTransformCom);

	/*값 바인딩..*/
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);
	
	m_pShaderCom->Begin(ShaderPassNumber);
	m_pShaderCom->Bind_Resources(ShaderPassNumber);
	
#ifdef _DEBUG
	if(m_bDrawDebug)
		Render_Debug();

#endif // DEBUG

	return S_OK;
}

#ifdef _DEBUG

void Engine::UI_Render::Render_Imgui()
{

	if (ImGui::Checkbox("IsInteractable", &m_bInteractable))
	{
		
	}

}
#endif // _DEBUG


UI_Render* Engine::UI_Render::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	UI_Render* pInstance = new UI_Render(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"UI_Render 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::UI_Render::Clone(void* arg)
{
	UI_Render* pInstance = new UI_Render(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"UI_Render 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}

void Engine::UI_Render::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pDebugVB);
	Safe_Release(m_pLineShader);
#endif // _DEBUG

	Safe_Release(m_pTransformCom);
	__super::Free();
}


#ifdef _DEBUG
void Engine::UI_Render::Render_Debug()
{
	CHECK_JUST_NULL(m_pLineShader);

	// 사각형을 그리려면 점 5개

	_float2 vNewSize = m_pOwner->Get_HitboxSize();
	_float vRotation = m_pOwner->Get_CombinedRotation();
	_vector vPos = m_pOwner->Get_Position();
	_matrix matRot = XMMatrixRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(vRotation));

	_matrix matDebugWorld = XMMatrixScaling(vNewSize.x, vNewSize.y, 1.f) * matRot * XMMatrixTranslation(XMVectorGetX(vPos), XMVectorGetY(vPos), 0.f);
	_float4x4 fMatDebugWorld;

	XMStoreFloat4x4(&fMatDebugWorld, matDebugWorld);

	if (FAILED(m_pLineShader->Bind_Matrix_FullSlot(BUFFER_OBJECT,"g_WorldMatrix", fMatDebugWorld)))
		return;



	m_pLineShader->Begin(_UINT(m_pOwner->Get_SelectState()));
	m_pLineShader->Bind_Resources(0);
	UINT stride = sizeof(VTXPOSCOR), offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pDebugVB, &stride, &offset);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	m_pContext->Draw(5, 0);
}
#endif // _DEBUG
#pragma region parsing
void Engine::UI_Render::Save_Data(ordered_json& pJson)
{
	pJson["Interatable"] = m_bInteractable;

}
void Engine::UI_Render::Load_Data(GameObject* pOwner, ComponentData& Data, UITYPE* eType)
{
	__super::Load_Data(pOwner, Data, eType);

	CHECK_JUST_NULL(pOwner);

	m_pTransformCom = m_pOwner->Get_Transform();
	if (m_pTransformCom)
		Safe_AddRef(m_pTransformCom);

	ordered_json root = Data.ComJson;

	*eType = UITYPE::ROOT;

}
#pragma endregion




