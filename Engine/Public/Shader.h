#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class Buffer;

class ENGINE_DLL Shader final : public Component
{
private:
	explicit Shader();
	explicit Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Shader(const Shader& original);
	virtual ~Shader();

public:
	HRESULT Initialize_Prototype(const _wstring& _shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _elementsDesc, _uint _numElements, const SHADERENTRIES _entries);
	HRESULT Initialize(void* arg);
	HRESULT Ready_Buffers(const RESOURCEINFO& _resourceinfo);
	HRESULT Ready_Sampler();
	HRESULT ReflectResources(ID3DBlob* _blob, _uint _stageMask);
	HRESULT AssignResourcesToPass(_uint _passIndex);

public:
	[[deprecated("이 함수는 레거시 함수입니다 ByHandle이나 FullSlot 함수를 사용해주십시오.")]]
	HRESULT Bind_RawValue(const _string& _constvarname, const void* _value, _uint _length);
	HRESULT Bind_RawValue_FullName(const _string& _constbuffername, const _string& _constvarname, const void* _value, _uint _length);
	HRESULT Bind_RawValue_FullSlot(const _uint _constbufferslot, const _string& _constvarname, const void* _value, _uint _length);
	HRESULT Bind_RawValue_ByHandle(SHADERHANDLE _handle, const void* _value, _uint _length);
    HRESULT Bind_EntireBuffer_BySlot(_uint _slot, const void* _data, _uint _length);

	[[deprecated("이 함수는 레거시 함수입니다 ByHandle이나 FullSlot 함수를 사용해주십시오.")]]
	HRESULT Bind_Matrix(const _string& _constvarname, const _float4x4& _value);
	HRESULT Bind_Matrix_FullName(const _string& _constbuffername, const _string& _constvarname, const _float4x4& _value);
	HRESULT Bind_Matrix_FullSlot(const _uint _constbufferslot, const _string& _constvarname, const _float4x4& _value);
	HRESULT Bind_Matrix_ByHandle(SHADERHANDLE _handle, const _float4x4& _value);

	[[deprecated("이 함수는 레거시 함수입니다 ByHandle이나 FullSlot 함수를 사용해주십시오.")]]
	HRESULT Bind_Matrices(const _string& _constvarname, const _float4x4* _value, _uint _length);
	HRESULT Bind_Matrices_FullSlot(_uint _constBufferSlot, const _string& _constvarname, const _float4x4* _value, _uint _length);
	HRESULT Bind_Matrices_ByHandle(SHADERHANDLE _handle, const _float4x4* _value, _uint _length);

    HRESULT Bind_SRV(const _string& _constbuffername, ID3D11ShaderResourceView* _srv);
    HRESULT Bind_SRV_FullSlot(const _uint _slot, ID3D11ShaderResourceView* _srv, _uint _stageMask = stage_VS | stage_PS);
    HRESULT Bind_SRVs(const _string& _constbuffername, ID3D11ShaderResourceView** _srvs, _uint _count);
    HRESULT Bind_SRVs_FullSlot(const _uint _slot, ID3D11ShaderResourceView** _srvs, _uint _count, _uint _stageMask = stage_VS | stage_PS);

	HRESULT Bind_Resources(_uint _passIndex);
	// GPU에 값 던지기
	HRESULT Commit(_uint _passIndex);
	HRESULT Begin(_uint _passIndex, _bool _Shadow = false);
	HRESULT Update_ContantBuffer(ID3D11Buffer* _buffer, const void* _data, _uint _byteSize);


	//패스가져오기
	const PASS& Get_Pass(_uint iPassIndex);
	vector<Pass> Get_VecPass()
	{
		return m_vecPass;
	}
	unordered_map<_string, Buffer*> Get_umapBuffer()
	{
		return m_umapBuffers;
	}
	const SHADERENTRIES& Get_ShaderEntries()
	{
		return m_eShaderEntries;
	}
	_int Get_NumPass()
	{
		return m_iNumPass;
	}
	SHADERHANDLE Get_Handle(const _string& _name);


private:
	void LogError(ID3DBlob* _errorBlob);
	_wstring AnsiToWide(const char* str);

private:
	vector<Pass> m_vecPass; // 쉐이더가 가지고 있는 Pass들
	_uint m_iNumPass = {};	// Pass의 개수

	SHADERENTRIES m_eShaderEntries;	// 쉐이더의 진입 함수 이름(VS_MAIN, PS_MAIN, PS_MAIN2)등등

	unordered_map<_string, Buffer*> m_umapBuffers; // 쉐이더가 보관하는 변수 Buffer를 string으로 저장
	Buffer* m_arrBuffersBySlot[14] = { nullptr }; // 쉐이더가 보관하는 변수 Buffer를 slot(int)로 저장

	class SamplerManager* m_pSamplerManager = { nullptr };
	
	vector<RESOURCEINFO> m_vecResourceInfos; /* 쉐이더가 가지고 있는 모든 리소스들의 정보 */

	vector<ID3D11ShaderReflection*> m_vecReflectors; // 쉐이더 생성시 리소스 정보 받아오기용

public:
	static Shader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* elementsDesc, _uint numElements
		, const SHADERENTRIES _entries);
	virtual Component* Clone(void* arg);

public:
	void Free() override final;
};

NS_END
