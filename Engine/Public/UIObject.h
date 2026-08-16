#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class Shader;
class Mouse;

class UI_Render;
class UI_Button;
class UI_Progress;

/*UI Object는 기본적으로 UI_RenderComp를 소지한다.*/


class ENGINE_DLL UIObject : public GameObject
{

public:
    enum class SELECTSTATE { NONESELECT, SELECT };

public:
    struct UIHitBox
    {
        _float L = 0.f;
        _float R = 0.f;
        _float T = 0.f;
        _float B = 0.f;


    };
    typedef struct tagUIObjectDesc : public GameObject::GAMEOBJECT_DESC
    {
        _float fX{}, fY{}, fCX{}, fCY{};
        _float fWindowX{ 0 }, fWindowY{ 0 };
        _float fAlpha = { 1.f };
        _int iZOrder = { 0 };

        string UIBaseType = "NONE";
        string UIType = "DEFAULT";
        //사용할 쉐이더프로토타입이름(기본적으로 vtxpostex)
        _uint           iShaderNumber = 4;
        UIObject* pParent = nullptr;
        bool            bInitActive = true;

        bool            bAddUIManager = true;

    }UIOBJECT_DESC;

    //typedef struct tag_UI_SubElement
    //{
    //    _wstring    wstrName;       // 표시 이름
    //    _float* pPosX;          // 위치 X 포인터
    //    _float* pPosY;          // 위치 Y 포인터
    //    _float* pSizeX;         // 크기 X 포인터
    //    _float* pSizeY;         // 크기 Y 포인터
    //    _bool       bUseMatrix = false;     // 행렬 직접 사용 여부
    //    _float4x4* pMatrix = nullptr;        // 행렬 포인터 (bUseMatrix가 true일 때)
    //}UI_SUB_ELEMENT;

protected:
    explicit UIObject();
    explicit UIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObject(const UIObject& original);
    virtual ~UIObject();

    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual HRESULT Ready_Components(void* pArg);

public:
    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);



    //이제 UI_Rendercomponent가 수행함
    HRESULT Bind_ShaderResources(UI_BUFFER_PACKET* pPacket, const _float fTimeDelta);
    // 이제 사용하지 않음
    HRESULT Bind_ShaderResources(_uint _passnum) { return S_OK; }

public:
    /// UI.전용의 렌더컴포넌트 추가함수(외부에서 호출용)
    HRESULT         Add_NewRenderComponent(_uint ProtoTypeLevel, wstring PrototypeName, wstring componentName, void* pArg);
    HRESULT         Add_NewRenderComponent(wstring componentName, UIComponent* pComponent);


    /// UI.전용의 UI컴포넌트 추가함수(외부에서 호출용)
    HRESULT         Add_NewUIComponent(UITYPE eType, _uint ProtoTypeLevel, wstring PrototypeName, wstring componentName, void* pArg);
    HRESULT         Add_NewUIComponent(UITYPE eType, wstring componentName, UIComponent* pComponent);

    //Animcomp에서 쓰는함수
    void        Set_AnimValue(UIANIMTYPE eType, _float3 vResult);
    //Event함수
    virtual void        Execute_By_Event(const string& strActionName, void* pArg);

    void        Reset_AnimationComp();

    void        Bind_SetActiveFalse(function<void()> Func);
    void        Bind_On_Active_By_CloseWindow(function<void()> Func) {m_CloseWindowEvent = Func;}
    void        Bind_On_Active_By_OpenWindow(function<void()> Func) { m_OpenWindowEvent = Func; }

public:
    void        Play_Animation(string AnimClipName);
    void        Stop_Animation();


public:
    virtual Shader* Get_Shader() { return m_pShaderCom; }
public:
    void        Set_RenderComponent(UI_Render* pRender) { m_pUIRenderer = pRender; }
    void        Set_ButtonComponent(UI_Button* pButton) { m_pUIButton = pButton; }

