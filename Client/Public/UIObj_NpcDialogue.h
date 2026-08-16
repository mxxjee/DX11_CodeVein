#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_Text;

class UIObj_NpcDialogue :
    public UIObject
{
public:
    enum class DIALOGUEUIEVENT_TYPE { UPDATE_NAME, UPDATE_DESC,EXIT,END};
    struct DialogueUIEvent
    {
        DIALOGUEUIEVENT_TYPE eType;
        wstring m_Text = L"";

    };
protected:
    explicit UIObj_NpcDialogue();
    explicit UIObj_NpcDialogue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_NpcDialogue(const UIObj_NpcDialogue& original);
    virtual ~UIObj_NpcDialogue();


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

    static UIObj_NpcDialogue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;
public:
    void Free() override;

private:
    UIObj_Text* m_pNPCName_Text = nullptr;
    UIObj_Text* m_pNPCDesc_Text = nullptr;

};
NS_END

