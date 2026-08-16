#include "MT_Defines.h"
#include "SplinePath.h"

SplinePath::SplinePath()
    : m_isLoop(false) 
{
}

SplinePath::~SplinePath() {
    Clear();
}

// 점 추가
void SplinePath::AddPoint(SplineNode node) {
    m_Nodes.push_back(node);
}

// 모든 점 삭제
void SplinePath::Clear() {
    m_Nodes.clear();
}

// 스플라인 상의 위치 구하기
Vector3 SplinePath::GetPositionOnSpline(_float t) {
    if (m_Nodes.empty()) return Vector3::Zero;
    if (m_Nodes.size() == 1) return m_Nodes[0].vPos;

    _uint numPoints = (_uint)m_Nodes.size();

    // 루프가 아닌데 t가 범위를 넘으면 끝점 반환
    if (!m_isLoop && t >= (_float)numPoints - 1) return m_Nodes.back().vPos;
    if (t < 0) return m_Nodes.front().vPos;

    // 인덱스 계산
    _uint p1 = (_uint)t;
    _uint p0 = p1 - 1;
    _uint p2 = p1 + 1;
    _uint p3 = p1 + 2;

    _float localT = t - (_float)p1;

    if (m_isLoop) {
        p0 = (p0 % numPoints + numPoints) % numPoints;
        p1 = (p1 % numPoints + numPoints) % numPoints;
        p2 = (p2 % numPoints + numPoints) % numPoints;
        p3 = (p3 % numPoints + numPoints) % numPoints;
    }
    else {
        p0 = max(0, min(p0, numPoints - 1));
        p1 = max(0, min(p1, numPoints - 1));
        p2 = max(0, min(p2, numPoints - 1));
        p3 = max(0, min(p3, numPoints - 1));
    }

    return Vector3::CatmullRom(m_Nodes[p0].vPos, m_Nodes[p1].vPos, m_Nodes[p2].vPos, m_Nodes[p3].vPos, localT);
}

Vector3 SplinePath::GetTangent(_float t) {
    const _float epsilon = 0.01f;

    Vector3 prev = GetPositionOnSpline(t - epsilon);
    Vector3 next = GetPositionOnSpline(t + epsilon);

    Vector3 tangent = next - prev;
    tangent.Normalize();

    return tangent;
}

_float SplinePath::GetTotalLength() {
    if (m_Nodes.size() < 2) return 0.0f;

    _float length = 0.0f;
    _float step = 0.1f;

    _float maxT = m_isLoop ? (_float)m_Nodes.size() : (_float)(m_Nodes.size() - 1);

    Vector3 prevPos = GetPositionOnSpline(0.0f);

    for (_float t = step; t <= maxT; t += step) {
        Vector3 currPos = GetPositionOnSpline(t);
        length += Vector3::Distance(prevPos, currPos);
        prevPos = currPos;
    }

    Vector3 finalPos = GetPositionOnSpline(maxT);
    length += Vector3::Distance(prevPos, finalPos);

    return length;
}

_float SplinePath::GetTFromDistance(_float distance) {
    if (distance <= 0.0f) return 0.0f;
    if (m_Nodes.size() < 2) return 0.0f;

    _float accumulatedDist = 0.0f;
    _float step = 0.05f; // 탐색 정밀도

    _float maxT = m_isLoop ? (_float)m_Nodes.size() : (_float)(m_Nodes.size() - 1);
    Vector3 prevPos = GetPositionOnSpline(0.0f);

    for (_float t = step; t <= maxT; t += step) {
        Vector3 currPos = GetPositionOnSpline(t);
        _float segmentDist = Vector3::Distance(prevPos, currPos);

        // 목표 거리를 초과했으면, 해당 구간 내에서 보간하여 정확한 t 반환
        if (accumulatedDist + segmentDist >= distance) {
            _float overflow = distance - accumulatedDist;
            _float ratio = overflow / segmentDist;
            return (t - step) + (ratio * step);
        }

        accumulatedDist += segmentDist;
        prevPos = currPos;
    }

    return maxT; // 거리가 전체 길이보다 길면 끝점 t 반환
}


void SplinePath::SetIsLoop(_bool bLoop) {
    m_isLoop = bLoop;
}

// 점 삭제
void SplinePath::RemovePoint(_uint index) {
    if (index >= 0 && index < m_Nodes.size()) {
        m_Nodes.erase(m_Nodes.begin() + index);
    }
}

