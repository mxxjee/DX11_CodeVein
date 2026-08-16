#pragma once
#include "MT_Defines.h"

using namespace DirectX::SimpleMath;

#ifdef _DEBUG
#pragma comment(lib, "DirectXTKD.lib")
#else
#pragma comment(lib, "DirectXTK.lib")
#endif

class SplinePath {
public:
    SplinePath();
    ~SplinePath();

    void AddPoint(SplineNode node);
    void Clear();

    Vector3 GetPositionOnSpline(_float t);
    void Save(std::ofstream& outFile);
    void Load(std::ifstream& inFile);
    void Render(PrimitiveBatch<VertexPositionColor>* batch);

    //호출용 함수들.
    size_t GetPointCount() const { return m_Nodes.size(); }
    Vector3 GetTangent(_float t);

    _float GetTotalLength();
    _float GetTFromDistance(_float distance);

    void SetIsLoop(_bool bLoop);

    void RemovePoint(_uint index);
    void InsertPoint(_uint index, SplineNode node);
    void UpdatePoint(_uint index, SplineNode Newnode);

    void SetPointType(int index, POINT_TYPE type);
    void SetPointRadius(int index, _float radius);

    const vector<SplineNode>& Get_Nodes() const { return m_Nodes; }

private:
    void DrawCube(PrimitiveBatch<VertexPositionColor>* batch, Vector3 center, _float size, XMVECTOR color);
    void DrawRing(PrimitiveBatch<VertexPositionColor>* batch, Vector3 center, _float radius, XMVECTOR color);

private:
    bool m_isLoop = false;
    vector<SplineNode> m_Nodes;

};

