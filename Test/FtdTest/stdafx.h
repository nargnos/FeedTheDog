// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
//#define CRTDBG
#ifdef CRTDBG
// boost里面用的malloc跟crt的宏定义冲突，如果用_CRTDBG_MAP_ALLOC将编译错误
//#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>
#endif
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <Common.h>
