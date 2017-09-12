#pragma once

class TColor
{
public:
	TColor() {}
	
	TColor(float r, float g, float b, float a) {
		m_r = r;
		m_g = g;
		m_b = b;
		m_a = a;
	}
	
	TColor(const TColor & c) {
		m_r = c.m_r;
		m_g = c.m_g;
		m_b = c.m_b;
		m_a = c.m_a;
	}

	const TColor& operator=(TColor & c) {
		m_r = c.m_r;
		m_g = c.m_g;
		m_b = c.m_b;
		m_a = c.m_a;
		return *this;
	}

public:
	void SetColor(float r, float g, float b, float a) {
		m_r = r;
		m_g = g;
		m_b = b;
		m_a = a;
	}

	TColor operator + (const TColor& c) { return TColor(c.m_r + m_r, c.m_g + m_g, c.m_b + m_b, c.m_a + m_a); }
	TColor operator - (const TColor& c) { return TColor(c.m_r - m_r, c.m_g - m_g, c.m_b - m_b, c.m_a - m_a); }
	TColor operator * (const TColor& c) { return TColor(c.m_r * m_r, c.m_g * m_g, c.m_b * m_b, c.m_a * m_a); }
	TColor operator * (float f) { return TColor(f* m_r, f * m_g, f * m_b, f * m_a); }

	const TColor& operator += (const TColor& c) { m_r += c.m_r; m_g += c.m_g; m_b += c.m_b; m_a += c.m_a; return *this; }
	const TColor& operator -= (const TColor& c) { m_r -= c.m_r; m_g -= c.m_g; m_b -= c.m_b; m_a -= c.m_a; return *this; }

	bool operator != (const TColor& c) { return (m_r != c.m_r || m_g != c.m_g || m_b != c.m_b || m_a != c.m_a); }
	bool operator == (const TColor& c) { return !(*this != c); }

public:
	float m_r;
	float m_g;
	float m_b;
	float m_a;
};

