#pragma once
#include "UI_Render.h"

NS_BEGIN(Engine)

class NewTexture;
class VIBuffer_Rect;

class ENGINE_DLL UI_Sprite :
    public UI_Render
{
public:
    enum class SPRITETYPE { SHEET,SEQUENCE };

public:
    typedef struct tagSpriteCompDesc : public tagUICompDesc
    {
        SPRITETYPE      eType=SPRITETYPE::SHEET;
        
        _uint         Row = 1;
        _uint         Col = 1;

        _uint         iMaxFrame = 1; //첫프레임 0
        wstring         TextureComponentKey= L"Prototype_Component_UITexture_UIResource";
        bool            bLoop = true;
        float           fSpeed = 5.f;
        bool            m_bInitPlay = false;  //처음 생성하자마자 그냥 바로 update 시킬건지
    }SPRITEDESC;

protected:
    explicit UI_Sprite();
    explicit UI_Sprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UI_Sprite(const UI_Sprite& original);
    virtual ~UI_Sprite();


public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    virtual _int	Update(const _float fTimeDelta);
    virtual _int	Update_Late(const _float fTimeDelta);


public:
    virtual			HRESULT		Render_UI(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket);

public:
    static UI_Sprite* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual Component* Clone(void* arg);

public:
    void		Set_SpriteType(SPRITETYPE eType) { m_eType = eType; }

private:
    HRESULT         Bind_Value_SpriteSheet(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket);
    HRESULT         Bind_Value_SequenceImage(_uint ShaderPassNumber, UI_BUFFER_PACKET* pPacket);

    void            Update_UVValue();

public:
    void Free() override;

#ifdef _DEBUG
public:
    virtual void		Render_Imgui();

private:
    vector<const char*>		m_ComboItems; // ImGui 전용 캐시
    ImGuiTextFilter         m_HierarchyFilter;//검색창 imgui
#endif

#pragma region parsing
public:
    virtual void	Save_Data(ordered_json& pJson) override;
    virtual void	Load_Data(class GameObject* pOwner, ComponentData& Data, UITYPE* eType) override;

#pragma endregion

private:
    SPRITETYPE          m_eType;

private:
    /*Imgui에서 표시할값들(실제 세팅해야하는 값들)*/
    //개별사진인 경우 idx를 교체
    vector<string>      m_TextureKeys;

    //단일 시트인경우 uv설정
    _uint m_iRow;     //X x Y 인 스프라이트시트
    _uint m_iCol;


private:
    NewTexture* m_pTextureCom = { nullptr };
    VIBuffer_Rect* m_pVIBufferCom = { nullptr };

    wstring             m_ProtoTextureKey = L"";
    vector<_uint>       m_TextureIdx;


    _uint       m_iCurrentFrame = 0;    //현재 가리키고 있는 프레임수
    _uint       m_iMaxFrame = 0;        //전체 프레임개수, 보통 Row x col이지만 제한하기 위해 사용
    
    _float      m_fFrameTime = 0;   //fTimeDelta 누적변수값
    _float      m_fFrameSpeed = 0;  //프레임 속도값


    bool        m_bLoop = false;
private:
    /*GPU에게 전달할용 , 매프레임마다 구한다*/
    _float2     m_vUVScale=_float2(1.f,1.f);
    _float2     m_vUVOffset=_float2(0.f,0.f);
        

private:
    bool        m_bPlay = false;
    bool        m_bInitPlay = false;    //생성하자마자 시작할건지



};


NS_END

