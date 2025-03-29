@echo off
echo Copying DLL...
copy /Y libs\your-library.dll bin\Debug\

echo Running Ray Tracer...
bin\Debug\ray-tracer.exe

pause