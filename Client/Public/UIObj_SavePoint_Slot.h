#pragma once
#include "UIObj_GlowButton.h"

NS_BEGIN(Client)
class UIObj_SavePoint_Slot :
    public UIObj_GlowButton
{
protected:
    explicit UIObj_SavePoint_Slot();
    explicit UIObj_SavePoint_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_SavePoint_Slot(const UIObj_SavePoint_Slot& original);
    virtual ~UIObj_SavePoint_Slot();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

public:
    static UIObj_SavePoint_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
public:
    virtual void        After_ApplyData();
public:
    virtual void        Execute_By_Event(const string& strActionName, void* pArg);
    virtual void        Set_Idx(_uint i);



public:
    void Free() override;


private:
    LEVEL           m_eLevel;   //이 버트능ㄹ 누를 시 텔레포트할 레벨
    SAVE_POINT_INFO*       m_SaveInfo;

};

NS_END

