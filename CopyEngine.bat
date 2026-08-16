// 명령어			옵션			원본 파일이 있는 위치			사본 파일을 저장할 위치

xcopy			/y			.\Engine\Bin\*.dll		.\Client\Bin\
xcopy			/y			.\Engine\Bin\*.dll		.\ShaderTool\Bin\
xcopy			/y			.\Engine\Bin\*.dll		.\MapTool\Bin\
xcopy			/y			.\Engine\Bin\*.dll		.\UITool\Bin\
xcopy			/y			.\Engine\Bin\*.dll		.\VFXTool\Bin\
xcopy			/y			.\Engine\Bin\*.dll		.\AnimationTool\Bin\

xcopy			/y			.\Engine\Bin\Engine.lib		.\EngineSDK\Library\
robocopy		".\Engine\ThirdPartyLib" ".\EngineSDK\Library" *.lib /XD "PhysX" /R:0 /W:0
robocopy		".\Engine\Public" ".\EngineSDK\Include" *.* /S /XD "PhysX" /R:0 /W:0

xcopy			/y			.\Engine\Shader\*.*		.\Client\Shader\
xcopy			/y			.\Engine\Shader\*.*		.\Shader\
xcopy			/y			.\Engine\Shader\*.*		.\UITool\Shader\
xcopy			/y			.\Engine\Shader\*.*		.\VFXTool\Shader\
xcopy			/y			.\Engine\Shader\*.*		.\AnimationTool\Shader\