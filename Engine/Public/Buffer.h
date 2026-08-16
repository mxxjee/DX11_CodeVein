#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL Buffer final : public Base
{
public:
	typedef struct BufferVariable {
		_uint iOffset = {};
		_uint iSize = {};
		D3D_SHADER_VARIABLE_CLASS eClass;
	}BFVARIABLE;

private:
	explicit Buffer();
	explicit Buffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~Buffer();

public:
	HRESULT Initialize(ID3D11ShaderReflectionConstantBuffer* _pCBReflect, _uint _slotnum, D3D_SHADER_INPUT_TYPE _type = D3D_SIT_CBUFFER);
	_int Get_VariableIndex(const _string& _name);

	HRESULT Bind_RawValue(const _string& _constvarname,	const void* _data, const _uint _length);
	HRESULT Bind_RawValue_ByIndex(_int iIndex, const void* pData, _uint iLength);
	HRESULT Bind_Matrix(const _string& _constvarname, const _float4x4& _value);
	HRESULT Bind_Matrix_ByIndex(_int iIndex, const _float4x4& _value);
	HRESULT Bind_Matrices(const _string& _constvarname, const _float4x4* _value, _uint _length);
	HRESULT Bind_Matrices_ByIndex(_int iIndex, const _float4x4* _value, _uint _length);
    HRESULT Bind_EntireBuffer(const void* _data, _uint _length);
	HRESULT Commit();
	HRESULT Bind_Buffer(_uint stageMask);
	_bool Find_Variable(const _string& _varname);

	unordered_map<_string, _uint> Get_UmapVariablenames()
	{
		return m_umapVariablenames;
	}
	
private:
	ID3D11Buffer* m_pBuffer = { nullptr };				// const버퍼
	vector<BFVARIABLE> m_vecVariables;					// 버퍼가 가지고 있는 변수들
	unordered_map<_string, _uint> m_umapVariablenames;	// 버퍼가 가지고 있는 변수의 이름
	_uint		m_iSlot = {};							// 버퍼의 슬롯 번호
	_uint		m_iSize = {};							// 전체 크기
	vector<_ubyte>       m_pCPUData;					// CPU 스테이징 버퍼
	_bool        m_bDirty   = false;					// 업데이트 필요

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
	static Buffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3D11ShaderReflectionConstantBuffer* _pCBReflect, _uint _slotnum, D3D_SHADER_INPUT_TYPE _type = D3D_SIT_CBUFFER);

public:
	void Free() override final;

};

NS_END
