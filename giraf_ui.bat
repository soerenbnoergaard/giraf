@echo off

set COMMAND=giraf %1

set /p filename=< %1
echo %filename%

set /p X="x: "
set COMMAND=%COMMAND% -x%X%

:loop
set /p Y="y: " || set "Y="
if "%Y%" neq "" (
    set COMMAND=%COMMAND% -y%Y%
    goto loop
)

set /p "OPTIONS=options: "
set COMMAND=%COMMAND% %OPTIONS%

start %COMMAND%

