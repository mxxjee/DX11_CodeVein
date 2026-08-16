#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <cstring>
#include <filesystem>
#include <cstdint>

#include "json.hpp"
using ordered_json = nlohmann::ordered_json;

using namespace std;
namespace fs = std::filesystem;


//////////////////////////////////////////////////////// float 구조체 ////////////////////////////////////////////////////////
struct Float2 { float x = 0.f, y = 0.f; };
struct Float3 { float x = 0.f, y = 0.f, z = 0.f; };
struct Float4 { float x = 0.f, y = 0.f, z = 0.f, w = 0.f; };
/******************************************************* float 구조체 *******************************************************/


//////////////////////////////////////////////////////// VariantValue ////////////////////////////////////////////////////////
using VariantValue = std::variant<
std::monostate,         // tag 0
int,                    // tag 1
unsigned int,           // tag 2
float,                  // tag 3
bool,                   // tag 4
std::string,            // tag 5
Float2,                 // tag 6
Float3,                 // tag 7
Float4                  // tag 8
> ;

using VariantMap = std::unordered_map<std::string, VariantValue>;
/******************************************************* VariantValue *******************************************************/


//////////////////////////////////////////////////////// enum ////////////////////////////////////////////////////////
enum class ANIM_NOTIFY_TYPE : int
{
    NOTIFY = 0,
    NOTIFY_STATE = 1,
};

enum class ANIM_EVENT_TYPE : int
{
    PLAY_SOUND = 0,
    SPAWN_PARTICLE = 1,
    ACTIVE_COLLIDER = 2,
    CAMERA = 3,
    SPAWN_TRAIL = 4,
    PLAYER_ANIM = 5,
    MONSTER_ANIM = 6,
    DISSOLVE_FLAG = 7, // 추가됨 : 디졸브 플래그
};
/******************************************************* enum *******************************************************/


//////////////////////////////////////////////////////// 바이너리 유틸리티 ////////////////////////////////////////////////////////
void BinWriteString(fstream& file, const string& str)
{
    int len = static_cast<int>(str.size());
    file.write(reinterpret_cast<const char*>(&len), sizeof(int));
    if (len > 0)
        file.write(str.data(), len);
}

void BinReadString(fstream& file, string& str)
{
    int len = 0;
    file.read(reinterpret_cast<char*>(&len), sizeof(int));
    str.resize(len);
    if (len > 0)
        file.read(&str[0], len);
}

void BinWriteVariant(fstream& file, const VariantValue& val)
{
    uint8_t tag = static_cast<uint8_t>(val.index());
    file.write(reinterpret_cast<const char*>(&tag), sizeof(tag));

    switch (tag)
    {
    case 0: break;
    case 1: { int v = std::get<int>(val);              file.write(reinterpret_cast<const char*>(&v), sizeof(v)); } break;
    case 2: { unsigned int v = std::get<unsigned int>(val); file.write(reinterpret_cast<const char*>(&v), sizeof(v)); } break;
    case 3: { float v = std::get<float>(val);           file.write(reinterpret_cast<const char*>(&v), sizeof(v)); } break;
    case 4: { bool v = std::get<bool>(val);             file.write(reinterpret_cast<const char*>(&v), sizeof(v)); } break;
    case 5: { BinWriteString(file, std::get<string>(val)); } break;
    case 6: { Float2 v = std::get<Float2>(val);         file.write(reinterpret_cast<const char*>(&v), sizeof(v)); } break;
    case 7: { Float3 v = std::get<Float3>(val);         file.write(reinterpret_cast<const char*>(&v), sizeof(v)); } break;
    case 8: { Float4 v = std::get<Float4>(val);         file.write(reinterpret_cast<const char*>(&v), sizeof(v)); } break;
    }
}

void BinReadVariant(fstream& file, VariantValue& val)
{
    uint8_t tag = 0;
    file.read(reinterpret_cast<char*>(&tag), sizeof(tag));

    switch (tag)
    {
    case 0: val = std::monostate{}; break;
    case 1: { int v = 0;              file.read(reinterpret_cast<char*>(&v), sizeof(v)); val = v; } break;
    case 2: { unsigned int v = 0;     file.read(reinterpret_cast<char*>(&v), sizeof(v)); val = v; } break;
    case 3: { float v = 0.f;          file.read(reinterpret_cast<char*>(&v), sizeof(v)); val = v; } break;
    case 4: { bool v = false;         file.read(reinterpret_cast<char*>(&v), sizeof(v)); val = v; } break;
    case 5: { string s; BinReadString(file, s); val = std::move(s); } break;
    case 6: { Float2 v; file.read(reinterpret_cast<char*>(&v), sizeof(v)); val = v; } break;
    case 7: { Float3 v; file.read(reinterpret_cast<char*>(&v), sizeof(v)); val = v; } break;
    case 8: { Float4 v; file.read(reinterpret_cast<char*>(&v), sizeof(v)); val = v; } break;
    }
}
/******************************************************* 바이너리 유틸리티 *******************************************************/


//////////////////////////////////////////////////////// 데이터 구조체 ////////////////////////////////////////////////////////
struct ConvKeys
{
    float mTime = 0.f;
    Float3 mValue;
};

struct ConvRotKeys
{
    float mTime = 0.f;
    Float4 mValue;
};

struct ConvChannel
{
    unsigned int mChannelIndex = 0;
    unsigned int mNumScalingKeys = 0;
    unsigned int mNumRotationKeys = 0;
    unsigned int mNumPositionKeys = 0;
    unsigned int mNumKeyFrames = 0;
    vector<ConvKeys>    mScalingKeys;
    vector<ConvRotKeys> mRotationKeys;
    vector<ConvKeys>    mPositionKeys;
};

