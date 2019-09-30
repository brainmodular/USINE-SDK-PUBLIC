set LOCAL_DIR=..\..\
set REPO_SOURCES_DIR="D:\Mes Documents\Usine\Dev\Sourceforge repo\trunk"
set OUTPUT_FILE_PATH=%LOCAL_DIR%"Sensomusic SDK Distrib "%date:~6,4%_%date:~3,2%_%date:~0,2%.zip

Rem make output folder-------------------------------------------

Rem zip output folder-------------------------------------------
7z a -tzip %OUTPUT_FILE_PATH% %REPO_SOURCES_DIR%*

 
