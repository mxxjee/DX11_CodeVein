#ifndef Engine_Function_h__
#define Engine_Function_h__

namespace Engine
{
    // 템플릿은 기능의 정해져있으나 자료형은 정해져있지 않은 것
    // 기능을 인스턴스화 하기 위하여 만들어두는 틀

    template<typename T>
    void Safe_Delete(T& pointer)
    {
        if (pointer)
        {
            delete pointer;
            pointer = nullptr;
        }
    }

    template<typename T>
    void Safe_Delete_Array(T& pointer)
    {
        if (pointer)
        {
            delete[] pointer;
            pointer = nullptr;
        }
    }

    template<typename T>
    _uint Safe_AddRef(T& pointer)
    {
        _uint iCount = 0;

        if (pointer)
        {
            iCount = pointer->AddRef();
        }
        else
        {
            MSG_ON(L"RefCount Add Failed\nCheck Pointer Assignment", L"Empty Address!");
#ifdef _DEBUG
            __debugbreak();
#endif
        }

        return iCount;
    }

    template<typename T>
    _uint Safe_AddReff(T& pointer)
    {
        _uint iCount = 0;

        if (pointer)
        {
            iCount = pointer->AddRef();
        }

        return iCount;
    }

    template<typename T>
    _uint Safe_AddRef_PerFrame(T& pointer)
    {
        _uint iCount = 0;

        if (pointer)
        {
            iCount = pointer->AddRef_PerFrame();
        }
        else
        {
            MSG_ON(L"RefCount Add Failed\nCheck Pointer Assignment", L"Empty Address!");
#ifdef _DEBUG
            __debugbreak();
#endif
        }

        return iCount;
    }

    template<typename T>
    _uint Safe_Release(T& pointer)
    {
        _uint iCount = 0;

        if (pointer != nullptr)
        {
            iCount = pointer->Release();

            if (0 == iCount)
                pointer = nullptr;
        }

        return iCount;
    }

    template<typename T>
    void Safe_PhysX_AddRef(T& physx)
    {
        if (physx)
        {
            // PhysX 객체의 참조 카운트를 1 증가시킴
            // DirectX와 달리 리턴값이 void
            physx->acquireReference();
        }
    }

    template<typename T>
    void Safe_PhysX_Release(T& physx)
    {
        if (physx)
        {
            physx->release(); // 리턴값 없음
            physx = nullptr;  // 무조건 nullptr로 초기화
        }
    }

    template<typename T>
    _uint Safe_Release_PerFrame(T& pointer)
    {
        _uint iCount = 0;

        if (pointer)
        {
            iCount = pointer->Release_PerFrame();

            if (0 == iCount)
                pointer = nullptr;
        }

        return iCount;
    }

    // XMFLOAT4x4, _float4x4행렬을 받아와서 전치해주는 함수
    inline void TransposeMatrix(XMFLOAT4X4& mat) noexcept
    {
        float t0 = mat._11, t1 = mat._12, t2 = mat._13, t3 = mat._14;
        float t4 = mat._21, t5 = mat._22, t6 = mat._23, t7 = mat._24;
        float t8 = mat._31, t9 = mat._32, t10 = mat._33, t11 = mat._34;
        float t12 = mat._41, t13 = mat._42, t14 = mat._43, t15 = mat._44;

        mat._11 = t0;  mat._12 = t4;  mat._13 = t8;  mat._14 = t12;
        mat._21 = t1;  mat._22 = t5;  mat._23 = t9;  mat._24 = t13;
        mat._31 = t2;  mat._32 = t6;  mat._33 = t10; mat._34 = t14;
        mat._41 = t3;  mat._42 = t7;  mat._43 = t11; mat._44 = t15;
    }
    inline void TransposeMatrix(XMFLOAT4X4& out, const XMFLOAT4X4& in) noexcept
    {
        if (&out == &in)
        {
            TransposeMatrix(out);
            return;
        }
        const float* src = &in._11;
        float* dst = &out._11;
        dst[0] = src[0];  dst[1] = src[4];  dst[2] = src[8];  dst[3] = src[12];
        dst[4] = src[1];  dst[5] = src[5];  dst[6] = src[9];  dst[7] = src[13];
        dst[8] = src[2];  dst[9] = src[6];  dst[10] = src[10]; dst[11] = src[14];
        dst[12] = src[3];  dst[13] = src[7];  dst[14] = src[11]; dst[15] = src[15];
    }

