#include "Engine_Define.h"
#include "MyFont.h"

//#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::MyFont::MyFont()
{
}

Engine::MyFont::MyFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext)//, m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    //Safe_AddRef(m_pGameInstance);
}

Engine::MyFont::~MyFont()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::MyFont::Initialize(const _wstring& _fontPath)
{
    m_pFont = new SpriteFont(m_pDevice, _fontPath.c_str());
    m_pFont->GetSpriteSheet(&m_pFontSRV);
    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 폰트 출력 함수 ////////////////////////////////////////////////////////
void Engine::MyFont::Draw_Text(SpriteBatch* _batch, const _wstring& _text, const _float2& _position, _fvector _color, const _float fRotation,_float2 vOrigin,const _float2& _size)
{
    XMFLOAT2 vPos = { _position.x, _position.y };
    XMFLOAT2 vOri = { vOrigin.x, vOrigin.y };

    // 3. DrawString 호출 (가장 범용적인 오버로드 사용)
    m_pFont->DrawString(
        _batch,
        _text.c_str(),
        vPos,          
        _color,         
        fRotation,      
        vOri,           // XMFLOAT2 (Origin)
        _size 
    );
}

void Engine::MyFont::Draw_Text_Outline4(SpriteBatch* _batch, const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor, const _float fRotation, const _float2& _size)
{
    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x, _position.y - 1), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);
    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x, _position.y + 1), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);
    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x - 1, _position.y), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);
    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x + 1, _position.y), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);

    m_pFont->DrawString(_batch, _text.c_str(), _position, _color, 0.f, _float2(0.f, 0.f), _size);
}

void Engine::MyFont::Draw_Text_Outline8(SpriteBatch* _batch, const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor, const _float fRotation, const _float2& _size)
{
    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x - 1, _position.y - 1), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);
    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x + 1, _position.y - 1), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);
    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x, _position.y - 1), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);

    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x - 1, _position.y), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);
    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x + 1, _position.y), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);

    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x - 1, _position.y + 1), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);
    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x + 1, _position.y + 1), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);
    m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x, _position.y + 1), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);

    m_pFont->DrawString(_batch, _text.c_str(), _position, _color, 0.f, _float2(0.f, 0.f), _size);
}

void Engine::MyFont::Draw_Text_Outline_Thickness(SpriteBatch* _batch, const _float Thickness, const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor, const _float fRotation, const _float2& _size)
{
    for (float x = -Thickness; x <= Thickness; x += 1.f)
    {
        for (float y = -Thickness; y <= Thickness; y += 1.f)
        {
            if (x == 0.f && y == 0.f) continue; // 원본 자리는 비워둠
            m_pFont->DrawString(_batch, _text.c_str(), XMFLOAT2(_position.x + x, _position.y + y), _outlinecolor, 0.f, _float2(0.f, 0.f), _size);
        }
    }
    // 마지막에 원본 그리기
    m_pFont->DrawString(_batch, _text.c_str(), _position, _color, 0.f, _float2(0.f, 0.f), _size);
}


/******************************************************* 폰트 출력 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MyFont* Engine::MyFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& _fontPath)
{
    MyFont* pInstance = new MyFont(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(_fontPath), L"MyFont 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::MyFont::Free()
{
    __super::Free();

    Safe_Delete(m_pFont);
    Safe_Release(m_pFontSRV);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    //Safe_Release(m_pGameInstance);

}
/******************************************************* 객체 반환 함수 *******************************************************/

