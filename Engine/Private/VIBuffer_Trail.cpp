#include "Engine_Define.h"
#include "VIBuffer_Trail.h"

Engine::VIBuffer_Trail::VIBuffer_Trail()
{
}

Engine::VIBuffer_Trail::VIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: VIBuffer(pDevice, pContext)
{
}

Engine::VIBuffer_Trail::VIBuffer_Trail(const VIBuffer_Trail& original)
	: VIBuffer(original)
	, m_tTrailDesc(original.m_tTrailDesc)
	, m_iMaxVertices(original.m_iMaxVertices)
{
}

HRESULT Engine::VIBuffer_Trail::Initialize_Prototype(const TRAIL_DESC* pDesc)
{
	m_tTrailDesc = *pDesc;
	m_iMaxVertices = m_tTrailDesc.iMaxPoints * 2;		// 각 점마다 Root, Tip으로 2개의 정점있음
	m_iVertexStride = sizeof(VTXTRAIL);
	m_iNumVertices = 0;
	m_iNumVertexBuffers = 1;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iMaxVertices;		// 버퍼 전체의 크기
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;							// 정점의 CPU/GPU 접근방식
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	CHECK_FAILED(m_pDevice->CreateBuffer(&VBDesc, nullptr, &m_pVB), E_FAIL);

	// D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP 방식이라 인덱스 버퍼는 없음

	return S_OK;
}

HRESULT Engine::VIBuffer_Trail::Initialize(void* arg)
{
	Safe_Release(m_pVB);

	D3D11_BUFFER_DESC VBDesc{};
	VBDesc.ByteWidth = m_iVertexStride * m_iMaxVertices;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	CHECK_FAILED(m_pDevice->CreateBuffer(&VBDesc, nullptr, &m_pVB), E_FAIL);

	return S_OK;
}

HRESULT Engine::VIBuffer_Trail::Render(const _float fTimeDelta)
{
	if (m_iActiveVertices < 4)		// 최소 한 쌍의 Point가 있어야 하므로 최소 정점은 4개여야함
		return S_OK;

	m_pContext->Draw(m_iActiveVertices, 0);

	return S_OK;
}

HRESULT Engine::VIBuffer_Trail::Bind_Resource()
{
	_uint iOffset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pVB, &m_iVertexStride, &iOffset);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

void Engine::VIBuffer_Trail::Add_Point(const _float3& vRoot, const _float3 vTip)
{
	// 최소 거리 검사 -> 정지 상태일 때 점을 추가할 필요가 없다.
	if (m_deqPoints.empty() == false)
	{
		_vector vLastRoot = XMLoadFloat3(&m_deqPoints.front().vRoot);
		_vector vNewRoot = XMLoadFloat3(&vRoot);
		_float fDistance = XMVectorGetX(XMVector3Length(vLastRoot - vNewRoot));

		if (fDistance < m_tTrailDesc.fMinDistance)
			return;
	}

	TRAIL_POINT trailPoint{};
	trailPoint.vRoot = vRoot;
	trailPoint.vTip = vTip;
	trailPoint.fTimeStamp = m_fAccumulatedTime;

	m_deqPoints.push_front(trailPoint);		// 최신 점은 항상 앞에 넣기

	// 최대 개수가 넘어가면 가장 오래된 점을 제거
	while (m_deqPoints.size() > m_tTrailDesc.iMaxPoints)
		m_deqPoints.pop_back();
}

void Engine::VIBuffer_Trail::Update_Trail(const _float fTimeDelta)
{
	m_fAccumulatedTime += fTimeDelta;

	// 수명을 다한 점을 뒤에서부터 제거
	while (m_deqPoints.empty() == false)
	{
		// 제일 오래된 점의 생성시간을 통해 비교
		_float fLifeTime = m_fAccumulatedTime - m_deqPoints.back().fTimeStamp;
		if (fLifeTime > m_tTrailDesc.fLifeTime)
			m_deqPoints.pop_back();
		else
			break;
	}
	
	Make_TriangleStrip();
}

void Engine::VIBuffer_Trail::Set_TrailDesc(const TRAIL_DESC& desc)
{
	_uint iNewMaxVertices = desc.iMaxPoints * 2;
	if (iNewMaxVertices != m_iMaxVertices)
	{
		m_tTrailDesc = desc;
		m_iMaxVertices = iNewMaxVertices;
		m_deqPoints.clear();

		Safe_Release(m_pVB);

		D3D11_BUFFER_DESC VBDesc{};
		VBDesc.ByteWidth = m_iVertexStride * m_iMaxVertices;
		VBDesc.Usage = D3D11_USAGE_DYNAMIC;
		VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		m_pDevice->CreateBuffer(&VBDesc, nullptr, &m_pVB);
	}
	else
	{
		m_tTrailDesc = desc;
	}
}

