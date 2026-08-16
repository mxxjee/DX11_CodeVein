#pragma once
#include "Client_Define.h"
#include "Component.h"

NS_BEGIN(Client)
class ItemComponent :
    public Component
{
   /* typedef struct ItemCompDesc
    {

    };*/
protected:
	explicit ItemComponent();
	explicit ItemComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit ItemComponent(const ItemComponent& original);
	virtual ~ItemComponent();


public:
    HRESULT Initialize_Prototype(LIGHT_DESC& Desc);
    HRESULT Initialize(void* _arg);
    _int	Update_Priority(const _float fTimeDelta);
    _int Update(const _float fTimeDelta);
    _int	Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta) override;



public:
    bool            Use_Item(void* pArg);
    ItemInfo&       Get_Item();


public:
    void Free() override;

private:
    ItemInfo            m_ItemInfo;

};
NS_END

