#pragma once
#include "UIObject.h"
namespace Engine
{
    class UI_Image;

}
/*Scene menu에 의해서 왼/오 화살표를통해 포커싱된 sceneslot을 따라 상태변경*/
NS_BEGIN(Client)
class UIObj_SceneSlot :
    public UIObject
{
    //EXTRA- 포커싱된 경우
    //DEFAULT- 포커싱X 축소모드

    enum class SceneSlot_State { EXTRA, DEFAULT, INIT,END };
    enum ArrowType{LEFT,RIGHT};
public:
    struct SceneSlotEvent
    {
        _uint           m_iFocusIdx;

    };

    struct SceneSlotUpdateColorEvent
    {
        _uint       m_iObjectID=0;
        wstring TexKey = L"";
    };
protected:
    explicit UIObj_SceneSlot();
    explicit UIObj_SceneSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_SceneSlot(const UIObj_SceneSlot& original);
    virtual ~UIObj_SceneSlot();
public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);
public:
    virtual void        After_ApplyData();
    //virtual void        Set_Active(_bool _isActive);

public:
    static UIObj_SceneSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void        Set_Idx(_uint i) { m_iIdx = i; }
    void        Change_State();
    void        Change_Texture(wstring TexName);
public:
    void Free() override;
private:
    void    Move_By_FocusIdx(int iDiff, const _float fTimeDelta, bool bLerp);
    void    Size_By_FocusIdx(int iDiff);

public:
    virtual void        Set_Active(_bool _isActive);
    void        Set_InitOrigin(_float2 Origin) { m_fInitOrigin = Origin; }
    void        Set_InitSize(_float2 OriginSize) { m_fInitSize = OriginSize; }

private:
    void        Bind_Arrow_Func();

private:
    /*숨겨야해서 SET_ACTIVE FALSE로 제어해야하하는 것들 캐싱*/
    vector<UIObject*>   Arrows;
    UIObject*           m_pFocusMenu = nullptr;
    UIObject*           m_pBottom_Back = nullptr;
    UIObject*           m_pSceneType = nullptr;


    SceneSlot_State         m_eSlotState= SceneSlot_State::INIT;
    SceneSlot_State         m_ePreSlotState = SceneSlot_State::END;

    _uint                   m_iIdx = 0;
    _uint                   m_iFocusIdx = 0;


    UI_Image*           m_pImageComp = nullptr;
private:
    _float2             m_fInitSize;
    _float              m_fRatio = 0.7f;     //줄일비율(default상태일땐 작게보임)
    _float              m_OffSetX = 30.f;           // x 오프셋
    
            //움직이기 위한 기준점(여기서부터 ㅓㅇㄹ마나떨어졌는지) 
    _float2             m_fInitOrigin;


                    //연출초기화값(위치)
    _float2             m_fInitPosition;

};
NS_END
