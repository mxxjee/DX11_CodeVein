#include "Engine_Define.h"
#include "UI_SlotGrid.h"
#include "UIObject.h"
#include "Mouse.h"


Engine::UI_SlotGrid::UI_SlotGrid()
{
}

Engine::UI_SlotGrid::UI_SlotGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UI_Render(pDevice,pContext)
{
}

Engine::UI_SlotGrid::UI_SlotGrid(const UI_SlotGrid& original)
	:UI_Render(original)
{
}

Engine::UI_SlotGrid::~UI_SlotGrid()
{
}

HRESULT Engine::UI_SlotGrid::Initialize_Prototype()
{	return S_OK;
}

HRESULT Engine::UI_SlotGrid::Initialize(void* arg)
{	/*Texture, Buffer를 확정적으로 만들어준다.툴에서 설정할수있도록 한다.*/
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	m_pMouse = Mouse::GetInstance();
	GRID_DESC* pDesc = nullptr;

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
		if (arg)
		{
			pDesc = static_cast<GRID_DESC*>(arg);
			m_tGridDesc = *pDesc;

			if (pDesc->ProtoNewTexKey != L"")
			{
				m_Proto_NewTexKey = m_tGridDesc.ProtoNewTexKey;

				m_pOwner->Add_Component(0, m_tGridDesc.ProtoNewTexKey, Com_NewTexture, RCAST(Component**)(&m_pTextureCom));
				Safe_AddRef(m_pTextureCom);
			}

			m_pTransform = m_pOwner->Get_Transform();

			Rebuild(m_pTextureCom->Get_NumTextures());
		}
	
	}


	return S_OK;
}

_int	Engine::UI_SlotGrid::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);

	/*외부에서따로호출해주기로 변경 (지금은 테스트라서 여기다놓은거)*/
	//dwWheel = m_pGameInstance->MouseMove(MOUSEMOVESTATE::WHEEL);
	//if(dwWheel != 0)
	//{
	//	// 부호에 따라 방향 결정
	//	// 위로 밀면(>0) -1, 아래로 당기면(<0) 아래쪽 줄로 가야 하니 +1
	//	_int iStep = (dwWheel > 0) ? -1 : 1;

	//	this->Scroll_Wheel(iStep);
	//}

	/*if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::LB))
	{
		m_iFocusIdx = Get_FocusIdx_On_Click();

	}*/
	return 0;
}

HRESULT		Engine::UI_SlotGrid::Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket)
{
//#ifdef _DEBUG
//	if (m_bDrawDebug)
//		Render_Debug();
//
//#endif // DEBUG

	CHECK_NULLPTR(m_pShaderCom);
	CHECK_NULLPTR(m_pTextureCom);
	CHECK_NULLPTR(m_pVIBufferCom);
	CHECK_NULLPTR(m_pTransform);

	
	_uint iBeginIdx = Get_BeginDataIdx();
	_uint iRenderCount = min<_uint>(m_tGridDesc.iNumColums * m_tGridDesc.iVisibleRows, m_iTotalCount - iBeginIdx);

	m_SlotZones.clear();

	//부모의 월드행렬을 베이스로
	_float4x4 ParentWorldMatrix;
	XMStoreFloat4x4(&ParentWorldMatrix, m_pTransformCom->Get_WorldMatrix());
	_float fParentX = ParentWorldMatrix._41;
	_float fParentY = ParentWorldMatrix._42;

	for (_uint i = 0; i < iRenderCount; ++i)
	{
		_uint iDataIdx = iBeginIdx + i;

		//슬롯위치계산
		_float2 vSlotLocalPos = Get_SlotLocalPos(i);

		//슬롯용 임시월드행렬(셰이더바인딩용)
		_float4x4 SlotWorldMatrix;
		XMStoreFloat4x4(&SlotWorldMatrix, XMMatrixIdentity());

		SlotWorldMatrix._11 = m_tGridDesc.vSlotSize.x;
		SlotWorldMatrix._22 = m_tGridDesc.vSlotSize.y;

		SlotWorldMatrix._41 = fParentX + vSlotLocalPos.x;
		SlotWorldMatrix._42 = fParentY + vSlotLocalPos.y;

		
		if (iDataIdx == m_iFocusIdx)
			pPacket->g_fUIIntensity = 1.5f;

		else if (iDataIdx == m_iHoverIdx)
			pPacket->g_fUIIntensity = 0.5f;

		else
			pPacket->g_fUIIntensity = 1.f;


		CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, SlotWorldMatrix), E_FAIL);
		CHECK_FAILED(m_pTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 0, iDataIdx), E_FAIL);

		m_pShaderCom->Bind_EntireBuffer_BySlot(BUFFER_UI, pPacket, sizeof(UI_BUFFER_PACKET));

		CHECK_FAILED(m_pShaderCom->Begin(ShaderPassNumber), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_Resources(ShaderPassNumber), E_FAIL);
		CHECK_FAILED(m_pVIBufferCom->Bind_Resource(), E_FAIL);
		CHECK_FAILED(m_pVIBufferCom->Render(0.f), E_FAIL);


		/*피킹용 저장*/
		SLOT_ZONE tZone;
		tZone.iDataIdx = i;
		tZone.vWorldPos.x = SlotWorldMatrix._41 + (Engine_WINCX * 0.5f);
		tZone.vWorldPos.y = -SlotWorldMatrix._42 + (Engine_WINCY * 0.5f);
		m_SlotZones.push_back(tZone);
	}

	return S_OK;
}

