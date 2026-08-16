#pragma once
#include "UIComponent.h"

/*이벤트 처리가 필요한 컴포넌트가 object에 부착되었을때,
//그 object에 추가로 붙는 컴포넌트*/
//이벤트 처리를 받는 역할

NS_BEGIN(Engine)
class ENGINE_DLL UI_EventReactor :
    public UIComponent
{
public:
    //FilterID 가져오는 방법 정의
    enum class ETargetSource { 
        Player,     //플레이어꺼
        Self,       //컴포넌트 주인꺼
        Owner_Target,   //주인의 Target꺼
        Manual  //고유번호, 직접지정
    };


    struct EventBinding 
    {
        _uint    m_iResponseID; //TargetID와 비교해서같으면 1차 확인
        string  m_ResponseAction;  //기다리는 명령이름이자 실행시킬 액션이름
        wstring  m_ReponseComponent; //반응할 컴포넌트

        size_t  m_iResPonseActionHash=0;

        ordered_json To_Json() const
        {
            ordered_json j;
            j["ResponseAction"] = m_ResponseAction;
            j["ReponseComponent"] = wstringToString(m_ReponseComponent);

            return j;
        }

        void    From_Json(const ordered_json& j)
        {
            m_ResponseAction = j.value("ResponseAction", "");
            m_ReponseComponent = stringToWstring(j.value("ReponseComponent", ""));

            m_iResPonseActionHash = hash<string>{}(m_ResponseAction);
                

        }

    };


protected:
    explicit UI_EventReactor();
    explicit UI_EventReactor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UI_EventReactor(const UI_EventReactor& original);
    virtual ~UI_EventReactor();

public:
    HRESULT             Initialize_Prototype() override;
    HRESULT				Initialize(void* arg) override;

    void                Set_ID(_uint iTargetID) { m_iTargetID = iTargetID; }
    void                Add_Binding(const EventBinding& eBinding);

   

#ifdef _DEBUG
public:
    virtual void		Render_Imgui();
    
private:
    void                Create_Binding();
    void                Open_BindingList();

#endif


public:
    static UI_EventReactor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual Component* Clone(void* arg);


#pragma region parsing
public:
    virtual void	Save_Data(ordered_json& pJson) override;
    virtual void	Load_Data(class GameObject* pOwner, ComponentData& Data, UITYPE* eType) override;

#pragma endregion

public:
    void Free() override;
private:
    /*순회하면서 */
    vector<EventBinding>        m_Bindings;


    //Imgui에서 지정
    _uint                       m_iTargetID;    //이 컴포넌트가 붙은 uiobject의 오너의 타입
    EventHandle                 m_Handle;   //해제를 위해 저장

};
NS_END

