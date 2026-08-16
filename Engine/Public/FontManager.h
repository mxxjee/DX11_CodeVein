#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class MyFont;
class Shader;

class FontManager final : public Base
{
private:
    explicit FontManager();
    explicit FontManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~FontManager();

public:
    HRESULT Initialize();

public:
    HRESULT Load_Font(const _wstring& _fontName, const _wstring& _fontPath);
    HRESULT Add_BitmapFont(const _wstring& _BitmapFontName, const wstring& strFntPath, _string& FontTextureName);

    HRESULT    Set_PsOutLineShader();

    class BitmapFont* Get_BitmapFont(const _wstring _bitmapName);

    MyFont* Get_Font(const _wstring& _fontName);
    SpriteBatch* Get_Batch() { return m_pBatch; }

    void Draw_Text_Begin();
    void Draw_Text_Outline_Begin(ID3D11ShaderResourceView* pFont,ID3D11Texture2D* pTex2D, float fThickness,_float4 OutLineColor,_float4 Color);
    void Draw_BitMapText_Begin();
    void Draw_Text_Mask_Begin();
    void Draw_Text_Alpha_Begin();

    void Set_OutlineShader(class Shader* pShader);


    void Draw_Text(const _wstring& _fontName, const _wstring& _text, const _float2& _position, const _fvector _color, const _float _Rotation,const _float2& _size = _float2(1.f, 1.f));
    void Draw_Text_Outline4(const _wstring& _fontName, const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor = XMVectorSet(0.f, 0.f, 0.f, 1.f), const _float fRotation=0.f, const _float2& _size = _float2(1.f, 1.f));
    void Draw_Text_Outline8(const _wstring& _fontName, const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor = XMVectorSet(0.f, 0.f, 0.f, 1.f), const _float fRotation=0.f, const _float2& _size = _float2(1.f, 1.f));
    
    // 모아놨다가 한번에 출력
    void Add_Text_Stack(const TEXT_DESC& _textdesc);
    void Render_Texts_Stacked();

    void Draw_Text_End();

    void Clear_FontManager();

public:
#ifdef _DEBUG
    //모든 폰트의 이름출력(Imgui용)
    vector<string>      Get_All_FontName();
    vector<string>      Get_All_BitmapFontName();
#endif // _DEBUG

private:
    unordered_map<_wstring, class MyFont*> m_umapFonts;
    unordered_map < _wstring, class BitmapFont*> m_umapBitmapFonts;

    vector<FONT_DRAW> m_vecFontDraw;
    SpriteBatch* m_pBatch = { nullptr };

    ID3D11BlendState* m_pOldBlendState = { nullptr };
    ID3D11DepthStencilState* m_pOldDepthState = { nullptr };
    ID3D11RasterizerState* m_pOldRasterState = { nullptr };
    _float m_fOldBlendFactor[4]{};
    _uint m_iOldSampleMask = {};
    _uint m_iOldStencilRef = {};

    vector<_wstring> m_vecStoreTexts;


private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };
    class Shader* m_pOutlineShader = { nullptr };

public:
    static FontManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
    void Free() override final;
    ID3D11PixelShader* m_pOutlinePS = nullptr;
};

NS_END

/*MakeSpriteFont "경기천년제목 Bold" /FontSize:9 /FastPack /CharacterRegion:0x0020-0x00FF /CharacterRegion:0x3131-0x3163 /CharacterRegion:0xAC00-0xD800 /DefaultCharacter:0xAC00 Gyungi_B9.spritefont */

