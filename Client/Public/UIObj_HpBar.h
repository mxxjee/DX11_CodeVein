#pragma once
#include "UIObj_ProgressBar.h"

namespace Engine
{
    class UI_Image;
}

NS_BEGIN(Client)
class UIObj_BitmapFont;
class UIObj_Text;


class UIObj_HpBar :
    public UIObj_ProgressBar
{
public:
    enum EVENTTYPE{SET_OWNER,INITIALIZE,TAKE_DAMAGE,DEAD,END};

    struct HpBarEvent
    {
        EVENTTYPE eType;
        void* pArg = nullptr;
        _uint iObjectID = 0;


    };
protected:
    explicit UIObj_HpBar();
    explicit UIObj_HpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_HpBar(const UIObj_HpBar& original);
    virtual ~UIObj_HpBar();
public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);
public:
    virtual void        After_ApplyData();

    void        Set_Owner_ObjectID(_uint iObjectID) { m_pOwnerObjectID = iObjectID;}
    void        Clear_Owner_ObjectID() { m_pOwnerObjectID = UINT_MAX; }
    virtual     void Set_Active(_bool _isActive);

    void        Set_NameTex(string TexName);

public:
    void Free() override;

public:
    static UIObj_HpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
    
private:
    void        On_Initialize(void* pArg);
    void        On_SetOwner(_uint ObjID);




    void        On_Damage_Active(void* pArg);

public:
    void        On_Dead(void* pArg);


private:
    UIObj_BitmapFont* m_pDamageText = nullptr;
    UIObj_Text*         m_pNameText = nullptr;
    
private:
    Alarm           m_DamageEffectAlarm;

    _uint           m_pOwnerObjectID=UINT_MAX;
    UI_Image*       m_pNameImgComp = nullptr;

  
};
NS_END

