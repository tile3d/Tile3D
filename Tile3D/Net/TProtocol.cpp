#include "TProtocol.h"


void TProtocol::Encode(TOctetsStream& os) const
{
	os.CompactUInt32(m_type);
	TOctetsStream po;
	Marshal(po);
	os << po;
}


void TProtocol::Decode(const TOctetsStream & os)
{

}