HRESULT Engine::UI_SlotGrid::Change_TargetTexture(wstring ProtoTexKey, _uint ProtoLevel, void* pArg, _uint TotalCount)
{
	if (m_pTextureCom)
		Safe_Release(m_pTextureCom);

	//교체
	m_pTextureCom = (NewTexture*)m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, ProtoLevel,ProtoTexKey, pArg);
	
	// 3. 데이터 갱신
	if (m_pTextureCom)
	{
		if (TotalCount == 9999)
			Rebuild(m_pTextureCom->Get_NumTextures());
		else
			Rebuild(TotalCount);
	}
		
	m_iHoverIdx = -1;

	return S_OK;
}

_uint Engine::UI_SlotGrid::Get_FocusIdx_On_Click(bool bUpdate)
{

	POINT ptMouse = m_pMouse->Get_MousePos(); // 윈도우 스크린 좌표

	// [스크린 좌표 방식 충돌 체크]
	// Render_UI에서 이번 프레임에 저장한 m_SlotZones를 순회함
	for (auto& tZone : m_SlotZones)
	{
		_float fHalfX = m_tGridDesc.vSlotSize.x * 0.5f;
		_float fHalfY = m_tGridDesc.vSlotSize.y * 0.5f;

		// 마우스 좌표가 슬롯의 스크린 영역 안에 들어왔는지 확인
		if (ptMouse.x >= tZone.vWorldPos.x - fHalfX && ptMouse.x <= tZone.vWorldPos.x + fHalfX &&
			ptMouse.y >= tZone.vWorldPos.y - fHalfY && ptMouse.y <= tZone.vWorldPos.y + fHalfY)
		{
			_int iRealIdx = (m_iCurrentRow * m_tGridDesc.iNumColums) + tZone.iDataIdx;

			if (iRealIdx < (_int)m_iTotalCount)
			{
				if (bUpdate)                                                                                                                                                                                
				{
					m_iFocusIdx = iRealIdx;
					return m_iFocusIdx;
				}

				else
					return m_iFocusIdx;
				
			}
		}
	}

	return -1;
}

