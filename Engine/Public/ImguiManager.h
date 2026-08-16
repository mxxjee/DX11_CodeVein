#pragma once
#include "Base.h"

/*Imgui 초기화 코드모아놓은 코드입니다..
각 툴의 MainApp부분에 호출하시면됩니다.
따로 게임인스턴스화는 안되어있습니다

또한  Imgui 윈도우들을 관리하여 알아서 update를 돌려줍니다.
*/

NS_BEGIN(Engine)
class ImguiWindow;

class ENGINE_DLL ImguiManager : public Base
{
    DECLARE_SINGLETON(ImguiManager)

private:
    explicit ImguiManager();
    virtual ~ImguiManager();

public:
    void            Init(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* device_context);
    _int	        Update_Priority(const _float fTimeDelta);
    _int            Update(const _float fTimeDelta);
    _int            Render();

    void            Test();
    void            BeginDockSpace();

public:
    // 윈도우 메시지를 ImGui로 전달하는 중계 함수
    LRESULT WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


public:
                        //ImguiManager가 관리할 윈도우 추가하는 함수
    HRESULT            RegisterWindow(ImguiWindow* pInstance);

                        //윈도우 찾는함수
    ImguiWindow*    Find_Window(string _WindowTitle);

                        //리셋할 윈도우
    void            Reset_Window(string _WindowName);

    void            Reset_All_Window();

                        // 모든 윈도우 열고 닫기
    void            Set_Open_All_Window(_bool _open);

public:
    void            Free() override;

private:
    unordered_map<string, ImguiWindow*>        m_Windows;
    ID3D11DeviceContext* m_pContext = { nullptr };

private:
    _bool        show_another_window = { false };
    _bool        show_demo_window = { false };
    _float4     clearColor = { 0.f,0.f,0.f,0.f };
};

NS_END
