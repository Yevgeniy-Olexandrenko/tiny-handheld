#include "commons.h"
#include "memory.h"

namespace th
{
	namespace memory
	{
		Binary::Binary() : m_type(Binary::Unknown), m_addr(0) {}
		Binary::Binary(const uint8_p* addr) : m_type(Binary::Flash), m_addr(addr) {}
		Binary::Binary(const uint8_s* addr) : m_type(Binary::Storage), m_addr(addr) {}
		
		void Binary::startMultiAccess() const
		{
			// TODO
		}
		
		void Binary::stopMultiAccess() const
		{
			// TODO
		}
		
		void Binary::get(Binary::Index i, u08& b0) const
		{
			if (m_type == Binary::Storage)
			{
				b0 = reinterpret_cast<const uint8_s *>(m_addr)[i];
			}
			else if (m_type == Binary::Flash)
			{
				b0 = reinterpret_cast<const uint8_p *>(m_addr)[i];
			}
		}
		
		void Binary::get(Binary::Index i, u08& b0, u08& b1) const
		{
			if (m_type == Binary::Storage)
			{
				const uint8_s *data = reinterpret_cast<const uint8_s *>(m_addr) + i;
				b0 = *data++;
				b1 = *data;
			}
			else if (m_type == Binary::Flash)
			{
				const uint8_p *data = reinterpret_cast<const uint8_p *>(m_addr) + i;
				b0 = *data++;
				b1 = *data;
			}
		}
		
		void Binary::get(Binary::Index i, u08& b0, u08& b1, u08& b2) const
		{
			if (m_type == Binary::Storage)
			{
				const uint8_s *data = reinterpret_cast<const uint8_s *>(m_addr) + i;
				b0 = *data++;
				b1 = *data++;
				b2 = *data;
			}
			else if (m_type == Binary::Flash)
			{
				const uint8_p *data = reinterpret_cast<const uint8_p *>(m_addr) + i;
				b0 = *data++;
				b1 = *data++;
				b2 = *data;
			}
		}
	}
}
