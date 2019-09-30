set LOCAL_DIR=..\..\
set OUTPUT_DIR="D:\Mes Documents\Usine\Dev\Sourceforge repo\trunk\"
set TAGS_DIR=tags\

Rem make output folder-------------------------------------------
rmdir %OUTPUT_DIR% /s /Q
mkdir %OUTPUT_DIR%
mkdir %OUTPUT_DIR%bin\
mkdir %OUTPUT_DIR%"bin\SDK Examples\"
mkdir %OUTPUT_DIR%examples\
mkdir %OUTPUT_DIR%libs\
mkdir %OUTPUT_DIR%libs\Juce_v3\
mkdir %OUTPUT_DIR%libs\FTDI\
mkdir %OUTPUT_DIR%libs\hidapi\
mkdir %OUTPUT_DIR%libs\wiiuse\
mkdir %OUTPUT_DIR%modules\
mkdir %OUTPUT_DIR%sdk\

Rem copy examples bin sub folder-------------------------------------------
:: xcopy %LOCAL_DIR%"bin\SDK Examples\"*.* %OUTPUT_DIR%"bin\SDK Examples\" /s

Rem copy examples folder-------------------------------------------
xcopy %LOCAL_DIR%examples\*.* %OUTPUT_DIR%examples\ /s

Rem copy libs sub folder-------------------------------------------
xcopy %LOCAL_DIR%libs\Juce_v3\*.* %OUTPUT_DIR%libs\Juce_v3\ /s
xcopy %LOCAL_DIR%libs\FTDI\*.* %OUTPUT_DIR%libs\FTDI\ /s
xcopy %LOCAL_DIR%libs\hidapi\*.* %OUTPUT_DIR%libs\hidapi\ /s
xcopy %LOCAL_DIR%libs\wiiuse\*.* %OUTPUT_DIR%libs\wiiuse\ /s

Rem copy modules folder-------------------------------------------
xcopy %LOCAL_DIR%modules\*.* %OUTPUT_DIR%modules\ /s

Rem copy sdk folder-------------------------------------------
xcopy %LOCAL_DIR%sdk\*.* %OUTPUT_DIR%sdk\ /s


Rem files -------------------------------------------
xcopy %LOCAL_DIR%Licence.txt %OUTPUT_DIR%*
xcopy %LOCAL_DIR%"Release Notes.txt" %OUTPUT_DIR%*
 
