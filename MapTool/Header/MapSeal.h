#pragma once
#include "MapObject.h"
#include "MT_Defines.h"

class CMapSeal final : public MapObject
{
private:
	explicit CMapSeal(ID3D11Device* pD, ID3D11DeviceContext* pC);
	explicit CMapSeal(const CMapSeal& original);
	virtual ~CMapSeal() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* _arg) override;
	_int Update_Priority(const _float fTimeDelta) override;
	_int Update_Parallel(const _float fTimeDelta) override;
	_int Update(const _float fTimeDelta) override;
	_int Update_Late(const _float fTimeDelta) override;
	HRESULT Render(const _float fTimeDelta) override;

	virtual ordered_json Get_ExtraData() override;

private:
	MAP_TYPE m_eMSMapType = MAP_TYPE::NONE;
	_uint m_iMSIndex = 0;

private:
	HRESULT Ready_Components();

public:
	static CMapSeal* Create(ID3D11Device* pD, ID3D11DeviceContext* pC);
	virtual GameObject* Clone(void* arg) override;
	virtual void Free() override;
};