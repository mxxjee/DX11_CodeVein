#pragma once
#include "MapObject.h"
#include "MT_Defines.h"

class CTriggerBox final : public MapObject
{
public:
	enum TRIGGER_TYPE { TRIGGER_NONE, TRIGGER_SCENE_CHANGE, TRIGGER_SPAWN, TRIGGER_SOUND, TRIGGER_END };

private:
	explicit CTriggerBox(ID3D11Device* pD, ID3D11DeviceContext* pC);
	explicit CTriggerBox(const CTriggerBox& original);
	virtual ~CTriggerBox() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* _arg) override;
	_int Update_Priority(const _float fTimeDelta) override;
	_int Update_Parallel(const _float fTimeDelta) override;
	_int Update(const _float fTimeDelta) override;
	_int Update_Late(const _float fTimeDelta) override;
	HRESULT Render(const _float fTimeDelta) override;

	void Setup_Trigger_Data(TRIGGER_TYPE eType, const _wstring& wstrOption) {
		m_eTriggerType = eType;
		m_strTriggerOption = wstrOption;
	}

private:
	TRIGGER_TYPE m_eTriggerType = TRIGGER_NONE;
	_wstring     m_strTriggerOption = L"";
	_bool        m_bIsTriggered = false;

private:
	HRESULT Ready_Components();

public:
	static CTriggerBox* Create(ID3D11Device* pD, ID3D11DeviceContext* pC);
	virtual GameObject* Clone(void* arg) override;
	virtual void Free() override;
};