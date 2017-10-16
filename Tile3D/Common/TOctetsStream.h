#pragma once

#include "TOctets.h"
#include "TTypes.h"
#include <Container\TString.h>
#include <Container\TArray.h>

class TOctetsStream
{
public:
	TOctetsStream() {
		m_pos = 0;
		m_transpos = 0;
	}

	~TOctetsStream() {}


public:
	class MarshalException {};

	template<typename T1, typename T2> static inline T1 BinaryCast(const T2 x)  {
		union X1 {
			T1 v1;
			T2 v2;
			X1(const T2 & value) : v2(value) {}
		};
		return X1(x).v1;
	}

	template<typename T> TOctetsStream& Push(T t) {
		m_octets.Push(&t, sizeof(t));
		return *this;
	}

	template<typename T> void Pop(const T & t) const {
		if (m_pos + sizeof(t) > m_octets.Size()) {
			throw MarshalException();
		}

		const_cast<T&>(t) = *(T*)(m_octets.Begin() + m_pos);
		m_pos += sizeof(T);
	}

	unsigned char PopByte8() const {
		unsigned char c;
		Pop(c);
		return c;
	}

	unsigned short PopByte16() const {
		unsigned short s;
		Pop(s);
		return ByteOrder16(s);
	}

	unsigned short PopByte32() const {
		unsigned int i;
		Pop(i);
		return ByteOrder32(i);
	}

	TOctetsStream& CompactUInt32(unsigned int x) {
		if (x < 0x80) {
			return Push((unsigned char)x);
		}
		else if (x < 0x8000) {
			return Push(ByteOrder16(x | 0x8000));
		}
		else if (x < 0x20000000) {
			return Push(ByteOrder32(x | 0xc0000000));
		}
		Push((unsigned char)0xe0);
		return Push(ByteOrder32(x));
	}

	const TOctetsStream& UncompactUInt32(const unsigned int & x) const {
		if (m_pos == m_octets.Size()) {
			throw MarshalException();
		}

		switch (*((unsigned char*)m_octets.Begin() + m_pos) & 0xe0){
			case 0xe0:
			{
				PopByte8();
				const_cast<unsigned int&>(x) = PopByte32();
				return *this;
			}

			case 0xc0:
			{
				const_cast<unsigned int&>(x) = PopByte32() & ~0xc0000000;
				return *this;
			}

			case 0xa0:
			case 0x80:
			{
				const_cast<unsigned int&>(x) = PopByte16() & ~0x8000;
				return *this;
			}

			const_cast<unsigned int&>(x) = PopByte8();
		}
	}

//marshal
public:
	TOctetsStream& operator << (char x) {
		return Push(x);
	}

	TOctetsStream& operator << (unsigned char x) {
		return Push(x);
	}

	TOctetsStream& operator << (bool x) {
		return Push(x);
	}

	TOctetsStream& operator << (short x) {
		return Push(ByteOrder16(x));
	}

	TOctetsStream& operator << (unsigned short x) {
		return Push(ByteOrder16(x));
	}

	TOctetsStream& operator << (int x) {
		return Push(ByteOrder32(x));
	}

	TOctetsStream& operator << (unsigned int x) {
		return Push(ByteOrder32(x));
	}

	TOctetsStream& operator << (int64 x) {
		return Push(ByteOrder64(x));
	}

	TOctetsStream& operator << (uint64 x) {
		return Push(ByteOrder64(x));
	}

	TOctetsStream& operator << (float x) {
		return Push(ByteOrder32(BinaryCast<unsigned int, float>(x)));
	}

	TOctetsStream& operator << (double x) {
		return Push(ByteOrder64(BinaryCast<uint64, double>(x)));
	}

	TOctetsStream& operator << (const TOctets & x) {
		CompactUInt32(x.Size());
		m_octets.Push(x.Begin(), x.Size());
	}

	TOctetsStream& operator << (const TString & x) {
		CompactUInt32(x.GetLength());
		m_octets.Push(x.ToString(), x.GetLength());
	}

