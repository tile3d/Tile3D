#pragma once

template <typename T>
struct IsPointer
{
	enum { Value = false };
};

template <typename T> struct IsPointer<               T*> { enum { Value = true }; };
template <typename T> struct IsPointer<const          T*> { enum { Value = true }; };
template <typename T> struct IsPointer<const          T> { enum { Value = IsPointer<T>::Value }; };

