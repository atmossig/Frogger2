@echo off

REM Update Skin PlugIn Into Max Directory

set _U_NETWORK_=x:\general\skinplugin

REM Find Max directory

set _U_MAXPATH_=
if exist c:\3dsmax2.5\3dsmax.exe set _U_MAXPATH_=c:\3dsmax2.5
if exist c:\PROGRA~1\3dsmax2.5\3dsmax.exe set _U_MAXPATH_=c:\progra~1\3dsmax2.5
if exist c:\3dsmax2\3dsmax.exe set _U_MAXPATH_=c:\3dsmax2
if exist c:\PROGRA~1\3dsmax2\3dsmax.exe set _U_MAXPATH_=c:\progra~1\3dsmax2

if exist %_U_MAXPATH_%\3dsmax.exe goto update

:error
@echo no update - cannot find Max directory
@exit

REM Update files

:update
copy %_U_MAXPATH_%\plugins\skin.dlm %_U_MAXPATH_%\plugins\skin.bak
:delplug
@echo need to exit max to update skin plugin
del %_U_MAXPATH_%\plugins\skin.dlm
if exist %_U_MAXPATH_%\plugins\skin.dlm goto delplug
copy %_U_NETWORK_%\skin.dlm %_U_MAXPATH_%\plugins\skin.dlm

@echo skin plugin updated
@exit
