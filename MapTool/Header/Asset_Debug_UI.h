#pragma once
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
class GameObject;
class Mouse;
class Light;
NS_END

class CAsset_Debug_UI : public ImguiWindow
{
private:
	CAsset_Debug_UI(ID3D11Device* pD, ID3D11DeviceContext* pC);
	virtual ~CAsset_Debug_UI() = default;

public:
	HRESULT Initialize(void* pArg);
	virtual _uint Update_Contents(_float fDT);
	void ObjCreator();
	void ObjEditor();
	void AddPointLight();
private:
	GameInstance* m_pGameInstance = { nullptr };
	Mouse* m_pMouse = { nullptr };

	//颇老 风飘
	_string m_strCurrentPath;
	_string m_strSelectedFile;

	//积己侩
	_uint m_iLightCount = 0;
	inline static _float objScale[3] = { 1.f, 1.f, 1.f };
	inline static _float objPos[3] = { 0.f, 0.f, 0.f };
	inline static _float objRot[4] = { 0.f, 0.f, 0.f,0.f };

	LIGHT lightType = LIGHT::END;
	inline static _float4 vLightDirection = {-1.f, -1.f, -1.f, 0.f};
	inline static _float4 vLightDiffuse = { 1.f, 1.f, 1.f, 1.f };
	inline static _float4 vLightAmbient = { 1.f, 1.f, 1.f, 1.f };
	inline static _float4 vLightSpecular = { 1.f, 1.f, 1.f, 1.f };
	inline static _float4 vLightPosition = { 5.f, 5.f, -5.f, 1.f };
	
	inline static _int SelectedItem = 0;
	inline static _int SelectedLight = 0;

	_float3 m_fPos = { 0.f, 0.f, 0.f };
	_float3 m_fScale = { 1.f, 1.f, 1.f };
	_float3 m_fRotation = { 0.f, 0.f, 0.f };
	_float3 m_fBaseScale = { 0.f, 0.f, 0.f };
	_float m_fmulScale = 0.f;

	//===================================== 蝴 =================================
	unordered_map<_uint, Light*>* m_umapLightsPtr;
	vector<LIGHT_DESC*> m_DescVec;
public:
	static CAsset_Debug_UI* Create(ID3D11Device* pD, ID3D11DeviceContext* pC, void* pArg);
	virtual void Free();

};

