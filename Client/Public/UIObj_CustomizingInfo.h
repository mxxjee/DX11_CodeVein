#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_Text;

class UIObj_CustomizingInfo :
    public UIObject
{
public:
    /*Hover한 메뉴가 바뀔때마다 받는 이벤트!!*/

    struct CustomizingInfoUIEvent
    {
        CUSTOMIZING_TYPE         m_eType;
    };
private:
    //호버한 카테고리가 바뀔때마다 info의 값을 갱신해야한다.
    //이 내용은 json에 미리 기록해놓고 focustype이 갱신될때마다 이 값을 참고해서 바꾼다.
    struct Customizing_Info
    {
        CUSTOMIZING_TYPE m_eType;
        wstring m_InfoTitle = L"";      //타이틀:보통 커스터마이징이름
        wstring m_Desc=L"";     //커스터마이징에 대한 설명

        _float2  TitlePos;      //타이틀위치(아이콘+텍스트)
        _float2  Background_Pos;        //배경 위치
        _float2  Background_Scale;      //배경 스케일 (텍스트가 크면 가로로넓어짐)

        
    };


protected:
    explicit UIObj_CustomizingInfo();
    explicit UIObj_CustomizingInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_CustomizingInfo(const UIObj_CustomizingInfo& original);
    virtual ~UIObj_CustomizingInfo();

    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);
public:
    virtual void        After_ApplyData();

public:
    void Free() override;

private:
    HRESULT         Ready_Info_Data();

public:
    static UIObj_CustomizingInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


private:
    /*세팅하기위한 변수들 추가*/
    UIObj_Text* m_pTitle_Text = nullptr;
    UIObj_Text* m_pSub_Text = nullptr;
    
    UIObject* m_pTitle = nullptr;
    UIObject* m_pBackground = nullptr;


private:
    unordered_map<CUSTOMIZING_TYPE, Customizing_Info>     m_CustomInfos;

};
NS_END

