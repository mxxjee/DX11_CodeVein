#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Calculator final : public Base
{
private:
    explicit Calculator();
    explicit Calculator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~Calculator();

public:
    HRESULT Initialize();

public:
    //랜덤값(float)
    _float RandomValue(_float _min, _float _max);
    // 랜덤값(uint)
    // min과 max값이 포함되어 나옴
    _uint RandomValue_int(_uint _min, _uint _max);
    //쿼터니온 -> 디그리
    _vector QuaternionToDegrees(_fvector _quaternion);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

public:
    static Calculator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
    void Free() override final;

};

NS_END
