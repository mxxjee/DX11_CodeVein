#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer_Terrain_Flat final : public VIBuffer
{
private:
	explicit VIBuffer_Terrain_Flat();
	explicit VIBuffer_Terrain_Flat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit VIBuffer_Terrain_Flat(const VIBuffer_Terrain_Flat& original);
	virtual ~VIBuffer_Terrain_Flat();

public:
	HRESULT Initialize_Prototype(const _wstring& heightfilepath);
	HRESULT Initialize(void* arg) override;

private:
	_uint m_iNumVerticesX = {};
	_uint m_iNumVerticesZ = {};

public:
	static VIBuffer_Terrain_Flat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& heightfilepath);
	virtual Component* Clone(void* arg) override;

public:
	void Free() override final;

};

NS_END
