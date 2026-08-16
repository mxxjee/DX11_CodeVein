#pragma once
#include "UIComponent.h"

/*이 컴포넌트가 붙은 ui들은 월드상에 배치된다.*/
NS_BEGIN(Engine)
class GameObject;

class ENGINE_DLL UI_WorldComponent:
    public UIComponent
{
public:
    struct WorldUICompDesc : public UIComponent::UICOMPDESC
    {
        GameObject* pTarget = nullptr;          //따라다닐객체
        _float3     vWorldPos = { 0.f,0.f,0.f };    //타겟이 없을 경우의 월드위치
        _float3     vOffset = { 0.f,0.f,0.f };      //타겟기준 오프셋


        _float      fMaxVisibleDist = 50.f;         //보일 수 있는 최대거리
        _bool        bDistanceScaling = true;       //거리에 따라서 크기변할건지.
        _float       MinScale = 0.5f;           //거리에 따라 작아질 경우 최소 스케일 설정
        _uint        TargetCameraIdx = 1;

    };
protected:
    explicit UI_WorldComponent();
    explicit UI_WorldComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UI_WorldComponent(const UI_WorldComponent& original);
    virtual ~UI_WorldComponent();

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    virtual _int	Update(const _float fTimeDelta);
    virtual _int	Update_Late(const _float fTimeDelta);
    
    void        Clear_Target();
    void        Set_Target(GameObject* pObj) { m_pTarget = pObj; }
    void        Set_OffSet(_float3 vOffset) { m_vOffSet = vOffset; }

    virtual void	Render_Imgui();

public:
	void        Set_WorldPos(_float3 vWorldPos) { m_vWorldPos = vWorldPos; }
public:
    virtual void	Save_Data(ordered_json& pJson);
    //세팅이후 넘겨줄값이있다면 설정(UI인경우 컴포넌트 Type판단이필요)
    virtual void	Load_Data(class GameObject* pOwner, ComponentData& Data, UITYPE* eType);

public:
    static UI_WorldComponent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual Component* Clone(void* arg);

public:
    void Free() override;



private:
    GameObject* m_pTarget = nullptr;
    _float3     m_vWorldPos=_float3(0.f,0.f,0.f);//타겟이 없을 경우의 월드위치
    _float3     m_vOffSet;

    _float      m_fMaxVisibleDist;
    _bool       m_bDistanceScaling;
    _float      m_MinScale;

    _uint       m_TargetCameraIdx=1;


private:
    _vector         m_vBasePos;

};
NS_END

