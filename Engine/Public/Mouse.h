#pragma once

#include "Engine_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class GameInstance;
class Layer;
class Model;
NS_END

NS_BEGIN(Engine)
class ENGINE_DLL Mouse final : public Base
{
    DECLARE_SINGLETON(Mouse);
private:
    explicit Mouse();
    virtual ~Mouse();


public:
    HRESULT Initialize_Prototype();
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    _int Update_Priority(const _float fTimeDelta);
    _int Update(const _float fTimeDelta);
    _int Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta);

private:
    HRESULT Bind_ShaderResources();
    
    // 마우스 잠구기
    void Lock_Mouse();

public:
    // All 쓰지마
    void Picking_Object_All();
    _bool Picking_Object_Terrain(PICKING_DESC& _desc);
    _bool Picking_Object_GameObj(PICKING_DESC& _desc);
    _bool Picking_Object_GameObject_Pixel(class GameObject*& pGameObject);

    _bool MouseClicked()        { return m_bMouseClickedInWindow; }
    _bool MouseDragging()       { return m_bMouseDraggingInWindow; }
    POINT Get_MousePos()        { return m_MousePos; }
    POINT Get_MouseDelta()      { return m_MouseDelta; }

    void        Set_Interaction(bool b) { m_bInteraction = b; }
    bool        Get_Interaction() { return m_bInteraction; }

    
    void        Get_MouseRay(_vector& outRaypos, _vector& outRayDir);

private:
    POINT       m_MousePos = {};
    POINT       m_MouseDelta = {};
 
    _bool m_bMouseLock = { false };

    _bool m_bMouseClickedInWindow = { false };
    _bool m_bMouseDraggingInWindow = { false };

    vector<EventHandle> m_vecEvent;

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

public:
    void Free() override final;

private:
    bool        m_bInteraction = false; //Interaction여부

};

NS_END
