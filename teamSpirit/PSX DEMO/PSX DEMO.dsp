# Microsoft Developer Studio Project File - Name="PSX DEMO" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=PSX DEMO - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PSX DEMO.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PSX DEMO.mak" CFG="PSX DEMO - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PSX DEMO - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "PSX DEMO - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/teamSpirit/PSX DEMO", XHCBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PSX DEMO - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "PSX DEMO - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PSX DEMO - Win32 Release"
# Name "PSX DEMO - Win32 Debug"
# Begin Group "Psx Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Psx\audio.c
# End Source File
# Begin Source File

SOURCE=.\Psx\audio.h
# End Source File
# Begin Source File

SOURCE=.\Psx\BackDrop.c
# End Source File
# Begin Source File

SOURCE=.\Psx\BackDrop.h
# End Source File
# Begin Source File

SOURCE=.\Psx\bbpsiani.c
# End Source File
# Begin Source File

SOURCE=.\Psx\bbpsiani.h
# End Source File
# Begin Source File

SOURCE=.\Psx\bff_load.c
# End Source File
# Begin Source File

SOURCE=.\Psx\bff_load.h
# End Source File
# Begin Source File

SOURCE=.\Psx\CR_lang.c
# End Source File
# Begin Source File

SOURCE=.\Psx\CR_lang.h
# End Source File
# Begin Source File

SOURCE=.\Psx\custom.c
# End Source File
# Begin Source File

SOURCE=.\Psx\custom2.c
# End Source File
# Begin Source File

SOURCE=.\Psx\Eff_Draw.c
# End Source File
# Begin Source File

SOURCE=.\Psx\Eff_Draw.h
# End Source File
# Begin Source File

SOURCE=.\Psx\fadeout.c
# End Source File
# Begin Source File

SOURCE=.\Psx\fadeout.h
# End Source File
# Begin Source File

SOURCE=.\Psx\file.c
# End Source File
# Begin Source File

SOURCE=.\Psx\file.h
# End Source File
# Begin Source File

SOURCE=.\Psx\font.c
# End Source File
# Begin Source File

SOURCE=.\Psx\font.h
# End Source File
# Begin Source File

SOURCE=.\Psx\Frogger2.lnk
# End Source File
# Begin Source File

SOURCE=.\Psx\gametext.h
# End Source File
# Begin Source File

SOURCE=.\Psx\islfile.c
# End Source File
# Begin Source File

SOURCE=.\Psx\islfile.h
# End Source File
# Begin Source File

SOURCE=.\Psx\islmem.c
# End Source File
# Begin Source File

SOURCE=.\Psx\islmem.h
# End Source File
# Begin Source File

SOURCE=.\Psx\islpsi.h
# End Source File
# Begin Source File

SOURCE=.\Psx\isltex.c
# End Source File
# Begin Source File

SOURCE=.\Psx\isltex.h
# End Source File
# Begin Source File

SOURCE=.\Psx\islvideo.c
# End Source File
# Begin Source File

SOURCE=.\Psx\islvideo.h
# End Source File
# Begin Source File

SOURCE=.\Psx\LoadingBar.c
# End Source File
# Begin Source File

SOURCE=.\Psx\LoadingBar.h
# End Source File
# Begin Source File

SOURCE=.\Psx\Main.c
# End Source File
# Begin Source File

SOURCE=.\Psx\Main.h
# End Source File
# Begin Source File

SOURCE=.\Psx\Makefile.mak
# End Source File
# Begin Source File

SOURCE=.\Psx\map_draw.c
# End Source File
# Begin Source File

SOURCE=.\Psx\map_draw.h
# End Source File
# Begin Source File

SOURCE=.\Psx\memcard.c
# End Source File
# Begin Source File

SOURCE=.\Psx\memcard.h
# End Source File
# Begin Source File

SOURCE=.\Psx\Objects.c
# End Source File
# Begin Source File

SOURCE=.\Psx\Objects.h
# End Source File
# Begin Source File

SOURCE=.\Psx\ObjVewer.h
# End Source File
# Begin Source File

