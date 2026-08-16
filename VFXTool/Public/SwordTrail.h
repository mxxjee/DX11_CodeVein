#pragma once
#include "VFXTool_Define.h"
#include "TrailEffect.h"

NS_BEGIN(Engine)
class VIBuffer_Trail;
class Shader;
class NewTexture;
NS_END

NS_BEGIN(VFXTool)

class SwordTrail final : public TrailEffect
{
private:
	explicit SwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit SwordTrail(const SwordTrail& original);
	virtual ~SwordTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int    Update_Priority(const _float fTimeDelta) override;
	virtual _int    Update(const _float fTimeDelta) override;
	virtual _int    Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

public:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

public:
	HRESULT Change_Texture(const _wstring strFilePath);								// 텍스쳐 교체 함수
	HRESULT Change_NoiseTexture(const _wstring strFilePath);						// 노이즈 텍스쳐 교체 함수
	HRESULT Change_MaskTexture(const _wstring strFilePath);							// 마스크 텍스처 교체 함수
	HRESULT Change_Shader(const _wstring strShaderTag);								// 쉐이더 교체 함수

private:
	_float m_fAccumulatedTime = {};

	// 테스트용 변수들
	_bool	m_bIsTest = { true };
	_float	m_fTestAngle = {};
	_float	m_fTestRadius = { 3.f };
	_float	m_fTestSpeed = { 5.f };

public:
	static SwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END