@echo off
echo Compilation du projet NetFlow...

cl /Fe:netflow.exe ^
   src\graphe.c ^
   src\liste_chainee.c ^
   src\dijkstra.c ^
   src\backtracking.c ^
   src\securite.c ^
   src\utils.c ^
   src\main.c ^
   /I src

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Compilation reussie!
    echo Lancez le programme avec: netflow.exe
) else (
    echo.
    echo Erreur de compilation!
)

pause
