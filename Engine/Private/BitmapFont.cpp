#include "Engine_Define.h"
#include "BitmapFont.h"
#include "GameInstance.h"

inline BitmapFont::BitmapFont()
{
}
inline BitmapFont::BitmapFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :m_pDevice{pDevice},m_pContext{pContext}
{
}
inline BitmapFont::~BitmapFont()
{
}
////////////////////////////////////////////////////////////////////////////////////
inline HRESULT BitmapFont::Initialize(const _wstring& strFntPath,  _string& FontTextureName)
{
    m_pGameInstance = GameInstance::GetInstance();

    //NewTexture에서 가져오기
    Base* pBase = m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, L"Prototype_Component_Texture_BitmapFont");
    if (pBase)
    {
        m_pTextureCom = dynamic_cast<NewTexture*>(pBase);
        //Safe_AddRef(m_pTextureCom);

        m_pSRV = m_pTextureCom->Get_SRV(FontTextureName,&m_TexIdx);

    }

    //fnt파일열기
    ifstream fin(strFntPath);
    if (!fin.is_open())
        return E_FAIL;


    string line;
    while (getline(fin, line))
    {
        if (line.find("common") == 0)
        {
            m_fLineHeight = (float)Parse_Value(line, "LineHeight");
        }

        else if(line.find("char ")==0)
        {
            int id = Parse_Value(line, "id");
            BM_CHARACTER desc;
            desc.x= Parse_Value(line, "x");
            desc.y = Parse_Value(line, "y");

            desc.width = Parse_Value(line, "width");
            desc.height = Parse_Value(line, "height");

            desc.xoffset = Parse_Value(line, "xoffset");
            desc.yoffset = Parse_Value(line, "yoffset");
            desc.xadvance = Parse_Value(line, "xadvance");

            m_Characters[id] = desc;

        }
    }

    return S_OK;
}
inline const BM_CHARACTER* BitmapFont::Get_Character(int ch) const
{
    auto iter = m_Characters.find(ch);
    return (iter != m_Characters.end()) ? &iter->second : nullptr;
}

void Engine::BitmapFont::Free()
{
    Safe_Release(m_pTextureCom);
    __super::Free();
}

int Engine::BitmapFont::Parse_Value(const string& line, const string& key)
{
    size_t pos = line.find(key + "=");
    if (pos == string::npos)
        return 0;


    string sub = line.substr(pos + key.length() + 1);
    stringstream    ss(sub);
    int value;
    ss >> value;
    return value;
    return 0;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
BitmapFont* Engine::BitmapFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strFntPath, _string& FontTextureName)
{
    BitmapFont* pInstance = new BitmapFont(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(strFntPath, FontTextureName), L"BitmapFont 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/