    // float4x4형을 정규화 하는 함수
    inline _float4x4 IdentityMatrix() noexcept
    {
        _float4x4 returnmatrix = {};
        returnmatrix._11 = 1.f;  returnmatrix._12 = 0.f;  returnmatrix._13 = 0.f;  returnmatrix._14 = 0.f;
        returnmatrix._21 = 0.f;  returnmatrix._22 = 1.f;  returnmatrix._23 = 0.f;  returnmatrix._24 = 0.f;
        returnmatrix._31 = 0.f;  returnmatrix._32 = 0.f;  returnmatrix._33 = 1.f;  returnmatrix._34 = 0.f;
        returnmatrix._41 = 0.f;  returnmatrix._42 = 0.f;  returnmatrix._43 = 0.f;  returnmatrix._44 = 1.f;
        return returnmatrix;
    }

    inline _float4x4 IdentityMatrix(XMFLOAT4X4& mat) noexcept
    {
        mat._11 = 1.f;  mat._12 = 0.f;  mat._13 = 0.f;  mat._14 = 0.f;
        mat._21 = 0.f;  mat._22 = 1.f;  mat._23 = 0.f;  mat._24 = 0.f;
        mat._31 = 0.f;  mat._32 = 0.f;  mat._33 = 1.f;  mat._34 = 0.f;
        mat._41 = 0.f;  mat._42 = 0.f;  mat._43 = 0.f;  mat._44 = 1.f;
        return _float4x4() = {};
    }

    inline _matrix NormalizeMatrix(_fmatrix _mat)
    {
        _matrix result = _mat;
        result.r[0] = XMVector3Normalize(result.r[0]);
        result.r[1] = XMVector3Normalize(result.r[1]);
        result.r[2] = XMVector3Normalize(result.r[2]);
        return result;
    }

    // string을 wstring으로 바꿔주는 함수
    inline _wstring stringToWstring(const _string& _str)
    {
        if (_str.empty()) return L"failed to wstring";

        _int size_needed = MultiByteToWideChar(CP_UTF8, 0, _str.data(), CAST(_int)(_str.size()), nullptr, 0);

        if (size_needed <= 0) return L"failed to wstring";

        _wstring _wstr(size_needed, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, _str.data(), CAST(_int)(_str.size()), &_wstr[0], size_needed);

        return _wstr;
    }

    // wstring을 string으로 바꿔주는 함수
    inline _string wstringToString(const _wstring& _wstr)
    {
        if (_wstr.empty()) return "failed to string";

        _int size_needed = WideCharToMultiByte(CP_UTF8, 0, _wstr.data(), CAST(_uint)(_wstr.size()), nullptr, 0, nullptr, nullptr);

        if (size_needed <= 0) return "failed to string";

        _string str(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, _wstr.data(), CAST(_uint)(_wstr.size()), &str[0], size_needed, nullptr, nullptr);

        return str;
    }

    // ========== ObjectKey 추출 함수 ==========
    // 경로에서 ObjectKey 추출
    // 입력: "../../Resources/Model/Wonder_Acute/Wonder_Acute.siho"
    // 출력: "Model/Wonder_Acute/Wonder_Acute"
    inline _string ExtractObjectKey(const _wstring& _wstrFilePath)
    {
        _string strPath = wstringToString(_wstrFilePath);

        // /Resources/ 또는 \Resources\ 찾기
        size_t pos = strPath.find("/Resources/");
        if (pos == _string::npos)
            pos = strPath.find("\\Resources\\");

        if (pos == _string::npos)
        {
            // Resources 못 찾으면 파일명만 반환
            size_t lastSlash = strPath.find_last_of("/\\");
            size_t lastDot = strPath.find_last_of('.');
            if (lastSlash != _string::npos && lastDot != _string::npos)
                return strPath.substr(lastSlash + 1, lastDot - lastSlash - 1);
            return strPath;
        }

        // /Resources/ 이후부터 확장자 제외
        _string result = strPath.substr(pos + 11);  // "/Resources/" 길이 = 11

        // 확장자 제거
        size_t dotPos = result.find_last_of('.');
        if (dotPos != _string::npos)
            result = result.substr(0, dotPos);

        // 백슬래시를 슬래시로 통일
        for (char& c : result)
        {
            if (c == '\\')
                c = '/';
        }

        return result;
    }

