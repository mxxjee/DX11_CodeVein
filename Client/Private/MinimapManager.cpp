#include "Client_Define.h"
#include "MinimapManager.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"
#include "MinimapRenderComponent.h"
#include "Transform.h"
#include "Shader.h"
#include "RenderTargetManager.h"
#include "Camera.h"




IMPLEMENT_SINGLETON(MinimapManager);

Client::MinimapManager::MinimapManager()
{
}

Client::MinimapManager::~MinimapManager()
{
}

HRESULT Client::MinimapManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,_uint iWidth, _uint iHeight)
{
	// 이미 Initialize됐는지 확인용
	if (m_pDevice != nullptr)
		return S_OK;

	m_pDevice = pDevice;
	m_pContext = pContext;


	m_pGameInstance = GameInstance::GetInstance();



	CHECK_FAILED(Ready_Minimap_Resource(pDevice, pContext,iWidth,iHeight),E_FAIL);

	m_pVIBuffer = VIBuffer_Rect::Create(m_pDevice,m_pContext);


	//미니맵이 쓸 쉐이더
	m_pShader = m_pGameInstance->Get_Shader_Prototype(Proto_Shader(L"VTXMinimap"));
	m_vecCommonQueue.reserve(100);

	m_vMapCenterUV = World_To_MinimapUV(XMLoadFloat3(&m_vMapCenter));

	m_pLightArrowImg = m_pGameInstance->Load_Texture(L"../../Resources/UI/Textures/Common/Textures/T_Arrow_UI_Alpha.png", true);//발자국



	//행렬저장
	m_matWorldMatrix = IdentityMatrix();
	m_matViewMatrix = IdentityMatrix();
	m_matProjMatrix = IdentityMatrix();

	m_vArrowPos = _float2(0.5f, 0.5f) + _float2(0.f, -0.03f);

	//////각 맵에대한 센터저장
	m_MapCenters[LEVEL::MAIN] = _float3(-156.154f, 8.90f, 99.1936);
	m_MapCenters[LEVEL::BASE] = _float3(0.f, 8.90f,-11.f);
	m_MapCenters[LEVEL::SAMPLE] = _float3(15.9388f, -25.f, 0.f);
	m_MapCenters[LEVEL::CHURCH] = _float3(0.f, 0.f, 0.f);
	return S_OK;
}
HRESULT Client::MinimapManager::Ready_Minimap_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight)
{
	//발자국전용
	m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Minimap_Acuumulate,
		1024.f, 1024.f, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));

	//최종본
	m_pGameInstance-> Add_RenderTarget(RenderTargets::Target_Minimap_Final,
		1024.f, 1024.f, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));


	//센터전용
	m_pGameInstance->Add_RenderTarget(RenderTargets::Target_Minimap_Center,
		1024.f, 1024.f, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f));

	m_pGameInstance->Add_MultiRenderTarget(MRT::MINIMAP_FINAL, RenderTargets::Target_Minimap_Final);

	return S_OK;
}


void Client::MinimapManager::Render_To_Texture(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pContext);

	_uint iNumVP = 1;
	D3D11_VIEWPORT OldVP;
	m_pContext->RSGetViewports(&iNumVP, &OldVP);


	ID3D11RenderTargetView* pOldRTV = nullptr;
	ID3D11DepthStencilView* pOldDSV = nullptr;
	m_pContext->OMGetRenderTargets(1, &pOldRTV, &pOldDSV);



	CHECK_FAILED(Render_Minimap_Accumulate(fTimeDelta),);
	CHECK_FAILED(Render_Minimap_Center(fTimeDelta), );
	CHECK_FAILED(Render_Minimap_Final(fTimeDelta),);



	ID3D11ShaderResourceView* pNullSRV = nullptr;
	m_pContext->PSSetShaderResources(2, 1, &pNullSRV); // Accumulate를 썼던 2번 슬롯 해제
	// UI에서 미니맵을 굽는 슬롯이 4번이라면 4번도 해제
	m_pContext->PSSetShaderResources(4, 1, &pNullSRV);
	m_pContext->OMSetRenderTargets(1, &pOldRTV, pOldDSV);
	m_pContext->RSSetViewports(1, &OldVP);

	//임시 RTV/DSV 해제
	Safe_Release(pOldRTV);
	Safe_Release(pOldDSV);

	//(잠깐주석처리) 큐 비우기
	//m_vecCommonQueue.clear();




}

