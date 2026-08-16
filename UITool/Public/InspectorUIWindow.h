#pragma once
#include "ImguiWindow.h"

namespace Engine
{
    class GameInstance;
    class UIObject;

}

NS_BEGIN(UITool)
class InspectorUIWindow :
    public ImguiWindow
{
protected:
    InspectorUIWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~InspectorUIWindow() = default;

public:
     virtual void Free();


public:
    virtual HRESULT Initialize(void* pArg);

    static InspectorUIWindow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    virtual     _uint       Update_Contents(_float fTimeDelta);
    virtual     _int            Reset();


private:
    GameInstance*       pGameInstance = { nullptr };
    UIObject*           m_pSelectObject = { nullptr };


    vector<const char*>  m_ComponentVec;
    vector<wstring>     m_ComponentProtoVec;


    bool        m_bClicked = false;

    vector<const char*>     m_Basekey;
    vector<const char*>     m_Chars;

};

NS_END

