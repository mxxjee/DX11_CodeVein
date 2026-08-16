#pragma once
#include "Client_Define.h"
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
NS_END

NS_BEGIN(Client)
class IMGUI_UIDebug :
    public ImguiWindow
{
private:
	explicit IMGUI_UIDebug();
	explicit IMGUI_UIDebug(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~IMGUI_UIDebug();


public:
	HRESULT Initialize(void* arg);
	_uint Update_Contents(_float fTimeDelta) override final;
	_int Render(const _float fTimeDelta);

	
public:
	static IMGUI_UIDebug* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* arg = nullptr);


private:
	GameInstance* m_pGameInstance = { nullptr };

public:
	void Free() override final;
};

NS_END
