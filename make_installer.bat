:: set MSVC environment
set "current_path=%cd%"
call C:\Qt\6.5.3\msvc2019_64\bin\qtenv2.bat
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
cd %current_path%

:: create working directories
mkdir release
cd release
C:\Qt\6.5.3\msvc2019_64\bin\qmake.exe %current_path%\shell-rc.pro -spec win32-msvc "CONFIG+=qtquickcompiler"
C:\Qt\Tools\QtCreator\bin\jom\jom.exe qmake_all
C:\Qt\Tools\QtCreator\bin\jom\jom.exe

:: copy created files
cd ..\
xcopy /s release\shell-rc.exe installer\packages\com.csabigee.shell_rc\data\ /Y
xcopy /s qjoysticks\lib\SDL\bin\windows\msvc\x64\SDL2.dll installer\packages\com.csabigee.shell_rc\data\ /Y
xcopy /s qjoysticks\lib\SDL\bin\windows\msvc\x64\SDL2.lib installer\packages\com.csabigee.shell_rc\data\ /Y

C:\Qt\6.5.3\msvc2019_64\bin\windeployqt --opengl --printsupport installer\packages\com.csabigee.shell_rc\data\shell-rc.exe

cd installer

:: create installer
C:\Qt\Tools\QtInstallerFramework\4.6\bin\binarycreator.exe -c config\config.xml -p packages %current_path%\shell_rc.exe

echo All done!
pause
cd ..\..
rmdir release /s /q
