#include "Engine_Define.h"
#include "UI_Text.h"
#include "Shader.h"
#include "UIObject.h"
#include "MyFont.h"
#include "GameObject.h"
#include "Sampler.h"


#ifdef _DEBUG
#include "Editor_Utils.h"
#endif // _DEBUG


Engine::UI_Text::UI_Text()
{
}

Engine::UI_Text::UI_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UI_Render(pDevice,pContext)
{
	m_iSavePriority = 0;

}

Engine::UI_Text::UI_Text(const UI_Text& original)
    :UI_Render(original), m_pBatch(original.m_pBatch)
{
}

Engine::UI_Text::~UI_Text()
{
}

HRESULT Engine::UI_Text::Initialize_Prototype()
{
	m_pBatch = m_pGameInstance->Get_Batch();

    return S_OK;
}

HRESULT Engine::UI_Text::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	m_pScissorRS = m_pGameInstance->Get_RS("RS_ScissorEnable");
	Safe_AddRef(m_pScissorRS);

	m_pDefaultRS = m_pGameInstance->Get_RS("RS_Default");
	Safe_AddRef(m_pDefaultRS);

	m_pAlphaBS = m_pGameInstance->Get_BS("BS_PremultipliedAlpha");
	Safe_AddRef(m_pAlphaBS);

	if (m_pOwner != nullptr)
	{
		TEXTDESC* pDesc = static_cast<TEXTDESC*>(arg);
		if (pDesc)
		{
			/*매니저에서 원본 폰트참조받기*/
			m_ProtoFontName = pDesc->ProtoFontName;
			m_pFont=m_pGameInstance->Get_Font(pDesc->ProtoFontName);
			if (m_pFont)
			{
				Safe_AddRef(m_pFont);
				ID3D11ShaderResourceView* pFontSRV = nullptr;
				m_pFont->Get_SpriteFont()->GetSpriteSheet(&pFontSRV);

				ID3D11Resource* pResource = nullptr;
				pFontSRV->GetResource(&pResource);
				m_pTexture = (ID3D11Texture2D*)pResource;
				Safe_Release(pResource);
				Safe_Release(pFontSRV);
			}

			m_Text = pDesc->InitText;
			m_TextColor = pDesc->vDefaultFontColor;

			m_bTypeEffect = pDesc->bTypeEffect;
			
			m_fOffSetScale = pDesc->fScale;
			m_fOffSetRotation = pDesc->fRotation;
			m_vOffSetPos = pDesc->vPosition;


			Component* ppVIBuffercom = m_pOwner->Get_Component_FromName(L"Prototype_Component_VIBuffer_Rect");
			if (!ppVIBuffercom)
			{
				m_pOwner->Add_Component(0, L"Prototype_Component_VIBuffer_Rect", Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));
				Safe_AddRef(m_pVIBufferCom);
			}


		}
	}


	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = 512;  // 충분한 크기
	texDesc.Height = 128;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	m_pDevice->CreateTexture2D(&texDesc, nullptr, &m_pBakeTexture);
	m_pDevice->CreateRenderTargetView(m_pBakeTexture, nullptr, &m_pBakeRTV);
	m_pDevice->CreateShaderResourceView(m_pBakeTexture, nullptr, &m_pBakeSRV);

    return S_OK;
}