	template<typename T>TOctetsStream& operator << (const TArray<T> & x) {
		CompactUInt32(x.Size());
		for (int i = 0; i < x.Size(); ++i) {
			*this << x[i];
		}
		return *this;
	}

//unmarshal
public:
	const TOctetsStream& operator >> (const char & x) const {
		Pop(x);
		return *this;
	}

	const TOctetsStream& operator >> (const unsigned char & x) const {
		Pop(x);
		return *this;;
	}

	const TOctetsStream& operator >> (const bool & x) const {
		Pop(x);
		return *this;
	}

	const TOctetsStream& operator >> (const short & x) const {
		Pop(x);
		const_cast<short&>(x) = ByteOrder16(x);
		return *this;	
	}

	const TOctetsStream& operator >> (const unsigned short & x)const {
		Pop(x);
		const_cast<unsigned short&>(x) = ByteOrder16(x);
		return *this;
	}

	const TOctetsStream& operator >> (const int & x) const {
		Pop(x);
		const_cast<int&>(x) = ByteOrder32(x);
		return *this;
	}

	const TOctetsStream& operator >> (const unsigned int & x) const {
		Pop(x);
		const_cast<unsigned int&>(x) = ByteOrder32(x);
		return *this;
	}

	const TOctetsStream& operator >> (const int64 & x) const {
		Pop(x);
		const_cast<int64&>(x) = ByteOrder64(x);
		return *this;
	}

	const TOctetsStream& operator >> (const uint64 &  x)const  {
		Pop(x);
		const_cast<uint64&>(x) = ByteOrder64(x);
		return *this;
	}

	const TOctetsStream& operator >> (const float & x) const {
		unsigned int v;
		Pop(v);
		v = ByteOrder32(v);
		const_cast<float&>(x) = BinaryCast<float, unsigned int>(v);
		return *this;
	}

	const TOctetsStream& operator >> (const double & x) const {
		unsigned long long v;
		Pop(v);
		v = ByteOrder64(v);
		const_cast<double&>(x) = BinaryCast<double, unsigned long long>(v);
		return *this;
	}

	const TOctetsStream& operator >> (const TOctets & x) const {
		unsigned int len;
		UncompactUInt32(len);
		if (len > m_octets.Size() - m_pos) {
			throw MarshalException();
		}

		const_cast<TOctets&>(x).Push(m_octets.Begin() + m_pos, len);
		m_pos += len;
		return *this;
	}

	const TOctetsStream& operator >> (const TString & x) const  {
		unsigned int len;
		UncompactUInt32(len);
		if (len > m_octets.Size() - m_pos) {
			throw MarshalException();
		}

		const_cast<TString&>(x).Assign(m_octets.Begin() + m_pos, len);
		m_pos += len;
		return *this;
	}

	 template<typename T>TOctetsStream& operator >> (const TArray<T> & x) const {
		unsigned int len;
		UncompactUInt32(len);

		for (int i = 0; i < len; ++i) {
			T t;
			*this >> t;
			x.Add(t);
		}
		return *this;
	}



private:
	short ByteOrder16(short x) const {
	#ifdef USE_LITTLE_ENDIAN
		return x;
	#else
		unsigned char * s = (unsigned char*)&x;
		return (short)(s[0] << 8 | s[1]);
		//return ((x & 0xff00) >> 8) | ((x & 0x00ff) << 8);
	#endif
	}

	int ByteOrder32(int x) const {
	#ifdef USE_LITTLE_ENDIAN
		return x;
	#else
		unsigned char * s = (unsigned char*)&x;
		return (int)(s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3]);
		//return ((x & 0xff000000) >> 24) | ((x & 0x00ff0000) >> 16) | ((x & 0x0000ff00) << 8 | ((x & 0x000000ff) << 24));
	#endif
	}

	int64 ByteOrder64(int64 x) const {
	#ifdef USE_LITTLE_ENDIAN
		return x;
	#else
		union
		{
			int64 lvalue;
			int ivalue[2];
		}in, out;

		in.lvalue = x;
		out.ivalue[0] = ByteOrder32(in.ivalue[1]);
		out.ivalue[1] = ByteOrder32(in.ivalue[0]);
		return out.lvalue;
	#endif
	}

private:
	TOctets m_octets;
	mutable unsigned int m_pos;
	unsigned int m_transpos;
};
