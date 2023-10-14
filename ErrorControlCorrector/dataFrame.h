#pragma once
#ifndef ECC_DATA_FRAME_H
#define ECC_DATA_FRAME_H

#include <iostream>
#include <bitset>

template<size_t _NBits, size_t _KBits>
struct DataFrame
{
	std::bitset<_NBits + _KBits> m_dataFrame;

	void clear()
	{
		m_dataFrame.reset();
	}

	//template<size_t _fNBits, size_t _fKBits>
	friend std::ostream& operator<<(
		std::ostream& out,
		const DataFrame<_NBits, _KBits>& df)
	{
		out << df.m_dataFrame;
		return out;
	}
};

#endif // !ECC_DATA_FRAME_H
