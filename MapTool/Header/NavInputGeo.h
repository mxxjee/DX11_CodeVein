#pragma once
#include "Base.h"
#include "MT_Defines.h"

class CNavInputGeo : public Base
{
public:
    CNavInputGeo();
    ~CNavInputGeo();

    bool LoadGeometry(const vector<DirectX::XMFLOAT3>& vertices, const std::vector<int>& indices);
    
    const float* getMeshVerts() const { return m_verts.data(); }
    const int* getMeshTris() const { return m_tris.data(); }
    int getMeshVertCount() const { return (int)(m_verts.size() / 3); }
    int getMeshTriCount() const { return (int)(m_tris.size() / 3); }

    const float* getMeshBMin() const { return m_bmin; }
    const float* getMeshBMax() const { return m_bmax; }

private:
    vector<float> m_verts;
    vector<int> m_tris;

    float m_bmin[3];
    float m_bmax[3];
};