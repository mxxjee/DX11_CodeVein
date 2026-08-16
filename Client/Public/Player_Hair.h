// Player_Hair.h

#pragma once
#include "Client_Define.h"
#include "PartObject.h"
#include "Player.h"
NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(Client)
class Player_Hair final : public PartObject
{
public:
	typedef struct tagPlayerHairDesc : public PartObject::PARTOBJECT_DESC
	{
		const _float* pDissolveTime = { nullptr };
	}HAIR_DESC;

private:
    explicit Player_Hair();
    explicit Player_Hair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Player_Hair(const Player_Hair& original);
    virtual ~Player_Hair();
public:
    virtual HRESULT Initialize_Prototype(LEVEL _level);
    virtual HRESULT Initialize(void* arg) override;
    virtual _int    Update_Priority(const _float fTimeDelta) override;
    virtual _int    Update(const _float fTimeDelta) override;
    virtual _int    Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;
    virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;
    PLAYER_HAIR_SHADER_DESC Get_PlayerHairShaderDesc() { return m_tPlayerHairShader; }
    PLAYER_HAIR_SHADER_DESC* Get_PlayerHairShaderDescPtr() { return &m_tPlayerHairShader; }

    _int* Get_HairMeshNumPrt() { return &m_iHairMeshNum; }

    void Switch_HairMesh(_int _iNewIndex); // 헤어 모델 교체

private:
    PLAYER_HAIR_SHADER_DESC m_tPlayerHairShader = {};

private:
    HRESULT Ready_ShaderValue();
    HRESULT Ready_MasterModel();
    HRESULT Ready_Components();
    HRESULT Bind_ShaderResources();

    // PhysX 헤어 물리
    HRESULT Ready_HairBoneChain();

private:
    Model* m_pMasterModelCom = {};
    class Player* m_pPlayer = { nullptr };

public:
    static Player_Hair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override final;

private:
    Texture* m_pRimNoiseTexture = { nullptr };
    _float m_iRimNoiseTime = 0;
    _bool* m_pFocus;
	_int m_iHairMeshNum = {};
	const _float* m_pDissolveTime = { nullptr };


};
NS_END