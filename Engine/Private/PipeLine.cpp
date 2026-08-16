#include "Engine_Define.h"
#include "PipeLine.h"

#include "Shader.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::PipeLine::PipeLine()
{
}

Engine::PipeLine::PipeLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	//처음한번 초기화
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		_matrix prematrix = XMMatrixIdentity();
		DirectX::XMStoreFloat4x4(&m_prevmatMatrices[i], prematrix);
	}
}

Engine::PipeLine::~PipeLine()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::PipeLine::Update(const _float fTimeDelta)
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		XMStoreFloat4x4(&m_matInverseMatrices[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matMatrices[i])));
	}

	memcpy(&m_vCameraPosition, &m_matInverseMatrices[D3DTS_VIEW].m[3], sizeof(_float4));

	m_tCamPipeline.g_ViewMatrix = m_matMatrices[D3DTS_VIEW];
	m_tCamPipeline.g_ProjMatrix = m_matMatrices[D3DTS_PROJ];
	m_tCamPipeline.g_InverseViewMatrix = m_matInverseMatrices[D3DTS_VIEW];
	m_tCamPipeline.g_InverseProjMatrix = m_matInverseMatrices[D3DTS_PROJ];
	m_tCamPipeline.g_vCamPosition = m_vCameraPosition;

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 바인딩 함수 ////////////////////////////////////////////////////////
void Engine::PipeLine::Set_Transform(D3DTRANSFORM eD3DTransform, _matrix matrix)
{
	XMStoreFloat4x4(&m_matMatrices[eD3DTransform], matrix);
}

HRESULT Engine::PipeLine::Bind_PipeLineMatrix(Shader* shader, const _string& constant, D3DTRANSFORM transform)
{
	return shader->Bind_Matrix_FullSlot(0, constant, m_matMatrices[_UINT(transform)]);
}

HRESULT Engine::PipeLine::Bind_PipeLineMatrix_View(Shader* _shader)
{
	return _shader->Bind_Matrix_ByHandle(g_ViewMatrix, m_matMatrices[D3DTRANSFORM::D3DTS_VIEW]);
}

HRESULT Engine::PipeLine::Bind_PipeLineMatrix_Proj(Shader* _shader)
{
	return _shader->Bind_Matrix_ByHandle(g_ProjMatrix, m_matMatrices[D3DTRANSFORM::D3DTS_PROJ]);
}

HRESULT Engine::PipeLine::Bind_PipeLineInverseMatrix(Shader* shader, const _string& constant, D3DTRANSFORM transform)
{
	return shader->Bind_Matrix_FullSlot(0, constant, m_matInverseMatrices[_UINT(transform)]);
}

HRESULT Engine::PipeLine::Bind_PipeLineInverseMatrix_View(Shader* _shader)
{
	return _shader->Bind_Matrix_ByHandle(g_InverseViewMatrix, m_matInverseMatrices[D3DTRANSFORM::D3DTS_VIEW]);
}

HRESULT Engine::PipeLine::Bind_PipeLineInverseMatrix_Proj(Shader* _shader)
{
	return _shader->Bind_Matrix_ByHandle(g_InverseProjMatrix, m_matInverseMatrices[D3DTRANSFORM::D3DTS_PROJ]);
}

HRESULT Engine::PipeLine::Bind_CameraPosition(Shader* shader)
{
    //shader->Bind_RawValue_FullSlot(0, "g_NearPlane", &m_fNear, sizeof(_float));
    //shader->Bind_RawValue_FullSlot(0, "g_FarPlane", &m_fFar, sizeof(_float));
	//return shader->Bind_RawValue_FullSlot(0, constant, &m_vCameraPosition, sizeof(_float4));
	return shader->Bind_RawValue_ByHandle(g_vCamPosition, &m_vCameraPosition, sizeof(_float4));
}

HRESULT Engine::PipeLine::Bind_PipeLine_All(Shader* _shader)
{
	return _shader->Bind_EntireBuffer_BySlot(BUFFER_CAMERA, &m_tCamPipeline, sizeof(Camera_Buffer));
}
/******************************************************* 쉐이더 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// 카메라 Near, Far ////////////////////////////////////////////////////////
void Engine::PipeLine::Set_CamNearFar(_float _near, _float _far)
{
    if (_near != 0) m_fNear = _near;
    if (_far != 0) m_fFar = _far;
}

/******************************************************* 카메라 Near, Far *******************************************************/



//////////////////////////////////////////////////////// 카메라 이전 View, Porj저장 ////////////////////////////////////////////////////////
void Engine::PipeLine::Update_PrevMatrices()
{
	for (size_t i = 0; i < D3DTS_END; i++)
	{
		m_prevmatMatrices[i] = m_matMatrices[i];
	}
}
/******************************************************* 카메라 이전 View, Proj 저장 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
PipeLine* Engine::PipeLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new PipeLine(pDevice, pContext);
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::PipeLine::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
/******************************************************* 객체 반환 함수 *******************************************************/

