set OUTPUT_DIR=..\..\..\ModulesDistribRealease\
set LEAP_LIB_DIR=..\..\libs\LeapSDK\lib\
set OUTPUT_FILE_PATH=..\..\..\ModulesDistribRealease_%date:~6,4%_%date:~3,2%_%date:~0,2%.zip

Rem make output folder-------------------------------------------
rmdir %OUTPUT_DIR% /s /Q
mkdir %OUTPUT_DIR%
mkdir %OUTPUT_DIR%Modules\
mkdir %OUTPUT_DIR%Bin\
mkdir %OUTPUT_DIR%Bin\osx-32\
mkdir %OUTPUT_DIR%Bin\win-32\
mkdir %OUTPUT_DIR%Bin\win-64\

Rem copy modules and devices-------------------------------------------
xcopy ..\..\bin\*.* %OUTPUT_DIR%Modules\ /s

Rem copy leap motion dependancies-------------------------------------------
 xcopy %LEAP_LIB_DIR%osx\libLeap.dylib %OUTPUT_DIR%Bin\osx-32\ 
 xcopy %LEAP_LIB_DIR%x86\Leap.dll      %OUTPUT_DIR%Bin\win-32\ 
 xcopy %LEAP_LIB_DIR%x86\msvcp100.dll  %OUTPUT_DIR%Bin\win-32\ 
 xcopy %LEAP_LIB_DIR%x86\msvcr100.dll  %OUTPUT_DIR%Bin\win-32\ 
 xcopy %LEAP_LIB_DIR%x64\Leap.dll      %OUTPUT_DIR%Bin\win-64\ 
 xcopy %LEAP_LIB_DIR%x64\msvcp100.dll  %OUTPUT_DIR%Bin\win-64\ 
 xcopy %LEAP_LIB_DIR%x64\msvcr100.dll  %OUTPUT_DIR%Bin\win-64\ 

Rem zip output folder-------------------------------------------
7z a -tzip %OUTPUT_FILE_PATH% %OUTPUT_DIR%*

 