struct ConvNotify
{
    float fFrame = 0.f;
    float fStartFrame = 0.f;
    float fEndFrame = 0.f;
    bool  bActive = false;
    bool  bAttached = false;
    unsigned int iNotifyId = 0;

    ANIM_NOTIFY_TYPE eNotify_Type = ANIM_NOTIFY_TYPE::NOTIFY;
    ANIM_EVENT_TYPE  eNotify_Event = ANIM_EVENT_TYPE::PLAY_SOUND;

    VariantMap UmapEvent;
    string SocketName;
};

struct ConvAnimation
{
    bool         mIsLoopAnim = false;
    bool         mLinkedAnim = false;
    bool         mCompleteAnim = false;
    unsigned int mNumChannels = 0;
    int          mNextAnimIndex = -1;
    float        mDuration = 0.f;
    float        mTickPerSecond = 0.f;
    float        mNextAnimLerpDuration = 0.2f;
    string       mName;
    vector<ConvChannel> mChannels;
    vector<ConvNotify>  mNotifyes;
};
/******************************************************* 데이터 구조체 *******************************************************/


//////////////////////////////////////////////////////// 바이너리 읽기 ////////////////////////////////////////////////////////
void ReadChannel(fstream& file, ConvChannel& ch)
{
    file.read(reinterpret_cast<char*>(&ch.mChannelIndex), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&ch.mNumScalingKeys), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&ch.mNumRotationKeys), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&ch.mNumPositionKeys), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&ch.mNumKeyFrames), sizeof(unsigned int));

    ch.mScalingKeys.resize(ch.mNumScalingKeys);
    for (auto& key : ch.mScalingKeys)
    {
        file.read(reinterpret_cast<char*>(&key.mTime), sizeof(float));
        file.read(reinterpret_cast<char*>(&key.mValue), sizeof(Float3));
    }

    ch.mRotationKeys.resize(ch.mNumRotationKeys);
    for (auto& key : ch.mRotationKeys)
    {
        file.read(reinterpret_cast<char*>(&key.mTime), sizeof(float));
        file.read(reinterpret_cast<char*>(&key.mValue), sizeof(Float4));
    }

    ch.mPositionKeys.resize(ch.mNumPositionKeys);
    for (auto& key : ch.mPositionKeys)
    {
        file.read(reinterpret_cast<char*>(&key.mTime), sizeof(float));
        file.read(reinterpret_cast<char*>(&key.mValue), sizeof(Float3));
    }
}

void ReadNotify(fstream& file, ConvNotify& n)
{
    file.read(reinterpret_cast<char*>(&n.fFrame), sizeof(float));
    file.read(reinterpret_cast<char*>(&n.fStartFrame), sizeof(float));
    file.read(reinterpret_cast<char*>(&n.fEndFrame), sizeof(float));
    file.read(reinterpret_cast<char*>(&n.bActive), sizeof(bool));
    file.read(reinterpret_cast<char*>(&n.bAttached), sizeof(bool));
    file.read(reinterpret_cast<char*>(&n.iNotifyId), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&n.eNotify_Type), sizeof(ANIM_NOTIFY_TYPE));
    file.read(reinterpret_cast<char*>(&n.eNotify_Event), sizeof(ANIM_EVENT_TYPE));

    unsigned int count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(unsigned int));
    n.UmapEvent.clear();
    n.UmapEvent.reserve(count);
    for (unsigned int i = 0; i < count; ++i)
    {
        string key;
        VariantValue value;
        BinReadString(file, key);
        BinReadVariant(file, value);
        n.UmapEvent.emplace(std::move(key), std::move(value));
    }

    int nameLen = 0;
    file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
    n.SocketName.resize(nameLen);
    if (nameLen > 0)
        file.read(&n.SocketName[0], nameLen);
}

ConvAnimation ReadAnimation(fstream& file)
{
    ConvAnimation anim;

    file.read(reinterpret_cast<char*>(&anim.mIsLoopAnim), sizeof(bool));
    file.read(reinterpret_cast<char*>(&anim.mLinkedAnim), sizeof(bool));
    file.read(reinterpret_cast<char*>(&anim.mCompleteAnim), sizeof(bool));
    file.read(reinterpret_cast<char*>(&anim.mNumChannels), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&anim.mNextAnimIndex), sizeof(int));
    file.read(reinterpret_cast<char*>(&anim.mDuration), sizeof(float));
    file.read(reinterpret_cast<char*>(&anim.mTickPerSecond), sizeof(float));
    file.read(reinterpret_cast<char*>(&anim.mNextAnimLerpDuration), sizeof(float));

    int nameLen = 0;
    file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
    anim.mName.resize(nameLen);
    if (nameLen > 0)
        file.read(&anim.mName[0], nameLen);

    anim.mChannels.resize(anim.mNumChannels);
    for (auto& ch : anim.mChannels)
        ReadChannel(file, ch);

    unsigned int notifyCount = 0;
    file.read(reinterpret_cast<char*>(&notifyCount), sizeof(unsigned int));
    anim.mNotifyes.resize(notifyCount);
    for (auto& n : anim.mNotifyes)
        ReadNotify(file, n);

    return anim;
}
/******************************************************* 바이너리 읽기 *******************************************************/


