# Microsoft Developer Studio Project File - Name="Blank Library" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Blank Library - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Blank Library.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Blank Library.mak" CFG="Blank Library - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Blank Library - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Blank Library - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/teamSpirit/MDX Library", TLNBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Blank Library - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Blank_Library___Win32_Release"
# PROP BASE Intermediate_Dir "Blank_Library___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Blank_Library___Win32_Release"
# PROP Intermediate_Dir "Blank_Library___Win32_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Blank Library - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Blank_Library___Win32_Debug"
# PROP BASE Intermediate_Dir "Blank_Library___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Blank_Library___Win32_Debug"
# PROP Intermediate_Dir "Blank_Library___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W1 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Blank_Library___Win32_Debug\mdx.lib"

!ENDIF 

# Begin Target

# Name "Blank Library - Win32 Release"
# Name "Blank Library - Win32 Debug"
# Begin Source File

SOURCE=.\mdx.h
# End Source File
# Begin Source File

SOURCE=.\mdxActor.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxActor.h
# End Source File
# Begin Source File

SOURCE=.\mdxCRC.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxCRC.h
# End Source File
# Begin Source File

SOURCE=.\mdxD3D.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxD3D.h
# End Source File
# Begin Source File

SOURCE=.\mdxDDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxDDraw.h
# End Source File
# Begin Source File

SOURCE=.\mdxDrivers.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxDText.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxDText.h
# End Source File
# Begin Source File

SOURCE=.\mdxFont.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxFont.h
# End Source File
# Begin Source File

SOURCE=.\mdxInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxInfo.h
# End Source File
# Begin Source File

SOURCE=.\mdxLandscape.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxLandscape.h
# End Source File
# Begin Source File

SOURCE=.\mdxMath.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxMath.h
# End Source File
# Begin Source File

SOURCE=.\mdxObject.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxObject.h
# End Source File
# Begin Source File

SOURCE=.\mdxPoly.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxPoly.h
# End Source File
# Begin Source File

SOURCE=.\mdxProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxProfile.h
# End Source File
# Begin Source File

SOURCE=.\mdxRender.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxRender.h
# End Source File
# Begin Source File

SOURCE=.\mdxTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxTexture.h
# End Source File
# Begin Source File

SOURCE=.\mdxTiming.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxTiming.h
# End Source File
# Begin Source File

SOURCE=.\mdxWindows.cpp
# End Source File
# Begin Source File

SOURCE=.\mdxWindows.h
# End Source File
# Begin Source File

SOURCE=.\mgeReport.cpp
# End Source File
# Begin Source File

SOURCE=.\mgeReport.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Target
# End Project
