#pragma once

#include "Character.h"

NS_BEGIN(Engine)

class ENGINE_DLL ContainerObject abstract : public Character
{
protected:
    explicit ContainerObject();
    explicit ContainerObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit ContainerObject(const ContainerObject& original);
    virtual ~ContainerObject();

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int    Update_Parallel(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    virtual HRESULT Render(const _float fTimeDelta) override;

public:
    HRESULT Add_PartObject(_uint _prototypeLevelIndex, const _wstring& _prototypeName, const _wstring& _partOBJName, void* arg, _bool _addRef = false);
    class PartObject* Find_PartObject(const _wstring& _partOBJName);
    unordered_map<_wstring, class PartObject*>& Get_PartObjectsMap()
    {
        return m_umapPartObjects;
    }

protected:
    unordered_map<_wstring, class PartObject*> m_umapPartObjects;

public:
    static ContainerObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual GameObject* Clone(void* arg);

public:
    void Free() override;

};

NS_END
