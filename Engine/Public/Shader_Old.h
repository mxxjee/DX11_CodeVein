#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Shader_Old final : public Component
{
private:
	explicit Shader_Old();
	explicit Shader_Old(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Shader_Old(const Shader_Old& original);
	virtual ~Shader_Old();

public:
	HRESULT Initialize_Prototype(const _wstring& Shader_OldFilePath, const D3D11_INPUT_ELEMENT_DESC* elementsDesc, _uint numElements);
	HRESULT Initialize(void* arg);

public:
	HRESULT Bind_RawValue(const _string& _constvarname,	const void* _data, const _uint _length);
	HRESULT Bind_Matrix(const _string& constantname, const _float4x4* matrix);
	HRESULT Bind_SRV(const _string& constantname, ID3D11ShaderResourceView* SRV);
	HRESULT Bind_SRVs(const _string& constantname, ID3D11ShaderResourceView** SRV, _uint numsrv);
	HRESULT Begin(_uint passIndex);

private:
	_uint m_iNumPass = {};
	ID3DX11Effect* m_pEffect = { nullptr };
	vector<ID3D11InputLayout*> m_vecInputLayouts = {};

public:
	static Shader_Old* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& Shader_OldFilePath, const D3D11_INPUT_ELEMENT_DESC* elementsDesc, _uint numElements);
	virtual Component* Clone(void* arg);

public:
	void Free() override final;

};

NS_END
