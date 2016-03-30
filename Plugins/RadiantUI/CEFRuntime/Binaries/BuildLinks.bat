cd /d %~dp0
cd Release
mklink libcef.dll ..\..\..\ThirdParty\CEF\Release\libcef.dll
mklink libEGL.dll ..\..\..\ThirdParty\CEF\Release\libEGL.dll
mklink libGLESv2.dll ..\..\..\ThirdParty\CEF\Release\libGLESv2.dll
mklink d3dcompiler_43.dll ..\..\..\ThirdParty\CEF\Release\d3dcompiler_43.dll
mklink d3dcompiler_47.dll ..\..\..\ThirdParty\CEF\Release\d3dcompiler_47.dll
mklink natives_blob.bin ..\..\..\ThirdParty\CEF\Release\natives_blob.bin
mklink snapshot_blob.bin ..\..\..\ThirdParty\CEF\Release\snapshot_blob.bin
mklink cef.pak ..\..\..\ThirdParty\CEF\Resources\cef.pak
mklink cef_100_percent.pak ..\..\..\ThirdParty\CEF\Resources\cef_100_percent.pak
mklink cef_200_percent.pak ..\..\..\ThirdParty\CEF\Resources\cef_200_percent.pak
mklink cef_extensions.pak ..\..\..\ThirdParty\CEF\Resources\cef_extensions.pak
mklink devtools_resources.pak ..\..\..\ThirdParty\CEF\Resources\devtools_resources.pak
mklink icudtl.dat ..\..\..\ThirdParty\CEF\Resources\icudtl.dat
cd ../Debug
mklink libcef.dll ..\..\..\ThirdParty\CEF\Debug\libcef.dll
mklink libEGL.dll ..\..\..\ThirdParty\CEF\Debug\libEGL.dll
mklink libGLESv2.dll ..\..\..\ThirdParty\CEF\Debug\libGLESv2.dll
mklink d3dcompiler_43.dll ..\..\..\ThirdParty\CEF\Debug\d3dcompiler_43.dll
mklink d3dcompiler_47.dll ..\..\..\ThirdParty\CEF\Debug\d3dcompiler_47.dll
mklink natives_blob.bin ..\..\..\ThirdParty\CEF\Debug\natives_blob.bin
mklink snapshot_blob.bin ..\..\..\ThirdParty\CEF\Debug\snapshot_blob.bin
mklink cef.pak ..\..\..\ThirdParty\CEF\Resources\cef.pak
mklink cef_100_percent.pak ..\..\..\ThirdParty\CEF\Resources\cef_100_percent.pak
mklink cef_200_percent.pak ..\..\..\ThirdParty\CEF\Resources\cef_200_percent.pak
mklink cef_extensions.pak ..\..\..\ThirdParty\CEF\Resources\cef_extensions.pak
mklink devtools_resources.pak ..\..\..\ThirdParty\CEF\Resources\devtools_resources.pak
mklink icudtl.dat ..\..\..\ThirdParty\CEF\Resources\icudtl.dat
pause