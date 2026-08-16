#pragma once
#include "Engine_Define.h"
#include "MeshEffect.h"
#include "VIBuffer_Trail.h"

NS_BEGIN(Engine)

class ENGINE_DLL TrailEffect abstract : public MeshEffect
{
public:
	typedef struct tagTrailEffectDesc : public MeshEffect::MESHEFFECT_DESC
	{
	}TRAILEFFECT_DESC;

protected:
	explicit TrailEffect();
	explicit TrailEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit TrailEffect(const TrailEffect& original);
	virtual ~TrailEffect() = default;

public:
	VIBuffer_Trail::TRAIL_DESC		Get_TrailDesc();
	virtual _bool					Get_IsTrailEffect() override { return true; }

	void			Set_TrailDesc(const VIBuffer_Trail::TRAIL_DESC& trailDesc);
	virtual void	Set_Active(_bool _isActive) override;
	virtual void	Set_WorldMatrix(const _float4x4& vWorldOffset) override {};

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _int    Update_Priority(const _float fTimeDelta) override;
	virtual _int    Update(const _float fTimeDelta) override;
	virtual _int    Update_Late(const _float fTimeDelta) override;
	virtual HRESULT Render(const _float fTimeDelta) override;

public:
	void	Add_TrailPoint(const _float3& vRoot, const _float3& vTip);
	void	Reset_Trail();

protected:
	VIBuffer_Trail*		m_pTrailBuffer = { nullptr };
	_bool				m_bIsFading = {};
	_int				m_iTrailPassIndex = {};	

public:
	virtual void Free() override;
};

NS_END