#pragma region Picking함수
    // 마우스가 UI 위에 있는지 검사
    virtual _bool IsMouseOver(const POINT& _mousePos);

    //부모-자식관계에서의 가장 높은 우선순위의 자식에 대한 픽킹처리
    UIObject* Pick_Recursive(ENGINEMODE eMode, const POINT& _mousePos, _vector vRayPos = XMVectorSet(0.f, 0.f, 0.f, 0.f), _vector vRayDir = XMVectorSet(0.f, 0.f, 0.f, 0.f));


    // 피킹 이벤트 콜백
    virtual void OnClick();
    virtual void OnClickCancle();     //전에 클릭된 상태였는데, 다른걸로 클릭이교체됐을때.


    virtual void OnHover();
    virtual void OnHoverEnter();
    virtual void OnHoverExit();

    //드래그 cancle여부
    virtual void    OnDragging() {}
    virtual void    OnDraggingExit();


    // 렌더 순서 (높을수록 위에 그려짐 = 먼저 피킹됨)
    void Set_ZOrder(_int _zOrder) { m_iZOrder = _zOrder; }
    virtual _int Get_ZOrder() const { return m_iZOrder; }
    bool    Get_IsInPool() { return m_bInPool; }
    virtual _float	Get_CombinedZ() const { return m_Combined.m_fZ; }


#pragma endregion Picking함수

protected:
#pragma region Bind함수
    HRESULT Bind_OrthoMatrices(class Shader* shader) const;
    HRESULT Bind_OrthoMatrices(class Shader* shader, const _string& viewmatrixname, const _string& projmatrixname) const;
#pragma endregion

public:
#pragma region Set함수
    // UI의 위치를 Window좌표에 맞춰주는 함수
    void Set_Position();
    // UI의 위치를 Window좌표에 맞춰주는 함수(오버라이드 됨)
    void Set_Position(_float _fX, _float _fY);

    void    Set_Position_By_World(_float3 vWorldPos);
    void Set_Size();
    void Set_Size(_float _fSizeX, _float _fSizeY);
    void Set_Alpha(_float _fAlpha);
    void Set_UVOffSet(_float2 OffSet);
    void Set_Rotation(_float fRotation);

    void        Set_Color(_float4 vColor);

    void        Set_Combined_Poistion(_float fX, _float fY, _float fZ = 0.f);

    void    Set_SelectState(SELECTSTATE eState) { m_eSelectMode = eState; }

    virtual void Set_Visible(_bool _isVisible);

    virtual void Set_Active(_bool _isActive);
    virtual void Set_Active(_bool isActive, _bool bUseAnim);
    void        Set_Active_Force(_bool isActive);   //강제로 끄고켜기
    void        Set_Active_Delay(_float fSecond);       //Active이후 N초 뒤에 알아서 SeT_active호출한다.


    void        Set_Dead(_bool _isalive);
    void        Set_PassNum(_uint i) { m_iShaderSlotNum = i; }

    void        Set_BaseType(string str) { m_BaseType = str; }
    void        Set_UIType(string str) { m_UIType = str; }

    void        Set_Dirty(bool b) { m_bIsDirty = b; }
    void        Set_HashName() { m_NameHash = hash<wstring>{}(m_wstrName); }
    void        Set_BlurUI(bool b) { m_bBlurUI = b; }

    void        Set_RenderGroup(RENDER_GROUP eGroup)
    {
        m_eRenderGroup = eGroup;
        for (auto& pChild : m_vecChildren)
            pChild->Set_RenderGroup(eGroup);

    };

    void        Set_InPooling(bool b) { m_bInPool = b; }
    void        Set_Persistent(bool b) { m_bPersistent = b; }

    void        Set_CombinedAlpha(_float f) { m_Combined.m_fAlpha = f; }
    void        Bind_OnClickEvent(function<void()> Func) { m_OnClickEvent = Func; }
#pragma endregion Set함수