void Client::MinimapManager::Render_Centered_Minimap(_float fZoom,Shader* pShader)
{
	CHECK_JUST_NULL(pShader);
	CHECK_JUST_NULL(m_pPlayerTransform);
	_vector vPlayerPos = m_pPlayerTransform->Get_State(DIRECTION::POSITION);
	_float2 vPlayerUV = World_To_MinimapUV(vPlayerPos);

	
	//회전값먹일 대상카메라가져오기
	if (m_pRotateCamera)
	{
		_float CameraAngle = m_pRotateCamera->Get_Yaw();
		pShader->Bind_RawValue_ByHandle(g_fCameraAngle, &CameraAngle, sizeof(_float));

		_float2 vPlayerCenterUV = Get_FinalPosOnUI(fZoom, vPlayerUV, vPlayerUV, CameraAngle);
		pShader->Bind_RawValue_ByHandle(g_vPlayerUV, &vPlayerUV, sizeof(_float2));

	}
	
	//셰이더에전달하기
	pShader->Bind_RawValue_ByHandle(g_fZoom, &fZoom, sizeof(_float));


}

void Client::MinimapManager::Render_Full_Minimap(Shader* pShader)
{
	CHECK_JUST_NULL(pShader);

	//전체인벤토리용(축소)
	//셰이더에전달하기
	_float fZoom = 3.f;


	//맵center중심으로 
	pShader->Bind_RawValue_ByHandle(g_vCenterUV, &m_vMapCenterUV, sizeof(_float2));
	pShader->Bind_RawValue_ByHandle(g_fZoom, &fZoom, sizeof(_float));

}

void Client::MinimapManager::Set_Current_Level(LEVEL eLevel)
{
	auto iter = m_MapCenters.find(eLevel);
	if (iter != m_MapCenters.end())
		m_vMapCenter = iter->second;

	else
	{
		MSG_BOX("이 레벨에 대한 센터가 존재하지않습니다.");
		BREAK;
	}
}

HRESULT Client::MinimapManager::Render_Minimap_Accumulate(const _float& fTimeDelta)
{
	CHECK_NULLPTR(m_pShader);



	//클리핑설정
	
	//행렬바인딩
	CHECK_FAILED(m_pShader->Bind_Matrix_ByHandle(g_WorldMatrix, m_matWorldMatrix),E_FAIL );
	CHECK_FAILED(m_pShader->Bind_Matrix_ByHandle(g_ViewMatrix, m_matViewMatrix), E_FAIL);
	CHECK_FAILED(m_pShader->Bind_Matrix_ByHandle(g_ProjMatrix, m_matProjMatrix), E_FAIL);


	ID3D11RenderTargetView* pAccumRTV = m_pGameInstance->Get_RenderTargetView(RenderTargets::Target_Minimap_Acuumulate);
	_float4 vClear = { 0.f, 0.f, 0.f, 0.f };
	m_pContext->ClearRenderTargetView(pAccumRTV, (_float*)&vClear);

	// 2. 타겟 설정 (DSV는 NULL로 줘서 사이즈 에러 방지)
	m_pContext->OMSetRenderTargets(1, &pAccumRTV, nullptr);

	D3D11_VIEWPORT vp = { 0.f, 0.f, 1024.f, 1024.f, 0.f, 1.f };
	m_pContext->RSSetViewports(1, &vp);

	// 3. 발자국 그리기 (Additive 혹은 AlphaBlend)
	m_pGameInstance->Set_BlendState(BSTATE::ADDITIVE);

	for (auto& tData : m_vecTraceQueue)
	{

		m_bCliping = false;
		m_pShader->Bind_RawValue_ByHandle(g_isClipping, &m_bCliping, sizeof(int));
		m_pShader->Bind_RawValue_ByHandle(g_vTargetUV, &tData.UV, sizeof(_float2));

		_float fZeroRotation = 0.f;
		m_pShader->Bind_RawValue_ByHandle(g_fRotation, &fZeroRotation, sizeof(_float));

		tData.m_pTargetComponent->Render_Trace(tData.UV, m_pShader, fTimeDelta);

		
	}

	m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);
	return S_OK;
}


