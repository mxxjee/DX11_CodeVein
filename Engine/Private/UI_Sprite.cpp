#include "Engine_Define.h"
#include "UI_Sprite.h"
#include "UIObject.h"
#include "Shader.h"
#include "NewTexture.h"
#include "VIBuffer_Rect.h"

#ifdef _DEBUG
#include "Editor_Utils.h"
#endif // _DEBUG

Engine::UI_Sprite::UI_Sprite()
{
}

Engine::UI_Sprite::UI_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UI_Render(pDevice, pContext)
{
}

Engine::UI_Sprite::UI_Sprite(const UI_Sprite& original)
    :UI_Render(original)
{
	m_iSavePriority = 0;

}

Engine::UI_Sprite::~UI_Sprite()
{
}

HRESULT Engine::UI_Sprite::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Engine::UI_Sprite::Initialize(void* arg)
{
	/*Texture, Buffer를 확정적으로 만들어준다.툴에서 설정할수있도록 한다.*/
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);
	if (m_pOwner != nullptr)
	{
		//오너에게 vibuffer를 추가한다.
		//기본 texture를 추가한다.
		m_pOwner->Add_Component(0, L"Prototype_Component_VIBuffer_Rect", Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));
		Safe_AddRef(m_pVIBufferCom);

		if (arg != nullptr)
		{
			SPRITEDESC* pDesc = CAST(SPRITEDESC*)(arg);
			if (pDesc->TextureComponentKey != L"")
			{
				m_ProtoTextureKey = pDesc->TextureComponentKey;

				m_pOwner->Add_Component(0, pDesc->TextureComponentKey, Com_NewTexture, RCAST(Component**)(&m_pTextureCom));
				Safe_AddRef(m_pTextureCom);
			}

			m_eType = pDesc->eType;

			m_iRow = pDesc->Row;
			m_iCol = pDesc->Col;


			m_iMaxFrame = pDesc->iMaxFrame;
			m_bLoop = pDesc->bLoop;

			m_fFrameSpeed = pDesc->fSpeed;
			m_bInitPlay = pDesc->m_bInitPlay;
			m_bPlay = m_bInitPlay;

	
		}

		m_pOwner->Set_RenderComponent(this);

	}


#ifdef _DEBUG
	m_ComboItems.push_back("SHEET");
	m_ComboItems.push_back("SEQUENCE");

#endif // _DEBUG

	return S_OK;
}


_int Engine::UI_Sprite::Update(const _float fTimeDelta)
{
	//프레임계산
	if (m_eType == SPRITETYPE::SHEET)
		Update_UVValue();

	CHECK_FALSE_RESULT(m_bPlay,0);
	m_fFrameTime += fTimeDelta * m_fFrameSpeed;
	
	m_iCurrentFrame = (int)m_fFrameTime;

	if (m_fFrameTime >= (float)m_iMaxFrame)
	{
		if (m_bLoop)
		{
			m_fFrameTime = 0.f;
			m_iCurrentFrame = 0;
		}
	}
	


	return S_OK;
}

_int Engine::UI_Sprite::Update_Late(const _float fTimeDelta)
{
	CHECK_FALSE_RESULT(m_bPlay,0);
	return 0;
}


HRESULT Engine::UI_Sprite::Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket)
{
    CHECK_NULLPTR(m_pShaderCom);
    CHECK_NULLPTR(m_pTransformCom);
    CHECK_NULLPTR(m_pVIBufferCom);
	CHECK_NULLPTR(pPacket);

    CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);
	CHECK_TRUE_RESULT(m_TextureIdx.empty(),E_FAIL);

    if (m_pTextureCom)
    {
		switch (m_eType)
		{
		case Engine::UI_Sprite::SPRITETYPE::SHEET:
			Bind_Value_SpriteSheet(ShaderPassNumber, pPacket);
			break;
		case Engine::UI_Sprite::SPRITETYPE::SEQUENCE:
			Bind_Value_SequenceImage(ShaderPassNumber, pPacket);
			break;
		}
    }

	/*실제 쉐이더에 바인딩*/
	m_pShaderCom->Bind_EntireBuffer_BySlot(BUFFER_UI, pPacket, sizeof(UI_BUFFER_PACKET));


	CHECK_FAILED(m_pShaderCom->Begin(ShaderPassNumber), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_Resources(ShaderPassNumber), E_FAIL);
	CHECK_FAILED(m_pVIBufferCom->Bind_Resource(), E_FAIL);
	CHECK_FAILED(m_pVIBufferCom->Render(0.f), E_FAIL);


#ifdef _DEBUG
	if (m_bDrawDebug)
		Render_Debug();

#endif // DEBUG

	return S_OK;
}

