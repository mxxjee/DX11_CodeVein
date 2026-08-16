#pragma once

#include "Component.h"
#include "Light.h"
#include "LightManager.h"

NS_BEGIN(Engine)

class ENGINE_DLL LightComponent final : public Component
{
private:
    explicit LightComponent();
    explicit LightComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit LightComponent(const LightComponent& original);
    virtual ~LightComponent();

public:
    HRESULT Initialize_Prototype(LIGHT_DESC& Desc);
    HRESULT Initialize(void* _arg);
    _int	Update_Priority(const _float fTimeDelta);
    _int    Update(const _float fTimeDelta, _vector fPosition);
    _int	Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta) override;

public:
    _matrix& Get_Light_Matrix();
    const LIGHT_DESC& Get_LightDesc() const { return m_LightDesc; }
    _int Get_LightIndex() const { return m_iLightIndex; }

private:
    LIGHT_DESC   m_LightDesc = {};
    class Light* m_pLight;
    _int m_iLightIndex = {};
    _matrix m_pMatrix;

    //============= 일렁이는 조명 ================
    _bool   m_bFlicker = false;             // 일렁임 켜기/끄기
    _float  m_fFlickerTime = 0.f;           // 시간에 따른 사인파
    _float  m_fFlickerSpeed = 5.0f;         // 일렁이는 속도
    _float  m_fFlickerMin = 0.7f;           // 원래 수치 최소
    _float  m_fFlickerMax = 1.1f;           // 원래 수치 최대

    //기존값
    _float  m_fBaseRange = 0.f;
    _float4 m_vBaseDiffuse = _float4(0.f, 0.f, 0.f, 0.f);

    _bool m_bIsVisible = true;
public:
    void Set_Flicker(_bool bFlicker, _float fSpeed, _float fMin, _float fMax)
    {
        m_bFlicker = bFlicker;
        m_fFlickerSpeed = fSpeed;
        m_fFlickerMin = fMin;
        m_fFlickerMax = fMax;
    }

    void Set_Visible(_bool _bVisible);
    _bool Is_Visible() const { return m_bIsVisible; }

public:
    static LightComponent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LIGHT_DESC& Desc);
    virtual LightComponent* Clone(void* arg);

public:
    void Free() override final;

};

NS_END
