#pragma once
#include "ImguiWindow.h"

NS_BEGIN(Engine)
class GameInstance;
class Light;
class Mouse;
NS_END

NS_BEGIN(ShaderTool)
class ShaderImgui :
    public ImguiWindow
{
protected:
    virtual HRESULT Initialize(void* pArg) override;
    ShaderImgui(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~ShaderImgui() = default;

public:
    static ShaderImgui* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
    virtual void Free();
    void PickGameObject();
    void GameObjectTab();
    void AddPointLight();
    void LightTab();
    void ShaderTab();
    void SaveLoadShader();
    void HDRTab();
    void ShadowTab();
    void ColorTab();
    void FogTab();
    void PBRTab();
    void ChangeMapColor();
    void GodRayTab();
    void AddSpotLight();
    void PlayerCustomTab();
    void CameraTab();
    HRESULT CreatePointLight(_int PointCnt);

public:
    //자식이 구현할 함수, 실제 이 윈도우가 무엇을하는지 if(Imgui::CheckBox)이런걸 적어주면된다.
    virtual     _uint       Update_Contents(_float fTimeDelta); //
    bool        bTest = false;
    _int m_iMeshNum = {};

private:
    unordered_map<_uint, Light*>* m_umapLightsPtr;
    GameInstance* m_pGameInstance = {};
    _int Prev_LightSize = {};
    vector<LIGHT_DESC*> m_DescVec;
    Mouse* m_pMouse = {};
    GameObject* m_pNowSelectGameObj = {};
    GameObject* m_pPrevSelectGameObj = {};
    _float4 m_vHDROption = {};
    _float4 m_vSSAOOption = {};
    SHADOW_DESC m_vStaticShadowDesc = {};
    SHADOW_DESC m_vDynamicShadowDesc = {};

    //ColorGrading용 
    ShaderDesc m_vShaderDesc;

    //캐릭터용
    PLAYER_SHADER_DESC m_vPlayerShaderDesc;
    PLAYER_MESHTEX_NUM* m_pMeshTex;
};
NS_END