#pragma region Get함수
    _float Get_X() const { return m_Local.m_fX; }
    _float Get_Y() const { return m_Local.m_fY; }
    _float Get_SizeX() const { return m_Local.m_fSizeX; }
    _float Get_SizeY() const { return m_Local.m_fSizeY; }
    //virtual void Get_SubElements(vector<UI_SUB_ELEMENT>& _out) { return; }
    _float Get_Alpha() const { return m_Local.m_fAlpha; }
    _float Get_Rotation() const { return m_Local.m_fRotationZ; }
    _float2 Get_UVOffSet() const { return m_Local.m_UVOffset; }

    _float4 Get_Color();
    _float4 Get_OriginColor();
    _uint   Get_PassNum() { return m_iShaderSlotNum; }
    UITransform Get_Combined() { return m_Combined; }
    _float Get_CombinedX() const { return m_Combined.m_fX; }
    _float Get_CombinedY() const { return m_Combined.m_fY; }
    _float Get_CombinedSizeX() const { return m_Combined.m_fSizeX; }
    _float Get_CombinedSizeY() const { return m_Combined.m_fSizeY; }
    _float Get_CombinedAlpha() const { return m_Combined.m_fAlpha; }
    _float Get_CombinedRotation() const { return m_Combined.m_fRotationZ; }
    _float2 Get_CombinedUVOffSet() const { return m_Combined.m_UVOffset; }

    Matrix       Get_UIViewMatrix() { return m_matViewMatrix; }
    Matrix       Get_UIProjMatrix() { return m_matProjectionMatrix; }

    SELECTSTATE      Get_SelectState() { return m_eSelectMode; }

    bool            Is_Interactable();
    void            Set_Interatable(bool b);

    UI_Render* Get_UIRenderComponent() { return m_pUIRenderer; }

    const   UITransform& Get_LocalTransform() { return m_Local; }
    const   UITransform& Get_CombinedTransform() { return m_Combined; }

    const  _float2& Get_PaddingOffSet() { return m_LocalPadding; }
    const   UIHitBox& Get_HitBoxArea() { return m_LocalHitBoxArea; }
    const   _float2 Get_HitboxSize() { return m_HitBoxSize; }

    string        Get_BaseType() { return m_BaseType; }
    string            Get_UIType() { return m_UIType; }


    size_t          Get_BaseType_By_Size_t() { return m_iBaseType; }
    size_t            Get_UIType_By_Size_t() { return m_iUIType; }

    UI_Progress* Get_MaskProgress() { return m_pMaskProgress; }
    bool            IsBlurUI() { return m_bBlurUI; }

    _float2         Get_InitSize() { return m_fInitSize; }
    _bool           Get_IsPersistent() { return m_bPersistent; }

    _bool           Is_WorldUI() { return m_eRenderGroup == RENDER_GROUP::WORLD_UI; }
    _bool           Use_MaskProgress() { return m_bMaskProgress; }

    _float          Get_Value() { return m_fValue; }
#pragma endregion Get함수

public:
#pragma region Parent-Child함수
    HRESULT         Add_Child(UIObject* pObj);  //실시간으로 추가
    HRESULT         Add_Child(GameObject* pObj);

    virtual HRESULT         Add_Child_OnLoad(UIObject* pObj); //이미로컬좌표가 계산되어있고,부모-자식만 연결할때.
    virtual HRESULT         Add_Child_OnLoad(GameObject* pObj); //이미로컬좌표가 계산되어있고,부모-자식만 연결할때.


    HRESULT         Add_Child_OnMap(UIObject* pObj);
    HRESULT         Remove_Child_OnMap(UIObject* pObj);

    void            Set_Parent(UIObject* pParent);
    void            Detach_Child();
    void            Detach_Me_From_Parent();    //부모가존재할떄, 부모에서 나를 끊는다.

    UIObject* Get_Parent() { return m_pParent; }
    vector<UIObject*>* Get_Children() { return &m_vecChildren; }
    HRESULT         Check_Duplicate_Child(UIObject* pObj);

    UIObject* Get_Child(wstring wstrName);
