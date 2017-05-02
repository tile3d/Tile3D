#pragma once

#include <Math/TVector3.h>
#include <Math/TVector4.h>
#include <Math/TMatrix3.h>
#include <Math/TMatrix4.h>
#include <Math/TQuaternion.h>
#include <Math/TPlane.h>
#include <Math/TMathUtil.h>
#include "CppUnitTest.h"

#define PI 3.1415926535f

using namespace std;

std::wstringstream& operator <<(std::wstringstream& s, const TVector3& t);
std::wstringstream& operator <<(std::wstringstream& s, const TVector4& t);
std::wstringstream& operator <<(std::wstringstream& s, const TMatrix3 & t);
std::wstringstream& operator <<(std::wstringstream& s, const TMatrix4 & t);
std::wstringstream& operator <<(std::wstringstream& s, const TQuaternion & t);
std::wstringstream& operator <<(std::wstringstream& s, const TPlane & t);

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework
		{
			template<> inline std::wstring ToString<TVector3>(const TVector3& t) { RETURN_WIDE_STRING(t); }
			template<> inline std::wstring ToString<TVector4>(const TVector4& t) { RETURN_WIDE_STRING(t); }
			template<> inline std::wstring ToString<TMatrix3>(const TMatrix3 & t) { RETURN_WIDE_STRING(t); }
			template<> inline std::wstring ToString<TMatrix4>(const TMatrix4 & t) { RETURN_WIDE_STRING(t); }
			template<> inline std::wstring ToString<TQuaternion>(const TQuaternion & t) { RETURN_WIDE_STRING(t); }
			template<> inline std::wstring ToString<TPlane>(const TPlane & t) { RETURN_WIDE_STRING(t); }
		}
	}
}