HRESULT Client::MinimapManager::Render_Minimap_Center(const _float& fTimeDelta)
{
	m_pGameInstance->Begin_RT(RenderTargets::Target_Minimap_Center, nullptr);

	int bFasle = true;
	m_pShader->Bind_RawValue_ByHandle(g_isClipping, &bFasle,sizeof(int));

	//기존에그렸던 발자국 기록했던 RTV를 텍스처로넣어준다.
	m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Minimap_Acuumulate, m_pShader, 2);


	m_pShader->Begin(2);
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render(fTimeDelta);

	m_pGameInstance->End_MRT();

	return S_OK;
}

HRESULT Client::MinimapManager::Render_Minimap_Final(const _float& fTimeDelta)
{
	m_pGameInstance->Begin_RT(RenderTargets::Target_Minimap_Final, nullptr);

	//기존에그렸던 발자국 기록했던 RTV를 텍스처로넣어준다.
	m_pGameInstance->Bind_RT_ShaderResource_FullSlot(RenderTargets::Target_Minimap_Acuumulate, m_pShader, 2);

	m_pShader->Begin(2);
	m_pVIBuffer->Bind_Resource();
	m_pVIBuffer->Render(fTimeDelta);


	m_pGameInstance->Set_BlendState(BSTATE::BLEND);

	if (m_PlayerData.m_pTargetComponent)
	{
		Render_PlayerArrow(true, 1.f, fTimeDelta);

		m_pShader->Bind_RawValue_ByHandle(g_vTargetUV, &m_PlayerData.UV, sizeof(_float2));
		m_PlayerData.m_pTargetComponent->Render_Icon(0.f,m_PlayerData.UV, 1.f, m_pShader, fTimeDelta);
	
		

	}

	///////////////실시간 아이콘 출력 (일반꺼출력하고)
	for (auto& desc : m_vecCommonQueue)
	{
		m_pShader->Bind_RawValue_ByHandle(g_vTargetUV, &desc.UV, sizeof(_float2));
		

		desc.m_pTargetComponent->Render_Icon(0.f,desc.UV,1.f,m_pShader, fTimeDelta);
	}

	
	m_pGameInstance->End_MRT();
	m_pGameInstance->Set_BlendState(BSTATE::DEFAULT);
	return S_OK;

}

_float2 Client::MinimapManager::World_To_MinimapUV(XMVECTOR vPos)
{
	//월드의 X->U, Z축을 V로 매칭
	_float3 fPosition;
	_float2 vUV = { 0.f, 0.f };

	XMStoreFloat3(&fPosition, vPos);

	_float fDiffX = fPosition.x - m_vMapCenter.x;
	_float fDiffY = fPosition.z - m_vMapCenter.z;


	//범위정규화(0~1)
	vUV.x = (fDiffX / m_fMapWorldSize) + 0.5f;
	vUV.y = (fDiffY / m_fMapWorldSize) + 0.5f;

	vUV.y = 1.f - vUV.y;
	return vUV;
}

void Client::MinimapManager::Render_Icons_On_UI(_float fZoom, _bool bIsCentered,_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pShader);

	_float fAngle = (m_pRotateCamera) ? m_pRotateCamera->Get_Yaw() : 0.f;
	_float2 vPlayerUV = m_PlayerData.UV;
	

	m_bCliping = true;
	m_pShader->Bind_RawValue_ByHandle(g_isClipping, &m_bCliping, sizeof(int));


	//Full지도라면, 회전X
	if (!bIsCentered)
		fAngle = 0.f;

	//플레이어 렌더
	if (m_PlayerData.m_pTargetComponent)
	{
		Render_PlayerArrow(false,fZoom, fTimeDelta);
		Render_Player(fZoom, fTimeDelta);


	}

	m_bCliping = true;
	m_pShader->Bind_RawValue_ByHandle(g_isClipping, &m_bCliping, sizeof(int));
	for (auto& desc : m_vecCommonQueue)
	{
		//UI상의 uv계산

		_float2 vFinalPosOnUI=Get_FinalPosOnUI(fZoom,desc.UV,vPlayerUV,-fAngle);
		
		if (vFinalPosOnUI.x < 0.f || vFinalPosOnUI.x > 1.f ||
			vFinalPosOnUI.y < 0.f || vFinalPosOnUI.y > 1.f) continue;

		_float fZeroRotation = 0.f;
		m_pShader->Bind_RawValue_ByHandle(g_vTargetUV, &vFinalPosOnUI, sizeof(_float2));
		m_pShader->Bind_RawValue_ByHandle(g_fRotation, &fZeroRotation, sizeof(_float));


		desc.m_pTargetComponent->Render_Icon(fAngle,vFinalPosOnUI, fZoom,m_pShader, 0.f);
	}


	
	//m_vecCommonQueue.clear();

}

