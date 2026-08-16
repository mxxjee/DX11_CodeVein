#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class MinimapManager;

class UIObj_Minimap :
    public UIObject
{
public:
    struct MiniampUIEvent
    {
        MINIMAP_TYPE eType;
        bool bHide;

    };
protected:
    explicit UIObj_Minimap();
    explicit UIObj_Minimap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Minimap(const UIObj_Minimap& original);
    virtual ~UIObj_Minimap();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    static UIObj_Minimap* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


public:
    void Free() override;
public:
    virtual void        After_ApplyData();
private:
    MINIMAP_TYPE            m_MinimapType;
    MinimapManager*         m_pMinimapManager = nullptr;

};
NS_END
