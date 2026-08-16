#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class Buffer;

class ENGINE_DLL ComputeShader final : public Component
{
private:
	explicit ComputeShader();
	explicit ComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit ComputeShader(const ComputeShader& original);
	virtual ~ComputeShader();

public:
	// ComputeShader Pass         
	HRESULT Initialize_Prototype(const _wstring& _shaderFilePath, const _string& _shaderEntry);
	HRESULT Initialize(void* arg);

private:
	HRESULT Ready_Buffers(const RESOURCEINFO& _resourceinfo);
	//        
	// (b0, s0, t0, u0    )
	HRESULT ReflectResources(ID3DBlob* _blob);

public:
#pragma region  
	[[deprecated("이 함수는 레거시 함수입니다 ByHandle이나 FullSlot 함수를 사용해주십시오.")]]
	HRESULT Bind_RawValue(const _string& _constvarname, const void* _value, _uint _length);
	HRESULT Bind_RawValue_FullSlot(_uint _slotNum, const _string& _constvarname, const void* _value, _uint _length);
	HRESULT Bind_RawValue_ByHandle(SHADERHANDLE _handle, const void* _value, _uint _length);

	[[deprecated("이 함수는 레거시 함수입니다 ByHandle이나 FullSlot 함수를 사용해주십시오.")]]
	HRESULT Bind_Matrix(const _string& _constvarname, const _float4x4& _value);
	HRESULT Bind_Matrix_FullSlot(const _uint _constbufferslot, const _string& _constvarname, const _float4x4& _value);
	HRESULT Bind_Matrix_ByHandle(SHADERHANDLE _handle, const _float4x4& _value);

	[[deprecated("이 함수는 레거시 함수입니다 ByHandle이나 FullSlot 함수를 사용해주십시오.")]]
	HRESULT Bind_Matrices(const _string& _constvarname, const _float4x4* _value, _uint _length);
	HRESULT Bind_Matrices_FullSlot(const _uint _constbufferslot, const _string& _constvarname, const _float4x4* _value, _uint _length);
	HRESULT Bind_Matrices_ByHandle(SHADERHANDLE _handle, const _float4x4* _value, _uint _length);

	HRESULT Bind_SRV(const _string& _name, ID3D11ShaderResourceView* _srv);
	HRESULT Bind_SRV_FullSlot(_uint _slot, ID3D11ShaderResourceView* _srv);
	HRESULT Bind_UAV(const _string& _name, ID3D11UnorderedAccessView* _uav);
	HRESULT Bind_UAV_FullSlot(_uint _slot, ID3D11UnorderedAccessView* _uav);

	HRESULT Bind_EntireBuffer_BySlot(_uint _slot, const void* _data, _uint _length);

#pragma endregion  


#pragma region  
	HRESULT Dispatch(_uint _groupX, _uint _groupY, _uint _groupZ);
	HRESULT None_UnbiendDispatch(_uint _groupX, _uint _groupY, _uint _groupZ); //반복문 이전값 유지 필요할 때
	_uint3 Get_ThreadGroupSize() const { return m_vThreadGroupSize; }

#pragma endregion  

	// CS 로컬 핸들 조회
	SHADERHANDLE Get_Handle(const _string& _variableName) const;

private:
#pragma region  
	void LogError(ID3DBlob* _errorBlob);
public:
	void Unbind_Resources();
#pragma endregion  


private:
	// 쉐이더 멤버변수
	ID3D11ComputeShader* m_pCS = {};			// ComputeShader
	ID3D11ShaderReflection* m_pReflector = {};  // Reflector -> 이게 쉐이더의 코드를 읽어오는거임

	// 상수 버퍼 관리용 컨테이너 
	UMAP<_string, Buffer*> m_umapBuffers;			// 상수 버퍼 컨테이너(구조체 이름으로 저장)(Camera, Object, ...)
	Buffer* m_arrBuffersBySlot[14] = { nullptr };   // 상수 버퍼 컨테이너(레지스터 번호로 저장){b0, b1, b2 ...}

	// 리소스 정보
	vector<RESOURCEINFO> m_vecResourceInfos;
	vector<RESOURCEINFO> m_vecSRVInfos;   // t 레지스터(t0, t1, t2 ...)
	vector<RESOURCEINFO> m_vecUAVInfos;   // u 레지스터(u0, u1, u2 ...)

	// numthreads(X, Y, Z) 
	_uint3 m_vThreadGroupSize = {};

	// CS 로컬 핸들 캐시 (변수명 -> SHADERHANDLE)
	UMAP<_string, SHADERHANDLE> m_umapHandles;

public:
	// ComputeShader Pass         
	static ComputeShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& _shaderFilePath, const _string& _entryPoint);
	virtual ComputeShader* Clone(void* arg);

public:
	void Free() override final;

};

NS_END