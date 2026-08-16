#pragma once
#include "UIObject.h"
namespace Engine
{
    class UI_Image;


}
NS_BEGIN(Client)
class UIObj_Text;

/*호버된 슬롯에 저장된 item정보들 중 텍스처와 아이템이름/설명을 ui에 연동*/

class UIObj_Window_ShopDesc :
    public UIObject
{
public:
    enum class ShopDescUIEventType{SET_ICONTEXTURE,SET_ITEMDESC,END};
    struct ShopDescUIEvent
    {
        ShopDescUIEventType eType;
        ItemInfo* pInfo;

    };
protected:
    explicit UIObj_Window_ShopDesc();
    explicit UIObj_Window_ShopDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Window_ShopDesc(const UIObj_Window_ShopDesc& original);
    virtual ~UIObj_Window_ShopDesc();

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

private:
    void            Change_Texture(ItemInfo*    pInfo);
    void            Change_ItemDesc(ItemInfo* pInfo);


public:
    static UIObj_Window_ShopDesc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


private:
    UI_Image* m_pSelectIconImg = nullptr;//hover한 슬롯 텍스처표시하기위한 컴포넌트 캐싱(주인 : SelectSlot_Icon)
    UIObj_Text* m_pDesc_Text_Name = nullptr;
    UIObj_Text* m_pDesc_Text_Info = nullptr;

};
NS_END

