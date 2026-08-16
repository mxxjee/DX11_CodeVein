#pragma once
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
class Mouse;

class ENGINE_DLL ImguiRTV :
    public ImguiWindow
{
protected:
    virtual HRESULT Initialize(void* pArg) override;
    ImguiRTV(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~ImguiRTV() = default;

public:
    static ImguiRTV* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
    virtual void Free();
    void Debug_RTV();

public:
    //자식이 구현할 함수, 실제 이 윈도우가 무엇을하는지 if(Imgui::CheckBox)이런걸 적어주면된다.
    virtual     _uint       Update_Contents(_float fTimeDelta); //


private:
    vector<ID3D11ShaderResourceView*> m_pSRVs;
    GameInstance* m_pGameInstance = {};
    vector<const char*> m_pRTVName;
    Mouse* m_pMouse = {};
    ImVec2 m_vSize = {};


};
NS_END