#pragma endregion


#pragma region Debug 함수

#ifdef _DEBUG
public:
    enum class RESIZE_DIRECTION { DIR_NONE, DIR_L, DIR_R, DIR_T, DIR_B, DIR_LT, DIR_RT, DIR_LB, DIR_RB };
    enum class EDITMODE { NONE, MOVE, SCALE, END };
    virtual void        Render_IMGUI();
    void                Render_UIInfo();
    void                Render_UITransform();
    void                Render_ActiveEvent();

    void                Transform_With_Mouse(const _float fTImeDelta);
    void                Resize_ByDiff(RESIZE_DIRECTION eDir, _float fDiffX, _float fDiffY);
    RESIZE_DIRECTION    Get_ResizeDirection();
    void                Resize_ByMouse(RESIZE_DIRECTION eDir, long DeltaX, long DeltaY);

    HRESULT                Copy(UIObject** pOut);

    void Move(_float _speed, const _float fTimeDelta);
    void Move_Down(_float _speed, const _float fTimeDelta);
    void Move_Up(_float _speed, const _float fTimeDelta);
    void Move_Left(_float _speed, const _float fTimeDelta);
    void Move_Right(_float _speed, const _float fTimeDelta);

    void    Append_ActiveEvent(bool bActive);   //Imgui에서  버튼눌러서 이벤트바인드 시 동적할당해줌
    void    Set_PassNumber_To_Chlid(_uint i);  //모든자식에게 현재 이 버튼을 누른 객체의 패스넘버 지정해주는함수

    HRESULT    Upgrade_RenderComponent(wstring ComponentName, UI_Render* pNewRenderer);

public:
    bool        Is_Lock() { return m_bLock; }
    void        Set_Lock(bool b);
private:

    RESIZE_DIRECTION        m_eResizeDir = RESIZE_DIRECTION::DIR_NONE;
    EDITMODE                m_eEditMode = EDITMODE::NONE;
    bool                    m_bDragEnter = false;

    _float2                 m_vDragStartMousePos; // 드래그 시작 시 마우스 위치
    _float2                 m_vDragStartObjPos;   // 드래그 시작 시 오브젝트 위치
    _float2                 m_vDragStartObjSize;  // 드래그 시작 시 오브젝트 크기

    bool                    m_bLock = false;        //Imgui에서 픽킹막기
    bool                    m_bAddUIManager = true;
    _wstring                 m_RendererName = L"";

#endif // _DEBUG
    int                    m_iObjectNumber = -1;

#pragma region parsing
public:
    UIObjectInfo    Save_To_Json();
    void            Apply_Data_From_Info(UIObjectInfo& Info);
    virtual         void        After_ApplyData();
    void            Add_Component_After_Load(UITYPE eType, wstring componentName, UIComponent* pComp);
    void            Add_ActiveEvent(bool bActive, UI_ActiveEvent& pEvent);

#pragma endregion

protected:
    void        Update_Recursive(const _float fTimeDelta, _float fParentfX, _float fParentfY, _float fParentScalefX, _float fParentScalefY, _float fRotationZ, _float fAlpha, _float2 fUVOffSet, bool bParentDirty);
    void        Update_World_Recursive(const _float fTimeDelta, _matrix matParentWorld, _float fAlpha);


public:
    //씬정리시 저리할것들.
    virtual void        Release_Resources();

