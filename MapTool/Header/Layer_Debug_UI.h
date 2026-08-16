#pragma once
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
class GameObject;
NS_END

class CLayer_Debug_UI : public ImguiWindow
{
private:
	CLayer_Debug_UI(ID3D11Device* pD, ID3D11DeviceContext* pC);
	virtual ~CLayer_Debug_UI() = default;

public:
	HRESULT Initialize(void* pArg);
	virtual _uint Update_Contents(_float fDT);

	_uint Get_SelectedLightNum() { return m_iSelectedLightNum; }

private:
	GameInstance* m_pGameInstance = { nullptr };
	GameObject* m_pSelectedObject = { nullptr };
	_uint m_iSelectedLightNum = 0;

	_uint m_iIndex = 0;
	std::unordered_map<std::string, size_t> m_LayerObjectCounts;
public:
	static CLayer_Debug_UI* Create(ID3D11Device* pD, ID3D11DeviceContext* pC, void* pArg);
	virtual void Free();

};

