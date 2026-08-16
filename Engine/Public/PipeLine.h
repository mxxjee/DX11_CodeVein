#pragma once

/* 고정 기능 파이프라인의 뷰스페이스 행렬, 투영 행렬을 가지고 있는 클래스 */
/* 가지고 있는 행렬을 쉐이더에 넘겨주는 역할도 수행 */

#include "Base.h"

NS_BEGIN(Engine)

class Shader;

class PipeLine final : public Base
{
private:
	explicit PipeLine();
	explicit PipeLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit PipeLine(const PipeLine& original);
	virtual ~PipeLine();

	struct Camera_Buffer {
		_float4x4 g_ViewMatrix = {};
		_float4x4 g_ProjMatrix = {};
		_float4x4 g_PrevViewMatrix = {};
		_float4x4 g_PrevProjMatrix = {};
		_float4x4 g_InverseViewMatrix = {};
		_float4x4 g_InverseProjMatrix = {};
		_float4 g_vCamPosition = {};

		//SSAO
		_float4 g_Samples[32] = {};
		_float2 g_NoiseScale = {};
		_float g_SSAORadius = {};
		_float g_SSAOBias = {};
		_float4x4 g_CamViewMatrix = {};
		_float4x4 g_CamProjMatrix = {};
	};

public:
	_int	Update(const _float fTimeDelta);
	
	void Set_Transform(D3DTRANSFORM eD3DTransform, _matrix matrix);

	HRESULT Bind_PipeLineMatrix(Shader* _shader, const _string& constant, D3DTRANSFORM transform);
	HRESULT Bind_PipeLineMatrix_View(Shader* _shader);
	HRESULT Bind_PipeLineMatrix_Proj(Shader* _shader);
	HRESULT Bind_PipeLineInverseMatrix(Shader* shader, const _string& constant, D3DTRANSFORM transform);
	HRESULT Bind_PipeLineInverseMatrix_View(Shader* _shader);
	HRESULT Bind_PipeLineInverseMatrix_Proj(Shader* _shader);
	HRESULT Bind_CameraPosition(Shader* shader);
	HRESULT Bind_PipeLine_All(Shader* _shader);
    
	//near far 새로 세팅
    void Set_CamNearFar(_float _near, _float _far);
	void Update_PrevMatrices();
    inline _float4 Get_CameraPosition() { return m_vCameraPosition; }
    inline _float4x4 Get_PipeLineMatrix(D3DTRANSFORM _transform) { return m_matMatrices[_transform]; }
	inline _float4x4 Get_PrevPipeLineMatrix(D3DTRANSFORM _transform) { return m_prevmatMatrices[_transform]; }
    inline _float4x4 Get_PipeLineInversMatrix(D3DTRANSFORM _transfrom) { return m_matInverseMatrices[_transfrom]; }


private:
	_float4x4 m_matMatrices[D3DTS_END] = {};
	_float4x4 m_prevmatMatrices[D3DTS_END] = {};
	_float4x4 m_matInverseMatrices[D3DTS_END] = {};
	_float4 m_vCameraPosition = {};
    _float m_fNear = {};
    _float m_fFar = {};


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

private:
	Camera_Buffer m_tCamPipeline;

public:
	static PipeLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	void Free() override final;

};

NS_END
