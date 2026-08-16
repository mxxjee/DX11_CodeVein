#pragma once
#include "UI_Render.h"

NS_BEGIN(Engine)
class MyFont;

class ENGINE_DLL UI_Text :
    public UI_Render
{ 
public:
    typedef struct tagTextDesc:public tagUICompDesc
    {
        wstring         ProtoFontName = L"";
        _float4         vDefaultFontColor = _float4(0.f, 0.f, 0.f, 1.f);//기본색:검은색
        _float2         fScale = _float2(0.01f,0.01f);   //폰트크기 * 배율
        _float          fRotation = 0.f;        //부모 회전값에 더할 오프셋
        _float2         vPosition = _float2(0.f, 0.f);//부모 위치에 더할 오프셋
        wstring         InitText = L"Text";
        bool            bTypeEffect = false;        //한글자씩 출력하는 효과있을거냐
    }TEXTDESC;

protected:
    explicit UI_Text();
    explicit UI_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UI_Text(const UI_Text& original);
    virtual ~UI_Text();

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;


public:
    virtual			HRESULT		Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket);

public:
    static UI_Text* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual Component* Clone(void* arg);

public:
    void Free() override;



#ifdef _DEBUG
public:
    virtual void		Render_Imgui();
    void                Render_TextList();
    int                Get_CurrentIdx(const vector<string>&		AllFonts);

private:
    int                 m_CurrentIdx = 0;
#endif

public:
    void        Set_Text(wstring wstr);
    void        Set_ParentMask();

    void        Set_Spacing(_float f) { m_fSpacing = f; }
    void        Set_VIBuffer();
    void        Set_Color(_float4 vColor) { m_TextColor = vColor; }

    bool         Get_IsWorldFont() { return m_bWorldFont; }

    _float4         Get_OriginColor() { return m_vOriginColor; }
#pragma region parsing
public:
    virtual void	Save_Data(ordered_json& pJson) override;
    virtual void	Load_Data(class GameObject* pOwner,ComponentData& Data, UITYPE* eType) override;

#pragma endregion

private:
    _float4         m_vOriginColor;
    wstring         m_ProtoFontName = L"";
    MyFont*         m_pFont = nullptr;

    wstring         m_Text=L"";                                     //출력할 텍스트
    _float4         m_TextColor= _float4(0.f, 0.f, 0.f, 1.f);    //텍스트의 컬러
  
    bool            m_bTypeEffect=false;  //한글자씩 나오는 타이핑이펙트 사용할건지
    bool            m_bPlay = false;        //이펙트 플레이여부

    _float2          m_fOffSetScale = _float2(1.f,1.f);           //폰트크기 * 배율
    _float          m_fOffSetRotation = 0.f;        //부모 회전값에 더할 오프셋
    _float2         m_vOffSetPos = _float2(0.f, 0.f);//부모 위치에 더할 오프셋


    bool            m_bUseOutLine = false;
    _float4         m_TextOutLineColor = _float4(1.f, 1.f, 1.f, 1.f);
    _float          m_fThickness = 3.f;


    bool            m_bUseCenter = false;
private:
    SpriteBatch*        m_pBatch = nullptr;
    ID3D11Texture2D* m_pTexture = nullptr;

    bool            m_bUseMask = false;
    bool            m_bUseAlphaBlend = false;

    bool            m_bUseSpacing = false;      //간격모드
    _float          m_fSpacing = 0.f;


    bool            m_bWorldFont = false;

private:
    /*world ui관련*/
    // Texture Baking 관련
    ID3D11Texture2D* m_pBakeTexture = nullptr;
    ID3D11RenderTargetView* m_pBakeRTV = nullptr;
    ID3D11ShaderResourceView* m_pBakeSRV = nullptr;
    class VIBuffer_Rect* m_pVIBufferCom = { nullptr };

    wstring                   m_strPreText = L"";
    _float4                   m_vPreColor = { 0.f, 0.f, 0.f, 0.f };

public:
    HRESULT Bake_Text(bool bForce=false); // 텍스트를 텍스처로 굽는 함수
private:
    ID3D11RasterizerState* m_pScissorRS = nullptr;
    ID3D11RasterizerState* m_pDefaultRS = nullptr;
    ID3D11BlendState* m_pAlphaBS = nullptr;

   

};


NS_END