SOURCE=.\Psx\ObjViewer.c
# End Source File
# Begin Source File

SOURCE=.\Psx\ObjViewer.h
# End Source File
# Begin Source File

SOURCE=.\Psx\psi.c
# End Source File
# Begin Source File

SOURCE=.\Psx\psi.h
# End Source File
# Begin Source File

SOURCE=.\Psx\psiactor.c
# End Source File
# Begin Source File

SOURCE=.\Psx\psiactor.h
# End Source File
# Begin Source File

SOURCE=.\Psx\psitypes.h
# End Source File
# Begin Source File

SOURCE=.\Psx\psxfont.h
# End Source File
# Begin Source File

SOURCE=.\Psx\psxsprite.c
# End Source File
# Begin Source File

SOURCE=.\Psx\psxsprite.h
# End Source File
# Begin Source File

SOURCE=.\Psx\psxtongue.c
# End Source File
# Begin Source File

SOURCE=.\Psx\psxtongue.h
# End Source File
# Begin Source File

SOURCE=.\Psx\pTexture.c
# End Source File
# Begin Source File

SOURCE=.\Psx\pTexture.h
# End Source File
# Begin Source File

SOURCE=.\Psx\quatern.c
# End Source File
# Begin Source File

SOURCE=.\Psx\quatern.h
# End Source File
# Begin Source File

SOURCE=.\Psx\shell.h
# End Source File
# Begin Source File

SOURCE=.\Psx\snapshot.c
# End Source File
# Begin Source File

SOURCE=.\Psx\snapshot.h
# End Source File
# Begin Source File

SOURCE=.\Psx\temp_psx.c
# End Source File
# Begin Source File

SOURCE=.\Psx\temp_psx.h
# End Source File
# Begin Source File

SOURCE=.\Psx\Textures.c
# End Source File
# Begin Source File

SOURCE=.\Psx\Textures.h
# End Source File
# Begin Source File

SOURCE=.\Psx\timer.c
# End Source File
# Begin Source File

SOURCE=.\Psx\timer.h
# End Source File
# Begin Source File

SOURCE=.\Psx\water.c
# End Source File
# Begin Source File

SOURCE=.\Psx\water.h
# End Source File
# Begin Source File

SOURCE=.\Psx\world.c
# End Source File
# Begin Source File

SOURCE=.\Psx\world.h
# End Source File
# Begin Source File

SOURCE=.\Psx\World_Eff.c
# End Source File
# Begin Source File

SOURCE=.\Psx\World_Eff.h
# End Source File
# End Group
# Begin Group "Common Code"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Common\actor2.c
# End Source File
# Begin Source File

SOURCE=.\Common\actor2.h
# End Source File
# Begin Source File

SOURCE=.\Common\anim.c
# End Source File
# Begin Source File

SOURCE=.\Common\anim.h
# End Source File
# Begin Source File

SOURCE=.\Common\babyfrog.c
# End Source File
# Begin Source File

SOURCE=.\Common\babyfrog.h
# End Source File
# Begin Source File

SOURCE=.\Common\bbtimer.c
# End Source File
# Begin Source File

SOURCE=.\Common\bbtimer.h
# End Source File
# Begin Source File

SOURCE=.\Common\block.c
# End Source File
# Begin Source File

SOURCE=.\Common\block.h
# End Source File
# Begin Source File

SOURCE=.\Common\cam.c
# End Source File
# Begin Source File

SOURCE=.\Common\Cam.h
# End Source File
# Begin Source File

SOURCE=.\Common\charsel.c
# End Source File
# Begin Source File

SOURCE=.\Common\charsel.h
# End Source File
# Begin Source File

SOURCE=.\Common\codes.h
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

SOURCE=.\Common\decomp.h
# End Source File
# Begin Source File

SOURCE=.\Common\DefData.c
# End Source File
# Begin Source File

SOURCE=.\Common\DefData.h
# End Source File
# Begin Source File

SOURCE=.\Common\define.h
# End Source File
# Begin Source File

