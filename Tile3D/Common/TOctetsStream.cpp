#include "TOctetsStream.h"

TOctetsStream& TOctetsStream::CompactUInt32(unsigned int x) {
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


const TOctetsStream& TOctetsStream::UncompactUInt32(const unsigned int & x) const {
	if (m_pos == m_octets.Size()) {
		throw MarshalException();
	}

	switch (*((unsigned char*)m_octets.Begin() + m_pos) & 0xe0)
	{
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
	return *this;
}

TOctetsStream& TOctetsStream::CompactSInt32(int x)
{
	if (x >= 0) {
		if (x < 0x40) {
			return Push((unsigned char)x);
		}
		else if (x < 0x2000) {
			return Push(ByteOrder16(x | 0x8000));
		}
		else if (x < 0x10000000) {
			return Push(ByteOrder32(x | 0xc0000000));
		}

		Push((unsigned char)0xe0);
		return Push(ByteOrder32(x));
	}
	else if (x < 0) {
		x = -x;
		if (x < 0x40) {
			return Push((unsigned char)x | 0x40);
		}
		else if (x < 0x2000) {
			return Push(ByteOrder16(x | 0xa000));
		}
		else if (x < 0x10000000) {
			return Push(ByteOrder32(x | 0xd0000000));
		}
		Push((unsigned char)0xf0);
		return Push(ByteOrder32(x));
	}
	Push((unsigned char)0xf0);
	return Push(ByteOrder32(x));
}


const TOctetsStream& TOctetsStream::UncompactSInt32(const int & x) const
{
	if (m_pos == m_octets.Size()) {
		throw MarshalException();
	}

	switch (*((unsigned char*)m_octets.Begin() + m_pos) & 0xf0)
	{
		case 0xf0:
		{
			PopByte8();
			const_cast<int&>(x) = 0 - PopByte32();
			return *this;
		}

		case 0xe0:
		{
			PopByte8();
			const_cast<int&>(x) = PopByte32();
			return *this;
		}

		case 0xd0:
		{
			const_cast<int&>(x) = 0 - (PopByte32() & ~0xd0000000);
			return *this;
		}

		case 0xc0:
		{
			const_cast<int&>(x) = PopByte32() & ~0xc0000000;
			return *this;
		}

		case 0xb0:
		case 0xa0:
		{
			const_cast<int&>(x) = -(PopByte16() & ~0xa000);
			return *this;
		}

		case 0x90:
		case 0x80:
		{
			const_cast<int&>(x) = PopByte16() & ~0x8000;
			return *this;
		}

		case 0x70:
		case 0x60:
		case 0x50:
		case 0x40:
		{
			const_cast<int&>(x) = -(PopByte16() & ~0x40);
			return *this;
		}
	}
	const_cast<int&>(x) = PopByte8();
	return *this;
}
