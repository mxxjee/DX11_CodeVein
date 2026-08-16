// Sample_Pars_Shader.h

#pragma once
#include "Serializable.h"
#include "Client_Define.h"

NS_BEGIN(Engine)
class GameInstance;
NS_END

NS_BEGIN(Client)

class Sample_Pars_Shader final : public Serializable
{
private:
    explicit Sample_Pars_Shader();
    virtual ~Sample_Pars_Shader();

public:
    HRESULT Initialize();

    // 저장 관련
    void Start_Save(const SAVEEVENT& _event);
    void Collect_Data();

    // 로드 관련
    void Start_Load(const LOADEVENT& _event);

public:
    // 저장/로드할 데이터
    vector<ShaderObjectInfo> vecData = {};

public:
    // ========== Serializable 인터페이스 구현 ==========
    ordered_json To_Json() override
    {
        ordered_json dataArray = ordered_json::array();
        for (const auto& info : vecData)
        {
            dataArray.push_back(info.To_Json());
        }
        return dataArray;
    }

    void From_Json(const ordered_json& _jsonData) override
    {
        vecData.clear();

        for (const auto& objJson : _jsonData)
        {
            ShaderObjectInfo info;
            info.From_Json(objJson);
            vecData.push_back(info);
        }
    }

    void To_Binary(ofstream& _file) override
    {
        size_t count = vecData.size();
        _file.write(RCAST(_cchar)(&count), sizeof(size_t));

        for (const auto& info : vecData)
        {
            // ObjectKey
            size_t keyLen = info.strObjectKey.size();
            _file.write(RCAST(_cchar)(&keyLen), sizeof(size_t));
            _file.write(info.strObjectKey.c_str(), keyLen);

            // ObjectType
            _file.write(RCAST(_cchar)(&info.iObjectType), sizeof(_uint));

            // ShaderPrototype (wstring)
            _string shaderStr = wstringToString(info.wstrShaderPrototype);
            size_t shaderLen = shaderStr.size();
            _file.write(RCAST(_cchar)(&shaderLen), sizeof(size_t));
            _file.write(shaderStr.c_str(), shaderLen);

            // MeshPasses
            size_t passCount = info.vecMeshPasses.size();
            _file.write(RCAST(_cchar)(&passCount), sizeof(size_t));
            if (passCount > 0)
            {
                _file.write(RCAST(_cchar)(info.vecMeshPasses.data()), passCount * sizeof(_uint));
            }
        }
    }

    void From_Binary(ifstream& _file) override
    {
        vecData.clear();

        size_t count = 0;
        _file.read(RCAST(_pchar)(&count), sizeof(size_t));

        vecData.reserve(count);

        for (size_t i = 0; i < count; ++i)
        {
            ShaderObjectInfo info;

            // ObjectKey
            size_t keyLen = 0;
            _file.read(RCAST(_pchar)(&keyLen), sizeof(size_t));
            info.strObjectKey.resize(keyLen);
            _file.read(&info.strObjectKey[0], keyLen);

            // ObjectType
            _file.read(RCAST(_pchar)(&info.iObjectType), sizeof(_uint));

            // ShaderPrototype
            size_t shaderLen = 0;
            _file.read(RCAST(_pchar)(&shaderLen), sizeof(size_t));
            _string shaderStr;
            shaderStr.resize(shaderLen);
            _file.read(&shaderStr[0], shaderLen);
            info.wstrShaderPrototype = stringToWstring(shaderStr);

            // MeshPasses
            size_t passCount = 0;
            _file.read(RCAST(_pchar)(&passCount), sizeof(size_t));
            if (passCount > 0)
            {
                info.vecMeshPasses.resize(passCount);
                _file.read(RCAST(_pchar)(info.vecMeshPasses.data()), passCount * sizeof(_uint));
            }

            vecData.push_back(info);
        }
    }

private:
    GameInstance* m_pGameInstance = { nullptr };

public:
    static Sample_Pars_Shader* Create();

    void Free() override final;
};

NS_END