    // ========== Model 이름 등록 함수 ==========
    // 경로에서 ObjectKey에서 Model Tag 추출
    // 입력: "Model/Wonder_Acute/Wonder_Acute.siho"
    // 출력: "Prototype_Component_Model_Wonder_Acute"
    inline _wstring ObjKey_To_Model_Tag(const _string& objectKey)
    {
        // 마지막 '/' 이후의 문자열만 추출
        size_t lastSlash = objectKey.rfind('/');
        _string modelName;

        if (lastSlash != _string::npos)
            modelName = objectKey.substr(lastSlash + 1);
        else
            modelName = objectKey;

        // 확장자 제거 (.siho 등)
        size_t dotPos = modelName.rfind('.');
        if (dotPos != _string::npos)
            modelName = modelName.substr(0, dotPos);

        _string strTag = "Prototype_Component_Model_" + modelName;

        // string -> wstring 변환
        _wstring wstrTag;
        wstrTag.assign(strTag.begin(), strTag.end());

        return wstrTag;
    }

    // string 입력 버전
    inline _string ExtractObjectKey(const _string& _strFilePath)
    {
        return ExtractObjectKey(stringToWstring(_strFilePath));
    }

    // wstring 반환 버전
    inline _wstring ExtractObjectKeyW(const _wstring& _wstrFilePath)
    {
        return stringToWstring(ExtractObjectKey(_wstrFilePath));
    }


#pragma region 보간식
    // float형식끼리의 선형보간
    inline _float fLerp(_float fStart, _float fEnd, _float t)
    {
        return fStart + (fEnd - fStart) * t;
    }

    /* Easing함수 모음(위의 선형보간식과 같이 사용해서 time을 넣어주면 됨)*/
    /* ex) fLerp(0.f, 100.f, SmoothStep(ratio)); */

    // 부드럽게 시작하고 끝남 (가장 많이 씀)
    inline _float SmoothStep(_float t)
    {
        return t * t * (3.0f - 2.0f * t);
    }

