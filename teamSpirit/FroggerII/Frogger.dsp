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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

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
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Frogger - Win32 Release"
# Name "Frogger - Win32 Debug"
# Begin Group "Common"

# PROP Default_Filter ""
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

SOURCE=.\Common\menus.c
# End Source File
# Begin Source File

SOURCE=.\Common\menus.h
# End Source File
# Begin Source File

SOURCE=.\Common\objview.c
# End Source File
# Begin Source File

SOURCE=.\Common\objview.h
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

SOURCE=.\Common\singlemodetitle.c
# End Source File
# Begin Source File

SOURCE=.\Common\singlemodetitle.h
# End Source File
# Begin Source File

SOURCE=.\Common\sndview.c
# End Source File
# Begin Source File

SOURCE=.\Common\sndview.h
# End Source File
# Begin Source File

SOURCE=.\Common\Static.c
# End Source File
# Begin Source File

SOURCE=.\Common\Static.h
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

SOURCE=.\Common\Zbuffer.c
# End Source File
# End Group
# Begin Group "N64"

# PROP Default_Filter ""
# End Group
# Begin Group "PC"

# PROP Default_Filter ""
# End Group
# Begin Group "Editor"

# PROP Default_Filter ""
# End Group
# End Target
# End Project
