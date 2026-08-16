#include "Engine_Define.h"
#include "UI_BitmapText.h"
#include "UIObject.h"
#include "BitmapFont.h"
#include "GameInstance.h"
#include "Sampler.h"


Engine::UI_BitmapText::UI_BitmapText()
{
}

Engine::UI_BitmapText::UI_BitmapText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UI_Render(pDevice, pContext)
{
    m_iSavePriority = 0;
}

Engine::UI_BitmapText::UI_BitmapText(const UI_BitmapText& original)
    :UI_Render(original), m_pBatch(original.m_pBatch)
{

}

Engine::UI_BitmapText::~UI_BitmapText()
{
}

HRESULT Engine::UI_BitmapText::Initialize_Prototype()
{
    m_pBatch = m_pGameInstance->Get_Batch();


    return S_OK;
}

HRESULT Engine::UI_BitmapText::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    if (arg != nullptr)
    {
        BITMAP_TEXTDESC* pDesc = (BITMAP_TEXTDESC*)arg;


        // 1. 매니저에서 비트맵 폰트 리소스 가져오기
        m_ProtoBitmapFontName = pDesc->ProtoBitmapFontName;

        m_pBitmapFont = m_pGameInstance->Get_BitmapFont(pDesc->ProtoBitmapFontName);
        CHECK_NULLPTR(m_pBitmapFont);
        Safe_AddRef(m_pBitmapFont);

        m_wstrText = pDesc->InitText;
        m_vColor = pDesc->vDefaultFontColor;
        m_fScaleOffset = pDesc->fScale;
        m_fRotationOffset = pDesc->fRotation;
        m_vPositionOffset = pDesc->vPosition;

    }
   


    return S_OK;
}

HRESULT		Engine::UI_BitmapText::Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket)
{
  
    CHECK_NULLPTR(m_pBitmapFont);
    __super::Render_UI(ShaderPassNumber, pPacket);

    // 부모의 최종 변환 정보 가져오기
    const UITransform Transform = m_pOwner->Get_CombinedTransform();

    // 최종 출력 위치 및 배율 계산
    _float2 vBasePos = _float2(Transform.m_fX, Transform.m_fY) + m_vPositionOffset;
    _float  fFinalScale = Transform.m_fSizeX * m_fScaleOffset; // 가로 크기를 기준으로 배율 결정
    _float  fRotation = Transform.m_fRotationZ + m_fRotationOffset;

    _float fTotalWidth = Get_TextWidth(fFinalScale);
    _float curX = vBasePos.x - (fTotalWidth * 0.5f);

    m_pGameInstance->Draw_BitMapText_Begin(); // SpriteBatch->Begin()


    for (wchar_t ch : m_wstrText)
    {
        const BM_CHARACTER* pDesc = m_pBitmapFont->Get_Character((int)ch);
        if (!pDesc) continue;

        // 이미지 시트에서 자를 영역
        RECT rcSrc = { pDesc->x, pDesc->y, pDesc->x + pDesc->width, pDesc->y + pDesc->height };
        _float2 LT = { 0.f,0.f };

        //출력 위치 보정
        _float2 vDrawPos = {
            curX + (pDesc->xoffset * fFinalScale),
            vBasePos.y + (pDesc->yoffset * fFinalScale)
        };

        m_vColor.w = Transform.m_fAlpha;//알파값은 부모의 alpha따라가기

        m_pBatch->Draw(
            m_pBitmapFont->Get_SRV(),
            XMLoadFloat2(&vDrawPos),
            &rcSrc,
            XMLoadFloat4(&m_vColor),
            fRotation,
            XMLoadFloat2(&LT),
            fFinalScale
        );


       
        
      
        curX += (pDesc->xadvance * fFinalScale) + m_fMargin;
    }

    m_pGameInstance->Draw_Text_End(); // SpriteBatch->End()

    return S_OK;
}

_float Engine::UI_BitmapText::Get_TextWidth(_float fScale)
{
    if (!m_pBitmapFont || m_wstrText.empty()) return 0.f;

    _float fTotalWidth = 0.f;
    for (size_t i = 0; i < m_wstrText.length(); ++i)
    {
        const BM_CHARACTER* pDesc = m_pBitmapFont->Get_Character((int)m_wstrText[i]);
        if (!pDesc) continue;

        if (i < m_wstrText.length() - 1)
            fTotalWidth += (pDesc->xadvance * fScale) + m_fMargin;
        else
            fTotalWidth += (pDesc->width * fScale);
    }

    return fTotalWidth;
}