#ifdef _DEBUG
void	Engine::UI_Text::Render_Imgui()
{

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text("TextComponent");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	__super::Render_Imgui();

	//값조ㅓ절 UI
	//색상

	if (ImGui::ColorEdit4("TextColor", (_float*)&m_TextColor))
	{
		m_vOriginColor = m_TextColor;
	}

	ImGui::Checkbox("Use OutLine", &m_bUseOutLine);
	if (m_bUseOutLine)
	{
		ImGui::DragFloat("Thickness", &m_fThickness);
		ImGui::ColorEdit4("OutLine Color", (_float*)&m_TextOutLineColor);
	}
	ImGui::Checkbox("Use CenterAlign", &m_bUseCenter);
	
	if (ImGui::Checkbox("m_bUseMask", &m_bUseMask))
	{
		if (m_bUseMask)
		{
			Set_ParentMask();
		}
	}


	ImGui::Checkbox("UseAlphaBlend", &m_bUseAlphaBlend);

	ImGui::Checkbox("UseSpacing", &m_bUseSpacing);
	if (m_bUseSpacing)
	{
		if (ImGui::SliderFloat("Spacing", &m_fSpacing, 0.f, 1000.f))
		{

		}
	}


	if(ImGui::Checkbox("WorldFont", &m_bWorldFont))
	{
	
	}
	

	//오프셋값들
	ImGui::DragFloat2("OffSet_Scale", (_float*)&m_fOffSetScale.x,0.0001f);
	ImGui::DragFloat("OffSet_RotationZ", (_float*)&m_fOffSetRotation);
	ImGui::DragFloat2("OffSet_Pos", (_float*)&m_vOffSetPos.x);

	char Buff[1024];

	strcpy_s(Buff, sizeof(Buff), wstringToString(m_Text).c_str());
	Render_TextList();
	if(ImGui::InputTextMultiline("Text:", Buff,sizeof(Buff)))
	{
		m_Text = stringToWstring(Buff);
	}
}

void Engine::UI_Text::Render_TextList()
{
	vector<string>		AllFonts = m_pGameInstance->Get_All_FontName();
	vector<const char*>		AllFontsStr;

	for (auto& Name : AllFonts)
		AllFontsStr.push_back(Name.c_str());

	m_CurrentIdx = Get_CurrentIdx(AllFonts);

	
	if (ImGui::Combo("Text Type", &m_CurrentIdx, AllFontsStr.data(), (int)AllFontsStr.size()))
	{
		wstring SelectTextName = stringToWstring(AllFontsStr[m_CurrentIdx]);
		MyFont* pNewFont = m_pGameInstance->Get_Font(SelectTextName);
		if (pNewFont)
		{
			//원래있던것과 교체
			Safe_Release(m_pFont);
			m_pFont = pNewFont;
			if (m_pFont)
			{
				Safe_AddRef(m_pFont);
				ID3D11ShaderResourceView* pFontSRV = nullptr;
				m_pFont->Get_SpriteFont()->GetSpriteSheet(&pFontSRV);

				ID3D11Resource* pResource = nullptr;
				pFontSRV->GetResource(&pResource);
				m_pTexture = (ID3D11Texture2D*)pResource;

				Safe_Release(pResource);
				Safe_Release(pFontSRV);

				Bake_Text(true);
			}

			//원본이름갱신
			m_ProtoFontName = stringToWstring(AllFontsStr[m_CurrentIdx]);

		}
	}

}

int Engine::UI_Text::Get_CurrentIdx(const vector<string>& AllFonts)
{
	//currentIdx구하기
	int iIdx = 0;

	string CurrentFontName = wstringToString(m_ProtoFontName);
	for (auto& pFont : AllFonts)
	{
		if (pFont == CurrentFontName)
			return iIdx;

		else
			++iIdx;
	}

	return iIdx;
}


#endif // _DEBUG

