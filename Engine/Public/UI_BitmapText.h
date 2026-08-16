#pragma once
#include "UI_Render.h"

NS_BEGIN(Engine)
class BitmapFont;
class Shader;

class ENGINE_DLL UI_BitmapText :
    public UI_Render
{
public:
    typedef struct tagBitmapTextDesc :public tagUICompDesc
    {
        wstring         ProtoBitmapFontName = L"";
        wstring         InitText = L"123456";

        _float4         vDefaultFontColor = _float4(1.f, 1.f, 1.f, 1.f);//기본색:검은색
        _float         fScale = 0.01f;   //폰트크기 * 배율
        _float          fRotation = 0.f;        //부모 회전값에 더할 오프셋
        _float2         vPosition = _float2(0.f, 0.f);//부모 위치에 더할 오프셋
     
        bool            bTypeEffect = false;        //한글자씩 출력하는 효과있을거냐
    }BITMAP_TEXTDESC;


protected:
    explicit UI_BitmapText();
    explicit UI_BitmapText(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UI_BitmapText(const UI_BitmapText& original);
    virtual ~UI_BitmapText();

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;

#ifdef _DEBUG
public:
    virtual void		Render_Imgui();
    void                Render_TextList();
    int                Get_CurrentIdx(const vector<string>& AllFonts);
private:
    int                 m_CurrentIdx = 0;
#endif
public:
    virtual			HRESULT		Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket);
public:
    void Set_Text(const wstring& strText) { m_wstrText = strText; }
    void Set_Color(_float4 vColor) { m_vColor = vColor; }
    _float Get_TextWidth(_float fScale);


#pragma region parsing
public:
    virtual void	Save_Data(ordered_json& pJson) override;
    virtual void	Load_Data(class GameObject* pOwner, ComponentData& Data, UITYPE* eType) override;

#pragma endregion



private:
    wstring         m_ProtoBitmapFontName = L"";

    class BitmapFont* m_pBitmapFont = nullptr; // 참조할 리소스 객체
    wstring            m_wstrText = L"123456";
    _float4            m_vColor = _float4(1.f, 1.f, 1.f, 1.f);

    _float             m_fScaleOffset = 1.f;
    _float             m_fRotationOffset = 0.f;
    _float2            m_vPositionOffset = _float2(0.f, 0.f);
    _float             m_fMargin = 0.f;
public:
    static UI_BitmapText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual Component* Clone(void* arg) override;
    virtual void Free() override;


    bool            m_bUseCenter = false;
private:
    SpriteBatch* m_pBatch = nullptr;
    bool        m_bUseOutline = false;
    _float4     m_vOutlineColor = _float4(1.f, 1.f, 1.f, 1.f);
 
};

NS_END