_uint Engine::UI_SlotGrid::Get_FocusIdx_On_Hover()
{
	POINT ptMouse = m_pMouse->Get_MousePos(); // 윈도우 스크린 좌표

	// [스크린 좌표 방식 충돌 체크]
	// Render_UI에서 이번 프레임에 저장한 m_SlotZones를 순회함
	for (auto& tZone : m_SlotZones)
	{
		_float fHalfX = m_tGridDesc.vSlotSize.x * 0.5f;
		_float fHalfY = m_tGridDesc.vSlotSize.y * 0.5f;

		// 마우스 좌표가 슬롯의 스크린 영역 안에 들어왔는지 확인
		if (ptMouse.x >= tZone.vWorldPos.x - fHalfX && ptMouse.x <= tZone.vWorldPos.x + fHalfX &&
			ptMouse.y >= tZone.vWorldPos.y - fHalfY && ptMouse.y <= tZone.vWorldPos.y + fHalfY)
		{
			_int iRealIdx = (m_iCurrentRow * m_tGridDesc.iNumColums) + tZone.iDataIdx;

			if (iRealIdx < (_int)m_iTotalCount)
			{
				m_iHoverIdx = iRealIdx;
				return m_iHoverIdx;

			}

			else
				m_iHoverIdx = -1;

		}

		else
			m_iHoverIdx = -1;

	}

	return -1;
}

#ifdef _DEBUG
void Engine::UI_SlotGrid::Render_Imgui()
{
	if (ImGui::InputInt("Num cols:", &m_tGridDesc.iNumColums))
	{
		Rebuild(m_iTotalCount);
	}

	if (ImGui::InputInt("TotalCount", &m_iTotalCount))
	{
		Rebuild(m_iTotalCount);
	}

	ImGui::DragInt("VisibleRows", &m_tGridDesc.iVisibleRows);
	ImGui::DragFloat2("SlotSize", &m_tGridDesc.vSlotSize.x);
	ImGui::DragFloat2("Spacing", &m_tGridDesc.vSpacing.x);
	ImGui::DragFloat2("StartPos", &m_tGridDesc.vStartPos.x);

	char Buff[64];
	strcpy_s(Buff, sizeof(Buff), wstringToString(m_Proto_NewTexKey).c_str());
	if (ImGui::InputText("ProtoNewTexture", Buff, sizeof(Buff), ImGuiInputTextFlags_EnterReturnsTrue))
	{
		m_Proto_NewTexKey = stringToWstring(Buff);
		int iLevelIdx = 0;
		
		// 기존 컴포넌트 교체 로직 호출(일단테스트용 0넣기)
		GRID_DESC Desc;
		Change_TargetTexture(m_Proto_NewTexKey.c_str(), iLevelIdx, &Desc);

		// 텍스처 개수에 맞춰 자동 리빌드
		if (m_pTextureCom)
			Rebuild(m_pTextureCom->Get_NumTextures());

		COUT("Texture Swapped: " << Buff);

		
	}
}
#endif

void Engine::UI_SlotGrid::Save_Data(ordered_json& pJson)
{
	__super::Save_Data(pJson);

	pJson["ProtoTexture"] = wstringToString(m_Proto_NewTexKey);
	pJson["NumColums"] = m_tGridDesc.iNumColums;
	pJson["VisibleRows"] = m_tGridDesc.iVisibleRows;
	pJson["SlotSize"] = { m_tGridDesc.vSlotSize.x,m_tGridDesc.vSlotSize.y };
	pJson["Spacing"] = { m_tGridDesc.vSpacing.x,m_tGridDesc.vSpacing.y };
	pJson["StartPos"] = { m_tGridDesc.vStartPos.x,m_tGridDesc.vStartPos.y };
	pJson["ProtoLevel"] = iProtoLevel;



}

void Engine::UI_SlotGrid::Load_Data(GameObject* pOwner, ComponentData& Data, UITYPE* eType)
{
	__super::Load_Data(pOwner,Data,eType);

	CHECK_JUST_NULL(pOwner);
	ordered_json root = Data.ComJson;

	if (root.contains("NumColums"))
		m_tGridDesc.iNumColums = root["NumColums"];

	if (root.contains("VisibleRows"))
		m_tGridDesc.iVisibleRows = root["VisibleRows"];

	if (root.contains("SlotSize"))
		m_tGridDesc.vSlotSize = _float2(root["SlotSize"][0], root["SlotSize"][1]);

	if (root.contains("Spacing"))
		m_tGridDesc.vSpacing = _float2(root["Spacing"][0], root["Spacing"][1]);


	if (root.contains("StartPos"))
		m_tGridDesc.vStartPos = _float2(root["StartPos"][0], root["StartPos"][1]);

	if (root.contains("ProtoLevel"))
		iProtoLevel = root["ProtoLevel"];

	m_Proto_NewTexKey = stringToWstring(root.value("ProtoTexture", "Prototype_Component_UITexture_UIResource"));

	//오너에게 vibuffer를 추가한다.
	//기본 texture를 추가한다.
	m_pOwner->Add_Component(0, L"Prototype_Component_VIBuffer_Rect", Com_VIBuffer, RCAST(Component**)(&m_pVIBufferCom));
	Safe_AddRef(m_pVIBufferCom);

	m_pOwner->Add_Component(iProtoLevel, m_Proto_NewTexKey, Com_NewTexture, RCAST(Component**)(&m_pTextureCom));
	Safe_AddRef(m_pTextureCom);

	Rebuild(m_pTextureCom->Get_NumTextures());
	m_pTransform = m_pOwner->Get_Transform();
}

