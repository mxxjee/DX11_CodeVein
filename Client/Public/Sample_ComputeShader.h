#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class ComputeShader;
NS_END

NS_BEGIN(Client)

class Sample_ComputeShader final : public GameObject
{
private:
	explicit Sample_ComputeShader();
	explicit Sample_ComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Sample_ComputeShader(const Sample_ComputeShader& original);
	virtual ~Sample_ComputeShader();


public:
	HRESULT Initialize_Prototype(LEVEL _level);
	HRESULT Initialize(void* _arg);
	_int Update(const _float fTimeDelta);

private:
	HRESULT Ready_Components();
	HRESULT Ready_CShaderResources();
	HRESULT Ready_BoneCompute();

	// 1024개의 변수를 ComputeShader로 2배로 만들기
	void Test_ComputeShader_Base();
	void Test_ComputeShader_Multiple();
	void Test_ComputeShader_BoneMatrices();
	void Test_CPU_BoneMatrices();

private:
	ComputeShader* m_pCShader = { nullptr }; // 컴퓨트 쉐이더
	vector<_float> m_vecElements; // 예제용 배열
	_uint m_iElementCount = 1000000; // 배열에 채워넣을 개수(100만개)

	// Input, Output 예시용 버퍼들
	ID3D11Buffer* m_pInputBuffer = { nullptr };
	ID3D11Buffer* m_pOutputBuffer = { nullptr };
	ID3D11ShaderResourceView* m_pInputSRV = { nullptr };
	ID3D11UnorderedAccessView* m_pOutputUAV = { nullptr };
	ID3D11Buffer* m_pStagingBuffer = { nullptr };

	// 연속적인 업데이트 예시용
	ComputeShader* m_pCShader2 = { nullptr };
	vector<_float> m_vecElements2; // 예제용 배열
	ID3D11Buffer* m_pBothBuffer = { nullptr };
	ID3D11UnorderedAccessView* m_pOutputUAV2 = { nullptr };
	_float m_fTimer = {};

	// 본 행렬 업데이트 예시용
	_uint m_iNumBones = 500;
	_float4x4 m_matBones[500]{};
	ComputeShader* m_pCShader_Bone = { nullptr };
	ID3D11Buffer* m_pBoneBuffer = { nullptr };
	ID3D11UnorderedAccessView* m_pBoneUAV = { nullptr };

	// CPU 본 행렬 업데이트 예시용
	_float4x4 m_matCPUBones[500]{};

private:
	SHADERHANDLE g_vSample{};
	SHADERHANDLE g_matTest{};

public:
	static Sample_ComputeShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};

NS_END