void Client::MinimapManager::Render_PlayerArrow(bool bFull,_float fZoom,_float fTimeDelta)
{



	CHECK_JUST_NULL(m_pShader);
	CHECK_JUST_NULL(m_pLightArrowImg);
	CHECK_JUST_NULL(m_pPlayerTransform);

	///////////////플레이어앞대가리 헤드라이트ui//////////////////
	if (bFull && m_pRotateCamera != nullptr)
	{
		_float CameraAngle = m_pRotateCamera->Get_Yaw();
		_float fDist = 0.08f;
		_float fSin = sinf(CameraAngle);
		_float fCos = cosf(CameraAngle);

		_float2 vRotatedOffset;
		vRotatedOffset.x = 0.f * fCos - (-fDist) * fSin;
		vRotatedOffset.y = 0.f * fSin + (-fDist) * fCos;

		_float2 ArrowPos = m_PlayerData.UV + vRotatedOffset;

	

		m_bCliping = false;

		CHECK_FAILED(m_pShader->Bind_Matrix_ByHandle(g_WorldMatrix, m_matWorldMatrix), );
		CHECK_FAILED(m_pShader->Bind_Matrix_ByHandle(g_ViewMatrix, m_matViewMatrix), );
		CHECK_FAILED(m_pShader->Bind_Matrix_ByHandle(g_ProjMatrix, m_matProjMatrix), );


		m_pShader->Bind_RawValue_ByHandle(g_isClipping, &m_bCliping, sizeof(int));
		m_pShader->Bind_RawValue_ByHandle(g_vTargetUV, &ArrowPos, sizeof(_float2));
		m_pShader->Bind_RawValue_ByHandle(g_fRotation, &CameraAngle, sizeof(_float));


		_float fDynamicSize = 0.5f;
		m_pShader->Bind_RawValue_ByHandle(g_fIconScale, &fDynamicSize, sizeof(float));
		m_pShader->Bind_SRV_FullSlot(1, m_pLightArrowImg);

		m_pShader->Begin(1);
		m_pShader->Bind_Resources(1);
		Bind_Resource_By_Buffer();
		Render_By_Buffer(fTimeDelta);
	}
	
	else
	{

	

		_float CameraAngle = 0.f;
		m_bCliping = false;

		m_pShader->Bind_RawValue_ByHandle(g_isClipping, &m_bCliping, sizeof(int));
		m_pShader->Bind_RawValue_ByHandle(g_vTargetUV, &m_vArrowPos, sizeof(_float2));
		m_pShader->Bind_RawValue_ByHandle(g_fRotation, &CameraAngle, sizeof(_float));


		_float fDynamicSize = g_fArrowIconScale/fZoom;
		m_pShader->Bind_RawValue_ByHandle(g_fIconScale, &fDynamicSize, sizeof(float));
		m_pShader->Bind_SRV_FullSlot(1, m_pLightArrowImg);

		m_pShader->Begin(3);
		m_pShader->Bind_Resources(3);
		Bind_Resource_By_Buffer();
		Render_By_Buffer(fTimeDelta);
		
	}
	
}

