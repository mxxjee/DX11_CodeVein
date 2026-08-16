#pragma once

#include "Base.h"

NS_BEGIN(Engine)
class Mouse;
class UIObject;

class ENGINE_DLL UIManager final : public Base
{
public:
    struct UI_RULE
    {
        bool m_bIsActive = false;
        bool m_bUseAnim = false;

    };
private:
    explicit UIManager();
    explicit UIManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~UIManager();

public:
    HRESULT Initialize();
    void    Initialize_Global_Actions();

    _int	Update_Priority(const _float fTimeDelta);
    _int	Update(const _float fTimeDelta);
    _int	Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta);


public:
    const unordered_map<size_t, UIObject*>& Get_AllUIObjects() { return m_umapUIs; }
    HRESULT Add_UIObject(class UIObject* _ui, bool bReAdd = false);   //다시넣은거면 zorder 변경안하기 위해..
    void Clear_UIManager();
    void Delete_DeadUI();
    void Rename_Object(wstring OldName, UIObject* pObj);
    


    //관리대상에서 제외하는 함수
    HRESULT Remove_UIObject(class UIObject* _ui);
    HRESULT Remove_UIObject(_wstring UIName);

    inline UIObject* Find_UI_ByName(const _wstring& _uiName);
    void Visible_All_UI(_bool _isVisible);

    UIObject* Pick_UI(const POINT& _mousePos);
    void Process_Picking(const POINT& _mousePos, _bool _bClicked);

    void Set_Active(_bool _active) { m_bIsActive = _active; m_pHoveredUI = nullptr; }
    bool    Get_Active() { return m_bIsActive; }
    int Find_RenderSequence(UIObject* pTarget);
    void    Active_UI(bool _active, wstring _windowName);

    UIObject* Get_ClickedUI() { return m_pClickedUI; }
    void    Clear_ClickedUI() { m_pClickedUI = nullptr; }
    
    UIObject* Get_HoveredUI() { return m_pHoveredUI; }
    void    Clear_HoveredUI() { m_pHoveredUI = nullptr; }


    void    Set_Force_HoverUI(UIObject* pObj);
    size_t          Get_WindowStackCount() { return m_WindowStack.size(); }
    UIObject* Get_Top_At_WindowStack();

private:

            //클라이언트- Interactable가능한것만 픽킹
    UIObject* Client_Picking(const POINT& _mousePos,int highestZOrder);

            //모든 UIObject픽킹검사
    UIObject* Editor_Picking(const POINT& _mousePos,int highestZOrder);
private:
    unordered_map<size_t, UIObject*> m_umapUIs;         //마스터 mapUIS
    vector<UIObject*>               m_UIObjs;       //픽킹(역순으로 처리해야함)

    UIObject* m_pHoveredUI = { nullptr };
    UIObject* m_pClickedUI = { nullptr };
    UIObject* m_pDragTarget = nullptr;

    _bool m_bIsActive = { false };

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

public:
    static UIManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
    void Free() override final;


#ifdef _DEBUG
public:
    void        Render_Search();
    void        Render_Hierarchy();
    void        Render_Imguizmo();
    void        Change_RenderSequence(UIObject* pDst, UIObject* pSrc);
    UIObject*   Get_SelectObject() { return m_pImguiSelectObject; }
    void        Set_SelectObject(UIObject* pObj);
private:
    void        Render_Hierarchy_Recursive(UIObject* pRoot);
    
                //내가 드래그 시작 주체일떄
    void        Start_DragDrop(UIObject* pTarget);

                //내가 드롭 대상일때(드래그 드롭해서 충돌한 대상일때)
    void        Send_DragDrop(UIObject* pTarget);

                //빈공간으로 드래그드롭할떄(부모해제)
    void        Start_Drag_ToEmpty();

            //검색어로 작성된 이름이 자식중에 포함되있는지 반환
    bool        Has_Filtered_Child(UIObject* pObj);

private:
    UIObject*               m_pImguiSelectObject = nullptr;
    ImGuiTextFilter         m_HierarchyFilter;//검색창 imgui

 #endif // _DEBUG

private:
    static      int         m_iZOrder;
    unordered_map < string, function<void(const UI_MasterEvent&)>>   m_mapGlobalEvents;
    Mouse*      m_pMouse = nullptr;

    
private:
    unordered_map<wstring, _uint>        m_ObjIDs;


    /*Window ui 관리*/
public:
            //창을 새로열었을때.
    void        Push_Window(wstring _windowName, bool bPersistent = false,bool bLockInput=true);
    void        Close_Window(wstring _windowName);
    void        Close_Window();//맨위에있는 창 끄기
    void        Close_All_Menu();
    UIObject* Get_CurrentWindow() { return m_pCurrentWindow; }
    bool     Is_Empty_WindowStack() { return m_WindowStack.empty(); }

  
private:
    UIObject*           m_pCurrentWindow = nullptr;
     stack<size_t>        m_WindowStack;

#pragma region 풀링
public:  
    void    Register_Pooling_Func(UIPOOLINGFUNC         Func) { m_PoolingFunc = Func; }
                //풀링정보 등록
    void        Register_Factory(_uint eType, UIObjectInfo Info);
            //풀링정보를 통해서 미리 객체생성해놓기
    void        Add_PoolObject(_uint eType, _uint Size);

    UIObject* Get_PoolObject(_uint eType);
    void        Return_PoolObject(_uint eType, UIObject* pTarget);

    UIObjectInfo*     Get_Proto_At_UIPool(_uint eType);

private:
                //_uint : UITYPE 
    unordered_map<_uint, vector<UIObject*>>      m_UIPools; //실제풀링
    unordered_map<_uint, UIObjectInfo>        m_Proto_UIObjectInfo;          //풀링할 객체가없을 경우 참고할 원본UIData
    
    UIPOOLINGFUNC               m_PoolingFunc ;

public:
    /*레벨에 종속되지않고 지워지지않는애들*/
    HRESULT        Register_PersistentUI(class UIObject* _ui);
    inline UIObject* Find_PersistentUI_ByName(const _wstring& _uiName);

    void        Set_AllLoadPersistents(bool b)  { m_bLoadAllPersistents = b; }
    bool        Get_AllLoadPersistents()    { return m_bLoadAllPersistents; }
    private:
    unordered_map<_uint, UIObject*>      m_PersistentUIs;
    vector<UIObject*>                   m_PersistentVector;
    bool                                m_bLoadAllPersistents;  //중복로드 방지


#pragma endregion

#pragma region UIMODE
    /*UI창이 열리면 꺼지고 켜지는것들이 달라서 관리하기 위해 만듬*/
private:
                //모드에 따라서 어떤걸 활성화할지/비활성화할지 설정
    void        Register_UIMode_Rules();

public:
    void        Change_UIMode(UI_MODE eMode,bool bUseActiveAnim=false);
    
private:
    //각 모드에 따른 창 껏다키기 규칙설정
    //[모드] -> [ {그룹키, 보여줄지여부} ]
    unordered_map<UI_MODE, map<wstring, UI_RULE>> m_mapModeRules;

#pragma endregion





public:
    //씬전환시 정리하는 함수. ( peristent가ㅣ 아닌 ui들만정리)
    void        Clear_Scene_UI();
	bool        m_bMouseOnImgui = false; // Imgui위에 마우스가 올라와있는지 여부
};

NS_END
