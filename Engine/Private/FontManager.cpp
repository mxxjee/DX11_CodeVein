#include "Engine_Define.h"
#include "FontManager.h"

#include "GameInstance.h"
#include "MyFont.h"
#include "BitmapFont.h"

#include "Shader.h"
#include "Sampler.h"

#include "UIObject.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::FontManager::FontManager()
{
}

Engine::FontManager::FontManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

Engine::FontManager::~FontManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::FontManager::Initialize()
{
    m_pBatch = new SpriteBatch(m_pContext);
  
    CHECK_FAILED(Set_PsOutLineShader(), E_FAIL);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 관리 함수 ////////////////////////////////////////////////////////
HRESULT Engine::FontManager::Load_Font(const _wstring& _fontName, const _wstring& _fontPath)
{
    _wstring fontname = _fontName;
    if (m_umapFonts.contains(fontname))
    {
        MSG_ON((fontname + L" 은이미 있는 폰트입니다").c_str(), L"폰트 추가 실패!");
        BREAK;
        return E_FAIL;
    }

    MyFont* font = MyFont::Create(m_pDevice, m_pContext, _fontPath);
    CHECK_NULLPTR(font);


    m_umapFonts.emplace(fontname, font);

    return S_OK;
}
HRESULT Engine::FontManager::Add_BitmapFont(const _wstring& _BitmapFontName, const wstring& strFntPath, _string& FontTextureName)
{
    _wstring fontname = _BitmapFontName;
    if (m_umapBitmapFonts.contains(fontname))
    {
        MSG_ON((fontname + L" 은이미 있는 폰트입니다").c_str(), L"폰트 추가 실패!");
        BREAK;
        return E_FAIL;
    }

    BitmapFont* font = BitmapFont::Create(m_pDevice, m_pContext, strFntPath, FontTextureName);
    CHECK_NULLPTR(font);


    m_umapBitmapFonts.emplace(fontname, font);

    return S_OK;
}

HRESULT Engine::FontManager::Set_PsOutLineShader()
{
    if (m_pOutlineShader)
    {
        MSG_ON(L"아웃라인 쉐이더가 이미 존재합니다", L"어디서 2번 불렀나본디?");
        BREAK;
        return E_FAIL;
    }

    SHADERENTRY entryOutline[1] = {
        {"", "PS_MAIN"},
    };

    SHADERENTRIES shaderEntries;
    shaderEntries.pEntries = entryOutline;
    shaderEntries.iNumpass = 1;

    m_pOutlineShader = Shader::Create(
        m_pDevice, m_pContext,
        L"../../Shader/Shader_Font.hlsl",
        nullptr, 0,  // VS 없으니까 InputLayout 불필요
        shaderEntries);

    CHECK_NULLPTR(m_pOutlineShader);

    return S_OK;
}

BitmapFont* Engine::FontManager::Get_BitmapFont(const _wstring _bitmapName)
{
    auto iter = m_umapBitmapFonts.find(_bitmapName);
    if (iter != m_umapBitmapFonts.end())
        return iter->second;

    return nullptr;
}
MyFont* Engine::FontManager::Get_Font(const _wstring& _fontName)
{
    auto iter = m_umapFonts.find(_fontName);
    if (iter != m_umapFonts.end())
        return iter->second;

    return nullptr;
}
/******************************************************* 컨테이너 관리 함수 *******************************************************/

void Engine::FontManager::Set_OutlineShader(Shader* pShader)
{ 
    m_pOutlineShader = pShader; 
    Safe_AddRef(pShader);
}

//////////////////////////////////////////////////////// 폰트 그리기 함수 ////////////////////////////////////////////////////////
void Engine::FontManager::Draw_Text_Begin()
{
    // 현재 렌더 스테이트 저장
    m_pContext->OMGetBlendState(&m_pOldBlendState, m_fOldBlendFactor, &m_iOldSampleMask);
    m_pContext->OMGetDepthStencilState(&m_pOldDepthState, &m_iOldStencilRef);
    m_pContext->RSGetState(&m_pOldRasterState);
 //   m_pGameInstance->Swap_OriginalBackbuffer();

    m_pBatch->Begin(SpriteSortMode_Deferred,
        m_pGameInstance->Get_BS(BSSET::PREMULTIPLIED_ALPHA),
        m_pGameInstance->Get_Sampler("LinearClampSampler")->GetSamplerState(), // 이 부분 추가
        nullptr,nullptr);
    
    //알파블렌딩 렌더스테이트설정


}

void Engine::FontManager::Draw_BitMapText_Begin()
{
    // 현재 렌더 스테이트 저장
    m_pContext->OMGetBlendState(&m_pOldBlendState, m_fOldBlendFactor, &m_iOldSampleMask);
    m_pContext->OMGetDepthStencilState(&m_pOldDepthState, &m_iOldStencilRef);
    m_pContext->RSGetState(&m_pOldRasterState);
    //   m_pGameInstance->Swap_OriginalBackbuffer();

    m_pBatch->Begin(SpriteSortMode_Deferred,
        m_pGameInstance->Get_BS(BSSET::BLEND),
        m_pGameInstance->Get_Sampler("LinearClampSampler")->GetSamplerState(), 
        nullptr, nullptr);

 

}

void Engine::FontManager::Draw_Text_Mask_Begin()
{    // 현재 렌더 스테이트 저장
    m_pContext->OMGetBlendState(&m_pOldBlendState, m_fOldBlendFactor, &m_iOldSampleMask);
    m_pContext->OMGetDepthStencilState(&m_pOldDepthState, &m_iOldStencilRef);
    m_pContext->RSGetState(&m_pOldRasterState);
    //   m_pGameInstance->Swap_OriginalBackbuffer();

    m_pBatch->Begin(SpriteSortMode_Immediate, // Immediate 권장
        m_pGameInstance->Get_BS(BSSET::BLEND),
        m_pGameInstance->Get_Sampler("LinearClampSampler")->GetSamplerState(), 
        nullptr, // DepthStencil
        m_pGameInstance->Get_RS(RSSET::SCISSORENABLE));


}

void Engine::FontManager::Draw_Text_Alpha_Begin()
{
    m_pContext->OMGetBlendState(&m_pOldBlendState, m_fOldBlendFactor, &m_iOldSampleMask);
    m_pContext->OMGetDepthStencilState(&m_pOldDepthState, &m_iOldStencilRef);
    m_pContext->RSGetState(&m_pOldRasterState);
    //   m_pGameInstance->Swap_OriginalBackbuffer();

    m_pBatch->Begin(SpriteSortMode_Deferred,
        m_pGameInstance->Get_BS(BSSET::BLEND),
        m_pGameInstance->Get_Sampler("LinearClampSampler")->GetSamplerState(),
        nullptr, nullptr);

}


void Engine::FontManager::Draw_Text_Outline_Begin(ID3D11ShaderResourceView* pFont,ID3D11Texture2D* pTex2D, float fThickness, _float4 OutLineColor, _float4 Color)
{
    m_pContext->OMGetBlendState(&m_pOldBlendState, m_fOldBlendFactor, &m_iOldSampleMask);
    m_pContext->OMGetDepthStencilState(&m_pOldDepthState, &m_iOldStencilRef);
    m_pContext->RSGetState(&m_pOldRasterState);
    m_pGameInstance->Swap_OriginalBackbuffer();

    // 텍스처 해상도 추출
    D3D11_TEXTURE2D_DESC texDesc;
    pTex2D->GetDesc(&texDesc);
    _float2 vSize = _float2((float)texDesc.Width, (float)texDesc.Height);

    // 핸들 기반 cbuffer 값 세팅
    m_pOutlineShader->Bind_RawValue_ByHandle(g_TextureSize, &vSize, sizeof(_float2));
    m_pOutlineShader->Bind_RawValue_ByHandle(g_OutLineThickness, &fThickness, sizeof(float));
    m_pOutlineShader->Bind_RawValue_ByHandle(g_OutLineColor, &OutLineColor, sizeof(_float4));

    // PS 가져오기
    const PASS& outlinePass = m_pOutlineShader->Get_Pass(0);

    m_pBatch->Begin(SpriteSortMode_Immediate,
        m_pGameInstance->Get_BS(BSSET::BLEND),
        nullptr, // 샘플러는 람다에서 SamplerManager로 바인딩
        nullptr, nullptr, [&]
        {
            // cbuffer 커밋
            m_pOutlineShader->Bind_Resources(0);

            // PS 바인딩
            m_pContext->PSSetShader(outlinePass.pPS, nullptr, 0);

            // SamplerManager를 통해 LinearSampler(s0) 바인딩
            m_pGameInstance->Bind_Sampler(0, stage_PS);

            // SRV 바인딩
            m_pContext->PSSetShaderResources(0, 1, &pFont);
        });
}

void Engine::FontManager::Draw_Text(const _wstring& _fontName, const _wstring& _text, const _float2& _position, _fvector _color, _float _Rotation, const _float2& _size)
{
    if (!m_umapFonts.contains(_fontName))
    {
        MSG_ON((_fontName + L" 폰트가 없습니다").c_str(), L"폰트 추가 실패!");
        BREAK;
        return;
    }

    m_umapFonts[_fontName]->Draw_Text(m_pBatch, _text, _position, _color, _Rotation, _float2(0.f,0.f),_size);
}

void Engine::FontManager::Add_Text_Stack(const TEXT_DESC& _textdesc)
{
    auto iter = m_umapFonts.find(_textdesc.wstrFontName);
    if (iter==m_umapFonts.end())
    {
        MSG_ON((_textdesc.wstrFontName + L"폰트가 없습니다.").c_str(), L"등록 실패");
        BREAK;
        return;
    }

    m_vecFontDraw.push_back(_textdesc);
}

void Engine::FontManager::Render_Texts_Stacked()
{

    for (FONT_DRAW& font : m_vecFontDraw)
    {
        switch (font.eSetting)
        {
        case TEXTDRAW_SETTING::NORMAL:
            Draw_Text(font.wstrFontName, font.wstrDrawText, font.vPosition, XMLoadFloat4(&font.vColor),0.f, font.vSize);
            break;
        case TEXTDRAW_SETTING::DIR4:
            Draw_Text_Outline4(font.wstrFontName, font.wstrDrawText, font.vPosition, XMLoadFloat4(&font.vColor), XMLoadFloat4(&font.vOutlineColor), 0.f,font.vSize);
            break;
        case TEXTDRAW_SETTING::DIR8:
            Draw_Text_Outline8(font.wstrFontName, font.wstrDrawText, font.vPosition, XMLoadFloat4(&font.vColor), XMLoadFloat4(&font.vOutlineColor), 0.f, font.vSize);
            break;
        }
    }

    Clear_FontManager();
}

void Engine::FontManager::Draw_Text_Outline4(const _wstring& _fontName, const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor, const _float fRotation,const _float2& _size)
{
    if (!m_umapFonts.contains(_fontName))
    {
        MSG_ON((_fontName + L" 폰트가 없습니다").c_str(), L"폰트 추가 실패!");
        BREAK;
        return;
    }

    m_umapFonts[_fontName]->Draw_Text_Outline4(m_pBatch, _text, _position, _color, _outlinecolor, fRotation, _size);
}

void Engine::FontManager::Draw_Text_Outline8(const _wstring& _fontName, const _wstring& _text, const _float2& _position, const _fvector _color, const _fvector _outlinecolor, const _float fRotation, const _float2& _size)
{
    if (!m_umapFonts.contains(_fontName))
    {
        MSG_ON((_fontName + L" 폰트가 없습니다").c_str(), L"폰트 추가 실패!");
        BREAK;
        return;
    }

    m_umapFonts[_fontName]->Draw_Text_Outline8(m_pBatch, _text, _position, _color, _outlinecolor, fRotation,_size);
}

void Engine::FontManager::Draw_Text_End()
{
    m_pBatch->End();

    // 이전 렌더 스테이트 복원
    m_pContext->OMSetBlendState(m_pOldBlendState, m_fOldBlendFactor, m_iOldSampleMask);
    m_pContext->OMSetDepthStencilState(m_pOldDepthState, m_iOldStencilRef);
    m_pContext->RSSetState(m_pOldRasterState);

    // Release
    Safe_Release(m_pOldBlendState);
    Safe_Release(m_pOldDepthState);
    Safe_Release(m_pOldRasterState);
}

void Engine::FontManager::Clear_FontManager()
{
    m_vecFontDraw.clear();
}

#ifdef _DEBUG
vector<string> Engine::FontManager::Get_All_FontName()
{
    vector<string> vReturn;

    for (auto& pair : m_umapFonts)
    {
        if (pair.second)
            vReturn.push_back(wstringToString(pair.first));

    }
    return vReturn;
}

vector<string> Engine::FontManager::Get_All_BitmapFontName()
{
    vector<string> vReturn;

    for (auto& pair : m_umapBitmapFonts)
    {
        if (pair.second)
            vReturn.push_back(wstringToString(pair.first));

    }
    return vReturn;
}
/******************************************************* 폰트 그리기 함수 *******************************************************/
#endif // _DEBUG


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
FontManager* Engine::FontManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    FontManager* pInstance = new FontManager(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(), L"FontManager 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::FontManager::Free()
{

    Safe_Delete(m_pBatch);
    __super::Free();

    for (auto& font : m_umapFonts)
    {
        Safe_Release(font.second);
    }
    m_umapFonts.clear();

    for (auto& font : m_umapBitmapFonts)
    {
        Safe_Release(font.second);
    }
    m_umapBitmapFonts.clear();

    Safe_Release(m_pOutlineShader);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);


 

}
/******************************************************* 객체 반환 함수 *******************************************************/

