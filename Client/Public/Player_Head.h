#pragma once

#include "Client_Define.h"
#include "PartObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
class NewTexture;
NS_END

NS_BEGIN(Client)
class Player_Head final : public PartObject
{
public:
	typedef struct tagPlayerHeadDesc : public PartObject::PARTOBJECT_DESC
	{
		const _float* pDissolveTime = { nullptr };
	}HEAD_DESC;

private:
	explicit Player_Head();
	explicit Player_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_Head(const Player_Head& original);
	virtual ~Player_Head();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

	PLAYER_EYE_SHADER_DESC Get_PlayerEyeShaderDesc() { return m_tPlayerEyeShaderDesc; }
	PLAYER_BROW_SHADER_DESC Get_PlayerBrowShaderDesc() { return m_tPlayerBrowShaderDesc; }

	PLAYER_EYE_SHADER_DESC* Get_PlayerEyeShaderDescPtr() { return &m_tPlayerEyeShaderDesc; }
	PLAYER_BROW_SHADER_DESC* Get_PlayerBrowShaderDescPtr() { return &m_tPlayerBrowShaderDesc; }

	HEAD_TEXTURE_CHANGE* Get_HeadTexturePtr() { return &m_tHeadTextureChange; }

private:
	NewTexture* m_pBrowTextureCom = { nullptr };
	NewTexture* m_pEyelashTexture = { nullptr };
	NewTexture* m_pEyeDetailTexture = { nullptr };
	NewTexture* m_pEyeWhiteTexture = { nullptr };
	NewTexture* m_pEyeHighlightTexture = { nullptr };
	NewTexture* m_pTattoTexture = { nullptr };
	PLAYER_EYE_SHADER_DESC	m_tPlayerEyeShaderDesc = {}; //´«
	PLAYER_BROW_SHADER_DESC	m_tPlayerBrowShaderDesc = {}; //´«½ç
	HEAD_TEXTURE_CHANGE m_tHeadTextureChange;

	const _float* m_pDissolveTime = { nullptr };

private:
	HRESULT Ready_EyeShaderValue();
	HRESULT Ready_BrowShaderValue();

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static Player_Head* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;

};
NS_END