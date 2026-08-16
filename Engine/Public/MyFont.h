#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class MyFont final : public Base
{
private:
    explicit MyFont();
    explicit MyFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~MyFont();

public:
    HRESULT Initialize(const _wstring& _fontPath);
    void Draw_Text(SpriteBatch* _batch, const _wstring& _text, const _float2& _position, const _fvector _color, const _float fRotation, _float2 vOrigin, const _float2& _size);
    void Draw_Text_Outline4(SpriteBatch* _batch, const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor, const _float fRotation, const _float2& _size);
    void Draw_Text_Outline8(SpriteBatch* _batch, const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor, const _float fRotation, const _float2& _size);

    void Draw_Text_Outline_Thickness(SpriteBatch* _batch, const _float Thickness,const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor, const _float fRotation, const _float2& _size);

    SpriteFont* Get_SpriteFont() { return m_pFont; }

public:
    ID3D11ShaderResourceView* Get_SRV() { return m_pFontSRV; }

private:
    SpriteFont* m_pFont = { nullptr };
    
private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

public:
    static MyFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& _fontPath);
    ID3D11ShaderResourceView* m_pFontSRV = nullptr; 
public:
    void Free() override final;

};

NS_END