#ifdef _DEBUG
void	Engine::UI_BitmapText::Render_Imgui()
{

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("BitmapTextComponent");
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    __super::Render_Imgui();

    //값조ㅓ절 UI
    //색상
    ImGui::ColorEdit4("TextColor", (_float*)&m_vColor);
 

    //오프셋값들
    ImGui::DragFloat("OffSet_Scale", (_float*)&m_fScaleOffset);
    ImGui::DragFloat("OffSet_RotationZ", (_float*)&m_fRotationOffset);
    ImGui::DragFloat2("OffSet_Pos", (_float*)&m_vPositionOffset.x);
    ImGui::DragFloat("Margin", (_float*)&m_fMargin);

    ImGui::Checkbox("Edit Outline",&m_bUseOutline);
    if (m_bUseOutline)
    {
        if (ImGui::ColorEdit4("OutlineColor", (_float*)&m_vOutlineColor))
        {

        }
    }

    char Buff[128];

    strcpy_s(Buff, sizeof(Buff), wstringToString(m_wstrText).c_str());
    if (ImGui::InputText("Text:", Buff, sizeof(Buff)))
    {
        m_wstrText = stringToWstring(Buff);
    }

    Render_TextList();
}


void Engine::UI_BitmapText::Render_TextList()
{
    vector<string>		AllFonts = m_pGameInstance->Get_All_BitmapFontName();
    vector<const char*>		AllFontsStr;

    for (auto& Name : AllFonts)
        AllFontsStr.push_back(Name.c_str());

    m_CurrentIdx = Get_CurrentIdx(AllFonts);


    if (ImGui::Combo("Text Type", &m_CurrentIdx, AllFontsStr.data(), (int)AllFontsStr.size()))
    {
        wstring SelectTextName = stringToWstring(AllFontsStr[m_CurrentIdx]);
        BitmapFont* pNewFont = m_pGameInstance->Get_BitmapFont(SelectTextName);
        if (pNewFont)
        {
            //원래있던것과 교체
            Safe_Release(m_pBitmapFont);
            m_pBitmapFont = pNewFont;
            if (m_pBitmapFont)
            {
                Safe_AddRef(m_pBitmapFont);
 
            }

            //원본이름갱신
            m_ProtoBitmapFontName = stringToWstring(AllFontsStr[m_CurrentIdx]);

        }
    }

}

int Engine::UI_BitmapText::Get_CurrentIdx(const vector<string>& AllFonts)
{
    //currentIdx구하기
    int iIdx = 0;

    string CurrentFontName = wstringToString(m_ProtoBitmapFontName);
    for (auto& pFont : AllFonts)
    {
        if (pFont == CurrentFontName)
            return iIdx;

        else
            ++iIdx;
    }

    return iIdx;
}

#endif
UI_BitmapText* Engine::UI_BitmapText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    UI_BitmapText* pInstance = new UI_BitmapText(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(), L"UI_BitmapText 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

Component* Engine::UI_BitmapText::Clone(void* arg)
{
    UI_BitmapText* pInstance = new UI_BitmapText(*this);

    MSG_FAIL(pInstance->Initialize(arg), L"UI_BitmapText 복사 실패", L"Caution!!!", nullptr);

    return pInstance;
}

void Engine::UI_BitmapText::Free()
{
   
    Safe_Release(m_pBitmapFont);

    __super::Free();
}


#pragma region parsing
void	Engine::UI_BitmapText::Save_Data(ordered_json& pJson)
{

    pJson["FontName"] = wstringToString(m_ProtoBitmapFontName);
    pJson["InnerText"] = wstringToString(m_wstrText);
    pJson["TextColor"] = { m_vColor.x,m_vColor.y,m_vColor.z,m_vColor.w };

    pJson["OffSetScale"] = m_fScaleOffset;
    pJson["OffSetRotation"] = m_fRotationOffset;
    pJson["OffSetPos"] = { m_vPositionOffset.x,m_vPositionOffset.y };


    pJson["Margin"] = m_fMargin;


}
void Engine::UI_BitmapText::Load_Data(GameObject* pOwner, ComponentData& Data, UITYPE* eType)
{
    __super::Load_Data(pOwner, Data, eType);

    CHECK_JUST_NULL(pOwner);
    ordered_json root = Data.ComJson;

    if (root.contains("FontName"))
    {

        m_ProtoBitmapFontName = stringToWstring(root["FontName"]);
        m_pBitmapFont = m_pGameInstance->Get_BitmapFont(m_ProtoBitmapFontName);
        if (m_pBitmapFont)
        {
            Safe_AddRef(m_pBitmapFont);
        }
    }




    m_wstrText = stringToWstring(root.value("InnerText", ""));


    if (root.contains("TextColor") && root["TextColor"].is_array())
    {
        auto& color = root["TextColor"];
        m_vColor.x = color[0];
        m_vColor.y = color[1];
        m_vColor.z = color[2];
        m_vColor.w = color[3];
    }

  
    m_fScaleOffset = root.value("OffSetScale", 1.f);

    m_fRotationOffset = root.value("OffSetRotation", 0.f);


    if (root.contains("OffSetPos"))
        m_vPositionOffset = _float2(root["OffSetPos"][0], root["OffSetPos"][1]);

    m_fMargin = root.value("Margin", 0.f);

 
    *eType = UITYPE::BITMAPTEXT;

}
#pragma endregion
