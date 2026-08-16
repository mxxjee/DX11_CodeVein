#pragma once
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
class GameObject;
class Mouse;
NS_END

class Trigger_Controller_UI : public ImguiWindow
{
private:
    Trigger_Controller_UI(ID3D11Device* pD, ID3D11DeviceContext* pC);
    virtual ~Trigger_Controller_UI() = default;

public:
    HRESULT Initialize(void* pArg);
    virtual _uint Update_Contents(_float fDT);

private:
    void ObjCreator();
    void ObjEditor();

private:
    GameInstance* m_pGameInstance = { nullptr };
    Mouse* m_pMouse = { nullptr };

    // 생성용 기본 세팅값
    inline static _float ColScale[3] = { 1.f, 1.f, 1.f };
    inline static _float ColPos[3] = { 0.f, 0.f, 0.f };

    // 편집용 변수
    _float3 m_fPos = { 0.f, 0.f, 0.f };
    _float3 m_fScale = { 1.f, 1.f, 1.f };
    _float3 m_fBaseScale = { 0.f, 0.f, 0.f };
    _float m_fmulScale = 0.f;

public:
    static Trigger_Controller_UI* Create(ID3D11Device* pD, ID3D11DeviceContext* pC, void* pArg);
    virtual void Free();
};