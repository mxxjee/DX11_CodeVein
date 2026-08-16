#pragma once
#include "UIObj_SubWindow.h"


NS_BEGIN(Client)
class UIObj_SlotGrid;
class UIObj_Text;

class UIObj_Window_Bloodcode :
    public UIObj_SubWindow
{
protected:
    explicit UIObj_Window_Bloodcode();
    explicit UIObj_Window_Bloodcode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Window_Bloodcode(const UIObj_Window_Bloodcode& original);
    virtual ~UIObj_Window_Bloodcode();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);


public:
    static UIObj_Window_Bloodcode* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    UIObj_Window_Bloodcode* Clone(void* pArg) override;


public:
    void Free() override;
public:
    virtual void        After_ApplyData();




};

NS_END

