#pragma once
#include "UIObj_SubDesc.h"

NS_BEGIN(Client)
class UIObj_SkillDesc :
    public UIObj_SubDesc
{
    enum class Values{ CATEGORY, TYPE, SPENDVALUE, TIME, ATTACK_TYPE, END };

protected:
    explicit UIObj_SkillDesc();
    explicit UIObj_SkillDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_SkillDesc(const UIObj_SubDesc& original);
    virtual ~UIObj_SkillDesc();

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
    virtual void Update_Group(ItemInfo* pInfo);


public:
    static UIObj_SkillDesc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
public:
    void Free() override;


private:
    vector<UIObj_Text*> m_Texts;
    vector<UIObj_Text*> m_InputTexts;

    //UIObj_Text* m_pText_Category = nullptr;//계통
    //UIObj_Text* m_pText_Type = nullptr;//연혈타입
    //UIObj_Text* m_pText_SpendValue = nullptr;//소비 명혈
    //UIObj_Text* m_pText_time = nullptr;//재사용까지..
    //UIObj_Text* m_pText_Attack = nullptr;//공격 속성


    //UIObj_Text* m_pInput_Text_Category = nullptr;//계통 input
    //UIObj_Text* m_pInput_Text_Input = nullptr;//연혈타입 input
    //UIObj_Text* m_pInput_Text_SpendValue = nullptr;//소비명혈 input
    //UIObj_Text* m_pInput_Text_Time = nullptr;//재사용까지..input
    //UIObj_Text* m_pInput_Text_Time = nullptr;//재사용까지..input





};
NS_END

