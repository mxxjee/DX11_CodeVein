#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class HiZBuffer final : public Base
{
private:
	explicit HiZBuffer();
	explicit HiZBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~HiZBuffer();

public:
	HRESULT Initialize(_uint _width, _uint _height);
	// G-Buffer Target_Depth SRV로부터 Hi-Z mip Chain 전체를 갱신
	HRESULT Generate_MipChain(ID3D11ShaderResourceView* _depthSRV);
	// 윈도우 리사이즈 대응용(일단 만듬)
	HRESULT Resize(_uint _width, _uint _height);

	ID3D11ShaderResourceView* Get_SRV() const { return m_pHiZSRV; }
	_uint Get_MipLevels() const { return m_iMipLevels; }
	_uint Get_Width()     const { return m_iWidth; }
	_uint Get_Height()    const { return m_iHeight; }

private:
	HRESULT Create_Resources();
	void Release_Resources();

private:
	_uint m_iWidth = {};
	_uint m_iHeight = {};
	_uint m_iMipLevels = {};

	ID3D11Texture2D* m_pHiZTexture = { nullptr };
	ID3D11ShaderResourceView* m_pHiZSRV = { nullptr };
	vector<ID3D11UnorderedAccessView*> m_vecMipUAV;
	vector<ID3D11ShaderResourceView*> m_vecMipSRV;

	class ComputeShader* m_pDownsampleCS = { nullptr };

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class GameInstance* m_pGameInstance = { nullptr };

public:
	static HiZBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint _width, _uint _height);

public:
	void Free() override final;

};

NS_END
