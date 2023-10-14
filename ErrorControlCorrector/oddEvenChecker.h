#pragma once
#ifndef ECC_ODD_EVEN_CHECKER_H
#define ECC_ODD_EVEN_CHECKER_H

#include <bitset>
#include <exception>
#include "dataFrame.h"

enum ODD_EVEN_TYPE
{
	ODD,
	EVEN
};


template<size_t _NBits>
class OddEvenChecker
{
public:
	static bool isErrorWithSingleBit(
		const DataFrame<_NBits, 1>& msg,
		ODD_EVEN_TYPE type)
	{
		size_t numsOfOne = msg.m_dataFrame.count();
		if (ODD == type)
			return !(numsOfOne & 1);
		else if (EVEN == type)
			return (numsOfOne & 1);
		else
			throw "The ODD_EVEN_CHECKER type must be 'ODD' or 'EVEN'";
	}

	static void encoding(
		DataFrame<_NBits, 1>& msg,
		const std::bitset<_NBits>& bitsStream,
		ODD_EVEN_TYPE type)
	{
		size_t numsOfOne = bitsStream.count();
		for (size_t i = 0; i < _NBits; ++i)
			msg.m_dataFrame[i] = bitsStream[i];
		if (ODD == type && (numsOfOne & 1))
			return;
		if (EVEN == type && !(numsOfOne & 1))
			return;
		if (ODD != type && EVEN != type)
			throw "The ODD_EVEN_CHECKER type must be 'ODD' or 'EVEN'";
		msg.m_dataFrame[_NBits] = 1;
	}

	static void decoding(
		std::bitset<_NBits>& bitsStream,
		const DataFrame<_NBits, 1>& msg)
	{
		for (size_t i = 0; i < _NBits; ++i)
			bitsStream[i] = msg.m_dataFrame[i];
	}

};

#endif // !ECC_ODD_EVEN_CHECKER_H
