#pragma once
#include "ImguiWindow.h"

namespace Engine
{
    class GameInstance;
    class UIObject;

}
NS_BEGIN(UITool)
class LayerDebugWindow :
    public ImguiWindow
{
protected:
    LayerDebugWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~LayerDebugWindow() = default;

public:
    static LayerDebugWindow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    virtual void Free();
public:
    //자식이 구현할 함수, 실제 이 윈도우가 무엇을하는지 if(Imgui::CheckBox)이런걸 적어주면된다.
    virtual     _uint       Update_Contents(_float fTimeDelta) ; //

private:
    void    Show_CreateUIMenu();
    void    Show_PrefabMenu();

    HRESULT    Create_UIObject(UITYPE eUIType,_uint passNum=4);
    HRESULT    Create_PrefabType(UI_PREFAB ePrefabType);

private:
    GameInstance* pGameInstance = { nullptr };
  

};
NS_END
