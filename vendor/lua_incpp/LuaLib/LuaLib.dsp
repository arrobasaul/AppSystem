# Microsoft Developer Studio Project File - Name="LuaLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LuaLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LuaLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LuaLib.mak" CFG="LuaLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LuaLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LuaLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LuaLib - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x1c09 /d "NDEBUG"
# ADD RSC /l 0x1c09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "LuaLib - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x1c09 /d "_DEBUG"
# ADD RSC /l 0x1c09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "LuaLib - Win32 Release"
# Name "LuaLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\lapi.c
# End Source File
# Begin Source File

SOURCE=.\lauxlib.c
# End Source File
# Begin Source File

SOURCE=.\lbaselib.c
# End Source File
# Begin Source File

SOURCE=.\lcode.c
# End Source File
# Begin Source File

SOURCE=.\ldblib.c
# End Source File
# Begin Source File

SOURCE=.\ldebug.c
# End Source File
# Begin Source File

SOURCE=.\ldo.c
# End Source File
# Begin Source File

SOURCE=.\ldump.c
# End Source File
# Begin Source File

SOURCE=.\lfunc.c
# End Source File
# Begin Source File

SOURCE=.\lgc.c
# End Source File
# Begin Source File

SOURCE=.\liolib.c
# End Source File
# Begin Source File

SOURCE=.\llex.c
# End Source File
# Begin Source File

SOURCE=.\lmathlib.c
# End Source File
# Begin Source File

SOURCE=.\lmem.c
# End Source File
# Begin Source File

SOURCE=.\loadlib.c
# End Source File
# Begin Source File

SOURCE=.\lobject.c
# End Source File
# Begin Source File

SOURCE=.\lopcodes.c
# End Source File
# Begin Source File

SOURCE=.\lparser.c
# End Source File
# Begin Source File

SOURCE=.\lstate.c
# End Source File
# Begin Source File

SOURCE=.\lstring.c
# End Source File
# Begin Source File

SOURCE=.\lstrlib.c
# End Source File
# Begin Source File

SOURCE=.\ltable.c
# End Source File
# Begin Source File

SOURCE=.\ltablib.c
# End Source File
# Begin Source File

SOURCE=.\ltests.c
# End Source File
# Begin Source File

SOURCE=.\ltm.c
# End Source File
# Begin Source File

SOURCE=.\lundump.c
# End Source File
# Begin Source File

SOURCE=.\lvm.c
# End Source File
# Begin Source File

SOURCE=.\lzio.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\lapi.h
# End Source File
# Begin Source File

SOURCE=.\lauxlib.h
# End Source File
# Begin Source File

SOURCE=.\lcode.h
# End Source File
# Begin Source File

SOURCE=.\ldebug.h
# End Source File
# Begin Source File

SOURCE=.\ldo.h
# End Source File
# Begin Source File

SOURCE=.\lfunc.h
# End Source File
# Begin Source File

SOURCE=.\lgc.h
# End Source File
# Begin Source File

SOURCE=.\llex.h
# End Source File
# Begin Source File

SOURCE=.\llimits.h
# End Source File
# Begin Source File

SOURCE=.\lmem.h
# End Source File
# Begin Source File

SOURCE=.\lobject.h
# End Source File
# Begin Source File

SOURCE=.\lopcodes.h
# End Source File
# Begin Source File

SOURCE=.\lparser.h
# End Source File
# Begin Source File

SOURCE=.\lstate.h
# End Source File
# Begin Source File

SOURCE=.\lstring.h
# End Source File
# Begin Source File

SOURCE=.\ltable.h
# End Source File
# Begin Source File

SOURCE=.\ltm.h
# End Source File
# Begin Source File

SOURCE=.\lua.h
# End Source File
# Begin Source File

SOURCE=.\luainc.h
# End Source File
# Begin Source File

SOURCE=.\lualib.h
# End Source File
# Begin Source File

SOURCE=.\lundump.h
# End Source File
# Begin Source File

SOURCE=.\lvm.h
# End Source File
# Begin Source File

SOURCE=.\lzio.h
# End Source File
# End Group
# End Target
# End Project
