#pragma once
#include "UIObject.h"

NS_BEGIN(Client)

class UIObj_Text;
class UIObj_BitmapFont;

class UIObj_SubDesc :
    public UIObject
{
 
protected:
    explicit UIObj_SubDesc();
    explicit UIObj_SubDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_SubDesc(const UIObj_SubDesc& original);
    virtual ~UIObj_SubDesc();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    virtual void Update_Group(ItemInfo* pInfo);
    ITEM_CATEGORY       Get_Category() { return m_eCategory; }
public:
    virtual void        After_ApplyData();

public:
    static UIObj_SubDesc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void        Set_Category(ITEM_CATEGORY eCategory) { m_eCategory = eCategory; }
public:
    void Free() override;

private:
    vector<UIObj_Text*> m_Texts;
    vector<UIObj_Text*> m_InputTexts;



    UIObject* m_Seconds = nullptr;

    vector<UIObject*> m_Etcs;
    UIObj_BitmapFont* m_Text_Total = nullptr;
    UIObj_Text* m_Text_Count = nullptr;
    UIObj_BitmapFont* m_Text_Current = nullptr;
    UIObj_BitmapFont* m_Text_Slash = nullptr;



protected:
    ITEM_CATEGORY   m_eCategory = ITEM_CATEGORY::END;



};
NS_END