protected:
    //나=로컬 기준의 값들
    UITransform m_Local;


    //부모가있을경우, 부모+나 의 최종값들
    UITransform m_Combined;

    _float m_fWindowX = {};
    _float m_fWindowY = {};

    _float fOriginSizeX = {};
    _float fOriginSizeY = {};


    //생성했을때의 사이즈 (파싱이후 사이즈)
    _float2 m_fInitSize = {};

    _float4x4 m_matViewMatrix = {};
    _float4x4 m_matProjectionMatrix = {};

    Shader* m_pShaderCom = { nullptr };


    // 피킹 관련 변수들
    _bool m_bHovered = { false };
    _int m_iZOrder = { 0 };

    //쉐이더 slot number
    _uint   m_iShaderSlotNum = 4;   //기본 쉐이더pass 값 : NO_Discard

    //바운딩박스 오프셋
    _float2       m_LocalPadding = _float2(0.f, 0.f);
    UIHitBox        m_LocalHitBoxArea;
    _float2         m_HitBoxSize;

    //transform정보가 바꼈을때, 다시계산 플래그
    bool        m_bIsDirty = true;

    //그냥 쉐이더에던질 float값
    _float      m_fValue = 0.f;


public:
    static UIObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    virtual GameObject* Clone(void* arg) override;

public:
    virtual void On_Active_By_OpenWindow(bool b) { if (m_OpenWindowEvent) m_OpenWindowEvent(); };//openwindwo 로 호출된함수
    virtual void On_Active_By_CloseWindow(bool b) {if (m_CloseWindowEvent) m_CloseWindowEvent();}//closewindow로 호출된함수
public:
    void Free() override;

private:
    void        Set_MaskProgress();
private:
    SELECTSTATE      m_eSelectMode = SELECTSTATE::NONESELECT;


    /*UI들이 빠르게 접근하기 위해 캐싱해놓은 멤버변수들*/
protected:
            //UI가 렌더하기위해 기본적으로 소유하는 컴포넌트, Texture를 가지지않아도 shader를 통해 transform을 바인딩하는 기본역할
    UI_Render*      m_pUIRenderer = nullptr;
    UI_Button*      m_pUIButton = nullptr;



protected:
    UIObject*               m_pParent = nullptr;              //부모 (refcount관리 x)
    vector<UIObject*>       m_vecChildren;          //자식(refcount관리O),zorder순서때문에 vector사용

   
private:
    //UI_Image 바인드 전 먼저 셰이더에 바인딩되야할 컴포넌트들을 모아둠(UI전용)
    vector<UIComponent*>        m_UICompBindList;

                    //0번 active()일때/ 1번 disactive()호출시
    UI_ActiveEvent*      m_ActiveEvents[2] = {nullptr};

    function<void()>        m_OnClickEvent = nullptr;
    function<void()>         m_DeActiveEvent = nullptr;

    function<void()>         m_CloseWindowEvent= nullptr;
    function<void()>         m_OpenWindowEvent = nullptr;

/*비활성화 시 자식이 모두 비활성화되어야 부모를 비활성화시킴*/
private:
     bool           m_bClosing = false;
     bool           m_bBlurUI = false;
protected:
    string      m_BaseType = "NONE";
    string       m_UIType="DEFAULT";

    size_t      m_iBaseType = 0;
    size_t      m_iUIType = 0;
     
    size_t      m_NameHash = 0;

    wstring      m_UIDataStr = L"";   //파싱시 필요한 텍스트? 

#ifdef _DEBUG
    ENGINEMODE  m_eEngineMode = ENGINEMODE::END;
#endif // _DEBUG


protected:
    /*UIObject의 마스킹을 이용하기위한 부모객체의 progress를 전파*/
    UI_Progress*    m_pMaskProgress = nullptr;
    UI_Progress*    m_pProgress = nullptr;
    bool            m_bMaskProgress = true;

    

private:
    /*검색용 데이터들*/
    unordered_map<size_t, UIObject*>     m_mapChildren;     //검색을 위한 맵


    RENDER_GROUP        m_eRenderGroup = RENDER_GROUP::UI;

protected:
        /*Set_Active상태로만은 pool체크 불가-> 새로운 변수추가*/
    bool        m_bInPool = false;
    _bool      m_bPersistent = false;
    Mouse* m_pMouse = nullptr;


private:
    Alarm       m_DeActiveAlarm;

};

NS_END