void Engine::UI_SlotGrid::Rebuild(_int iTotalCount)
{
	m_iTotalCount = iTotalCount;
	m_iCurrentRow = 0;
	m_iMaxRows = (m_iTotalCount + m_tGridDesc.iNumColums - 1) / m_tGridDesc.iNumColums;

}

_float2 Engine::UI_SlotGrid::Scroll_Wheel(_int iStep)
{
	m_iCurrentRow += iStep; // iStep은 보통 1 또는 -1
	_int iScrollableRows = (int)m_iMaxRows - (int)m_tGridDesc.iVisibleRows;
	if (iScrollableRows < 0) iScrollableRows = 0;

	m_iCurrentRow = clamp<_int>(m_iCurrentRow, 0, iScrollableRows);

	// 스크롤바 연동을 위해 비율을 반환해 주면 좋음
	_float fRatio = 0.f;
	if (m_iMaxRows > m_tGridDesc.iVisibleRows)
		fRatio = (_float)m_iCurrentRow / (_float)(m_iMaxRows - m_tGridDesc.iVisibleRows);

	return _float2(fRatio, (_float)m_iCurrentRow);
}





UI_SlotGrid* Engine::UI_SlotGrid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	UI_SlotGrid* pInstance = new UI_SlotGrid(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(), L"UI_SlotGrid 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}


#ifdef _DEBUG
void	Engine::UI_SlotGrid::Render_Debug()
{
	CHECK_JUST_NULL(m_pLineShader);
	CHECK_JUST_NULL(m_pDebugVB);

	for (auto& tZone : m_SlotZones)
	{
		// 스크린 좌표를 다시 엔진 렌더링 좌표로 역변환
		_float fEngineX = tZone.vWorldPos.x - (Engine_WINCX * 0.5f);
		_float fEngineY = -(tZone.vWorldPos.y - (Engine_WINCY * 0.5f));

		// 테두리는 픽셀 크기 그대로 출력
		_matrix matDebugWorld = XMMatrixScaling(m_tGridDesc.vSlotSize.x, m_tGridDesc.vSlotSize.y, 1.f)
			* XMMatrixTranslation(fEngineX, fEngineY, 0.f);

		_float4x4 fMatDebugWorld;
		// 셰이더가 전치를 기다리므로 여기서도 Transpose 해줘야 함
		XMStoreFloat4x4(&fMatDebugWorld, XMMatrixTranspose(matDebugWorld));

		if (FAILED(m_pLineShader->Bind_Matrix_ByHandle(g_WorldMatrix, fMatDebugWorld)))
			continue;

		m_pLineShader->Begin(_UINT(m_pOwner->Get_SelectState()));
		m_pLineShader->Bind_Resources(0);

		UINT stride = sizeof(VTXPOSCOR), offset = 0;
		m_pContext->IASetVertexBuffers(0, 1, &m_pDebugVB, &stride, &offset);
		m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		m_pContext->Draw(5, 0);
	}

}
#endif // _DEBUG


Component* Engine::UI_SlotGrid::Clone(void* arg)
{
	UI_SlotGrid* pInstance = new UI_SlotGrid(*this);
	MSG_FAIL(pInstance->Initialize(arg), L"UI_SlotGrid 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}
void Engine::UI_SlotGrid::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	__super::Free();
}
