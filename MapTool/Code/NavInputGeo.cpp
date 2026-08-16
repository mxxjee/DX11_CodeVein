#include "MT_Defines.h"
#include "NavInputGeo.h"

CNavInputGeo::CNavInputGeo()
{
    m_bmin[0] = m_bmin[1] = m_bmin[2] = FLT_MAX;
    m_bmax[0] = m_bmax[1] = m_bmax[2] = -FLT_MAX;
}

CNavInputGeo::~CNavInputGeo()
{
}

bool CNavInputGeo::LoadGeometry(const std::vector<DirectX::XMFLOAT3>& vertices, const std::vector<int>& indices)
{
    if (vertices.empty() || indices.empty())
        return false;

    m_verts.clear();
    m_tris.clear();
    m_bmin[0] = m_bmin[1] = m_bmin[2] = FLT_MAX;
    m_bmax[0] = m_bmax[1] = m_bmax[2] = -FLT_MAX;

    m_verts.reserve(vertices.size() * 3);
    for (const auto& v : vertices)
    {
        m_verts.push_back(v.x);
        m_verts.push_back(v.y);
        m_verts.push_back(v.z);

        m_bmin[0] = min(m_bmin[0], v.x);
        m_bmin[1] = min(m_bmin[1], v.y);
        m_bmin[2] = min(m_bmin[2], v.z);

        m_bmax[0] = max(m_bmax[0], v.x);
        m_bmax[1] = max(m_bmax[1], v.y);
        m_bmax[2] = max(m_bmax[2], v.z);
    }

    m_tris = indices;

    return true;
}