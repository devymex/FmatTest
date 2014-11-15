#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

// Set Win32 SDK version
#include <SDKDDKVer.h>

// some CString constructors will be explicit
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

// MFC core and standard components
#include <afxwin.h>

// MFC extensions
#include <afxext.h>

// MFC support for Window Common Dialogs
#include <afxdialogex.h>

// MFC support for Windows Common Controls
#include <afxcmn.h>

// MFC support for ribbons and control bars
//#include <afxcontrolbars.h>

// MFC Automation classes
//#include <afxdisp.h>

// MFC support for Internet Explorer 4 Common Controls
//#include <afxdtctl.h>

//#include <windows.h>
//#include <Shlwapi.h>
//#include <shellapi.h>

// Opencv support
#include <opencv2\opencv.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#define _USE_MATH_DEFINES
#include <cassert>
#include <cmath>
