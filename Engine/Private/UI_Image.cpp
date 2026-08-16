#include "Engine_Define.h"
#include "UI_Image.h"
#include "UIObject.h"
#include "Shader.h"
#include "NewTexture.h"
#include "VIBuffer_Rect.h"
#include "RenderTargetManager.h"


#ifdef _DEBUG
#include "Editor_Utils.h"
#endif // _DEBUG





Engine::UI_Image::UI_Image()
	:UI_Render()
{
}

Engine::UI_Image::UI_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UI_Render(pDevice,pContext)
{
	m_iSavePriority = 0;

}

Engine::UI_Image::UI_Image(const UI_Image& original)
	:UI_Render(original)
{
}

Engine::UI_Image::~UI_Image()
{
}

HRESULT Engine::UI_Image::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::UI_Image::Initialize(void* arg)
{
	/*Texture, Buffer를 확정적으로 만들어준다.툴에서 설정할수있도록 한다.*/
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);
	if (m_pOwner != nullptr)
	{
		//오너에게 vibuffer를 추가한다.
		//기본 texture를 추가한다.
		Component* ppVIBuffercom = m_pOwner->Get_Component_FromName(L"Prototype_Component_VIBuffer_Rect");
		if (!ppVIBuffercom)
		{
			m_pOwner->Add_Component(0, L"Prototype_Component_VIBuffer_Rect", Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));
			Safe_AddRef(m_pVIBufferCom);
		}
	

		if (arg != nullptr)
		{
			UIIMAGEDESC* pDesc = CAST(UIIMAGEDESC*)(arg);
			if (pDesc->TextureComponentKey != L"")
			{
				m_ProtoTextureKey = pDesc->TextureComponentKey;

				m_pOwner->Add_Component(0, pDesc->TextureComponentKey, Com_NewTexture, RCAST(Component**)(&m_pTextureCom));
				Safe_AddRef(m_pTextureCom);


				//기본텍스처-white
				m_CurrentTexName = pDesc->TextureKey;
				m_iTargetTexNumber = m_pTextureCom->Get_TextureIdx(m_CurrentTexName);
				m_iNoiseTargetTexNumber = m_pTextureCom->Get_TextureIdx(m_CurrentTexName);
				m_iMaskTargetTexNumber = m_pTextureCom->Get_TextureIdx(m_CurrentTexName);

			}
		}
		
		//m_pOwner->Set_RenderComponent(this);
		return S_OK;
	}

	m_vHsv = RGBtoHSV(m_vColor);
	m_ColorEffect.m_InitHue= m_vHsv.x;
	return S_OK;
}