//////////////////////////////////////////////////////// 바이너리 쓰기 ////////////////////////////////////////////////////////
void WriteChannel(fstream& file, const ConvChannel& ch)
{
    file.write(reinterpret_cast<const char*>(&ch.mChannelIndex), sizeof(unsigned int));
    file.write(reinterpret_cast<const char*>(&ch.mNumScalingKeys), sizeof(unsigned int));
    file.write(reinterpret_cast<const char*>(&ch.mNumRotationKeys), sizeof(unsigned int));
    file.write(reinterpret_cast<const char*>(&ch.mNumPositionKeys), sizeof(unsigned int));
    file.write(reinterpret_cast<const char*>(&ch.mNumKeyFrames), sizeof(unsigned int));

    for (const auto& key : ch.mScalingKeys)
    {
        file.write(reinterpret_cast<const char*>(&key.mTime), sizeof(float));
        file.write(reinterpret_cast<const char*>(&key.mValue), sizeof(Float3));
    }
    for (const auto& key : ch.mRotationKeys)
    {
        file.write(reinterpret_cast<const char*>(&key.mTime), sizeof(float));
        file.write(reinterpret_cast<const char*>(&key.mValue), sizeof(Float4));
    }
    for (const auto& key : ch.mPositionKeys)
    {
        file.write(reinterpret_cast<const char*>(&key.mTime), sizeof(float));
        file.write(reinterpret_cast<const char*>(&key.mValue), sizeof(Float3));
    }
}

void WriteNotify(fstream& file, const ConvNotify& n)
{
    file.write(reinterpret_cast<const char*>(&n.fFrame), sizeof(float));
    file.write(reinterpret_cast<const char*>(&n.fStartFrame), sizeof(float));
    file.write(reinterpret_cast<const char*>(&n.fEndFrame), sizeof(float));
    file.write(reinterpret_cast<const char*>(&n.bActive), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&n.bAttached), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&n.iNotifyId), sizeof(unsigned int));
    file.write(reinterpret_cast<const char*>(&n.eNotify_Type), sizeof(ANIM_NOTIFY_TYPE));
    file.write(reinterpret_cast<const char*>(&n.eNotify_Event), sizeof(ANIM_EVENT_TYPE));

    unsigned int count = static_cast<unsigned int>(n.UmapEvent.size());
    file.write(reinterpret_cast<const char*>(&count), sizeof(unsigned int));
    for (const auto& [key, value] : n.UmapEvent)
    {
        BinWriteString(file, key);
        BinWriteVariant(file, value);
    }

    int nameLen = static_cast<int>(n.SocketName.size());
    file.write(reinterpret_cast<const char*>(&nameLen), sizeof(int));
    if (nameLen > 0)
        file.write(n.SocketName.data(), nameLen);
}

void WriteAnimation(fstream& file, const ConvAnimation& anim)
{
    file.write(reinterpret_cast<const char*>(&anim.mIsLoopAnim), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&anim.mLinkedAnim), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&anim.mCompleteAnim), sizeof(bool));
    file.write(reinterpret_cast<const char*>(&anim.mNumChannels), sizeof(unsigned int));
    file.write(reinterpret_cast<const char*>(&anim.mNextAnimIndex), sizeof(int));
    file.write(reinterpret_cast<const char*>(&anim.mDuration), sizeof(float));
    file.write(reinterpret_cast<const char*>(&anim.mTickPerSecond), sizeof(float));
    file.write(reinterpret_cast<const char*>(&anim.mNextAnimLerpDuration), sizeof(float));

    int nameLen = static_cast<int>(anim.mName.size());
    file.write(reinterpret_cast<const char*>(&nameLen), sizeof(int));
    if (nameLen > 0)
        file.write(anim.mName.data(), nameLen);

    for (const auto& ch : anim.mChannels)
        WriteChannel(file, ch);

    unsigned int notifyCount = static_cast<unsigned int>(anim.mNotifyes.size());
    file.write(reinterpret_cast<const char*>(&notifyCount), sizeof(unsigned int));
    for (const auto& n : anim.mNotifyes)
        WriteNotify(file, n);
}
/******************************************************* 바이너리 쓰기 *******************************************************/


//////////////////////////////////////////////////////// 메모리 -> JSON ////////////////////////////////////////////////////////
ordered_json VariantToJson(const VariantValue& val)
{
    ordered_json j;
    uint8_t tag = static_cast<uint8_t>(val.index());
    j["tag"] = tag;

    switch (tag)
    {
    case 0: j["value"] = nullptr; break;
    case 1: j["value"] = std::get<int>(val); break;
    case 2: j["value"] = std::get<unsigned int>(val); break;
    case 3: j["value"] = std::get<float>(val); break;
    case 4: j["value"] = std::get<bool>(val); break;
    case 5: j["value"] = std::get<string>(val); break;
    case 6: { const auto& v = std::get<Float2>(val); j["value"] = { v.x, v.y }; } break;
    case 7: { const auto& v = std::get<Float3>(val); j["value"] = { v.x, v.y, v.z }; } break;
    case 8: { const auto& v = std::get<Float4>(val); j["value"] = { v.x, v.y, v.z, v.w }; } break;
    }
    return j;
}

ordered_json KeysToJson(const ConvKeys& key)
{
    ordered_json j;
    j["Time"] = key.mTime;
    j["Value"] = { key.mValue.x, key.mValue.y, key.mValue.z };
    return j;
}

ordered_json RotKeysToJson(const ConvRotKeys& key)
{
    ordered_json j;
    j["Time"] = key.mTime;
    j["Value"] = { key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w };
    return j;
}

