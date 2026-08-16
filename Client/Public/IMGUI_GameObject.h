#pragma once

#include "Client_Define.h"
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
class GameObject;
NS_END

NS_BEGIN(Client)

class IMGUI_GameObject final : public ImguiWindow
{
private:
	explicit IMGUI_GameObject();
	explicit IMGUI_GameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~IMGUI_GameObject();

public:
	HRESULT Initialize(void* arg);
	_uint Update_Contents(_float fTimeDelta) override final;
	_int Render(const _float fTimeDelta);

	void Render_CameraButtons();
	void Render_Transform();
	void Render_Shader();

private:
	GameObject* m_pCurrentObject = { nullptr };

	_float4 m_vCurrentScale = {};
	_float4 m_vCurrentRotation = {};
	_float4 m_vCurrentPos = {};

	_int m_iMeshNum = { -1 };

private:
	GameInstance* m_pGameInstance = { nullptr };

public:
	static IMGUI_GameObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* arg = nullptr);

public:
	void Free() override final;
};

NS_END
