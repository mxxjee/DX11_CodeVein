#pragma once

#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

NS_BEGIN(Engine)



class InputManager final : public Base
{
	


private:
	explicit InputManager();
	explicit InputManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~InputManager();

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	HRESULT Update();
	bool	CheckHotKeyPressed(const HotKey& Key);
public:
#pragma region KeyBoard_Function
	_byte	Get_DIKeyState(_ubyte byKeyID) const
	{
		return m_byKeyState[byKeyID];
	}

	_bool KeyPress(_ubyte byKeyID) const
	{
		return (m_byKeyState[byKeyID] & 0x80);
	}

	_bool KeyDown(_ubyte byKeyID) const
	{
		return ((m_byKeyState[byKeyID] & 0x80) && !(m_byPrevKeyState[byKeyID] & 0x80));
	}

	_bool KeyUp(_ubyte byKeyID) const
	{
		return (!(m_byKeyState[byKeyID] & 0x80) && (m_byPrevKeyState[byKeyID] & 0x80));
	}

	//아무키나 눌렸는지 확인
	_bool		IsAnyKeyPressed();

#pragma endregion



#pragma region Mouse_Function
	const _byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) const
	{
		return m_tMouseState.rgbButtons[_UINT(eMouse)];
	}

	_bool MousePress(MOUSEKEYSTATE eMouse) const
	{
		return (m_tMouseState.rgbButtons[_UINT(eMouse)] & 0x80);
	}

	_bool MouseDown(MOUSEKEYSTATE eMouse) const
	{
		return ((m_tMouseState.rgbButtons[_UINT(eMouse)] & 0x80) && !(m_tPrevMouseState.rgbButtons[_UINT(eMouse)] & 0x80));
	}

	_bool MouseUp(MOUSEKEYSTATE eMouse) const
	{
		return (!(m_tMouseState.rgbButtons[_UINT(eMouse)] & 0x80) && (m_tPrevMouseState.rgbButtons[_UINT(eMouse)] & 0x80));
	}

	// 현재 마우스의 특정 축 좌표를 반환(마우스 위치?)
	__forceinline const _long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState) const
	{
		switch (eMouseState)
		{
		case MOUSEMOVESTATE::X: return m_tMouseState.lX;
		case MOUSEMOVESTATE::Y: return m_tMouseState.lY;
		case MOUSEMOVESTATE::WHEEL: return m_tMouseState.lZ;
		}
		return 0;
		//return *(((_long*)&m_tMouseState) + _UINT(eMouseState));
	}
#pragma endregion


#pragma region HotKey
	// 수식키 비트마스크 상수
	static constexpr _ubyte MODIFIER_CTRL = 0x01;
	static constexpr _ubyte MODIFIER_SHIFT = 0x02;
	static constexpr _ubyte MODIFIER_ALT = 0x04;

	HRESULT		Register_HotKey(_ubyte Key, bool bCtrl, bool bShift, bool bAlt, HotKeyCallback Func, HOTKEY_MODE eMode = HOTKEY_MODE::DOWN);

private:
	vector<HotKey>		m_Hotkeys;
	_ubyte				m_byModifierMask = {}; // 등록된 핫키들의 수식키 합집합
#pragma endregion

private:
	LPDIRECTINPUT8 m_pInputSDK = { nullptr };
	LPDIRECTINPUTDEVICE8 m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8 m_pMouse = { nullptr };

	_byte m_byKeyState[256]{};
	_byte m_byPrevKeyState[256]{};
	DIMOUSESTATE m_tMouseState = {};
	DIMOUSESTATE m_tPrevMouseState = {};

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

public:
	static InputManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HINSTANCE hInst, HWND hWnd);

public:
	void Free() override final;

};

NS_END

#endif // InputDev_h__