set VER="HH4-8.01.001"%VER%

echo ////////////////DOC////////////////
cd .\docs
call "GENERATE DOC.bat"
cd..

echo ////////////////COPY////////////////
Rmdir .\releasetmp /s /Q
mkdir .\releasetmp
del *.sdf /s /Q

xcopy .\modules\*.* .\releasetmp\modules\ /s
xcopy .\examples\*.* .\releasetmp\examples\ /s
xcopy .\sdk\*.* .\releasetmp\sdk\ /s
xcopy .\docs\final\*.* .\releasetmp\docs\ /s
del .\releasetmp\docs\doxygen\doxygen.exe
del .\releasetmp\docs\doxygen\libclang.dll
del .\releasetmp\docs\doxygen\doxyfile
copy "Release Notes.txt" .\releasetmp\
copy "Set Path example.bat" .\releasetmp\
copy .\docs\*.pdf .\releasetmp\
cd .\releasetmp\
del *.ipch /s /Q
del *.VC.db /s /Q
del *.usr-win64 /s /Q
del *.usr-win32 /s /Q
del *.pdb /s /Q
cd ..



echo ////////////////ZIP////////////////

cd releasetmp
set path="C:\Program Files (x86)\WinRAR\"; %path%
winrar a -afzip -r  "Sensomusic Usine SDK %VER%".zip *.*
del ..\releases\"Sensomusic Usine SDK "%VER%.zip 
copy "Sensomusic Usine SDK "%VER%.zip ..\releases\


echo ////////////////CLEAR////////////////
cd..
Rmdir .\releasetmp /s /Q

pause