void Client::MinimapManager::Render_Player(_float fZoom, _float fTimeDelta)
{
	//UI상의 uv계산
	_float CameraAngle = m_pRotateCamera->Get_Yaw();
	_float2 vFinalPosOnUI = Get_FinalPosOnUI(fZoom, m_PlayerData.UV, m_PlayerData.UV, CameraAngle);

	if (vFinalPosOnUI.x < 0.f || vFinalPosOnUI.x > 1.f ||
		vFinalPosOnUI.y < 0.f || vFinalPosOnUI.y > 1.f) return;

	_float fZeroRotation = 0.f;
	m_pShader->Bind_RawValue_ByHandle(g_vTargetUV, &vFinalPosOnUI, sizeof(_float2));

	m_PlayerData.m_pTargetComponent->Render_Icon(CameraAngle,m_PlayerData.UV, fZoom,m_pShader, fTimeDelta);


}

void Client::MinimapManager::Bind_Resource_By_Buffer()
{
	CHECK_JUST_NULL(m_pVIBuffer);
	m_pVIBuffer->Bind_Resource();
}

void Client::MinimapManager::Render_By_Buffer(const _float fTimeDelta)
{
	CHECK_JUST_NULL(m_pVIBuffer);
	m_pVIBuffer->Render(fTimeDelta);
}



void Client::MinimapManager::Free()
{

	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pLightArrowImg);

	__super::Free();
}

_float2 Client::MinimapManager::Get_FinalPosOnUI(_float fZoom, _float2 UV, _float2 PlayerUV, _float fAngle)
{
	_float2 vOffSet = UV - PlayerUV;
	vOffSet.y *= -1.f;

	//지형회전만큼 아이콘위치 설정
	_float fSin = sinf(-fAngle);
	_float fCos = cosf(-fAngle);

	_float2 vRotatedOffset;
	vRotatedOffset.x = vOffSet.x * fCos - vOffSet.y * fSin;
	vRotatedOffset.y = vOffSet.x * fSin + vOffSet.y * fCos;
	vRotatedOffset.y *= -1.f;
	//UI상의 uv계산
	_float2 vFinalPosOnUI;
	vFinalPosOnUI.x = 0.5f + (vRotatedOffset.x / fZoom);
	vFinalPosOnUI.y = 0.5f + (vRotatedOffset.y / fZoom);

	return vFinalPosOnUI;

}


void Client::MinimapManager::Add_Minimap_TraceQueue(const MINIMAPINFO_DESC& tDesc)
{
	CHECK_JUST_NULL(tDesc.m_pTransform);

	_float3 fCurrentPos;
	_vector vCurrentPos = tDesc.m_pTransform->Get_State(DIRECTION::POSITION);

	XMStoreFloat3(&fCurrentPos, vCurrentPos);

	_float fDist = Vector3::Distance(fCurrentPos, m_vLastPos);

	if (fDist > m_fValue)
	{
		MINIMAPDATA Data;
		Data.m_pTargetComponent = tDesc.m_pTargetComponent;
		Data.UV = World_To_MinimapUV(vCurrentPos);

		if (Data.UV.x < 0.f || Data.UV.x > 1.f ||
			Data.UV.y < 0.f || Data.UV.y > 1.f)
			return;


		m_vecTraceQueue.push_back(Data);
		m_vLastPos = fCurrentPos;
	}

	if (m_vecTraceQueue.size() > 500)
		m_vecTraceQueue.pop_front();

	
}

void Client::MinimapManager::Add_Minimap_CommonQueue(const MINIMAPINFO_DESC& tDesc)
{
	_vector vCurrentPos = tDesc.m_pTransform->Get_State(DIRECTION::POSITION);

	MINIMAPDATA Data;
	Data.m_pTargetComponent = tDesc.m_pTargetComponent;
	Data.UV = World_To_MinimapUV(vCurrentPos);


	m_vecCommonQueue.push_back(Data);
}

void Client::MinimapManager::Add_Minimap_PlayerData(const MINIMAPINFO_DESC& tDesc)
{
	_vector vCurrentPos = tDesc.m_pTransform->Get_State(DIRECTION::POSITION);

	MINIMAPDATA Data;
	Data.m_pTargetComponent = tDesc.m_pTargetComponent;
	Data.UV = World_To_MinimapUV(vCurrentPos);


	m_PlayerData = Data;
}

