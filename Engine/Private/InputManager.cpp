#include "Engine_Define.h"
#include "InputManager.h"

#include "GameInstance.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::InputManager::InputManager()
{
}

Engine::InputManager::InputManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice), m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

Engine::InputManager::~InputManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::InputManager::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// DInput 컴객체를 생성하는 함수
	MSG_FAIL(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, NULL),
		L"DINPUT 컴객체 생성 실패", L"오류!!!", E_FAIL);

	// 키보드 객체 생성
	MSG_FAIL(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr),
		L"DINPU 키보드 생성 실패", L"오류!!!", E_FAIL);

	// 생성된 키보드 객체에 대한 정보를 컴 객체에게 전달하는 함수
	CHECK_FAILED(m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard), E_FAIL);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	CHECK_FAILED(m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE), E_FAIL);

	// 장치에 대한 access 버전을 받아오는 함수
	CHECK_FAILED(m_pKeyBoard->Acquire(), E_FAIL);


	// 마우스 객체 생성
	MSG_FAIL(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr),
		L"DINPU 마우스 생성 실패", L"오류!!!", E_FAIL);

	// 생성된 마우스 객체에 대한 정보를 컴 객체에게 전달하는 함수
	CHECK_FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse), E_FAIL);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	CHECK_FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE), E_FAIL);

	// 장치에 대한 access 버전을 받아오는 함수
	CHECK_FAILED(m_pMouse->Acquire(), E_FAIL);

	// 핫키에 사용되는 수식키 비트마스크 초기화
	m_byModifierMask = 0;

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
HRESULT Engine::InputManager::Update()
{
	// 키 입력 방지
	if (!g_bAppActive)
	{
		memcpy_s(&m_byPrevKeyState, sizeof(m_byPrevKeyState), &m_byKeyState, sizeof(m_byKeyState));
		memcpy_s(&m_tPrevMouseState, sizeof(DIMOUSESTATE), &m_tMouseState, sizeof(DIMOUSESTATE));

		ZeroMemory(m_byKeyState, sizeof(m_byKeyState));
		ZeroMemory(&m_tMouseState, sizeof(m_tMouseState));

		return S_OK;
	}

	// 이전 상태 저장 후, 현재 상태 갱신 먼저 수행
	memcpy_s(&m_byPrevKeyState, sizeof(m_byPrevKeyState), &m_byKeyState, sizeof(m_byKeyState));
	memcpy_s(&m_tPrevMouseState, sizeof(DIMOUSESTATE), &m_tMouseState, sizeof(DIMOUSESTATE));
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_tMouseState);

	// 핫키 체크를 1회 수행하고, 소비된 키를 마스킹
	_bool bAnyHotkeyFired = false;

	for (auto& Key : m_Hotkeys)
	{
		if (!CheckHotKeyPressed(Key))
			continue;

		if (Key.m_CallBack)
			Key.m_CallBack();

		bAnyHotkeyFired = true;

		if (Key.eMode == HOTKEY_MODE::PRESS)
			m_byKeyState[Key.eKeyCode] = 0;
	}

	// 핫키가 하나라도 발동됐으면 수식키도 마스킹하여 일반 입력 차단
	if (bAnyHotkeyFired && m_byModifierMask != 0)
	{
		if (m_byModifierMask & MODIFIER_CTRL)
		{
			m_byKeyState[DIK_LCONTROL] = 0;
			m_byKeyState[DIK_RCONTROL] = 0;
		}
		if (m_byModifierMask & MODIFIER_SHIFT)
		{
			m_byKeyState[DIK_LSHIFT] = 0;
			m_byKeyState[DIK_RSHIFT] = 0;
		}
		if (m_byModifierMask & MODIFIER_ALT)
		{
			m_byKeyState[DIK_LALT] = 0;
			m_byKeyState[DIK_RALT] = 0;
		}
	}

	return S_OK;
}
/******************************************************* 업데이트 함수 *******************************************************/



HRESULT Engine::InputManager::Register_HotKey(_ubyte Key, bool bCtrl, bool bShift, bool bAlt, HotKeyCallback Func, HOTKEY_MODE eMode)
{
	HotKey Data;

	Data.eKeyCode = Key;
	Data.Ctrl = bCtrl;
	Data.Shift = bShift;
	Data.alt = bAlt;
	Data.eMode = eMode;
	Data.m_CallBack = Func;

	m_Hotkeys.emplace_back(Data);

	// 등록된 핫키의 수식키 조합을 비트마스크에 누적
	if (bCtrl)  m_byModifierMask |= MODIFIER_CTRL;
	if (bShift) m_byModifierMask |= MODIFIER_SHIFT;
	if (bAlt)   m_byModifierMask |= MODIFIER_ALT;

	return S_OK;
}

bool Engine::InputManager::CheckHotKeyPressed(const HotKey& Key)
{
	// 모드에 따라 메인 키 판정 분기
	_bool bMainKey = (Key.eMode == HOTKEY_MODE::DOWN)
		? KeyDown(Key.eKeyCode)
		: KeyPress(Key.eKeyCode);

	// 메인 키가 눌리지 않았다면 즉시 false반환
	if (!bMainKey)
		return false;

	_bool bPressedAlt = KeyPress(DIK_LALT);
	_bool bPressedShift = KeyPress(DIK_LSHIFT);
	_bool bPressedCtrl = KeyPress(DIK_LCONTROL);

	// 각 수식키 상태가 등록된 값과 정확히 일치하는지 확인
	return (bPressedCtrl == Key.Ctrl) &&
		(bPressedShift == Key.Shift) &&
		(bPressedAlt == Key.alt);
}

_bool Engine::InputManager::IsAnyKeyPressed()
{
	for (int i = 0; i < 256; ++i)
	{
		if (m_byKeyState[i] & 0x80)
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
InputManager* Engine::InputManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HINSTANCE hInst, HWND hWnd)
{
	InputManager* pInstance = new InputManager(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize(hInst, hWnd), L"InputManager 생성 실패", L"Caution!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::InputManager::Free()
{
	__super::Free();

	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
/******************************************************* 객체 반환 함수 *******************************************************/

