#pragma once
#include "Client_Define.h"
#include "PartObject.h"
#include "Player.h"

NS_BEGIN(Engine)
class Shader;
class Model;
NS_END

NS_BEGIN(Client)
class Player_Outer final : public PartObject
{
public:
	typedef struct tagPlayerOuterDesc : public PartObject::PARTOBJECT_DESC
	{
		const _float* pDissolveTime = { nullptr };
	}OUTER_DESC;

private:
	explicit Player_Outer();
	explicit Player_Outer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit Player_Outer(const Player_Outer& original);
	virtual ~Player_Outer();

public:
	virtual HRESULT Initialize_Prototype(LEVEL _level);
	virtual HRESULT Initialize(void* arg) override;
	virtual _int	Update_Priority(const _float fTimeDelta) override;
	virtual _int	Update(const _float fTimeDelta) override;
	virtual _int	Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;
	virtual HRESULT Render_Shadow(const _float fTimeDelta, _int iCascadeNum) override;

private:
	HRESULT Ready_Components();
	HRESULT Ready_MasterModel();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_ClothBoneChain();

	// 삭제됨 : Character::BoneChain_* 통합 함수로 대체
	// HRESULT Ready_ClothBoneRestPose();
	// void    Update_ClothBoneRestPose();
	// HRESULT Ready_PhysXCloth();
	// void    Update_PhysXCloth_Kinematic();
	// void    Fetch_PhysXCloth_Results();

private:
	// 삭제됨 : Character 공용 멤버로 이동
	// struct CLOTH_BONE_INFO    -> 전역 CHAIN_BONE_INFO
	// struct CLOTH_CHAIN_DESC   -> 전역 CHAIN_BONE_DESC
	// struct PHYSX_CHAIN_INFO   -> 전역 PHYSX_CHAIN_INFO
	// vector<CLOTH_CHAIN_DESC>  m_vecClothChains     -> Character::m_vecBoneChains
	// vector<PHYSX_CHAIN_INFO>  m_vecPhysXChains     -> Character::m_vecBoneChainPhysX
	// _bool                     m_bPhysXReady        -> Character::m_iBoneChainWaitFrames
	// vector<pair<...>>         m_vecFetchScratch    -> Character::m_vecBoneChainFetchScratch
	// _uint                     m_iPhysXWaitFrames   -> Character::m_iBoneChainWaitFrames

	_uint m_iPhysXWaitFrames = {};

	// Dissolve 관련 변수
	const _float* m_pDissolveTime = { nullptr };
	Player* m_pPlayer = {};
	Texture* m_pRimNoiseTexture = { nullptr };
	_float m_iRimNoiseTime = 0;
	_bool* m_pFocus;

public:
	static Player_Outer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level);
	GameObject* Clone(void* pArg) override;

public:
	void Free() override final;
};

NS_END