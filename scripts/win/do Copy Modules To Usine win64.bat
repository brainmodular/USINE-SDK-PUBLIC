xcopy /e /i /y "..\..\bin\All Modules\*.*" "%USINE_6_64_HOME%Modules\All Modules\" /s
del "%USINE_6_64_HOME%Modules\All Modules\*.usr-osx32" /s /Q
del "%USINE_6_64_HOME%Modules\All Modules\*.usr-win32" /s /Q