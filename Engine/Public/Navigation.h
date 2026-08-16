#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Navigation final : public Component
{
public:
    typedef struct tagNavigationDesc {
        const _float4x4* pParentMatrix = { nullptr };
        _int iCurrentCellIndex = { -1 };
    }NAVIGATION_DESC;

private:
    explicit Navigation();
    explicit Navigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Navigation(const Navigation& original);
    virtual ~Navigation();

public:
    HRESULT Initialize_Prototype(const _wstring& NavigationData);
    HRESULT Initialize(void* arg);
#ifdef _DEBUG
    HRESULT Render(const _float fTimeDelta);
#endif

    _bool IsMove(_fvector vResultPos);

    _vector SetUp_OnNavigation(const _fvector _worldPos);

private:
    HRESULT Setup_Neighbors();

private:
    _uint m_iNumCells = {};
    vector<class Cell*> m_vecCells;
    _int m_iCellIndex = { -1 }; // 내비게이션 컴포넌트가 현재 위치해 있는 셀 번호

    static const _float4x4* m_pParentMatrix;

#ifdef _DEBUG
    class Shader* m_pShader = { nullptr };
#endif

public:
    static Navigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& NavigationData);
    virtual Navigation* Clone(void* arg);

public:
    void Free() override final;

};

NS_END