    // Ken Perlin의 개선 버전 - 더 부드러움
    inline _float SmootherStep(_float t)
    {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    // 천천히 시작 (Ease In)
    inline _float EaseInQuad(_float t)
    {
        return t * t;
    }

    // 천천히 끝남 (Ease Out)
    inline _float EaseOutQuad(_float t)
    {
        return 1.0f - (1.0f - t) * (1.0f - t);
    }

    // 천천히 시작하고 천천히 끝남
    inline _float EaseInOutQuad(_float t)
    {
        return t < 0.5f
            ? 2.0f * t * t
            : 1.0f - pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
    }

    // Cubic 버전 (더 극적인 가속/감속)
    inline _float EaseInCubic(_float t)
    {
        return t * t * t;
    }

    inline _float EaseOutCubic(_float t)
    {
        return 1.0f - pow(1.0f - t, 3.0f);
    }

    inline _float EaseInOutCubic(_float _t)
    {
        // 0~0.5: 가속, 0.5~1: 감속
        if (_t < 0.5f)
            return 4.f * _t * _t * _t;
        else
        {
            _float f = (2.f * _t - 2.f);
            return 0.5f * f * f * f + 1.f;
        }
    }

    // 탄성 효과 (통통 튀는 느낌)
    inline _float EaseOutElastic(_float t)
    {
        if (t == 0.0f || t == 1.0f) return t;

        const _float c4 = (2.0f * XM_PI) / 3.0f;
        return pow(2.0f, -10.0f * t) * sin((t * 10.0f - 0.75f) * c4) + 1.0f;
    }

    // 살짝 뒤로 갔다가 나가는 효과
    inline _float EaseOutBack(_float t)
    {
        const _float c1 = 1.70158f;
        const _float c3 = c1 + 1.0f;

        return 1.0f + c3 * pow(t - 1.0f, 3.0f) + c1 * pow(t - 1.0f, 2.0f);
    }

    // 좌우로 흔들리면서 점점 감쇠하는 효과
    // frequency: 흔들리는 빈도 (높을수록 빠르게 흔들림)
    // decay: 감쇠 속도 (높을수록 빨리 멈춤)
    inline _float ShakeDecay(_float t, _float frequency = 8.0f, _float decay = 5.0f)
    {
        return sin(t * frequency * XM_2PI) * exp(-decay * t);
    }

    /// <summary>
    /// 펄스 효과 - 빠르게 올라갔다 천천히 내려옴 (피격, 스킬 발동 등)
    /// </summary>
    /// <param name="t">0~1 진행도</param>
    /// <param name="fPeak">피크 위치 (0~1, 낮을수록 빨리 최대)</param>
    /// <returns>0~1 사이 값</returns>
    inline _float Pulse(_float t, _float fPeak = 0.2f)
    {
        if (t < fPeak)
            return t / fPeak;
        else
            return 1.f - (t - fPeak) / (1.f - fPeak);
    }

    /// <summary>
    /// 바운스 효과 - 땅에 떨어져서 튀는 느낌
    /// </summary>
    /// <param name="t">0~1 진행도</param>
    /// <returns>0~1 사이 값 (여러 번 튀며 1에 수렴)</returns>
    inline _float EaseOutBounce(_float t)
    {
        const _float n1 = 7.5625f;
        const _float d1 = 2.75f;

        if (t < 1.f / d1)
            return n1 * t * t;
        else if (t < 2.f / d1)
            return n1 * (t -= 1.5f / d1) * t + 0.75f;
        else if (t < 2.5f / d1)
            return n1 * (t -= 2.25f / d1) * t + 0.9375f;
        else
            return n1 * (t -= 2.625f / d1) * t + 0.984375f;
    }

    //일정하게 이동
    inline _float MoveTowards(_float fCurrent, _float fTarget, _float fMaxDelta)
    {
        _float fDiff = fTarget - fCurrent;

        if (fabs(fDiff) <= fMaxDelta)
            return fTarget;


        return fCurrent + (fDiff > 0 ? fMaxDelta : -fMaxDelta);
    }

    // 0과 1 사이를 리턴해주는 함수
    inline _float saturate(_float fValue)
    {
        if (fValue < 0.f)
            return 0.f;
        else if (fValue > 1.f)
            return 1.f;

        return fValue;
    }
#pragma endregion 보간식



#pragma region Sin파
    /// <summary>
    /// sin파로 값이 min~max 사이를 부드럽게 왔다갔다 하는 함수
    /// </summary>
    /// <param name="fAccTime">누적 시간</param>
    /// <param name="fSpeed">속도</param>
    /// <param name="fMin">최소값(기본 0)</param>
    /// <param name="fMax">최대값(기본 1)</param>
    /// <returns></returns>
    inline _float SinWave(_float fAccTime, _float fSpeed, _float fMin = 0.f, _float fMax = 1.f)
    {
        // sin은 -1 ~ 1 → 0 ~ 1로 변환 후 min~max 범위로 매핑
        _float fNormalized = (sinf(fAccTime * fSpeed) + 1.f) * 0.5f;
        return fMin + (fMax - fMin) * fNormalized;
    }

    /// <summary>
    /// 0→1→0→1 선형 왕복 (삼각파)
    /// </summary>
    /// <param name="fAccTime">누적 시간</param>
    /// <param name="fPeriod">한 사이클 주기 (초)</param>
    /// <returns>0~1 사이 값</returns>
    inline _float PingPong(_float fAccTime, _float fPeriod = 1.f)
    {
        _float t = fmodf(fAccTime, fPeriod) / fPeriod;  // 0~1 정규화
        return t < 0.5f ? t * 2.f : 2.f - t * 2.f;
    }

    /// <summary>
    /// 호흡 효과 - 자연스러운 숨쉬기 느낌 (NPC idle, 아이콘 강조 등)
    /// </summary>
    /// <param name="fAccTime">누적 시간</param>
    /// <param name="fSpeed">속도</param>
    /// <returns>0~1 사이 값</returns>
    inline _float Breathe(_float fAccTime, _float fSpeed = 1.f)
    {
        _float t = (sinf(fAccTime * fSpeed) + 1.f) * 0.5f;
        return t * t * (3.f - 2.f * t);  // SmoothStep 적용
    }

    /// <summary>
    /// 심장박동 효과 - 두 번 뛰고 쉬는 패턴 (체력바 같은거 두쿵두쿵)
    /// </summary>
    /// <param name="fAccTime">누적 시간</param>
    /// <param name="fBPM">분당 박동수 (기본 60 = 1초에 1회)</param>
    /// <returns>0~1 사이 값</returns>
    inline _float Heartbeat(_float fAccTime, _float fBPM = 60.f)
    {
        _float fPeriod = 60.f / fBPM;
        _float t = fmodf(fAccTime, fPeriod) / fPeriod;  // 0~1 정규화

        // 두 번의 짧은 피크
        _float beat1 = expf(-powf((t - 0.1f) * 20.f, 2.f));
        _float beat2 = expf(-powf((t - 0.25f) * 20.f, 2.f)) * 0.6f;

        return beat1 + beat2;
    }
#pragma endregion Sin파


#pragma region 각도 보간
    // 각도 차이를 -PI ~ +PI 범위로 래핑
    inline _float WrapAngle(_float _angle)
    {
        while (_angle > XM_PI)  _angle -= XM_2PI;
        while (_angle < -XM_PI) _angle += XM_2PI;
        return _angle;
    }

    // 각도 전용 보간 (최단 경로)
    inline _float LerpAngle(_float _from, _float _to, _float _t)
    {
        return _from + WrapAngle(_to - _from) * _t;
    }

    _vector Slerp(_vector _a, _vector _b, _float _t)
    {
        // sin 기반 가중치로 호를 따라 이동
        float cosTheta = XMVectorGetX(XMQuaternionDot(_a, _b));
        float theta = acosf(cosTheta);
        float sinTheta = sinf(theta);

        float weight0 = sinf((1.0f - _t) * theta) / sinTheta;
        float weight1 = sinf(_t * theta) / sinTheta;

        return XMVectorAdd(
            XMVectorScale(_a, weight0),
            XMVectorScale(_b, weight1)
        );
    }
#pragma endregion

    //HSV->RGB변환
    inline _float4 HSVtoRGB(_float h, _float s, _float v)
    {
        _float r{ 0.f }, g{ 0.f }, b{ 0.f };
        _int i = (_int)(h / 60.0f) % 6;
        _float f = (h / 60.0f) - i;
        _float p = v * (1.0f - s);
        _float q = v * (1.0f - s * f);
        _float t = v * (1.0f - s * (1.0f - f));

        switch (i) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
        }
        return _float4(r, g, b, 1.0f);
    }