//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::UI_Image::Update(const _float fTimeDelta)
{
	if (m_bUseInfinitScroll)
		m_fScrollTime += fTimeDelta * m_fSpeed;

	if (m_ColorEffect.m_bUseColorEffect)
	{
		m_ColorEffect.m_fColorTime += fTimeDelta * m_ColorEffect.m_fColorSpeed;;
		float fAlpha = (sinf(m_ColorEffect.m_fColorTime) * 1.f) * 0.5f;

		float fTargetHue =fLerp(m_ColorEffect.m_MinHue, m_ColorEffect.m_MaxHue, fAlpha);
		
		m_vColor = HSVtoRGB(fTargetHue, m_vHsv.y, m_vHsv.z);

	}
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



HRESULT Engine::UI_Image::Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket)
{
	CHECK_NULLPTR(m_pShaderCom);
	CHECK_NULLPTR(m_pTransformCom);
	CHECK_NULLPTR(m_pVIBufferCom);
	CHECK_NULLPTR(pPacket);


	/*값 바인딩..*/

	if (m_bAddictiveBlending)
		m_pGameInstance->Set_BlendState(BSSET::ADDITIVE);

	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);
	
	if (m_pTextureCom)
	{
		pPacket->g_UIColor = m_vColor;

		pPacket->g_fUIIntensity = m_fIntensity;

		//CHECK_FAILED(m_pShaderCom->Bind_RawValue_FullSlot(BUFFER_UI, "g_UIColor", (_float4*)&m_vColor, sizeof(_float4)), E_FAIL);
																		 
		pPacket->g_fClip = m_fClip;

		// 0 = "g_Texture"
		if (m_eRenderMode == RENDER_TYPE::RENDER_NORMAL || m_eRenderMode == RENDER_TYPE::RENDER_RESOURCE_ONLY)
		{
			CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 0, m_iTargetTexNumber), E_FAIL);
			if (m_bUseBindExtra)
			{														// 2 = "g_NoiseMask", 1 = "g_TextureMask"
				CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 2, m_iNoiseTargetTexNumber), E_FAIL);
				CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 1, m_iMaskTargetTexNumber), E_FAIL);
			}
		}
	
		else if (m_eRenderMode == RENDER_TYPE::RENDER_BLUR)
		{
			CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 0, m_iTargetTexNumber), E_FAIL);
			m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_UI_BlurVertical, m_pShaderCom, 3);
		}

		else if (m_eRenderMode == RENDER_TYPE::RENDER_MINIMAP_FULL)
		{
			m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Minimap_Final, m_pShaderCom, 4);
		}

		else if (m_eRenderMode == RENDER_TYPE::RENDER_MINIMAP_CENTER)
		{
			m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Minimap_Center, m_pShaderCom, 4);
		}
	}

	if (m_bUseInfinitScroll)
	{
		pPacket->g_ScrollTime = m_fScrollTime;
		//CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ScrollTime, (_float*)&m_fScrollTime, sizeof(_float)), E_FAIL);
	}
	/*실제 쉐이더에 바인딩*/

	//리소스만 던지는애라면 그리진않는다.
	if (m_eRenderMode == RENDER_RESOURCE_ONLY)
		return S_OK;


	m_pShaderCom->Bind_EntireBuffer_BySlot(BUFFER_UI,  pPacket, sizeof(UI_BUFFER_PACKET));

	CHECK_FAILED(m_pShaderCom->Begin(ShaderPassNumber), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_Resources(ShaderPassNumber), E_FAIL);
	CHECK_FAILED(m_pVIBufferCom->Bind_Resource(), E_FAIL);
	CHECK_FAILED(m_pVIBufferCom->Render(0.f), E_FAIL);


	if (m_bAddictiveBlending)
		m_pGameInstance->Set_BlendState(BSSET::BLEND);

#ifdef _DEBUG
	if (m_bDrawDebug)
		Render_Debug();

#endif // DEBUG

	




	return S_OK;
}

void Engine::UI_Image::Change_Texture(string str)
{
	if (str == m_CurrentTexName)
		return;

	m_CurrentTexName = str;
	m_iTargetTexNumber = m_pTextureCom->Get_TextureIdx(m_CurrentTexName);
}

void Engine::UI_Image::Change_Texture(_uint i)
{
	m_CurrentTexName = m_pTextureCom->Get_TextureName((int)i);
	m_iTargetTexNumber = i;
}


