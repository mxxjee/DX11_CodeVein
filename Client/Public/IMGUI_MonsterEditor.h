#pragma once
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
class GameObject;
NS_END


NS_BEGIN(Client)
class Monster;
class IMGUI_MonsterEditor :
    public ImguiWindow
{
private:
    explicit IMGUI_MonsterEditor();
    explicit IMGUI_MonsterEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~IMGUI_MonsterEditor();

public:
    HRESULT Initialize(void* arg);
    _uint   Update_Contents(_float fTimeDelta) override final;
    static IMGUI_MonsterEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* arg = nullptr);
public:
    void Free() override final;

private:
    Monster* m_pTargetMonster = nullptr;
    GameInstance* m_pGameInstance = { nullptr };
};
NS_END