ordered_json ChannelToJson(const ConvChannel& ch)
{
    ordered_json j;
    j["ChannelIndex"] = ch.mChannelIndex;
    j["NumKeyFrames"] = ch.mNumKeyFrames;

    j["ScalingKeys"] = ordered_json::array();
    for (const auto& key : ch.mScalingKeys)
        j["ScalingKeys"].push_back(KeysToJson(key));

    j["RotationKeys"] = ordered_json::array();
    for (const auto& key : ch.mRotationKeys)
        j["RotationKeys"].push_back(RotKeysToJson(key));

    j["PositionKeys"] = ordered_json::array();
    for (const auto& key : ch.mPositionKeys)
        j["PositionKeys"].push_back(KeysToJson(key));

    return j;
}

ordered_json NotifyToJson(const ConvNotify& n)
{
    ordered_json j;
    j["Frame"] = n.fFrame;
    j["StartFrame"] = n.fStartFrame;
    j["EndFrame"] = n.fEndFrame;
    j["Active"] = n.bActive;
    j["Attached"] = n.bAttached;
    j["NotifyId"] = n.iNotifyId;
    j["NotifyType"] = static_cast<int>(n.eNotify_Type);
    j["EventType"] = static_cast<int>(n.eNotify_Event);
    j["SocketName"] = n.SocketName;

    ordered_json eventMap = ordered_json::object();
    for (const auto& [key, value] : n.UmapEvent)
        eventMap[key] = VariantToJson(value);

    j["EventData"] = eventMap;
    return j;
}

ordered_json AnimationToJson(const ConvAnimation& anim)
{
    ordered_json j;
    j["Name"] = anim.mName;
    j["Duration"] = anim.mDuration;
    j["TickPerSecond"] = anim.mTickPerSecond;
    j["IsLoopAnim"] = anim.mIsLoopAnim;
    j["LinkedAnim"] = anim.mLinkedAnim;
    j["CompleteAnim"] = anim.mCompleteAnim;
    j["NextAnimIndex"] = anim.mNextAnimIndex;
    j["NextAnimLerpDuration"] = anim.mNextAnimLerpDuration;
    j["NumChannels"] = anim.mNumChannels;

    j["Channels"] = ordered_json::array();
    for (const auto& ch : anim.mChannels)
        j["Channels"].push_back(ChannelToJson(ch));

    j["Notifies"] = ordered_json::array();
    for (const auto& n : anim.mNotifyes)
        j["Notifies"].push_back(NotifyToJson(n));

    return j;
}
/******************************************************* 메모리 -> JSON *******************************************************/


//////////////////////////////////////////////////////// JSON -> 메모리 ////////////////////////////////////////////////////////
VariantValue JsonToVariant(const ordered_json& j)
{
    uint8_t tag = j.value("tag", static_cast<uint8_t>(0));

    switch (tag)
    {
    case 0: return std::monostate{};
    case 1: return j["value"].get<int>();
    case 2: return j["value"].get<unsigned int>();
    case 3: return j["value"].get<float>();
    case 4: return j["value"].get<bool>();
    case 5: return j["value"].get<string>();
    case 6: {
        const auto& arr = j["value"];
        return Float2{ arr[0].get<float>(), arr[1].get<float>() };
    }
    case 7: {
        const auto& arr = j["value"];
        return Float3{ arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>() };
    }
    case 8: {
        const auto& arr = j["value"];
        return Float4{ arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>() };
    }
    }
    return std::monostate{};
}

ConvKeys JsonToKeys(const ordered_json& j)
{
    ConvKeys key;
    key.mTime = j.value("Time", 0.f);
    if (j.contains("Value"))
    {
        const auto& arr = j["Value"];
        key.mValue = Float3{ arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>() };
    }
    return key;
}

ConvRotKeys JsonToRotKeys(const ordered_json& j)
{
    ConvRotKeys key;
    key.mTime = j.value("Time", 0.f);
    if (j.contains("Value"))
    {
        const auto& arr = j["Value"];
        key.mValue = Float4{ arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>() };
    }
    return key;
}

ConvChannel JsonToChannel(const ordered_json& j)
{
    ConvChannel ch;
    ch.mChannelIndex = j.value("ChannelIndex", 0u);
    ch.mNumKeyFrames = j.value("NumKeyFrames", 0u);

    if (j.contains("ScalingKeys"))
        for (const auto& k : j["ScalingKeys"])
            ch.mScalingKeys.push_back(JsonToKeys(k));

    if (j.contains("RotationKeys"))
        for (const auto& k : j["RotationKeys"])
            ch.mRotationKeys.push_back(JsonToRotKeys(k));

    if (j.contains("PositionKeys"))
        for (const auto& k : j["PositionKeys"])
            ch.mPositionKeys.push_back(JsonToKeys(k));

    ch.mNumScalingKeys = static_cast<unsigned int>(ch.mScalingKeys.size());
    ch.mNumRotationKeys = static_cast<unsigned int>(ch.mRotationKeys.size());
    ch.mNumPositionKeys = static_cast<unsigned int>(ch.mPositionKeys.size());

    return ch;
}

ConvNotify JsonToNotify(const ordered_json& j)
{
    ConvNotify n;
    n.fFrame = j.value("Frame", 0.f);
    n.fStartFrame = j.value("StartFrame", 0.f);
    n.fEndFrame = j.value("EndFrame", 0.f);
    n.bActive = j.value("Active", false);
    n.bAttached = j.value("Attached", false);
    n.iNotifyId = j.value("NotifyId", 0u);
    n.eNotify_Type = static_cast<ANIM_NOTIFY_TYPE>(j.value("NotifyType", 0));
    n.eNotify_Event = static_cast<ANIM_EVENT_TYPE>(j.value("EventType", 0));
    n.SocketName = j.value("SocketName", string(""));

    if (j.contains("EventData"))
    {
        for (const auto& [key, val] : j["EventData"].items())
            n.UmapEvent[key] = JsonToVariant(val);
    }

    return n;
}