HRESULT Engine::UI_Text::Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket)
{
	CHECK_NULLPTR(pPacket);
	CHECK_NULLPTR(m_pFont);
	CHECK_NULLPTR(m_pBatch);

	//문제있으면 주석풀기!!!!
	//__super::Render_UI(ShaderPassNumber,pPacket);

	const UITransform	Transform = m_pOwner->Get_CombinedTransform();
	

	m_TextColor.w = Transform.m_fAlpha;//알파값은 부모의 alpha따라가기
	//m_TextColor.x = m_OriginColor.x*Transform.m_fAlpha;
	//m_TextColor.y = m_OriginColor.y * Transform.m_fAlpha;
	//m_TextColor.z = m_OriginColor.z * Transform.m_fAlpha;

	if (m_bUseOutLine)
		m_TextOutLineColor.w = m_TextColor.w;

	
	_float2 vOrigin = { 0.f,0.f };

	if (m_bUseCenter)
	{
		XMVECTOR vSize = m_pFont->Get_SpriteFont()->MeasureString(m_Text.c_str());
		XMStoreFloat2(&vOrigin, vSize * 0.5f);
	}
		
#pragma region 아웃라인 사용
	if (m_bUseOutLine)
	{
		
		_float2 vOrigin = _float2(Transform.m_fSizeX * 0.5f, Transform.m_fSizeY * 0.5f);

		m_pGameInstance->Draw_Text_Outline_Begin(m_pFont->Get_SRV(), m_pTexture, m_fThickness, m_TextOutLineColor, m_TextColor);
		m_pFont->Draw_Text(m_pBatch, m_Text,
			_float2(Transform.m_fX, Transform.m_fY) + m_vOffSetPos,
			XMLoadFloat4(&m_TextColor),//텍스트 색상을 넘겨주고!! outlinecolor는 따로바인딩.
			Transform.m_fRotationZ + m_fOffSetRotation,
			vOrigin,
			_float2(Transform.m_fSizeX, Transform.m_fSizeY) * m_fOffSetScale);

		m_pGameInstance->Draw_Text_End();
	}
#pragma endregion

	else if (m_bWorldFont)
	{
		
		if (m_Text == L""  || m_pOwner->Get_CombinedAlpha()<0.01f)
			return S_OK;

		//텍스트를 텍스처로 굽는다 (변경시에만 작동)
		Set_VIBuffer();
		Bake_Text();

		// 부모의 셰이더를 가져와서 3D 변수들 바인딩
		if (m_pShaderCom) {

		/*	if (m_pBakeSRV)
			{
				ImGui::Text("Baked Texture Preview:");
				ImGui::Image((void*)m_pBakeSRV, ImVec2(512, 128));
			}
*/


	
			CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix), E_FAIL);
		
			ID3D11ShaderResourceView* pNullSRV = nullptr;
			m_pContext->PSSetShaderResources(0, 1, &pNullSRV); // 0번 슬롯 일단 비우기
			
			//m_pContext->PSSetShaderResources(0, 1, &m_pBakeSRV);
			m_pShaderCom->Bind_SRV_FullSlot(0, m_pBakeSRV);

			pPacket->g_UIColor = m_TextColor;
			m_pShaderCom->Bind_EntireBuffer_BySlot(BUFFER_UI, pPacket, sizeof(UI_BUFFER_PACKET));

			m_pShaderCom->Begin(ShaderPassNumber); // 알파블렌딩 패스 번호 권장
			CHECK_FAILED(m_pShaderCom->Bind_Resources(ShaderPassNumber), E_FAIL);
			// 부모 UIObject가 가진 VIBuffer_Rect를 렌더링
			CHECK_FAILED(m_pVIBufferCom->Bind_Resource(), E_FAIL);
			CHECK_FAILED(m_pVIBufferCom->Render(0.f), E_FAIL);
		}


		return S_OK;
	}
	else if (m_bUseMask)
	{
#pragma region 마스킹1
		//m_pBatch->Begin(SpriteSortMode_Immediate,
		//	m_pGameInstance->Get_BS("BS_PremultipliedAlpha"),
		//	m_pGameInstance->Get_Sampler("LinearClampSampler")->GetSamplerState(), // 이 부분 추가
		//	nullptr, nullptr, nullptr);


		//m_pShaderCom->Bind_EntireBuffer_BySlot(BUFFER_UI, pPacket, sizeof(UI_BUFFER_PACKET));
		//ID3D11ShaderResourceView* pFontSRV = m_pFont->Get_SRV(); // 폰트의 SRV를 가져오는 함수
		//m_pContext->PSSetShaderResources(0, 1, &pFontSRV);
		//
		//m_pShaderCom->Bind_Resources(m_iPassNum);
		//m_pShaderCom->Begin(m_iPassNum);

		//
		//m_pFont->Draw_Text(m_pBatch, m_Text,
		//	_float2(Transform.m_fX, Transform.m_fY) + m_vOffSetPos,
		//	XMLoadFloat4(&m_TextColor),
		//	Transform.m_fRotationZ + m_fOffSetRotation,
		//	vOrigin,
		//	_float2(Transform.m_fSizeX, Transform.m_fSizeY) * m_fOffSetScale);

		//m_pBatch->End();
#pragma endregion

		D3D11_RECT scissorRect;
		scissorRect.left = (long)(Transform.m_fX - Transform.m_fSizeX * 0.5f);
		scissorRect.top = (long)(Transform.m_fY - Transform.m_fSizeY * 0.5f);
		scissorRect.right =(long)pPacket->g_fClipX; // 계산된 절단선
		scissorRect.bottom = (long)(Transform.m_fY + Transform.m_fSizeY * 0.5f);

		//경고창없애기위해! 
		scissorRect.left = (LONG)min(scissorRect.left, scissorRect.right);
		scissorRect.right = (LONG)max(scissorRect.left, scissorRect.right);
		scissorRect.top = (LONG)min(scissorRect.top, scissorRect.bottom);
		scissorRect.bottom = (LONG)max(scissorRect.top, scissorRect.bottom);

		m_pGameInstance->Draw_Text_Mask_Begin();

		//Scissor 영역 설정
		m_pContext->RSSetScissorRects(1, &scissorRect);

		//SpriteBatch 기본 셰이더 사용
		m_pFont->Draw_Text(m_pBatch, m_Text,
			_float2(Transform.m_fX, Transform.m_fY) + m_vOffSetPos,
			XMLoadFloat4(&m_TextColor),
			Transform.m_fRotationZ + m_fOffSetRotation,
			vOrigin,
			_float2(Transform.m_fSizeX * m_fOffSetScale.x, Transform.m_fSizeY * m_fOffSetScale.y) );


		
		//상태 복구
		m_pGameInstance->Draw_Text_End();
	
	}

	

	//간격모드
	else if (m_bUseSpacing)
	{
		if(m_bUseAlphaBlend)
			m_pGameInstance->Draw_Text_Alpha_Begin();


		else
			m_pGameInstance->Draw_Text_Begin();


		_float2  vCurrentPos = _float2(Transform.m_fX, Transform.m_fY) + m_vOffSetPos;
		_float	 fSizeWidth = Transform.m_fSizeX * m_fOffSetScale.x;

		for (auto& c : m_Text)
		{
			wchar_t str[2] = { c,L'\0' };
			m_pFont->Draw_Text(m_pBatch, str,
				vCurrentPos,
				XMLoadFloat4(&m_TextColor),
				Transform.m_fRotationZ + m_fOffSetRotation,
				vOrigin,
				_float2(fSizeWidth, Transform.m_fSizeY * m_fOffSetScale.y));


			_vector Size = m_pFont->Get_SpriteFont()->MeasureString(str);
			vCurrentPos.x += (XMVectorGetX(Size) * fSizeWidth) + m_fSpacing;

		}

		m_pGameInstance->Draw_Text_End();
	}

	else if (m_bUseAlphaBlend)
	{
		m_pGameInstance->Draw_Text_Alpha_Begin();

		m_pFont->Draw_Text(m_pBatch, m_Text,
			_float2(Transform.m_fX, Transform.m_fY) + m_vOffSetPos,
			XMLoadFloat4(&m_TextColor),
			Transform.m_fRotationZ + m_fOffSetRotation,
			vOrigin,
			_float2(Transform.m_fSizeX * m_fOffSetScale.x, Transform.m_fSizeY * m_fOffSetScale.y));

		m_pGameInstance->Draw_Text_End();

	}



	else
		{
		m_pGameInstance->Draw_Text_Begin();
		m_pFont->Draw_Text(m_pBatch, m_Text,
			_float2(Transform.m_fX, Transform.m_fY) + m_vOffSetPos,
			XMLoadFloat4(&m_TextColor),
			Transform.m_fRotationZ + m_fOffSetRotation,
			vOrigin,
			_float2(Transform.m_fSizeX * m_fOffSetScale.x, Transform.m_fSizeY * m_fOffSetScale.y));

		m_pGameInstance->Draw_Text_End();
		}
	







	
	return S_OK;
}

