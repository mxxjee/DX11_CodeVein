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
    inline const _wstring Proto_Com_VISkySphere = L"Prototype_Component_VIBuffer_SkySphere";
    inline const _wstring Proto_Com_VISphere = L"Prototype_Component_VIBuffer_Sphere";
    inline const _wstring Proto_Com_VIParticle = L"Prototype_Component_VIBuffer_Particle";


    inline const _wstring Proto_UIRender = L"Prototype_Component_UI_Render";
    inline const _wstring Proto_UIImage = L"Prototype_Component_UI_Image";
    inline const _wstring Proto_UIButton = L"Prototype_Component_UI_Button";
    inline const _wstring Proto_UIProgress = L"Prototype_Component_UI_Progress";
    inline const _wstring Proto_UIAnimation = L"Prototype_Component_UI_Animation";
    inline const _wstring Proto_UISprite = L"Prototype_Component_UI_Sprite";
    inline const _wstring Proto_UIText = L"Prototype_Component_UI_Text";
    inline const _wstring Proto_UIEventReactor = L"Prototype_Component_UI_EventReactor";
    inline const _wstring Proto_BitmapText = L"Prototype_Component_UI_BitmapFont";
    inline const _wstring Proto_UIWorld = L"Prototype_Component_UI_WorldUI";
    inline const _wstring Proto_UISlotGrid = L"Prototype_Component_UI_SlotGrid";


    inline const _wstring Proto_Com_StateMachine = L"Prototype_Component_StateMachine";
    inline const _wstring Proto_Com_PlayerStat = L"Prototype_Component_PlayerStat";
    inline const _wstring Proto_Com_MonsterStat = L"Prototype_Component_MonsterStat";
    inline const _wstring Proto_Com_WeaponStat = L"Prototype_Component_WeaponStat";

    inline const _wstring Proto_Com_Minimap= L"Prototype_Component_Minimap";

#define Proto_VIBuffer(name) L"Prototype_Component_VIBuffer_" name

    inline const _wstring Proto_Com_Shader_VTXPosTex = L"Prototype_Component_Shader_VTXPosTex";
    inline const _wstring Proto_Com_Shader_VTXPosCorlor = L"Prototype_Component_Shader_VtxPosColor";
    inline const _wstring Proto_Com_Shader_VTXNorTex = L"Prototype_Component_Shader_VTXNorTex";
    inline const _wstring Proto_Com_Shader_VTXMesh = L"Prototype_Component_Shader_VTXMesh";
    inline const _wstring Proto_Com_Shader_VTXCube = L"Prototype_Component_Shader_VTXCube";
    inline const _wstring Proto_Com_Shader_Sky = L"Prototype_Component_Shader_Sky";
    inline const _wstring Proto_Com_Shader_VTXAnimMesh = L"Prototype_Component_Shader_VTXAnimMesh";

    inline const _wstring Proto_Com_Shader_Cell = L"Prototype_Component_Shader_Cell";
    inline const _wstring Proto_Com_Shader_Effect = L"Prototype_Component_Shader_Effect";
    inline const _wstring Proto_Com_Shader_Particle_Rect = L"Prototype_Component_Shader_Particle_Rect";
    inline const _wstring Proto_Com_Shader_Particle_Point = L"Prototype_Component_Shader_Particle_Point";
    inline const _wstring Proto_Com_Shader_Compute_Particle = L"Prototype_Component_Shader_Compute_Particle";
    inline const _wstring Proto_Com_Shader_Compute_Particle_Converge = L"Prototype_Component_Shader_Compute_Particle_Converge";
    inline const _wstring Proto_Com_Shader_Compute_Particle_Dust = L"Prototype_Component_Shader_Compute_Particle_Dust";
    inline const _wstring Proto_Com_Shader_Trail = L"Prototype_Component_Shader_Trail";
    inline const _wstring Proto_Com_Shader_Decal = L"Prototype_Component_Shader_Decal";
    inline const _wstring Proto_Com_Shader_BlobShadow = L"Prototype_Component_Shader_BlobShadow";


#define Proto_Shader(name) L"Prototype_Component_Shader_" name


    inline const _wstring Proto_Com_Navigation = L"Prototype_Component_Navigation";
    inline const _wstring Proto_Com_Collider_Sphere = L"Prototype_Component_Collider_Sphere";
    inline const _wstring Proto_Com_Collider_AABB = L"Prototype_Component_Collider_AABB";
    inline const _wstring Proto_Com_Collider_OBB = L"Prototype_Component_Collider_OBB";
    inline const _wstring Proto_Com_BlobShadow = L"Prototype_Component_BlobShadow";

#pragma endregion Component



#pragma region Model

#define Proto_Model(name) L"Prototype_Component_Model_" name

#pragma endregion Model


}
