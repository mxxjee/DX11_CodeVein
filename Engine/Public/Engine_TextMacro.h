#pragma once

namespace Engine
{
#pragma region Components
    inline const wstring Com_Transform = L"Transform_Component";
    inline const wstring Com_VIBuffer = L"VI_Buffer_Component";
    inline const wstring Com_Shader = L"Shader_Component";
    inline const wstring Com_Compute = L"ComputeShader_Component";
    inline const wstring Com_Shader_Picking = L"Shader_Picking_Component";
    inline const wstring Com_BlobShadow = L"BlobShadow_Component";
    inline const wstring Com_Model = L"Model_Component";
    inline const wstring Com_Navigation = L"Navigation_Component";
    inline const wstring Com_Track = L"Navigation_Track";
    inline const wstring Com_StateMachine = L"StateMachine_Component";
    inline const wstring Com_Minimap = L"Minimap_Component";
    inline const wstring Com_Stat = L"Stat_Component";

    inline const wstring Com_Texture = L"Texture_Component";
    inline const wstring Com_Texture0 = L"Texture_Component0";
    inline const wstring Com_Texture1 = L"Texture_Component1";
    inline const wstring Com_Texture2 = L"Texture_Component2";
    inline const wstring Com_Texture3 = L"Texture_Component3";
    inline const wstring Com_Texture4 = L"Texture_Component4";
    inline const wstring Com_Texture5 = L"Texture_Component5";
    inline const wstring Com_Texture6 = L"Texture_Component6";
    inline const wstring Com_Texture7 = L"Texture_Component7";

    inline const wstring Com_NewTexture = L"NewTexture_Component";
    inline const wstring Com_NewTexture0 = L"NewTexture_Component0";
    inline const wstring Com_NewTexture1 = L"NewTexture_Component1";
    inline const wstring Com_NewTexture2 = L"NewTexture_Component2";
    inline const wstring Com_NewTexture3 = L"NewTexture_Component3";
    inline const wstring Com_NewTexture4 = L"NewTexture_Component4";
    inline const wstring Com_NewTexture5 = L"NewTexture_Component5";

    inline const wstring Com_Collider = L"Collider_Component";
    inline const wstring Com_Collider0 = L"Collider_Component0";
    inline const wstring Com_Collider1 = L"Collider_Component1";
    inline const wstring Com_Collider2 = L"Collider_Component2";
    inline const wstring Com_Collider3 = L"Collider_Component3";
    inline const wstring Com_Collider4 = L"Collider_Component4";
#pragma endregion


#pragma region RenderState
    inline const string RS_Default = "RS_Default";
    inline const string RS_SolidCW = "RS_SolidCW";
    inline const string RS_SolidCCW = "RS_SolidCCW";
    inline const string RS_SolidNone = "RS_SolidNone";
    inline const string RS_SolidScissor = "RS_SolidScissor";
    inline const string RS_WireframeCW = "RS_WireframeCW";
    inline const string RS_WireframeCCW = "RS_WireframeCCW";
    inline const string RS_WireframeNone = "RS_WireframeNone";
    inline const string RS_Shadow = "RS_Shadow";


    inline const string DSS_Default = "DSS_Default";
    inline const string DSS_Sky = "DSS_Sky";
    inline const string DSS_NoDepth = "DSS_NoDepth";


    inline const string BS_Default = "BS_Default";
    inline const string BS_AlphaBlend = "BS_AlphaBlend";
    inline const string BS_Additive = "BS_Additive";
#pragma endregion


#pragma region MultiRenderTarget
    inline const wstring MRT_GAMEOBJECT = L"MRT_GameObject";
    inline const wstring MRT_LIGHTACC = L"MRT_LightAcc";
    inline const wstring MRT_SHADOW = L"MRT_Shadow";
    inline const wstring MRT_HDR = L"MRT_HDR";
    inline const wstring MRT_HDR_BRIGHT = L"MRT_HDR_Bright";
    inline const wstring MRT_HDR_DOWNSAMPLE = L"MRT_HDR_DownSample";
    inline const wstring MRT_HDR_BLURHORIZONTAL = L"MRT_HDR_BlurHorizontal";
    inline const wstring MRT_HDR_BLURVERTICAL = L"MRT_HDR_BlurVertical";
#pragma endregion



#pragma region ConstBufferName
    // Camera (b0)
    inline const wstring CB_VIEWMATRIX         = L"g_ViewMatrix";
    inline const wstring CB_PROJMATRIX         = L"g_ProjMatrix";
    inline const wstring CB_INVERSE_VIEWMATRIX = L"g_InverseViewMatrix";
    inline const wstring CB_INVERSE_PROJMATRIX = L"g_InverseProjMatrix";
    inline const wstring CB_CAMERAPOSITION     = L"g_vCamPosition";

    // Object (b1)
    inline const wstring CB_WORLDMATRIX        = L"g_WorldMatrix";
    inline const wstring CB_OBJECTCOLOR        = L"g_vColor";
    inline const wstring CB_OBJECT_ID          = L"g_ObjectID";

    // Lights (b2)
    inline const wstring CB_LIGHT_RANGE        = L"g_fLightRange";
    inline const wstring CB_LIGHT_DIRECTION    = L"g_vLightDirection";
    inline const wstring CB_LIGHT_POSITION     = L"g_vLightPosition";
    inline const wstring CB_LIGHT_DIFFUSE      = L"g_vLightDiffuse";
    inline const wstring CB_LIGHT_AMBIENT      = L"g_vLightAmbient";
    inline const wstring CB_LIGHT_SPECULAR     = L"g_vLightSpecular";

    // Material (b3)
    inline const wstring CB_MTRL_AMBIENT       = L"g_vMtrlAmbient";
    inline const wstring CB_MTRL_SPECULAR      = L"g_vMtrlSpecular";

    // Bones (b4)
    inline const wstring CB_BONES_MATRICES     = L"g_BonesMatrices";

    // MorphBuffer (b5)
    inline const wstring CB_MORPH_WEIGHTS_PACKED = L"g_MorphWeightsPacked";

    // MorphIndices (b6)
    inline const wstring CB_MORPH_INDICES_PACKED = L"g_MorphIndicesPacked";

    // MorphInfos (b7)
    inline const wstring CB_NUM_ACTIVE_MORPHS  = L"g_NumActiveMorphs";
    inline const wstring CB_NUM_VERTICES       = L"g_NumVertices";
#pragma endregion





}
