#pragma once
#include "UIObj_CustomizeSelector.h"
#include "UI_SlotGrid.h"

NS_BEGIN(Client)
class CustomizingManager;


class UIObj_MeshSelector :
    public UIObj_CustomizeSelector
{
protected:
    explicit UIObj_MeshSelector();
    explicit UIObj_MeshSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_MeshSelector(const UIObj_MeshSelector& original);
    virtual ~UIObj_MeshSelector();

    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

private:
    void    Set_GridDesc(); //uidatastr에 따라서 desc정의

public:
    void Free() override;

public:
    virtual void        After_ApplyData();
public:
    wstring     Get_NewTexKey() { return m_ProtoTexKey; }
    GRID_DESC* Get_Desc() { return &m_tGridDesc; }
    _uint      Get_TotalTex() { return m_iTotalTex; }

public:
    static UIObj_MeshSelector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

    wstring m_ProtoTexKey = L"";
    
private:
    GRID_DESC        m_tGridDesc;
    _uint            m_iTotalTex = 20;

};
NS_END