ConvAnimation JsonToAnimation(const ordered_json& j)
{
    ConvAnimation anim;
    anim.mName = j.value("Name", string(""));
    anim.mDuration = j.value("Duration", 0.f);
    anim.mTickPerSecond = j.value("TickPerSecond", 0.f);
    anim.mIsLoopAnim = j.value("IsLoopAnim", false);
    anim.mLinkedAnim = j.value("LinkedAnim", false);
    anim.mCompleteAnim = j.value("CompleteAnim", false);
    anim.mNextAnimIndex = j.value("NextAnimIndex", -1);
    anim.mNextAnimLerpDuration = j.value("NextAnimLerpDuration", 0.2f);

    if (j.contains("Channels"))
        for (const auto& ch : j["Channels"])
            anim.mChannels.push_back(JsonToChannel(ch));

    anim.mNumChannels = static_cast<unsigned int>(anim.mChannels.size());

    if (j.contains("Notifies"))
        for (const auto& n : j["Notifies"])
            anim.mNotifyes.push_back(JsonToNotify(n));

    return anim;
}
/******************************************************* JSON -> 메모리 *******************************************************/


//////////////////////////////////////////////////////// 모델 바이너리 스킵 ////////////////////////////////////////////////////////
void SkipNode(fstream& file)
{
    unsigned int numChildren = 0;
    file.read(reinterpret_cast<char*>(&numChildren), sizeof(unsigned int));
    for (unsigned int i = 0; i < numChildren; ++i)
        SkipNode(file);

    // parentIndex(int) + transformation(float * 16)
    file.seekg(sizeof(int) + sizeof(float) * 16, ios::cur);

    // name
    int nameLen = 0;
    file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
    file.seekg(nameLen, ios::cur);
}

void SkipBone(fstream& file)
{
    unsigned int boneIndex = 0, numWeights = 0;
    file.read(reinterpret_cast<char*>(&boneIndex), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&numWeights), sizeof(unsigned int));

    int nameLen = 0;
    file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
    file.seekg(nameLen, ios::cur);

    // offsetMatrix(float * 16)
    file.seekg(sizeof(float) * 16, ios::cur);

    // weights: (uint + float) * numWeights
    file.seekg(static_cast<streamoff>(numWeights) * (sizeof(unsigned int) + sizeof(float)), ios::cur);
}

void SkipAnimMesh(fstream& file)
{
    int nameLen = 0;
    file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
    file.seekg(nameLen, ios::cur);

    unsigned int numVerts = 0;
    file.read(reinterpret_cast<char*>(&numVerts), sizeof(unsigned int));

    // uint + Float3 + Float3 = 4 + 12 + 12 = 28
    file.seekg(static_cast<streamoff>(numVerts) * 28, ios::cur);
}

void SkipMesh(fstream& file)
{
    bool hasNormals = false, hasTexcoords1 = false;
    unsigned int numBones = 0, matIndex = 0, numVerts = 0, numIndices = 0, numFaces = 0, numAnimMeshes = 0;

    file.read(reinterpret_cast<char*>(&hasNormals), sizeof(bool));
    file.read(reinterpret_cast<char*>(&hasTexcoords1), sizeof(bool));
    file.read(reinterpret_cast<char*>(&numBones), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&matIndex), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&numVerts), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&numIndices), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&numFaces), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&numAnimMeshes), sizeof(unsigned int));

    int nameLen = 0;
    file.read(reinterpret_cast<char*>(&nameLen), sizeof(int));
    file.seekg(nameLen, ios::cur);

    // Float3 * 4 + Float2 = 12*4 + 8 = 56 per vertex
    file.seekg(static_cast<streamoff>(numVerts) * 56, ios::cur);

    if (hasTexcoords1)
        file.seekg(static_cast<streamoff>(numVerts) * sizeof(Float2), ios::cur);

    // boneIndices
    file.seekg(static_cast<streamoff>(numBones) * sizeof(unsigned int), ios::cur);

    // faces: 3 uint per face
    file.seekg(static_cast<streamoff>(numFaces) * sizeof(unsigned int) * 3, ios::cur);

    for (unsigned int i = 0; i < numBones; ++i)
        SkipBone(file);

    for (unsigned int i = 0; i < numAnimMeshes; ++i)
        SkipAnimMesh(file);
}

void SkipMaterial(fstream& file)
{
    unsigned int numTexture[27] = {};
    file.read(reinterpret_cast<char*>(numTexture), sizeof(unsigned int) * 27);

    for (unsigned int i = 0; i < 27; ++i)
    {
        for (unsigned int j = 0; j < numTexture[i]; ++j)
        {
            unsigned int strLen = 0;
            file.read(reinterpret_cast<char*>(&strLen), sizeof(unsigned int));
            file.seekg(static_cast<streamoff>(strLen) * sizeof(wchar_t), ios::cur);
        }
    }
}

