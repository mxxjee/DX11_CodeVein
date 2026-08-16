#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL ImguiWindow abstract : public Base
{
public:
    typedef struct ImguiWindow_Desc
    {
        string	m_WindowTitle = "";//Imgui창 생성시 표시되는 이름
    }IMGUIWINDOW_DESC;

protected:
    explicit ImguiWindow() = default;
    explicit ImguiWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~ImguiWindow() = default;


public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg);
    virtual _int    Update_Priority(const _float fTimeDelta);
    virtual _int    Update(const _float fTimeDelta);
    virtual _int    Render(const _float fTimeDelta);

    //이 윈도우를 리셋,(보통 객체 삭제 시 선택한 객체를 nullptr로 만드는 행위)
    virtual _int     Reset() { return 0; }
    string          Get_WindowTitle() { return m_WindowTitle; }

    void Set_Open(_bool _open) { m_bOpen = _open; }

protected:
            //자식이 구현할 함수, 실제 이 윈도우가 무엇을하는지 if(Imgui::CheckBox)이런걸 적어주면된다.
    virtual     _uint       Update_Contents(_float fTimeDelta)=0; //
   
public:
   virtual void Free();

protected:
    ID3D11Device*           m_pDevice = { nullptr };
    ID3D11DeviceContext*    m_pContext = { nullptr };

    string  m_WindowTitle = "";     //창 타이틀
    bool    m_bOpen = true;        //창 열림,닫힘 여부
};

NS_END