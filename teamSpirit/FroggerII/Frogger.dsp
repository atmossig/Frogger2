# Microsoft Developer Studio Project File - Name="Frogger" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Frogger - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Frogger.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Frogger.mak" CFG="Frogger - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Frogger - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Frogger - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""$/teamSpirit/FroggerII", DJPAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Frogger - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /w /W0 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "PC_VERSION" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG" /d "PC_VERSION"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 gelf.lib winmm.lib dxguid.lib ddraw.lib dinput.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Frogger - Win32 Release"
# Name "Frogger - Win32 Debug"
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Common\actor.c
# End Source File
# Begin Source File

SOURCE=.\Common\actor.h
# End Source File
# Begin Source File

SOURCE=.\Common\anim.c
# End Source File
# Begin Source File

SOURCE=.\Common\anim.h
# End Source File
# Begin Source File

SOURCE=.\Common\babyfrogs.c
# End Source File
# Begin Source File

SOURCE=.\Common\babyfrogs.h
# End Source File
# Begin Source File

SOURCE=.\Common\backdrops.c
# End Source File
# Begin Source File

SOURCE=.\Common\backdrops.h
# End Source File
# Begin Source File

SOURCE=.\Common\cam.c
# End Source File
# Begin Source File

SOURCE=.\Common\Cam.h
# End Source File
# Begin Source File

SOURCE=.\Common\cameos.c
# End Source File
# Begin Source File

SOURCE=.\Common\cameos.h
# End Source File
# Begin Source File

SOURCE=.\Common\collect.c
# End Source File
# Begin Source File

SOURCE=.\Common\collect.h
# End Source File
# Begin Source File

SOURCE=.\Common\crc32.c
# End Source File
# Begin Source File

SOURCE=.\Common\crc32.h
# End Source File
# Begin Source File

SOURCE=.\Common\decomp.c
# End Source File
# Begin Source File

SOURCE=.\Common\decomp.h
# End Source File
# Begin Source File

SOURCE=.\Common\DefaultData.c
# End Source File
# Begin Source File

SOURCE=.\Common\DefaultData.h
# End Source File
# Begin Source File

SOURCE=.\Common\define.h
# End Source File
# Begin Source File

SOURCE=.\Common\enemies.c
# End Source File
# Begin Source File

SOURCE=.\Common\enemies.h
# End Source File
# Begin Source File

SOURCE=.\Common\event.c
# End Source File
# Begin Source File

SOURCE=.\Common\event.h
# End Source File
# Begin Source File

SOURCE=.\Common\eventfuncs.c
# End Source File
# Begin Source File

SOURCE=.\Common\eventfuncs.h
# End Source File
# Begin Source File

SOURCE=.\Common\frogger.c
# End Source File
# Begin Source File

SOURCE=.\Common\frogger.h
# End Source File
# Begin Source File

SOURCE=.\Common\frogmove.c
# End Source File
# Begin Source File

SOURCE=.\Common\frogmove.h
# End Source File
# Begin Source File

SOURCE=.\Common\frontend.c
# End Source File
# Begin Source File

SOURCE=.\Common\frontend.h
# End Source File
# Begin Source File

SOURCE=.\Common\game.c
# End Source File
# Begin Source File

SOURCE=.\Common\game.h
# End Source File
# Begin Source File

SOURCE=.\Common\general.c
# End Source File
# Begin Source File

SOURCE=.\Common\general.h
# End Source File
# Begin Source File

SOURCE=.\Common\hud.c
# End Source File
# Begin Source File

SOURCE=.\Common\hud.h
# End Source File
# Begin Source File

SOURCE=.\Common\incs.h
# End Source File
# Begin Source File

SOURCE=.\Common\Jalloc.c
# End Source File
# Begin Source File

SOURCE=.\Common\jalloc.h
# End Source File
# Begin Source File

SOURCE=.\Common\language.c
# End Source File
# Begin Source File

SOURCE=.\Common\language.h
# End Source File
# Begin Source File

SOURCE=.\Common\layoutvisual.c
# End Source File
# Begin Source File

SOURCE=.\Common\layoutvisual.h
# End Source File
# Begin Source File

SOURCE=.\Common\levelplaying.c
# End Source File
# Begin Source File

SOURCE=.\Common\levelplaying.h
# End Source File
# Begin Source File

SOURCE=.\Common\LevelProgession.h
# End Source File
# Begin Source File

SOURCE=.\Common\LevelProgression.c
# End Source File
# Begin Source File

SOURCE=.\Common\maths.c
# End Source File
# Begin Source File

SOURCE=.\Common\maths.h
# End Source File
# Begin Source File

SOURCE=.\Common\memload.c
# End Source File
# Begin Source File

SOURCE=.\Common\memload.h
# End Source File
# Begin Source File

SOURCE=.\Common\menus.c
# End Source File
# Begin Source File

SOURCE=.\Common\menus.h
# End Source File
# Begin Source File

SOURCE=.\Common\overlays.c
# End Source File
# Begin Source File

SOURCE=.\Common\overlays.h
# End Source File
# Begin Source File

SOURCE=.\Common\path.c
# End Source File
# Begin Source File