vector<ConvAnimation> ReadAnimationsFromAnimBinary(const string& path)
{
    vector<ConvAnimation> animations;

    fstream file(path, ios::in | ios::binary);
    if (!file.is_open())
    {
        cerr << "[Error] Cannot open: " << path << endl;
        return animations;
    }

    // 1. 매직넘버 확인
    char magic[4] = {};
    file.read(magic, 4);
    if (memcmp(magic, "SIHO", 4) != 0)
    {
        cerr << "[Error] Invalid magic number (expected 'SIHO'): " << path << endl;
        file.close();
        return animations;
    }

    cout << "  Magic: SIHO (OK)" << endl;

    // 2. MYMODEL 헤더 읽기 (mNumMeshes, mNumAllBones, mNumMaterials, mNumAnimations)
    unsigned int numMeshes = 0, numAllBones = 0, numMaterials = 0, numAnimations = 0;
    file.read(reinterpret_cast<char*>(&numMeshes), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&numAllBones), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&numMaterials), sizeof(unsigned int));
    file.read(reinterpret_cast<char*>(&numAnimations), sizeof(unsigned int));

    cout << "  Animations in file: " << numAnimations << endl;

    // 3. 애니메이션 읽기 (Node/Mesh/Material 없이 바로 Animation)
    animations.reserve(numAnimations);
    for (unsigned int i = 0; i < numAnimations; ++i)
    {
        animations.push_back(ReadAnimation(file));
        cout << "  [" << (i + 1) << "/" << numAnimations << "] " << animations.back().mName << endl;
    }

    file.close();

    cout << "[Success] Read " << animations.size() << " animations from: " << path << endl;
    return animations;
}
/******************************************************* 모델 바이너리 스킵 *******************************************************/


//////////////////////////////////////////////////////// Notify 추출 ////////////////////////////////////////////////////////
ordered_json NotifyToReadableJson(const ConvNotify& n)
{
    ordered_json j;

    j["NotifyId"] = n.iNotifyId;
    j["NotifyType"] = (n.eNotify_Type == ANIM_NOTIFY_TYPE::NOTIFY) ? "NOTIFY" : "NOTIFY_STATE";

    const char* eventNames[] = {
        "PLAY_SOUND", "SPAWN_PARTICLE", "ACTIVE_COLLIDER",
        "CAMERA", "SPAWN_TRAIL", "PLAYER_ANIM", "MONSTER_ANIM", "DISSOLVE_FLAG"
    };
    int eventIndex = static_cast<int>(n.eNotify_Event);
    j["EventType"] = (eventIndex >= 0 && eventIndex <= 7) ? eventNames[eventIndex] : "UNKNOWN";

    if (n.eNotify_Type == ANIM_NOTIFY_TYPE::NOTIFY)
    {
        j["Frame"] = n.fFrame;
    }
    else
    {
        j["StartFrame"] = n.fStartFrame;
        j["EndFrame"] = n.fEndFrame;
    }

    if (!n.SocketName.empty())
    {
        j["SocketName"] = n.SocketName;
        j["Attached"] = n.bAttached;
    }

    ordered_json eventData = ordered_json::object();
    for (const auto& [key, value] : n.UmapEvent)
    {
        ordered_json entry;
        uint8_t tag = static_cast<uint8_t>(value.index());
        entry["type"] = tag;

        switch (tag)
        {
        case 0: entry["value"] = nullptr; break;
        case 1: entry["value"] = std::get<int>(value); break;
        case 2: entry["value"] = std::get<unsigned int>(value); break;
        case 3: entry["value"] = std::get<float>(value); break;
        case 4: entry["value"] = std::get<bool>(value); break;
        case 5: entry["value"] = std::get<string>(value); break;
        case 6: { const auto& v = std::get<Float2>(value); entry["value"] = { v.x, v.y }; } break;
        case 7: { const auto& v = std::get<Float3>(value); entry["value"] = { v.x, v.y, v.z }; } break;
        case 8: { const auto& v = std::get<Float4>(value); entry["value"] = { v.x, v.y, v.z, v.w }; } break;
        }

        eventData[key] = entry;
    }
    j["EventData"] = eventData;

    return j;
}

// .sihoani -> .notify.json (Notify만 추출)
void ExtractNotifies(const string& inputPath, const string& outputPath)
{
    auto animations = ReadAnimationsFromAnimBinary(inputPath);
    if (animations.empty())
    {
        cerr << "[Warning] No animations in: " << inputPath << endl;
        return;
    }

    ordered_json root;
    root["SourceFile"] = fs::path(inputPath).filename().string();
    root["Animations"] = ordered_json::array();

    unsigned int totalNotifies = 0;

    for (const auto& anim : animations)
    {
        ordered_json animJson;
        animJson["Name"] = anim.mName;
        animJson["Duration"] = anim.mDuration;
        animJson["TickPerSecond"] = anim.mTickPerSecond;
        animJson["Notifies"] = ordered_json::array();

        for (const auto& n : anim.mNotifyes)
        {
            animJson["Notifies"].push_back(NotifyToReadableJson(n));
            totalNotifies++;
        }

        root["Animations"].push_back(animJson);
    }

    ofstream outFile(outputPath);
    if (!outFile.is_open())
    {
        cerr << "[Error] Cannot create: " << outputPath << endl;
        return;
    }

    outFile << root.dump(2);
    outFile.close();

    cout << "[Done] Notify 추출 완료: " << totalNotifies << "개 (" << animations.size() << "개 애니메이션)" << endl;
}
/******************************************************* Notify 추출 *******************************************************/



//////////////////////////////////////////////////////// Notify 병합 ////////////////////////////////////////////////////////
// 문자열 -> enum 변환
ANIM_NOTIFY_TYPE ParseNotifyType(const string& str)
{
    if (str == "NOTIFY_STATE") return ANIM_NOTIFY_TYPE::NOTIFY_STATE;
    return ANIM_NOTIFY_TYPE::NOTIFY;
}

