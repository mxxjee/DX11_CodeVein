#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class Camera;

class CameraManager final : public Base
{
private:
    explicit CameraManager();
    virtual ~CameraManager();

public:
    HRESULT Initialize();
    _int	Update_Priority(const _float fTimeDelta);
    _int	Update(const _float fTimeDelta);
    void    Update_Cameras(const _float fTimeDelta);
    _int	Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta);

public:
    void CameraManager_Reset(_uint _maxCameraCount);
    HRESULT Add_Camera(_uint _prototypeLevelID, const _wstring& _cameraName, _uint _levelID, _uint _cameraNum, class GameObject** _outCamera = nullptr, void* arg = nullptr);
    void Camera_Change(_uint _cameraNum);
    void Camera_Active(_uint _cameraNum, _bool _isActive = true);

    void Set_Aspect(_float _aspect);

    // 현재 카메라의 보고있는 위치 설정
    void Camera_LookAt(_vector _targetPosition);

    const _float3 Get_PrevCameraPos_Float3();
    const _vector Get_PrevCameraPos_Vector();

    const _float4x4 Get_PrevCamera_Float4x4();
    const _matrix Get_PrevCamera_Matrix();

    vector<Camera*> Get_CameraVector() { return m_vecCameras; }
    inline Camera* Get_Camera(_uint _cameraNum) {
        if (m_iNumCameras <= _cameraNum)
        {
            MSG_ON(L"대입한 카메라 번호가 실제 카메라 개수보다 높습니다", L"경고!!!");
            BREAK;
            return nullptr;
        }
        return m_vecCameras[_cameraNum];
    }
    inline Camera* Get_CurrentCamera() {
        return m_vecCameras[m_iCurrentCamera];
    }

private:
    _uint m_iNumCameras = {};
    _uint m_iCurrentCamera = {};
    vector<Camera*> m_vecCameras;
    _uint m_iPrevCamera = {};

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

public:
    static CameraManager* Create();

public:
    void Free() override final;

};

NS_END