// 점 삽입
void SplinePath::InsertPoint(_uint index, SplineNode node) {
    if (index >= 0 && index <= m_Nodes.size()) {
        m_Nodes.insert(m_Nodes.begin() + index, node);
    }
}

// 점 위치 수정
void SplinePath::UpdatePoint(_uint index, SplineNode newnode) {
    if (index >= 0 && index < m_Nodes.size()) {
        m_Nodes[index] = newnode;
    }
}

// 타입 설정
void SplinePath::SetPointType(int index, POINT_TYPE type) {
    if (index >= 0 && index < m_Nodes.size()) {
        m_Nodes[index].eType = type;
    }
}

// 반경 설정
void SplinePath::SetPointRadius(int index, _float radius) {
    if (index >= 0 && index < m_Nodes.size()) {
        m_Nodes[index].fRadius = radius;
    }
}

void SplinePath::Save(std::ofstream& outFile)
{
    outFile.write(reinterpret_cast<const char*>(&m_isLoop), sizeof(bool));

    size_t iCount = m_Nodes.size();
    outFile.write(reinterpret_cast<const char*>(&iCount), sizeof(size_t));

    if (iCount > 0)
    {
        outFile.write(reinterpret_cast<const char*>(m_Nodes.data()), iCount * sizeof(SplineNode));
    }
}

void SplinePath::Load(std::ifstream& inFile)
{
    Clear();

    inFile.read(reinterpret_cast<char*>(&m_isLoop), sizeof(bool));

    size_t iCount = 0;
    inFile.read(reinterpret_cast<char*>(&iCount), sizeof(size_t));

    if (iCount > 0)
    {
        m_Nodes.resize(iCount);
        inFile.read(reinterpret_cast<char*>(m_Nodes.data()), iCount * sizeof(SplineNode));
    }
}

void SplinePath::Render(PrimitiveBatch<VertexPositionColor>* batch) {
    if (m_Nodes.empty()) return;

    for (const auto& p : m_Nodes) {
        XMVECTOR color = Colors::Yellow; // 기본 길색상

        // 공터
        if (p.eType == POINT_TYPE::ZONE) {
            color = Colors::Red;

            DrawRing(batch, p.vPos, p.fRadius, Colors::Orange);
        }

        // 중심점크기
        DrawCube(batch, p.vPos, 0.8f, color);
    }

    if (m_Nodes.size() < 2) return;

    // 스플라인 경로(녹색)
    _float step = 0.05f;
    _float maxT = m_isLoop ? (_float)m_Nodes.size() : (_float)(m_Nodes.size() - 1);

    Vector3 prevPos = GetPositionOnSpline(0.0f);

    for (_float t = step; t <= maxT; t += step) {
        Vector3 currentPos = GetPositionOnSpline(t);
        batch->DrawLine(VertexPositionColor(prevPos, Colors::Green),
            VertexPositionColor(currentPos, Colors::Green));
        prevPos = currentPos;
    }

    if (m_isLoop) {
        batch->DrawLine(VertexPositionColor(prevPos, Colors::Green),
            VertexPositionColor(GetPositionOnSpline(0.0f), Colors::Green));
    }
}

void SplinePath::DrawCube(PrimitiveBatch<VertexPositionColor>* batch, Vector3 center, _float size, XMVECTOR color) {
    _float h = size * 0.5f;
    batch->DrawLine(VertexPositionColor(center - Vector3(h, 0, 0), color), VertexPositionColor(center + Vector3(h, 0, 0), color));
    batch->DrawLine(VertexPositionColor(center - Vector3(0, h, 0), color), VertexPositionColor(center + Vector3(0, h, 0), color));
    batch->DrawLine(VertexPositionColor(center - Vector3(0, 0, h), color), VertexPositionColor(center + Vector3(0, 0, h), color));
}

void SplinePath::DrawRing(PrimitiveBatch<VertexPositionColor>* batch, Vector3 center, _float radius, XMVECTOR color)
{
    const int segments = 32;
    _float angleStep = XM_2PI / (_float)segments;

    for (int i = 0; i < segments; ++i) {
        _float a1 = i * angleStep;
        _float a2 = (i + 1) * angleStep;

        Vector3 p1 = center + Vector3(cos(a1) * radius, 0.f, sin(a1) * radius);
        Vector3 p2 = center + Vector3(cos(a2) * radius, 0.f, sin(a2) * radius);

        batch->DrawLine(VertexPositionColor(p1, color), VertexPositionColor(p2, color));
    }
}
