#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)
class Mouse;
class UI_Image;
NS_END

NS_BEGIN(Client)
class UIObj_Cursor :
    public UIObject
{
public:
    struct CursorEvent
    {

        bool bEnable = true;
    };
public:
    enum class CURSORSTATE { DEFAULT, INTERACTION,END };

protected:
    explicit UIObj_Cursor();
    explicit UIObj_Cursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Cursor(const UIObj_Cursor& original);
    virtual ~UIObj_Cursor();


public:
    HRESULT Initialize_Prototype(_uint iLevel,void *pArg);


public:
    virtual HRESULT Ready_Components(void* pArg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);



public:
    static UIObj_Cursor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel,void* pArg);
private:
    Mouse*      m_pMouse = nullptr;
    UI_Image* m_pImage = nullptr;

    bool        m_bPreInteraction = false;
    bool        m_bCurInteraction = false;

    string      TexKey[_UINT(CURSORSTATE::END)];


public:
    void Free() override;


private:
    ID3D11BlendState* m_pOldBlendState = { nullptr };
    ID3D11DepthStencilState* m_pOldDepthState = { nullptr };
    ID3D11RasterizerState* m_pOldRasterState = { nullptr };
    ID3D11BlendState* m_AlphaBlendState = nullptr;

    _float m_fOldBlendFactor[4]{};
    _uint m_iOldSampleMask = {};
    _uint m_iOldStencilRef = {};

};
NS_END

