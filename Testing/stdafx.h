// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

// TODO: reference additional headers your program requires here
#include <stdafx.h>

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#define TEST_TRAIT(traitValue) TEST_METHOD_ATTRIBUTE(L"Trait", traitValue)
