#pragma once


namespace ProtoName
{
#pragma region Component
    //inline constexpr wstring_view Proto_Com;
    //constexpr _wstring Proto_Component;
    inline const _wstring Proto_Com_VIRect = L"Prototype_Component_VIBuffer_Rect";
    inline const _wstring Proto_Com_VITerrain = L"Prototype_Component_VIBuffer_Terrain";
    inline const _wstring Proto_Com_VITerrain_Flat = L"Prototype_Component_VIBuffer_Terrain_Flat";
    inline const _wstring Proto_Com_VITerrain_Cell = L"Prototype_Component_VIBuffer_Terrain_Cell";
    inline const _wstring Proto_Com_VICube = L"Prototype_Component_VIBuffer_Cube";
    inline const _wstring Proto_Com_VISkyDome = L"Prototype_Component_VIBuffer_SkyDome";
    inline const _wstring Proto_Com_VIParticle = L"Prototype_Component_VIBuffer_Particle";
    inline const _wstring Proto_UIRender = L"Prototype_Component_UI_Render";
    inline const _wstring Proto_UIImage = L"Prototype_Component_UI_Image";


#define Proto_VIBuffer(name) L"Prototype_Component_VIBuffer_" name

    inline const _wstring Proto_Com_Shader_VTXPosTex = L"Prototype_Component_Shader_VTXPosTex";
    inline const _wstring Proto_Com_Shader_VTXNorTex = L"Prototype_Component_Shader_VTXNorTex";
    inline const _wstring Proto_Com_Shader_VTXMesh = L"Prototype_Component_Shader_VTXMesh";
    inline const _wstring Proto_Com_Shader_VTXCube = L"Prototype_Component_Shader_VTXCube";
    inline const _wstring Proto_Com_Shader_VTXAnimMesh = L"Prototype_Component_Shader_VTXAnimMesh";
    inline const _wstring Proto_Com_Shader_VTXPlayerAnimMesh = L"Prototype_Component_Shader_VTXPlayerAnimMesh";
    inline const _wstring Proto_Com_Shader_Cell = L"Prototype_Component_Shader_Cell";
    inline const _wstring Proto_Com_Shader_Effect = L"Prototype_Component_Shader_Effect";
    inline const _wstring Proto_Com_Shader_Particle_Rect = L"Prototype_Component_Shader_Particle_Rect";
    inline const _wstring Proto_Com_Shader_Particle_Point = L"Prototype_Component_Shader_Particle_Point";
    inline const _wstring Proto_Com_Shader_BlobShadow = L"Prototype_Component_Shader_BlobShadow";


#define Proto_Shader(name) L"Prototype_Component_Shader_" name


    inline const _wstring Proto_Com_Navigation = L"Prototype_Component_Navigation";
    inline const _wstring Proto_Com_Collider_Sphere = L"Prototype_Component_Collider_Sphere";
    inline const _wstring Proto_Com_Collider_AABB = L"Prototype_Component_Collider_AABB";
    inline const _wstring Proto_Com_Collider_OBB = L"Prototype_Component_Collider_OBB";
    inline const _wstring Proto_Com_BlobShadow = L"Prototype_Component_BlobShadow";

#pragma endregion Component



#pragma region Model
    inline const _wstring Proto_Com_Model_Wonder_Acute = L"Prototype_Component_Model_Wonder_Acute";
    inline const _wstring Proto_Com_Model_PlayerMasterBone = L"Prototype_Component_Model_PlayerMasterBone";
    inline const _wstring Proto_Com_Model_PlayerBody_Base = L"Prototype_Component_Model_PlayerBody_Base";
    inline const _wstring Proto_Com_Model_PlayerHair_Base = L"Prototype_Component_Model_PlayerHair_Base";
    inline const _wstring Proto_Com_Model_PlayerHand_Base = L"Prototype_Component_Model_PlayerHand_Base";
    inline const _wstring Proto_Com_Model_PlayerHead_Base = L"Prototype_Component_Model_PlayerHead_Base";
    inline const _wstring Proto_Com_Model_PlayerPants_Base = L"Prototype_Component_Model_PlayerPants_Base";


#define Proto_Model(name) L"Prototype_Component_Model_" name

#pragma endregion Model


}