void Engine::VIBuffer_Trail::Reset()
{
	m_deqPoints.clear();
	m_iActiveVertices = 0;
	m_fAccumulatedTime = 0.f;
}

void Engine::VIBuffer_Trail::Make_TriangleStrip()
{
	// 버텍스 버퍼에 정점 데이터를 기록

	// 무조건 한 쌍을 이뤄야 하므로 점이 2개보다 작으면 0으로 만들기
	_uint iNumPoints = (_uint)m_deqPoints.size();
	if (iNumPoints < 2)
	{
		m_iActiveVertices = 0;
		return;
	}

	// 활성화 정점의 개수는 현재 Point의 두 배이다.
	m_iActiveVertices = iNumPoints * 2;

	D3D11_MAPPED_SUBRESOURCE SubResource{};
	if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
		return;

	VTXTRAIL* pVtxTrail = static_cast<VTXTRAIL*>(SubResource.pData);

	// 회전 각도의 cos/sin은 모든 포인트에서 동일하므로 한번만 계산
	_float fCos = cosf(XMConvertToRadians(m_tTrailDesc.fTexRotation));
	_float fSin = sinf(XMConvertToRadians(m_tTrailDesc.fTexRotation));

	for (_uint i = 0; i < iNumPoints; i++)
	{
		_float fLifeTime = m_fAccumulatedTime - m_deqPoints[i].fTimeStamp;	// 현재 포인트가 생성된 후 얼마나 지났는지 (누적 시간 - 생성 시간)
		_float fNormalizedTime = fLifeTime / m_tTrailDesc.fLifeTime;		// 0이면 방금 생긴 점, 1이면 수명이 다 된점
		_float fAlpha = 1.f - fNormalizedTime;								// 시간이 지남에 따라 Fade

		// vTexcoord.x => 0 일 때 무기의 뿌리, 1 일 때 무기의 끝
		// vTexcoord.y => 0 일 때 최신 점, 1일 때 오래된 점
		
		// ======================== Root (무기 뿌리, U = 0) ========================
		_float fRootU = 0.f;
		_float fRootV = fNormalizedTime;

		// UV 중심을 0,0으로 이동
		_float fRootCenterX = fRootU - 0.5f;
		_float fRootCenterY = fRootV - 0.5f;

		// 회전 행렬 공식 적용하고 다시 중심 복구
		_float fRotatedRootU = fRootCenterX * fCos - fRootCenterY * fSin + 0.5f;
		_float fRotatedRootV = fRootCenterX * fSin + fRootCenterY * fCos + 0.5f;

		pVtxTrail[i * 2 + 0].vPosition = m_deqPoints[i].vRoot;						// 현재 포인트의 Root 좌표
		pVtxTrail[i * 2 + 0].vTexcoord = _float2(fRotatedRootU, fRotatedRootV);		// U = 0이므로, 텍스쳐의 왼쪽
		pVtxTrail[i * 2 + 0].fAlpha = fAlpha;

		// ======================== Tip (무기 끝, U = 1) ========================
		_float fTipU = 1.f;
		_float fTipV = fNormalizedTime;

		// UV 중심을 0,0으로 이동
		_float fTipCenterX = fTipU - 0.5f;
		_float fTipCenterY = fTipV - 0.5f;

		// 회전 행렬 공식 적용하고 다시 중심 복구
		_float fRotatedTipU = fTipCenterX * fCos - fTipCenterY * fSin + 0.5f;
		_float fRotatedTipV = fTipCenterX * fSin + fTipCenterY * fCos + 0.5f;

		pVtxTrail[i * 2 + 1].vPosition = m_deqPoints[i].vTip;				// 현재 포인트의 Tip 좌표
		pVtxTrail[i * 2 + 1].vTexcoord = _float2(fRotatedTipU, fRotatedTipV);		// U = 1이므로, 텍스쳐의 오른쪽
		pVtxTrail[i * 2 + 1].fAlpha = fAlpha;
	}

	m_pContext->Unmap(m_pVB, 0);
}

VIBuffer_Trail* Engine::VIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TRAIL_DESC* pDesc)
{
	VIBuffer_Trail* pInstance = new VIBuffer_Trail(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(pDesc), L"VIBuffer_Trail 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}

Component* Engine::VIBuffer_Trail::Clone(void* pArg)
{
	VIBuffer_Trail* pInstance = new VIBuffer_Trail(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"VIBuffer_Trail 복사 실패", L"Caution!!!", nullptr);

	return pInstance;
}

void Engine::VIBuffer_Trail::Free()
{
	__super::Free();


}