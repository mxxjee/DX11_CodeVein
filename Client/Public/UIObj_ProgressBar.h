#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)
class UI_Progress;
NS_END

NS_BEGIN(Client)
class UIObj_ProgressBar :
    public UIObject
{

public:
    //MAIN:보통적으로 메인이되는 체력바(빨간색, 바로줄어듬)
    //BACK : 연출용 잔상용(보통 LERP로줄어듬)
    enum class ProgressCompType { MAIN, BACK };
protected:
    explicit UIObj_ProgressBar();
    explicit UIObj_ProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_ProgressBar(const UIObj_ProgressBar& original);
    virtual ~UIObj_ProgressBar();



public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);


    //    virtual HRESULT Ready_Components(void* pArg);


    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    virtual void        After_ApplyData();
    float		        Get_TargetRatio(bool bMain);
    float		        Get_RenderRatio(bool bMain);

public:
    void        Set_CurrentFloat(_float* pCurrent);
    void        Set_MaxFloat(_float* fMax);

public:
                        //bMain=메인fillbar제어할건지 여부,아닐경우 sub fillbar를제어
    void        Set_MinHue(bool bMain,_float vMinHue);

                //bMain=메인fillbar제어할건지 여부,아닐경우 sub fillbar를제어
    void        Set_MaxHue(bool bMain,_float vMaxHue);

            //bMain=메인fillbar제어할건지 여부,아닐경우 sub fillbar를제어
    void        Set_UseRandomColor(bool bMain, bool b);

            //바로설정할값(초기화해줄값)
    void        Init_Ratio(bool bMain, _float fValue);

    void        Compute_Ratio(bool bMain);
    void        Set_TargetRatio(bool bMain,_float Value);
    void        Set_RenderRatio(bool bMain, _float fValue);

public:
    static UIObj_ProgressBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


public:
    void Free() override;

private:
            //매개변수로 들어온 오브젝트의 프로그레스컴포넌트 캐싱하는함수
    void        Cache_ProgressComponent(UIObject* pObj, ProgressCompType eType);

protected:
    UI_Progress* m_pMainProgress=nullptr;
    UI_Progress* m_pBackProgress=nullptr;

    UIObject* m_pMainObj = nullptr;
    UIObject* m_pBackObj = nullptr;

    _float* m_fCurrent = nullptr;
    _float* m_fMax = nullptr;

    
protected:
    bool        m_bUseRandomColor = false;

};
NS_END
