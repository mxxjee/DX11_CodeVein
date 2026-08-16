#pragma once
#include "ImguiWindow.h"
namespace Engine
{
    class UIObject;
    class GameInstance;
}
NS_BEGIN(UITool)

class FunctionDebugWindow :
    public ImguiWindow
{
protected:
    FunctionDebugWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~FunctionDebugWindow() = default;

public:
    static FunctionDebugWindow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    virtual void Free();
public:
    //자식이 구현할 함수, 실제 이 윈도우가 무엇을하는지 if(Imgui::CheckBox)이런걸 적어주면된다.
    virtual     _uint       Update_Contents(_float fTimeDelta); //

private:
    void            Select_SavePath(string* pOut,string StartPath="..\\..\\Resources\\UI\\TestFiles");
    void            Select_LoadPath(string* pOut, string StartPath = "..\\..\\Resources\\UI\\TestFiles");
private:
    void    Show_CreateUIMenu();
    HRESULT    Create_UIObject(UITYPE eUIType);
    void        Show_All_Roots(UIObject** pOut);

private:
    GameInstance* pGameInstance = { nullptr };
    bool        m_bClientDebug = false;
    bool        m_bDrawDebug = true;

    string      CurrentLoadFile = "";
    bool        m_bIsOpenSavePrefabMenu = false;

    vector<const char*>     AllUINames;
    int iSelect = 0;
};

NS_END