SOURCE=.\Common\E3_Demo.c
# End Source File
# Begin Source File

SOURCE=.\Common\E3_Demo.h
# End Source File
# Begin Source File

SOURCE=.\Common\enemies.c
# End Source File
# Begin Source File

SOURCE=.\Common\enemies.h
# End Source File
# Begin Source File

SOURCE=.\Common\evenfunc.c
# End Source File
# Begin Source File

SOURCE=.\Common\evenfunc.h
# End Source File
# Begin Source File

SOURCE=.\Common\event.c
# End Source File
# Begin Source File

SOURCE=.\Common\event.h
# End Source File
# Begin Source File

SOURCE=.\Common\fixed.c
# End Source File
# Begin Source File

SOURCE=.\Common\fixed.h
# End Source File
# Begin Source File

SOURCE=.\Common\flatpack.c
# End Source File
# Begin Source File

SOURCE=.\Common\flatpack.h
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

SOURCE=.\Common\lang.h
# End Source File
# Begin Source File

SOURCE=.\Common\language.c
# End Source File
# Begin Source File

SOURCE=.\Common\language.h
# End Source File
# Begin Source File

SOURCE=.\Common\layout.c
# End Source File
# Begin Source File

SOURCE=.\Common\layout.h
# End Source File
# Begin Source File

SOURCE=.\Common\levplay.c
# End Source File
# Begin Source File

SOURCE=.\Common\levplay.h
# End Source File
# Begin Source File

SOURCE=.\Common\LevProg.c
# End Source File
# Begin Source File

SOURCE=.\Common\LevProg.h
# End Source File
# Begin Source File

SOURCE=.\Common\map.c
# End Source File
# Begin Source File

SOURCE=.\Common\map.h
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

SOURCE=.\Common\multi.c
# End Source File
# Begin Source File

SOURCE=.\Common\multi.h
# End Source File
# Begin Source File

SOURCE=.\Common\newpsx.c
# End Source File
# Begin Source File

SOURCE=.\Common\newpsx.h
# End Source File
# Begin Source File

SOURCE=.\Common\options.c
# End Source File
# Begin Source File

SOURCE=.\Common\options.h
# End Source File
# Begin Source File

SOURCE=.\Common\overlays.c
# End Source File
# Begin Source File

SOURCE=.\Common\overlays.h
# End Source File
# Begin Source File

SOURCE=.\Common\particle.c
# End Source File
# Begin Source File

SOURCE=.\Common\particle.h
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

SOURCE=.\Common\ptexture.c
# End Source File
# Begin Source File

SOURCE=.\Common\ptexture.h
# End Source File
# Begin Source File

SOURCE=.\Common\savegame.c
# End Source File
# Begin Source File

SOURCE=.\Common\savegame.h
# End Source File
# Begin Source File

SOURCE=.\Common\script.c
# End Source File
# Begin Source File

SOURCE=.\Common\script.h
# End Source File
# Begin Source File

SOURCE=.\Common\sndview.c
# End Source File
# Begin Source File

SOURCE=.\Common\sndview.h
# End Source File
# Begin Source File

SOURCE=.\Common\sonylibs.h
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

SOURCE=.\Common\story.c
# End Source File
# Begin Source File

SOURCE=.\Common\story.h
# End Source File
# Begin Source File

SOURCE=.\Common\textover.c
# End Source File
# Begin Source File

SOURCE=.\Common\textover.h
# End Source File
# Begin Source File

SOURCE=.\Common\tongue.c
# End Source File
# Begin Source File

SOURCE=.\Common\tongue.h
# End Source File
# Begin Source File

SOURCE=.\Common\training.c
# End Source File
# Begin Source File

SOURCE=.\Common\training.h
# End Source File
# Begin Source File

SOURCE=.\Common\types.c
# End Source File
# Begin Source File

SOURCE=.\Common\types.h
# End Source File
# Begin Source File

SOURCE=.\Common\types2d.h
# End Source File
# Begin Source File

SOURCE=.\Common\ultra64.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Psx\sonylibs.h
# End Source File
# End Target
# End Project