UI_Text* Engine::UI_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	UI_Text* pInstance = new UI_Text(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"UI_Text 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::UI_Text::Clone(void* arg)
{
	UI_Text* pInstance = new UI_Text(*this);

	MSG_FAIL(pInstance->Initialize(arg), L"UI_Text 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}

void Engine::UI_Text::Free()
{
	Safe_Release(m_pScissorRS);
	Safe_Release(m_pDefaultRS);
	Safe_Release(m_pAlphaBS);

	Safe_Release(m_pFont);

	Safe_Release(m_pBakeTexture);
	Safe_Release(m_pBakeRTV);
	Safe_Release(m_pBakeSRV);

	//Safe_Release(m_pTexture);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}

void Engine::UI_Text::Set_ParentMask()
{
	UI_Progress* pMaskProgress = m_pOwner->Get_MaskProgress();
	if (pMaskProgress)
	{
		m_bUseMask = true;
		m_pShaderCom = m_pOwner->Get_Shader();
	}

}

void Engine::UI_Text::Set_VIBuffer()
{
	CHECK_TRUE(m_pVIBufferCom != nullptr);

	Component* ppVIBuffercom = m_pOwner->Get_Component_FromName(L"Prototype_Component_VIBuffer_Rect");
	if (!ppVIBuffercom)
	{
		m_pOwner->Add_Component(0, L"Prototype_Component_VIBuffer_Rect", Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));
		Safe_AddRef(m_pVIBufferCom);
	}

}

#pragma region parsing

void	Engine::UI_Text::Save_Data(ordered_json& pJson)
{

	pJson["FontName"] = wstringToString(m_ProtoFontName);
	pJson["InnerText"] = wstringToString(m_Text);
	pJson["TextColor"] = { m_TextColor.x,m_TextColor.y,m_TextColor.z,m_TextColor.w };

	pJson["Use OutLine"] = m_bUseOutLine;
	if (m_bUseOutLine)
	{
		pJson["OutLine Color"] = { m_TextOutLineColor.x,m_TextOutLineColor.y,m_TextOutLineColor.z,m_TextOutLineColor.w };
		pJson["Thickness"] = m_fThickness;

	}
	
	pJson["UseCenterAlign"] = m_bUseCenter;
	pJson["UseAlphaBlend"] = m_bUseAlphaBlend;
	pJson["UseSpacing"] = m_bUseSpacing;
	if (m_bUseSpacing)
	{
		pJson["Spacing"] = m_fSpacing;
	}

	pJson["UseWorldFont"] = m_bWorldFont;

	pJson["OffSetScale"] = { m_fOffSetScale.x, m_fOffSetScale.y };
	pJson["OffSetRotation"] = m_fOffSetRotation;
	pJson["OffSetPos"] = { m_vOffSetPos.x,m_vOffSetPos.y };


	pJson["TypeEffect"] = m_bTypeEffect;
	


}
void Engine::UI_Text::Load_Data(GameObject* pOwner,ComponentData& Data, UITYPE* eType)
{
	__super::Load_Data(pOwner, Data,eType);

	CHECK_JUST_NULL(pOwner);
	ordered_json root = Data.ComJson;

	if (root.contains("FontName"))
	{

		m_ProtoFontName = stringToWstring(root["FontName"]);
		m_pFont = m_pGameInstance->Get_Font(m_ProtoFontName);
		if (m_pFont)
		{
			Safe_AddRef(m_pFont);
			ID3D11ShaderResourceView* pFontSRV = nullptr;
			m_pFont->Get_SpriteFont()->GetSpriteSheet(&pFontSRV);

			ID3D11Resource* pResource = nullptr;
			pFontSRV->GetResource(&pResource);
			m_pTexture = (ID3D11Texture2D*)pResource;
			Safe_Release(pResource);
			Safe_Release(pFontSRV);
		}
	}


	

	if (root.contains("TextColor") && root["TextColor"].is_array())
	{
		auto& color = root["TextColor"];
		m_TextColor.x = color[0];
		m_TextColor.y = color[1];
		m_TextColor.z = color[2];
		m_TextColor.w = color[3];

		m_vOriginColor = m_TextColor;

	}

	m_bUseOutLine= root.value("Use OutLine",false);
	if (m_bUseOutLine)
	{
		m_fThickness = root["Thickness"];

		auto& color = root["OutLine Color"];
		m_TextOutLineColor.x = color[0];
		m_TextOutLineColor.y = color[1];
		m_TextOutLineColor.z = color[2];
		m_TextOutLineColor.w = color[3];

	
	}

	
	m_bUseCenter = root.value("UseCenterAlign", false);
	m_bUseAlphaBlend = root.value("UseAlphaBlend", false);
	m_bUseSpacing = root.value("UseSpacing", false);
	if (m_bUseSpacing)
	{
		m_fSpacing = root.value("Spacing", 0.f);
	}

	if (root.contains("OffSetScale"))
	{
		auto& scale = root["OffSetScale"];

		if (scale.is_array())
		{
			m_fOffSetScale.x = scale[0];
			m_fOffSetScale.y = scale[1];
		}
			//단일 값인 경우 (float)
		else if (scale.is_number())
		{
			float fOldScale = scale.get<float>();
			m_fOffSetScale.x = fOldScale;
			m_fOffSetScale.y = fOldScale; // 기존 scale 값을 x, y에 동일하게 배분
		}
	}
	else
	{
		m_fOffSetScale = _float2(1.f, 1.f);
	}

	m_fOffSetRotation = root.value("OffSetRotation", 0.f);

	
	if (root.contains("OffSetPos"))
		m_vOffSetPos = _float2(root["OffSetPos"][0], root["OffSetPos"][1]);


	m_bTypeEffect = root.value("TypeEffect", false);
	m_bWorldFont = root.value("UseWorldFont", false);
	if (m_bWorldFont)
	{
		m_pOwner->Add_Component(0, L"Prototype_Component_VIBuffer_Rect", Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));
		Safe_AddRef(m_pVIBufferCom);
	}


	Set_Text(stringToWstring(root.value("InnerText", "")));
	Set_ParentMask();
	*eType = UITYPE::TEXT;

}
HRESULT Engine::UI_Text::Bake_Text(bool bForce)
{
	
	if (!bForce)
	{
		if (m_strPreText == m_Text)
			return S_OK;
	}


	ID3D11ShaderResourceView* pNullSRVs[16] = { nullptr };
	m_pContext->PSSetShaderResources(0, 16, pNullSRVs); // 모든 슬롯 클리어


	ID3D11ShaderResourceView* pNullSRV[1] = { nullptr };
	m_pContext->VSSetShaderResources(0, 1, pNullSRV);
	m_pContext->PSSetShaderResources(0, 1, pNullSRV);
	m_pContext->GSSetShaderResources(0, 1, pNullSRV);

	_uint iNumVP = 1;
	D3D11_VIEWPORT oldVP;
	m_pContext->RSGetViewports(&iNumVP, &oldVP);

	ID3D11RenderTargetView* pOldRTV = nullptr;
	ID3D11DepthStencilView* pOldDSV = nullptr;
	m_pContext->OMGetRenderTargets(1, &pOldRTV, &pOldDSV);


	_float clearColor[4] = {0.f, 0.f, 0.f,0.f };
	m_pContext->ClearRenderTargetView(m_pBakeRTV, clearColor);
	m_pContext->OMSetRenderTargets(1, &m_pBakeRTV, nullptr);

	D3D11_VIEWPORT vp = { 0.f, 0.f, 512.f, 128.f, 0.f, 1.f };
	m_pContext->RSSetViewports(1, &vp);

	m_pBatch->Begin();

	_float2 vPos = { 10.f, 64.f };
	XMVECTOR vSize = m_pFont->Get_SpriteFont()->MeasureString(m_Text.c_str());
	_float2 vOrigin = { 0.f, XMVectorGetY(vSize) * 0.5f };

	_float4 vWhite = { 1.f, 1.f, 1.f, 1.f };
	m_pFont->Draw_Text(m_pBatch, m_Text.c_str(),
		vPos, XMLoadFloat4(&vWhite), 0.f, vOrigin, _float2(1.f, 1.f));

	m_pBatch->End();

	// 3. 복구
	m_pContext->OMSetRenderTargets(1, &pOldRTV, pOldDSV);
	m_pContext->RSSetViewports(1, &oldVP);

	Safe_Release(pOldRTV);
	Safe_Release(pOldDSV);

	m_strPreText = m_Text;
	m_vPreColor = m_TextColor; //_float4(m_TextColor.x, m_TextColor.y, m_TextColor.z,1.f);

	return S_OK;
}
#pragma endregion

void        Engine::UI_Text::Set_Text(wstring wstr)
{
	m_Text = wstr;
	if (m_bWorldFont)
		Bake_Text();
}