#ifdef _DEBUG
void	Engine::UI_Sprite::Render_Imgui()
{
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text("SpriteComponent");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	__super::Render_Imgui();

	int iCurrentType = (int)m_eType;
	if (ImGui::Combo("SpriteType##UniqueSpriteTypeCombo", &iCurrentType, m_ComboItems.data(), (int)m_ComboItems.size()))
	{
		m_eType = CAST(SPRITETYPE)(iCurrentType);
	}

	ImGui::Text("Sprite Texture List");
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		// 빈 슬롯을 추가하거나, 즉시 파일 다이얼로그를 띄워 추가합니다.
		m_TextureKeys.push_back("UI4_Main/White");
		m_TextureIdx.push_back(m_pTextureCom->Get_TextureIdx(m_TextureKeys.back()));
	}

	/////////Debug용//////////////////////
	
	ImGui::SameLine();
	ImGui::Text("Current Index:%d", (int)m_iCurrentFrame);

	//////////Play & Stop////////////////
	ImGui::SameLine();
	if (ImGui::Button("Play")) {
		m_bPlay = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop")) {
		m_bPlay = false;
	}
	////////////////////////////////////////////////


	for (size_t i = 0; i < m_TextureKeys.size(); ++i)
	{
		//ImGui::PushID(i); // 같은 이름의 버튼 충돌 방지
		EditorUtils::Draw_ResourceSelector("Texture", "TextureList", m_HierarchyFilter, m_pTextureCom, &m_TextureKeys[i], &m_TextureIdx[i]);
		
		//ImGui::PopID();
	}

	if (m_eType == SPRITETYPE::SHEET)
	{
		//행,열입력
		if (ImGui::InputInt("Row", (int*)&m_iRow))
		{

		}

		if (ImGui::InputInt("Col", (int*)&m_iCol))
		{

		}

	}

	//프레임최대개수입력

	if (ImGui::InputInt("MaxFrame", (int*)&m_iMaxFrame))
	{

	}

	if (ImGui::DragFloat("Speed", (float*)&m_fFrameSpeed))
	{

	}
}
#endif

HRESULT Engine::UI_Sprite::Bind_Value_SpriteSheet(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket)
{
	pPacket->g_UVScale = m_vUVScale;
	pPacket->g_UVOffSet = m_vUVOffset;

	//CHECK_FAILED(m_pShaderCom->Bind_RawValue_FullSlot(BUFFER_UI, "g_UVScale", (_float2*)&m_vUVScale, sizeof(m_vUVScale)), E_FAIL);
	//CHECK_FAILED(m_pShaderCom->Bind_RawValue_FullSlot(BUFFER_UI, "g_UVOffSet", (_float2*)&m_vUVOffset, sizeof(m_vUVOffset)), E_FAIL);

	// 0 = "g_Texture"
	CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 0, m_TextureIdx[0]), E_FAIL);

	return S_OK;
}

HRESULT Engine::UI_Sprite::Bind_Value_SequenceImage(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket)
{
	pPacket->g_UVScale = m_vUVScale;
	pPacket->g_UVOffSet = m_vUVOffset;

/*	CHECK_FAILED(m_pShaderCom->Bind_RawValue_FullSlot(BUFFER_UI, "g_UVScale", (_float2*)&m_vUVScale, sizeof(m_vUVScale)), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_FullSlot(BUFFER_UI, "g_UVOffSet", (_float2*)&m_vUVOffset, sizeof(m_vUVOffset)), E_FAIL);


											*/					// 0 = "g_Texture"
	CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 0, m_TextureIdx[m_iCurrentFrame]), E_FAIL);

	return S_OK;
}

void Engine::UI_Sprite::Update_UVValue()
{
	m_vUVScale.x = 1.f / m_iRow;
	m_vUVScale.y = 1.f / m_iCol;

	m_vUVOffset.x = (m_iCurrentFrame % m_iRow) * m_vUVScale.x;
	m_vUVOffset.y = (m_iCurrentFrame / m_iCol) * m_vUVScale.y;
}

void Engine::UI_Sprite::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}


UI_Sprite* Engine::UI_Sprite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	UI_Sprite* pInstance = new UI_Sprite(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"UI_Sprite 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::UI_Sprite::Clone(void* arg)
{
	UI_Sprite* pInstance = new UI_Sprite(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"UI_Sprite 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}



#pragma region parsing
void	Engine::UI_Sprite::Save_Data(ordered_json& pJson)
{
	string SpriteType = "";
	if (m_eType == SPRITETYPE::SHEET)
		SpriteType = "SHEET";

	else
		SpriteType = "SEQUENCE";

	pJson["SpriteType"] = SpriteType;

	pJson["Row"] = m_iRow;
	pJson["Col"] = m_iCol;



	pJson["ProtoTexture"] = wstringToString(m_ProtoTextureKey);
	pJson["MaxFrame"] = m_iMaxFrame;
	pJson["FrameSpeed"] = m_fFrameSpeed;
	pJson["Loop"] = m_bLoop;
	pJson["InitPlay"] = m_bInitPlay;


}
void Engine::UI_Sprite::Load_Data(GameObject* pOwner, ComponentData& Data, UITYPE* eType)
{
	__super::Load_Data(pOwner, Data, eType);

	CHECK_JUST_NULL(pOwner);
	ordered_json root = Data.ComJson;
	m_pOwner->Add_Component(0, L"Prototype_Component_VIBuffer_Rect", Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));
	Safe_AddRef(m_pVIBufferCom);

	
	m_ProtoTextureKey = stringToWstring(root.value("ProtoTexture", "Prototype_Component_UITexture_UIResource"));
	m_pOwner->Add_Component(0, m_ProtoTextureKey, Com_NewTexture, RCAST(Component**)(&m_pTextureCom));
	Safe_AddRef(m_pTextureCom);


	if (root.contains("SpriteType"))
	{
		string SpriteType = root["SpriteType"];
		if (SpriteType == "SHEET")
			m_eType = SPRITETYPE::SHEET;
		else
			m_eType = SPRITETYPE::SEQUENCE;


	}
	
	//없으면 디폴트값 1,1기록
	m_iRow = root.value("Row", 1);
	m_iCol = root.value("Col", 1);

	
	m_iMaxFrame = root.value("MaxFrame", 1);
	m_fFrameSpeed = root.value("FrameSpeed", 5.f);
	m_bLoop = root.value("Loop", false);
	m_bInitPlay = root.value("InitPlay", false);

	*eType = UITYPE::SPRITE;



}
#pragma endregion