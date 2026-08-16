#pragma once
#include "Base.h"

namespace Engine
{
    class GameInstance;
    class UIObject;
}

/*커스터마이징 매니저 : 각 ui들의 이벤트 전달자.*/

NS_BEGIN(Client)
class Player;
class UIObj_CustomizeSelector;
class UIObj_Window_Palette;

class Player_Head;
class Player_Hair;
class Player_Body;

class CustomizingManager :
    public Base
{

    DECLARE_SINGLETON(CustomizingManager);

    
public: 
            //커마 메뉴는 클릭을 해야 창이고정됨
                //FOCUS - 다른 메뉴 호버 불가
                //READY-다른 메뉴 호버 가능
                //우선순위 ACTIVE_PICKER->FOCUS->READY
    enum CustomMizingstate {READY,FOCUS,ACTIVE_PICKER,END};
private:
    explicit CustomizingManager();
    virtual ~CustomizingManager();


public:
    HRESULT Initialize();   //파싱할데이터들 미리 파싱해놓기
    HRESULT Cache_Pickers();    //버튼을 눌러서 열 수 있는 선택창들(파레트,,이런것들) 미리캐싱

                //오른쪽클릭으로 창끄기 
    void        Update(const _float fTimeDelta);

public:
    /*custom menu button 호버 시 호출(갱신)*/
    void        Set_FocusType(CUSTOMIZING_TYPE eType);
    void        Set_FocusIdx(_uint iIdx) { m_iFocusIdx = iIdx; }

        //실제로 focus한 메뉴를 클릭했을때 호출(진짜 메뉴진입)
    void        On_FocusMenuEnter();

                //오른쪽클릭을 눌러서 메뉴나갔을때
    void        On_FocusMenuExit();



                //지금상태가 ready인데 창클식시->focus
            //지금상태가 focus이고 열린창이없을떄, ready
    HRESULT                Set_UIState(CustomMizingstate eState);
    void                Enter_State(CustomMizingstate eState);
    CustomMizingstate   Get_UIState() { return m_eUIState; }



    void        Set_Actie(bool b) { m_bActive = b; }
    bool        Is_Active() { return m_bActive; }

                    //선택목록을 띄운다(팔레트나 그리드 요런것들/ 타입과 호출한애 매개변수로 저장)
    void    Request_Open_ActivePicker(CUSTOMIZING_TYPE CustomType, CUSTOMIZING_VALUE_TYPE eType, UIObj_CustomizeSelector* Caller);

                //(오른쪽 버튼 클릭시 호출)닫으라고 요청한다. 요청하면서 부른애한테 이벤트같은거전달.
    void    Request_Close_ActivePicker();

            //연출때문에 hover exit가 안풀려서 직접호출하기위해 만듬.
    void    Hover_CustomizeSelector(UIObject* pObj);
    void    Set_HoverTargetField(UIObj_CustomizeSelector* pObj, int idx) { m_pHoverTargetField = pObj; m_iFocusIdx = idx; }



                //커스텀 데이타 갱신 및 display 아이콘에 반영
    void        Update_CustomData(CUSTOMIZING_VALUE_TYPE ValueType, void* pArg);


   
                //현재편집중인 selector에게 업데이트 시킴
    void        Update_CurrentTargetField(void* pArg);

private:
    /*색상 기본값 설정*/
    HRESULT Ready_SkinData();
    HRESULT Ready_HairData();
    HRESULT Ready_EyebrowData();
    HRESULT Ready_EyeData();
    HRESULT Ready_MakeUpData();
    HRESULT Ready_ClothesData();
    HRESULT Ready_FadcePaintData();

    //실제 연동할맵ㅇ ㅔ 저장하는함수
    HRESULT Update_ColorData(_float4 vColor,_float Saturate);
    HRESULT Update_TexData(_uint iTex);
    HRESULT Update_MeshData(_uint iTex);
    HRESULT Update_ValueData(_float fValue);

    HRESULT Update_ClothesData(_float4 vColor, _float Saturate);
public:
    /*각 selector가 이벤트구독을 통해서 이 함수를 호출해 자신의 defualt컬러로 세팅한다.*/
    CustomColorInfo* Get_InitialColor(CUSTOMIZING_TYPE eType, _int iIdx);

    /*각 selector가 이벤트구독을 통해서 이 함수를 호출해 자신의 texture idx로 세팅한다.*/
    CustomItemGridInfo* Get_InitialTexture(CUSTOMIZING_TYPE eType, _int iIdx);

    /*각 selector가 이벤트구독을 통해서 이 함수를 호출해 자신의 texture idx로 세팅한다.*/
    CustomItemGridInfo* Get_InitialMesh(CUSTOMIZING_TYPE eType, _int iIdx);

    /*각 selector가 이벤트구독을 통해서 이 함수를 호출해 자신의 texture idx로 세팅한다.*/
    CustomAlphaValueInfo* Get_InitialValue(CUSTOMIZING_TYPE eType, _int iIdx);


    //옷은 따로 로딩
    void        Update_Clothes_Idx_Preview(_uint ClothesIdx);

    void        Update_Clothes_Idx(_uint ClothesIdx);

public:
    void Free() override final;


private:
    CUSTOMIZING_TYPE m_eFocusType = CUSTOMIZING_TYPE::END;
    CUSTOMIZING_TYPE m_ePreFocusType = CUSTOMIZING_TYPE::END;
    CUSTOMIZING_TYPE     m_eEditingType = CUSTOMIZING_TYPE::END;

                                        //썸네일을 누를때마다 바뀐다.
    int             m_iFocusIdx = 0;        //색상을 여러개 선택하는데 메뉴안에서의 몇번쨰 색상값인지(연동용)

   
    bool            m_bEditing = false;     //edit중에는 호버x
private:
    GameInstance*   m_pGameInstance = nullptr;

private:
    unordered_map<CUSTOMIZING_TYPE, CustomData> m_CustomDatas;


public:
    //진짜 플레이엉게 값전달해서 연동시키는 함수
    void        Connet_Player() {}
                                                                                                    //아래값이  nullptr이라면 저장된데이터를, 아니면 특정데이터를넘김
    void        Connet_Player(CUSTOMIZING_TYPE eCustomType, CUSTOMIZING_VALUE_TYPE eCustomValueType, _uint iDataIdx, void* pData =nullptr);
   
private:
    CustomMizingstate           m_eUIState= CustomMizingstate::READY; //UI창 전체 상태
    CustomMizingstate           m_ePreState = CustomMizingstate::END; //UI창 전체 상태

    bool                        m_bActive = true;


    UIObject*                   m_pActivePicker = nullptr;    //현재 열려있는 선택창( color palette or scroll grid)
    UIObj_CustomizeSelector*    m_pCurrentTargetField = nullptr;    //선택창을 열라고 한 주체 ( 껐을대 값을 되돌려주기위해)
    UIObj_CustomizeSelector*    m_pPreTargetField = nullptr;   
    
    UIObj_CustomizeSelector*    m_pHoverTargetField = nullptr; ///.,..하드코딩같은데 진짜 경우가 게쉤이네
                                                        //경우에 상관없이 무조건 hover된 애 저장하는 용도

    UIObj_CustomizeSelector*    m_pSelectedTargetField = nullptr;
    int             m_iSelectFocusIdx = 0;


            
    vector<UIObject*>           m_pActivePickers;

    /*clothes는 따로 보관*/
    _uint               m_iClothesIdx = 0;
    _uint               m_iSelectClothesIdx = 0;
private:
    UIObj_Window_Palette* m_pPalette = nullptr;     //함수써야하므로 캐싱
    
#ifdef _DEBUG
    void       Debug_Current() {};
#endif // _DEBUG

private:
    //플레이어정보 캐싱
    Player* m_pMainPlayer = nullptr;
    PLAYER_SHADER_DESC* pPlayer_Shader_Desc;
    HEAD_TEXTURE_CHANGE* pHead_Texture_Desc = nullptr;

    //파트오브젝트 캐싱
    Player_Head* m_pPlayerHead = nullptr;
    Player_Hair* m_pPlayerHair = nullptr;
    Player_Body* m_pPlayerBody = nullptr;

    bool        m_bOpenValueType = false;
};
NS_END
