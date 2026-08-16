#pragma once

#include "Client_Define.h"
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
NS_END


NS_BEGIN(Client)

class IMGUI_Base abstract : public ImguiWindow
{
protected:
	explicit IMGUI_Base();
	explicit IMGUI_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~IMGUI_Base();

public:
	HRESULT Initialize(void* arg);
	_uint Update_Contents(_float fTimeDelta) override;
	_int Render(const _float fTimeDelta);

protected:
	GameInstance* m_pGameInstance = { nullptr };

public:
	static IMGUI_Base* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* arg = nullptr);

public:
	void Free() override;
};

NS_END