    //RGB to HSV

    inline _float3 RGBtoHSV(_float r, _float g, _float b)
    {
        _float3 hsv;
        _float min, max, delta;

        min = r < g ? r : g;
        min = min < b ? min : b;

        max = r > g ? r : g;
        max = max > b ? max : b;

        hsv.z = max; // Value(명도)
        delta = max - min;

        // 채도 (Saturation) 계산
        if (delta < 0.00001f)
        {
            hsv.y = 0;
            hsv.x = 0; // Hue는 정의되지 않지만 보통 0으로 세팅
            return hsv;
        }

        if (max > 0.0f) {
            hsv.y = (delta / max);
        }
        else {
            // max가 0이면 검은색
            hsv.y = 0;
            hsv.x = 0;
            return hsv;
        }

        // 색상 (Hue) 계산
        if (r >= max)                           // 빨간색 계열
            hsv.x = (g - b) / delta;
        else if (g >= max)                      // 초록색 계열
            hsv.x = 2.0f + (b - r) / delta;
        else                                    // 파란색 계열
            hsv.x = 4.0f + (r - g) / delta;

        hsv.x *= 60.0f; // 도(Degree) 단위로 변환

        if (hsv.x < 0.0f)
            hsv.x += 360.0f;

        return hsv;
    }

    inline _float3 RGBtoHSV(_float4 vColor)
    {
        return RGBtoHSV(vColor.x, vColor.y, vColor.z);
    }

    inline size_t HashString(const _string& _name)
    {
        // std::hash가 string을 받아서 size_t 정수를 리턴해줌
        // 같은 문자열은 항상 같은 숫자가 나옴
        static std::hash<string> hasher;
        return hasher(_name);
    }

    inline size_t HashWstring(const _wstring& _name)
    {
        // std::hash가 string을 받아서 size_t 정수를 리턴해줌
        // 같은 문자열은 항상 같은 숫자가 나옴
        static std::hash<wstring> hasher;
        return hasher(_name);
    }
}

#endif // Engine_Function_h__
