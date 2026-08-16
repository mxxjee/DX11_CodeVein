#include "Engine_Define.h"
#include "Frustum.h"
#include "GameInstance.h"

Frustum::Frustum()
	: m_pGameInstance{ GameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT Frustum::Initialize()
{
	return S_OK;
}

void Frustum::Update()
{
	/* Gribb/Hartmann 방식: View * Projection 행렬에서 직접 평면 추출 */
	_float4x4 viewMatrix = m_pGameInstance->Get_PipeLineMatrix(D3DTRANSFORM::D3DTS_VIEW);
	_float4x4 projMatrix = m_pGameInstance->Get_PipeLineMatrix(D3DTRANSFORM::D3DTS_PROJ);

	_matrix View = XMLoadFloat4x4(&viewMatrix);
	_matrix Proj = XMLoadFloat4x4(&projMatrix);
	_matrix VP = View * Proj;

	/* Transpose로 열 벡터를 행으로 변환 */
	_matrix VPT = XMMatrixTranspose(VP);

	/* 법선이 절두체 바깥을 향하도록 부호 반전 */
	_vector planes[6]{};
	planes[0] = -(VPT.r[3] - VPT.r[0]); // +x (오른쪽) : 부호 반전
	planes[1] = -(VPT.r[3] + VPT.r[0]); // -x (왼쪽) : 부호 반전
	planes[2] = -(VPT.r[3] - VPT.r[1]); // +y (위) : 부호 반전
	planes[3] = -(VPT.r[3] + VPT.r[1]); // -y (아래) : 부호 반전
	planes[4] = -(VPT.r[3] - VPT.r[2]); // +z (Far) : 부호 반전
	planes[5] = -(VPT.r[2]);            // -z (Near) : DirectX는 Z가 0~1

	/* 평면 정규화 후 저장 */
	for (size_t i = 0; i < 6; i++)
	{
		XMStoreFloat4(&m_vPlanes_InWorld[i], XMPlaneNormalize(planes[i]));
	}
}

void Frustum::Transform_ToLocalSpace(_fmatrix _worldMatrix)
{
	/* ============ 평면 변환 공식 사용 ============ */
	/* 평면 변환: P' = P * (M^-1)^T */
	_matrix WorldMatrixInverseTranspose = XMMatrixTranspose(XMMatrixInverse(nullptr, _worldMatrix));

	for (size_t i = 0; i < 6; i++)
	{
		XMStoreFloat4(&m_vPlanes_InLocal[i],
			XMPlaneNormalize(XMVector4Transform(XMLoadFloat4(&m_vPlanes_InWorld[i]), WorldMatrixInverseTranspose)));
	}
}

_bool Frustum::IsIn_InWorldSpace(_fvector vWorldPos, _float fRange) const
{
	for (size_t i = 0; i < 6; i++)
	{
		/* 법선이 바깥을 향하므로, dot > fRange면 평면 바깥 */
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlanes_InWorld[i]), vWorldPos)))
			return false;
	}

	return true;
}

_bool Frustum::IsIn_InLocalSpace(_fvector vLocalPos, _float fRange) const
{
	for (size_t i = 0; i < 6; i++)
	{
		if (fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vPlanes_InLocal[i]), vLocalPos)))
			return false;
	}

	return true;
}

_bool Engine::Frustum::IsIn_AABB_InWorldSpace(const BoundingBox& _aabb) const
{
	/* P-Vertex / N-Vertex 컬링 방식 */
	_float3 vMin{}, vMax{};

	vMin.x = _aabb.Center.x - _aabb.Extents.x;
	vMin.y = _aabb.Center.y - _aabb.Extents.y;
	vMin.z = _aabb.Center.z - _aabb.Extents.z;

	vMax.x = _aabb.Center.x + _aabb.Extents.x;
	vMax.y = _aabb.Center.y + _aabb.Extents.y;
	vMax.z = _aabb.Center.z + _aabb.Extents.z;

	for (size_t i = 0; i < 6; i++)
	{
		_vector vPlane = XMLoadFloat4(&m_vPlanes_InWorld[i]);

		/* N-Vertex: 평면 법선 반대 방향으로 가장 먼 꼭짓점 */
		_float3 vNVertex{};
		vNVertex.x = (XMVectorGetX(vPlane) >= 0.f) ? vMin.x : vMax.x;
		vNVertex.y = (XMVectorGetY(vPlane) >= 0.f) ? vMin.y : vMax.y;
		vNVertex.z = (XMVectorGetZ(vPlane) >= 0.f) ? vMin.z : vMax.z;

		/* N-Vertex가 평면 바깥(양수)이면 AABB 전체가 Frustum 바깥 */
		if (XMVectorGetX(XMPlaneDotCoord(vPlane, XMLoadFloat3(&vNVertex))) > 0.f)
			return false;
	}

	return true;
}

_bool Engine::Frustum::IsIn_AABB_InLoaclSpace(const BoundingBox& _aabb) const
{
	_float3 vMin{}, vMax{};

	vMin.x = _aabb.Center.x - _aabb.Extents.x;
	vMin.y = _aabb.Center.y - _aabb.Extents.y;
	vMin.z = _aabb.Center.z - _aabb.Extents.z;

	vMax.x = _aabb.Center.x + _aabb.Extents.x;
	vMax.y = _aabb.Center.y + _aabb.Extents.y;
	vMax.z = _aabb.Center.z + _aabb.Extents.z;

	for (size_t i = 0; i < 6; i++)
	{
		_vector vPlane = XMLoadFloat4(&m_vPlanes_InLocal[i]);

		_float3 vPVertex{};
		vPVertex.x = (XMVectorGetX(vPlane) >= 0.f) ? vMax.x : vMin.x;
		vPVertex.y = (XMVectorGetY(vPlane) >= 0.f) ? vMax.y : vMin.y;
		vPVertex.z = (XMVectorGetZ(vPlane) >= 0.f) ? vMax.z : vMin.z;

		if (XMVectorGetX(XMPlaneDotCoord(vPlane, XMLoadFloat3(&vPVertex))) > 0.f)
			return false;
	}

	return true;
}

Frustum* Frustum::Create()
{
	Frustum* pInstance = new Frustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : Frustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Frustum::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}