#pragma once
#include "UIObj_FocusMenu.h"

NS_BEGIN(Client)
class UIObj_Focus_NPCMenu :
    public UIObj_FocusMenu
{
    /*얘는 persistent고 선택지에 따라서 텍스트갱신.*/
public:
    enum class FocusNPCMenuEventType{UPDATE_TEXT,END};

    struct FocusNPCMenuUIEvent
    {
        FocusNPCMenuEventType eType;
        int m_iIdx;     //몇번째 버튼을바꿀건지
        wstring Text = L"";     //무슨텍스트로바꿀건지

    };
protected:
    explicit UIObj_Focus_NPCMenu();
    explicit UIObj_Focus_NPCMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Focus_NPCMenu(const UIObj_Focus_NPCMenu& original);
    virtual ~UIObj_Focus_NPCMenu();
public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);
public:
    void Free() override;
public:
    virtual void        After_ApplyData();
    virtual void        Set_Active(_bool _isActive);

public:
    static UIObj_Focus_NPCMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;




};
NS_END