ANIM_EVENT_TYPE ParseEventType(const string& str)
{
    if (str == "PLAY_SOUND")        return ANIM_EVENT_TYPE::PLAY_SOUND;
    if (str == "SPAWN_PARTICLE")    return ANIM_EVENT_TYPE::SPAWN_PARTICLE;
    if (str == "ACTIVE_COLLIDER")   return ANIM_EVENT_TYPE::ACTIVE_COLLIDER;
    if (str == "CAMERA")            return ANIM_EVENT_TYPE::CAMERA;
    if (str == "SPAWN_TRAIL")       return ANIM_EVENT_TYPE::SPAWN_TRAIL;
    if (str == "PLAYER_ANIM")       return ANIM_EVENT_TYPE::PLAYER_ANIM;
    if (str == "MONSTER_ANIM")      return ANIM_EVENT_TYPE::MONSTER_ANIM;
    if (str == "DISSOLVE_FLAG")     return ANIM_EVENT_TYPE::DISSOLVE_FLAG; // 추가됨 : 디졸브 플래그
    return ANIM_EVENT_TYPE::PLAY_SOUND;
}

// EventData의 값 타입을 추론해서 VariantValue로 변환
VariantValue InferVariantFromJson(const string& key, const ordered_json& val)
{
    if (val.is_null())          return std::monostate{};
    if (val.is_boolean())       return val.get<bool>();
    if (val.is_string())        return val.get<string>();
    if (val.is_array())
    {
        if (val.size() == 2) return Float2{ val[0].get<float>(), val[1].get<float>() };
        if (val.size() == 3) return Float3{ val[0].get<float>(), val[1].get<float>(), val[2].get<float>() };
        if (val.size() == 4) return Float4{ val[0].get<float>(), val[1].get<float>(), val[2].get<float>(), val[3].get<float>() };
    }
    if (val.is_number_float())  return val.get<float>();
    if (val.is_number_integer())
    {
        int v = val.get<int>();
        if (v < 0) return v;
        return static_cast<unsigned int>(v);
    }

    return std::monostate{};
}

ConvNotify ReadableJsonToNotify(const ordered_json& j)
{
    ConvNotify n;

    n.iNotifyId = j.value("NotifyId", 0u);
    n.eNotify_Type = ParseNotifyType(j.value("NotifyType", string("NOTIFY")));
    n.eNotify_Event = ParseEventType(j.value("EventType", string("PLAY_SOUND")));

    if (n.eNotify_Type == ANIM_NOTIFY_TYPE::NOTIFY)
    {
        n.fFrame = j.value("Frame", 0.f);
    }
    else
    {
        n.fStartFrame = j.value("StartFrame", 0.f);
        n.fEndFrame = j.value("EndFrame", 0.f);
    }

    n.SocketName = j.value("SocketName", string(""));
    n.bAttached = j.value("Attached", false);
    n.bActive = false;

    // 수정됨 : type 태그 기반으로 정확한 타입 복원
    if (j.contains("EventData"))
    {
        for (const auto& [key, entry] : j["EventData"].items())
        {
            // type 태그가 있으면 정확한 타입 복원
            if (entry.is_object() && entry.contains("type") && entry.contains("value"))
            {
                uint8_t tag = entry["type"].get<uint8_t>();
                const auto& val = entry["value"];

                switch (tag)
                {
                case 0: n.UmapEvent[key] = std::monostate{}; break;
                case 1: n.UmapEvent[key] = val.get<int>(); break;
                case 2: n.UmapEvent[key] = val.get<unsigned int>(); break;
                case 3: n.UmapEvent[key] = val.get<float>(); break;
                case 4: n.UmapEvent[key] = val.get<bool>(); break;
                case 5: n.UmapEvent[key] = val.get<string>(); break;
                case 6: n.UmapEvent[key] = Float2{ val[0].get<float>(), val[1].get<float>() }; break;
                case 7: n.UmapEvent[key] = Float3{ val[0].get<float>(), val[1].get<float>(), val[2].get<float>() }; break;
                case 8: n.UmapEvent[key] = Float4{ val[0].get<float>(), val[1].get<float>(), val[2].get<float>(), val[3].get<float>() }; break;
                }
            }
        }
    }

    return n;
}

// .notify.json + .sihoani -> .sihoani (Notify 병합)
void MergeNotifies(const string& notifyJsonPath, const string& sihoaniPath)
{
    // 1. 기존 .sihoani 읽기
    auto animations = ReadAnimationsFromAnimBinary(sihoaniPath);
    if (animations.empty())
    {
        cerr << "[Error] No animations in: " << sihoaniPath << endl;
        return;
    }

    // 2. .notify.json 읽기
    ifstream inFile(notifyJsonPath);
    if (!inFile.is_open())
    {
        cerr << "[Error] Cannot open: " << notifyJsonPath << endl;
        return;
    }

    ordered_json root;
    try
    {
        root = ordered_json::parse(inFile);
    }
    catch (const exception& e)
    {
        cerr << "[Error] JSON parse: " << e.what() << endl;
        return;
    }
    inFile.close();

    if (!root.contains("Animations"))
    {
        cerr << "[Error] No 'Animations' field in notify json" << endl;
        return;
    }

    // 3. 이름 기준으로 Notify 매칭 및 교체
    unsigned int mergedCount = 0;
    for (const auto& animJson : root["Animations"])
    {
        string name = animJson.value("Name", string(""));

        // 같은 이름의 애니메이션 찾기
        for (auto& anim : animations)
        {
            if (anim.mName == name)
            {
                // 기존 Notify 제거 후 JSON에서 읽은 것으로 교체
                anim.mNotifyes.clear();

                if (animJson.contains("Notifies"))
                {
                    for (const auto& nj : animJson["Notifies"])
                    {
                        anim.mNotifyes.push_back(ReadableJsonToNotify(nj));
                        mergedCount++;
                    }
                }

                cout << "  [Merged] " << name << " : " << anim.mNotifyes.size() << "개 Notify" << endl;
                break;
            }
        }
    }

    // 4. 백업 생성
    fs::path backupPath = fs::path(sihoaniPath).string() + ".backup";
    if (!fs::exists(backupPath))
    {
        fs::copy_file(sihoaniPath, backupPath);
        cout << "  [Backup] " << backupPath.filename().string() << endl;
    }

    // 5. .sihoani 덮어쓰기
    fstream outFile(sihoaniPath, ios::out | ios::binary);
    if (!outFile.is_open())
    {
        cerr << "[Error] Cannot write: " << sihoaniPath << endl;
        return;
    }

    const char magic[4] = { 'S', 'I', 'H', 'O' };
    outFile.write(magic, 4);

    unsigned int zero = 0;
    unsigned int animCount = static_cast<unsigned int>(animations.size());
    outFile.write(reinterpret_cast<const char*>(&zero), sizeof(unsigned int));
    outFile.write(reinterpret_cast<const char*>(&zero), sizeof(unsigned int));
    outFile.write(reinterpret_cast<const char*>(&zero), sizeof(unsigned int));
    outFile.write(reinterpret_cast<const char*>(&animCount), sizeof(unsigned int));

    for (const auto& anim : animations)
        WriteAnimation(outFile, anim);

    outFile.close();

    cout << "[Done] Notify 병합 완료: " << mergedCount << "개 Notify -> " << fs::path(sihoaniPath).filename().string() << endl;
}
/******************************************************* Notify 병합 *******************************************************/


