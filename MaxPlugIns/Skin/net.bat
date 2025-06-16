@echo off

REM Update Skin PlugIn Into Nextwork

set _U_NETWORK_=x:\general\skinplugin
set _U_MAXPATH_=c:\maxsdk\exe
REM set _U_MAXPATH_=c:\3dsmax2.5

if exist %_U_MAXPATH_%\3dsmax.exe goto update

:error
@echo no update - cannot find Max directory
@exit

REM Update files

:update
copy %_U_NETWORK_%\skin.dlm %_U_NETWORK_%\skin.bak
:delplug
del %_U_NETWORK_%\skin.dlm
if exist %_U_NETWORK_%\skin.dlm goto delplug
copy %_U_MAXPATH_%\plugins\skin.dlm %_U_NETWORK_%\skin.dlm

@echo skin plugin updated
@exit
