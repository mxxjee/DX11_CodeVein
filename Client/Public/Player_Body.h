#pragma once

#include "Client_Define.h"
#include "PartObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(Client)
class Player_Body final : public PartObject
{
public:
	typedef struct tagPlayerBodyDesc : public PartObject::PARTOBJECT_DESC
	{
		const _float* pDissolveTime = { nullptr };
	}BODY_DESC;

private:
	explicit Player_Body();
	explicit Player_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_Body(const Player_Body& original);
	virtual ~Player_Body();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;
	void Set_Hovered(_bool bHovered) { m_bIsHovered = bHovered; }

	PLAYER_BODY_SHADER_DESC Get_PlayerBodyShaderDesc() { return m_tPlayerBodyShader; }
	_int* Get_BodyMeshNumPtr() { return &m_iBodyMeshNum; }
	vector<vector<_float4>> Get_BodyColorSetUp() { return m_vInnerColor; }
private:
	_bool m_bIsHovered = false;
	PLAYER_BODY_SHADER_DESC m_tPlayerBodyShader;
	vector<vector<_float4>> m_vInnerColor;
	_int m_iBodyMeshNum = 0;
	_int m_iPrevMeshNum = 0;
	Texture* m_pRimNoiseTexture = { nullptr };

	const _float* m_pDissolveTime = { nullptr };
	_float m_iRimNoiseTime = 0;
	_bool* m_pFocus;

private:
	HRESULT Ready_ShaderValue();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static Player_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};
NS_END