SOURCE=.\Common\path.h
# End Source File
# Begin Source File

SOURCE=.\Common\platform.c
# End Source File
# Begin Source File

SOURCE=.\Common\platform.h
# End Source File
# Begin Source File

SOURCE=.\Common\sndview.c
# End Source File
# Begin Source File

SOURCE=.\Common\sndview.h
# End Source File
# Begin Source File

SOURCE=.\Common\specfx.c
# End Source File
# Begin Source File

SOURCE=.\Common\specfx.h
# End Source File
# Begin Source File

SOURCE=.\Common\sprite.c
# End Source File
# Begin Source File

SOURCE=.\Common\sprite.h
# End Source File
# Begin Source File

SOURCE=.\Common\textoverlays.c
# End Source File
# Begin Source File

SOURCE=.\Common\textoverlays.h
# End Source File
# Begin Source File

SOURCE=.\Common\title.c
# End Source File
# Begin Source File

SOURCE=.\Common\title.h
# End Source File
# Begin Source File

SOURCE=.\Common\tongue.c
# End Source File
# Begin Source File

SOURCE=.\Common\tongue.h
# End Source File
# Begin Source File

SOURCE=.\Common\types.h
# End Source File
# Begin Source File

SOURCE=.\Common\types2dgfx.h
# End Source File
# Begin Source File

SOURCE=.\Common\water.c
# End Source File
# Begin Source File

SOURCE=.\Common\water.h
# End Source File
# End Group
# Begin Group "N64"

# PROP Default_Filter ""
# Begin Group "Non-source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\N64\Block.lnk

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\makefile

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\N64\3DText.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\3DText.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\audio.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\audio.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Block.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Block.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Cfb.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Controll.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Controll.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\develop.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\develop.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\font.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\font.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\libmus.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\libmus_data.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\map.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\map.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\mbuffer.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\mbuffer.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\music.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Nnsched.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Nnsched.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\objects.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\objects.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\objview.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\objview.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\printgfx.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\printgfx.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Rdp_out0.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Rdp_outp.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\savegame.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\savegame.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Sfx.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Sfx.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Static.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Static.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Subcube.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Subcube.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\texture.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\texture.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\timerbar.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\timerbar.h

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\N64\Zbuffer.c

!IF  "$(CFG)" == "Frogger - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "PC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Pc\audio.c
# End Source File
# Begin Source File

SOURCE=.\Pc\audio.h
# End Source File
# Begin Source File

SOURCE=.\Pc\block.c
# End Source File
# Begin Source File

SOURCE=.\Pc\block.h
# End Source File
# Begin Source File

SOURCE=.\Pc\Controll.cpp
# End Source File
# Begin Source File

SOURCE=.\Pc\Controll.h
# End Source File
# Begin Source File

SOURCE=.\Pc\directx.cpp
# End Source File
# Begin Source File

SOURCE=.\Pc\directx.h
# End Source File
# Begin Source File

SOURCE=.\Pc\font.c
# End Source File
# Begin Source File

SOURCE=.\Pc\font.h
# End Source File
# Begin Source File

SOURCE=.\Pc\map.c
# End Source File
# Begin Source File

SOURCE=.\Pc\map.h
# End Source File
# Begin Source File

SOURCE=.\Pc\objects.c
# End Source File
# Begin Source File

SOURCE=.\Pc\objects.h
# End Source File
# Begin Source File

SOURCE=.\Pc\pcmisc.cpp
# End Source File
# Begin Source File

SOURCE=.\Pc\pcmisc.h
# End Source File
# Begin Source File

SOURCE=.\Pc\printgfx.c
# End Source File
# Begin Source File

SOURCE=.\Pc\printgfx.h
# End Source File
# Begin Source File

SOURCE=.\Pc\savegame.c
# End Source File
# Begin Source File

SOURCE=.\Pc\savegame.h
# End Source File
# Begin Source File

SOURCE=.\Pc\Subcube.c
# End Source File
# Begin Source File

SOURCE=.\Pc\Subcube.h
# End Source File
# Begin Source File

SOURCE=.\Pc\texture.c
# End Source File
# Begin Source File

SOURCE=.\Pc\texture.h
# End Source File
# End Group
# Begin Group "Editor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Editor\edcommand.c
# End Source File
# Begin Source File

SOURCE=.\Editor\edcommand.h
# End Source File
# Begin Source File

SOURCE=.\Editor\editdefs.c
# End Source File
# Begin Source File

SOURCE=.\Editor\editdefs.h
# End Source File
# Begin Source File

SOURCE=.\Editor\editfile.c
# End Source File
# Begin Source File

SOURCE=.\Editor\editfile.h
# End Source File
# Begin Source File

SOURCE=.\Editor\editor.c
# End Source File
# Begin Source File

SOURCE=.\Editor\editor.h
# End Source File
# Begin Source File

SOURCE=.\Editor\edittypes.h
# End Source File
# Begin Source File

SOURCE=.\Editor\toolbars.c
# End Source File
# Begin Source File

SOURCE=.\Editor\toolbars.h
# End Source File
# End Group
# End Target
# End Project
