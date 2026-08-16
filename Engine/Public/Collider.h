#pragma once

#include "Component.h"
#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"

NS_BEGIN(Engine)

class ENGINE_DLL Collider final : public Component
{
private:
    explicit Collider();
    explicit Collider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Collider(const Collider& original);
    virtual ~Collider();

public:
    HRESULT Initialize_Prototype(COLLIDER _type);
    HRESULT Initialize(void* _arg);
    _int	Update_Priority(const _float fTimeDelta);
    _int	Update(const _fmatrix& _wordMatrix);
    _int	Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta) override;

    _bool Is_Active() const { return m_bIsActive; }
    _bool Is_Collision() const { return m_bIsColl; }
    COLLIDER Get_Type() const { return m_eType; }
    COLLISION_GROUP Get_Group() const { return m_eGroup; }
    Bounding* Get_Bounding() { return m_pBounding; }
    Collider* Get_Other() const { return m_pOther; }

    void Set_Active(_bool _active) { m_bIsActive = _active; }
    void Set_Group(COLLISION_GROUP _group) { m_eGroup = _group; }
    void Set_Collision(_bool _isColl, Collider* _other = nullptr)
    {
        m_bIsColl = _isColl;
        m_pOther = _other;
    }

    _bool Intersects(Bounding* _other) { return m_pBounding->Intersects(_other); }


#ifdef _DEBUG
    void Render_Debug(PrimitiveBatch<DirectX::VertexPositionColor>* _batch) override;
    void Add_Debug_Render();
#endif // _DEBUG


private:
    COLLIDER            m_eType = {};               // Collider Type
    COLLISION_GROUP     m_eGroup = { COLLISION_GROUP::END };
    class Bounding*     m_pBounding = { nullptr };  // 하나의 Collider가 Bounding만을 소유
    _bool               m_bIsColl = { false };      // 현재 충돌 상태인지
    Collider*           m_pOther = { nullptr };     // 충돌 상대 저장
    _bool               m_bIsActive = { true };     // 활성화 상태


#ifdef _DEBUG
    /* Collider에서 DebugRender 삭제 이제 Renderer에서 해줌 */
    // PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = { nullptr };
    //  BasicEffect*									m_pEffect = { nullptr };
    //  ID3D11InputLayout*						    m_pInputLayout = { nullptr };
#endif

public:
    static Collider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, COLLIDER _type);
    virtual Collider* Clone(void* arg);

public:
    void Free() override final;

};

NS_END
