#pragma once
#include "UIObj_CustomizeSelector.h"

/*호버시 text전달 .*/
NS_BEGIN(Client)
class CustomizingManager;

class UIObj_ColorSelector :
    public UIObj_CustomizeSelector
{
public:
    enum class EventType { CHANGE_CLOTHESIDX, END };
    struct ColorSelectorUIEvent
    {
        EventType eType;
        int iSlotIdx = 0;
        int Idx = 0;    //colorInfo 가져올 idx
    };
protected:
    explicit UIObj_ColorSelector();
    explicit UIObj_ColorSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_ColorSelector(const UIObj_ColorSelector& original);
    virtual ~UIObj_ColorSelector();

    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

public:
    void Free() override;

public:
    virtual void        After_ApplyData();

    virtual void        OnHoverEnter();
    virtual void        OnHoverExit();


public:
    static UIObj_ColorSelector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


};
NS_END

