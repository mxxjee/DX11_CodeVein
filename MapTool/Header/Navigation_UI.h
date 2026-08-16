#pragma once
#include "MT_Defines.h"
#include "ImguiWindow.h"

class SplineMgr;

NS_BEGIN(Engine)
class GameInstance;
NS_END

class CNavigation_UI : public ImguiWindow
{
private:
    CNavigation_UI(ID3D11Device* pD, ID3D11DeviceContext* pC);
    virtual ~CNavigation_UI() = default;

public:
    HRESULT Initialize(void* pArg);
    virtual _uint Update_Contents(_float fDT);
    void Render_Spline_3D();

private:
    GameInstance* m_pGameInstance = nullptr;

    DirectX::BasicEffect* m_pEffect = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;
    int m_iSelectedFileIndex = 0;

public:
    static CNavigation_UI* Create(ID3D11Device* pD, ID3D11DeviceContext* pC, void* pArg);
    virtual void Free();
};