UI_Image* Engine::UI_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	UI_Image* pInstance = new UI_Image(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"UI_Image 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::UI_Image::Clone(void* arg)
{
	UI_Image* pInstance = new UI_Image(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"UI_Image 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}

#ifdef _DEBUG
void	Engine::UI_Image::Render_Imgui()
{
	
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text("ImageComponent");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	__super::Render_Imgui();

	if (ImGui::DragFloat2("Clip", (float*)&m_fClip))
	{

	}


	int iCurrentRenderType = (int)m_eRenderMode;
	//렌더타입선택
	if (ImGui::RadioButton("RENDER_NORMAL", &iCurrentRenderType, (int)RENDER_TYPE::RENDER_NORMAL)) {
		m_eRenderMode = RENDER_NORMAL;
		if (m_pOwner)
			m_pOwner->Set_BlurUI(false);

	}

	if (ImGui::RadioButton("RENDER_RESOURCE_ONLY", &iCurrentRenderType, (int)RENDER_TYPE::RENDER_RESOURCE_ONLY)) {
		m_eRenderMode = RENDER_RESOURCE_ONLY;
		if (m_pOwner)
			m_pOwner->Set_BlurUI(false);

	}

	if (ImGui::RadioButton("RENDER_BLUR", &iCurrentRenderType, (int)RENDER_TYPE::RENDER_BLUR)) {
		{
			m_eRenderMode = RENDER_BLUR;
			if (m_pOwner)
				m_pOwner->Set_BlurUI(true);

		}


		if (m_pOwner)
		{
			if(!m_pOwner->Is_WorldUI())
				m_pOwner->Set_PassNum(16);

			else
				m_pOwner->Set_PassNum(20);

		}
			
	}

	if (ImGui::RadioButton("RENDER_MINIMAP_FULL", &iCurrentRenderType, (int)RENDER_TYPE::RENDER_MINIMAP_FULL)) {
		{
			m_eRenderMode = RENDER_MINIMAP_FULL;
			if (m_pOwner)
			{
				m_pOwner->Set_BlurUI(false);
				m_pOwner->Set_PassNum(17);
			}
				
		}
	}

	if (ImGui::RadioButton("RENDER_MINIMAP_CENTER", &iCurrentRenderType, (int)RENDER_TYPE::RENDER_MINIMAP_CENTER)) {
		{
			m_eRenderMode = RENDER_MINIMAP_CENTER;
			if (m_pOwner)
			{
				m_pOwner->Set_BlurUI(false);
				m_pOwner->Set_PassNum(17);
			}

		}
	}

	ImGui::Checkbox("Use_BLEND::ADDICTIVE", &m_bAddictiveBlending);

	//서치가능한 현재 texture 리스트받아오기

	if (ImGui::TreeNode("Extra Settings"))
	{
		if (ImGui::ColorEdit4("Color", (_float*)&m_vOriginColor))
		{
			m_vColor = m_vOriginColor;
		}

		ImGui::SameLine();
		if (ImGui::DragFloat("Intensity", (float*)&m_fIntensity))
		{

		}

		ImGui::Checkbox("BindExtra", &m_bUseBindExtra);
		ImGui::Checkbox("Use InfinitScroll", &m_bUseInfinitScroll);
		if (m_bUseInfinitScroll)
		{
			ImGui::DragFloat("Speed", &m_fSpeed);
		}



		ImGui::TreePop();
	}

	Render_TextureList();

}

void Engine::UI_Image::Render_TextureList()
{

	CHECK_JUST_NULL(m_pTextureCom);
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	

	EditorUtils::Draw_ResourceSelector("Default Texture", "TextureList", m_HierarchyFilter, m_pTextureCom, &m_CurrentTexName, &m_iTargetTexNumber);
	if (m_bUseBindExtra)
	{
		EditorUtils::Draw_ResourceSelector("NOISE Texture", "Noise TextureList", m_HierarchyFilter, m_pTextureCom, &m_NoiseTexName, &m_iNoiseTargetTexNumber);
		EditorUtils::Draw_ResourceSelector("MASK Texture", "Mask TextureList", m_HierarchyFilter, m_pTextureCom, &m_MaskTexName, &m_iMaskTargetTexNumber);

	}
}

#endif // _DEBUG

void Engine::UI_Image::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();

}

#pragma region parsing
void	Engine::UI_Image::Save_Data(ordered_json& pJson)
{
	pJson["BindExtra"] = m_bUseBindExtra;

	if (m_bUseInfinitScroll)
	{
		pJson["UseInfinitScroll"] = m_bUseInfinitScroll;
		pJson["Speed"] = m_fSpeed;

	}
	pJson["Color"] = { m_vColor.x,m_vColor.y,m_vColor.z,m_vColor.w };
	pJson["Intensity"] = m_fIntensity;


	pJson["ProtoTexture"] = wstringToString(m_ProtoTextureKey);

	pJson["DiffuseTextureKey"] = m_CurrentTexName;
	pJson["NoiseTextureKey"] = m_NoiseTexName;
	pJson["MaskTextureKey"] = m_MaskTexName;

	pJson["Clip"] = { m_fClip.x,m_fClip.y };

	string RenderMode = "";
	switch (m_eRenderMode)
	{
	case Engine::UI_Image::RENDER_NORMAL:
		RenderMode = "RENDER_NORMAL";
		break;
	case Engine::UI_Image::RENDER_RESOURCE_ONLY:
		RenderMode = "RENDER_RESOURCE_ONLY";
		break;

	case Engine::UI_Image::RENDER_BLUR:
		RenderMode = "RENDER_BLUR";
		break;

	case Engine::UI_Image::RENDER_MINIMAP_FULL:
		RenderMode = "RENDER_MINIMAP_FULL";
		break;

	case Engine::UI_Image::RENDER_MINIMAP_CENTER:
		RenderMode = "RENDER_MINIMAP_CENTER";
		break;
	default:
		break;
	}
	pJson["RenderMode"] = RenderMode;

	pJson["ProtoTexLevel"] = m_iProtoTexLevel;
	pJson["UseAddictiveBlending"] = m_bAddictiveBlending;

}
void Engine::UI_Image::Load_Data(GameObject* pOwner, ComponentData& Data, UITYPE* eType)
{

	__super::Load_Data(pOwner, Data,eType);

	CHECK_JUST_NULL(pOwner);
	ordered_json root = Data.ComJson;
	
	m_bUseBindExtra = root.value("BindExtra", false);

	//새로추가
	m_bUseInfinitScroll = root.value("UseInfinitScroll", false);
	m_fSpeed = root.value("Speed", 0.f);
	if (root.contains("Color"))
		m_vColor = _float4(root["Color"][0], root["Color"][1], root["Color"][2], root["Color"][3]);

	else
		m_vColor = _float4(1.f, 1.f, 1.f, 1.f);

	//intensity
	
	if (root.contains("Intensity"))
		m_fIntensity = root.value("Intensity", 1.f);

	else
		m_fIntensity = 1.f;

	if (root.contains("ProtoTexLevel"))
		m_iProtoTexLevel = root["ProtoTexLevel"];


	m_ProtoTextureKey = stringToWstring(root.value("ProtoTexture", "Prototype_Component_UITexture_UIResource"));

	m_CurrentTexName = root.value("DiffuseTextureKey", DEFAULT_TEXTUREKEY);
	m_NoiseTexName = root.value("NoiseTextureKey", DEFAULT_TEXTUREKEY);
	m_MaskTexName = root.value("MaskTextureKey", DEFAULT_TEXTUREKEY);

	if (root.contains("Clip"))
	{
		m_fClip = _float2(root["Clip"][0], root["Clip"][1]);

	}
	string RenderMode = root.value("RenderMode", "RENDER_NORMAL");
	if(RenderMode=="RENDER_NORMAL")
		m_eRenderMode= RENDER_TYPE::RENDER_NORMAL;

	else if(RenderMode=="RENDER_RESOURCE_ONLY")
		m_eRenderMode = RENDER_TYPE::RENDER_RESOURCE_ONLY;

	else if(RenderMode=="RENDER_BLUR")
		m_eRenderMode = RENDER_TYPE::RENDER_BLUR;

	else if (RenderMode == "RENDER_MINIMAP_FULL")
		m_eRenderMode = RENDER_TYPE::RENDER_MINIMAP_FULL;

	else if (RenderMode == "RENDER_MINIMAP_CENTER")
		m_eRenderMode = RENDER_TYPE::RENDER_MINIMAP_CENTER;

	//오너에게 vibuffer를 추가한다.
	//기본 texture를 추가한다.
	m_pOwner->Add_Component(0, L"Prototype_Component_VIBuffer_Rect", Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));
	Safe_AddRef(m_pVIBufferCom);

	m_pOwner->Add_Component(m_iProtoTexLevel, m_ProtoTextureKey, Com_NewTexture, RCAST(Component**)(&m_pTextureCom));
	Safe_AddRef(m_pTextureCom);


	//기본텍스처-white
	m_iTargetTexNumber = m_pTextureCom->Get_TextureIdx(m_CurrentTexName);
	m_iNoiseTargetTexNumber = m_pTextureCom->Get_TextureIdx(m_NoiseTexName);
	m_iMaskTargetTexNumber = m_pTextureCom->Get_TextureIdx(m_MaskTexName);
	
	m_vHsv = RGBtoHSV(m_vColor);
	*eType = UITYPE::IMAGE;
	m_ColorEffect.m_InitHue = m_vHsv.x;
	m_vOriginColor = m_vColor;

	m_bAddictiveBlending = root.value("UseAddictiveBlending", false);

}
#pragma endregion