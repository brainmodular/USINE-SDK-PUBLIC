Rmdir .\html /s /Q
cd Doxygen
doxygen doxyfile
cd ..
Rmdir .\final /s /Q
mkdir .\final
mkdir .\final\html\
xcopy .\html\*.* .\final\html /s
mkdir .\final\Doxygen
xcopy .\Doxygen\*.* .\final\Doxygen /s
Rmdir .\html /s /Q
copy .\Doxygen\index.html .\final\ 
del .\final\Doxygen\doxygen.exe
del .\final\Doxygen\doxyfile
del .\final\Doxygen\libclang.dll


rem pause