//////////////////////////////////////////////////////// 메인 함수 ////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "========================================" << endl;
        cout << "  AnimConverter" << endl;
        cout << "========================================" << endl;
        cout << endl;
        cout << "  [드래그 드롭 사용법]" << endl;
        cout << "  .sihoani 파일 1개       -> Notify 추출 (.notify.json 생성)" << endl;
        cout << "  .notify.json 파일 1개   -> 같은 이름의 .sihoani에 Notify 병합" << endl;
        cout << "  .sihoani + .notify.json -> .notify.json의 Notify를 .sihoani에 병합" << endl;
        cout << endl;
        system("pause");
        return 0;
    }

    // 입력 파일 분류
    vector<string> sihoaniFiles;
    vector<string> notifyJsonFiles;

    for (int i = 1; i < argc; ++i)
    {
        fs::path p(argv[i]);
        if (!fs::exists(p))
        {
            cerr << "[Error] File not found: " << p.string() << endl;
            continue;
        }

        string filename = p.filename().string();
        string ext = p.extension().string();

        if (ext == ".sihoani")
        {
            sihoaniFiles.push_back(p.string());
        }
        else if (filename.find(".notify.json") != string::npos)
        {
            notifyJsonFiles.push_back(p.string());
        }
        else if (ext == ".json")
        {
            notifyJsonFiles.push_back(p.string());
        }
        else
        {
            cerr << "[Error] Unsupported: " << filename << endl;
        }
    }

    // Case 1: .sihoani만 있으면 -> Notify 추출
    if (!sihoaniFiles.empty() && notifyJsonFiles.empty())
    {
        for (const auto& path : sihoaniFiles)
        {
            fs::path inputPath(path);
            fs::path outputPath = inputPath.parent_path() / (inputPath.stem().string() + ".notify.json");

            cout << "========================================" << endl;
            cout << "[Notify 추출]" << endl;
            cout << "  입력: " << inputPath.filename().string() << endl;
            cout << "  출력: " << outputPath.filename().string() << endl;
            cout << "  경로: " << fs::absolute(inputPath).string() << endl;

            ExtractNotifies(path, outputPath.string());
        }
    }
    // Case 2: .notify.json만 있으면 -> 같은 이름의 .sihoani 자동 탐색 후 병합
    else if (sihoaniFiles.empty() && !notifyJsonFiles.empty())
    {
        for (const auto& jsonPath : notifyJsonFiles)
        {
            fs::path jp(jsonPath);
            // 00_Idle_N.notify.json -> 00_Idle_N.sihoani
            string stem = jp.stem().string(); // "00_Idle_N.notify"
            size_t pos = stem.find(".notify");
            if (pos != string::npos)
                stem = stem.substr(0, pos);   // "00_Idle_N"

            fs::path sihoaniPath = jp.parent_path() / (stem + ".sihoani");

            cout << "========================================" << endl;
            cout << "[Notify 병합]" << endl;
            cout << "  Notify JSON: " << jp.filename().string() << endl;
            cout << "  대상 Binary: " << sihoaniPath.filename().string() << endl;

            if (!fs::exists(sihoaniPath))
            {
                cerr << "[Error] .sihoani not found: " << sihoaniPath.string() << endl;
                continue;
            }

            MergeNotifies(jsonPath, sihoaniPath.string());
        }
    }
    // Case 3: 둘 다 있으면 -> 직접 지정 병합
    else if (!sihoaniFiles.empty() && !notifyJsonFiles.empty())
    {
        cout << "========================================" << endl;
        cout << "[Notify 병합 - 직접 지정]" << endl;

        // 첫 번째 .notify.json을 첫 번째 .sihoani에 병합
        for (size_t i = 0; i < notifyJsonFiles.size() && i < sihoaniFiles.size(); ++i)
        {
            cout << "  Notify JSON: " << fs::path(notifyJsonFiles[i]).filename().string() << endl;
            cout << "  대상 Binary: " << fs::path(sihoaniFiles[i]).filename().string() << endl;

            MergeNotifies(notifyJsonFiles[i], sihoaniFiles[i]);
        }
    }

    cout << endl;
    system("pause");
    return 0;
}
/******************************************************* 메인 함수 *******************************************************/