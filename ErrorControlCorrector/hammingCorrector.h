#pragma once
#ifndef ECC_HAMMING_CORRECTOR_H
#define ECC_HAMMING_CORRECTOR_H

#include "dataFrame.h"
#include <bitset>

size_t getHammingCodeLength(const size_t msgSize)
{
	if (msgSize <= 1)
		return 2;
	if (msgSize <= 4)
		return 3;
	size_t l = 4, r = msgSize + 1;
	while (l < r)
	{
		size_t mid = (l + r) >> 1;
		size_t cur = ((size_t)1 << mid) - 1 - mid;
		if (cur >= msgSize)
			r = mid;
		else
			l = mid + 1;
	}
	return r;
}

template<size_t _NBits, size_t _KBits>
class HammingCorrector
{
public:
	template<size_t _KBits>
	static void encoding(
		DataFrame<_NBits, _KBits>& msg,
		const std::bitset<_NBits>& bitsStream,
		ODD_EVEN_TYPE type)
	{
		size_t index = 0;
		for (size_t iMsg = 0; iMsg < _NBits + _KBits; ++iMsg)
		{
			size_t cur = iMsg + 1;
			if (!(cur & (cur - 1)))
				continue;
			msg.m_dataFrame[iMsg] = bitsStream[index];
			for (size_t iCorrect = 0; iCorrect < _KBits; ++iCorrect)
			{
				size_t correctIndex = ((size_t)1 << iCorrect) - 1;
				if ((cur >> iCorrect) & 1)
				{
					if (bitsStream[index])
						msg.m_dataFrame.flip(correctIndex);
				}
			}
			++index;
		}
		for (size_t iCorrect = 0; iCorrect < _KBits; ++iCorrect)
		{
			size_t correctIndex = ((size_t)1 << iCorrect) - 1;
			if (EVEN == type)
				continue;
			msg.m_dataFrame.flip(correctIndex);
		}
	}

	template<size_t _KBits>
	static bool isErrorWithSingleBitAndTakeCorrect(
		DataFrame<_NBits, _KBits>& msg,
		ODD_EVEN_TYPE type)
	{
		bool isErrorWithSingleBit = false;
		size_t detectIndex = 0;
		for (size_t iCorrect = 0; iCorrect < _KBits; ++iCorrect)
		{
			size_t curIndex = ((size_t)1 << iCorrect) - 1;
			size_t curSum = msg.m_dataFrame[curIndex];
			for (size_t iMsg = 0; iMsg < _NBits + _KBits; ++iMsg)
			{
				size_t cur = iMsg + 1;
				if (!(cur & (cur - 1)))
					continue;
				if ((cur >> iCorrect) & 1)
					curSum += msg.m_dataFrame[iMsg];
			}
			if (ODD == type && (curSum & 1))
				continue;
			if (EVEN == type && !(curSum & 1))
				continue;
			detectIndex += ((size_t)1 << iCorrect);
		}
		if (detectIndex > 0)
		{
			isErrorWithSingleBit = true;
			msg.m_dataFrame.flip(detectIndex - 1);
		}
		return isErrorWithSingleBit;
	}

	template<size_t _KBits>
	static void decoding(
		std::bitset<_NBits>& bitsStream,
		const DataFrame<_NBits, _KBits>& msg)
	{
		size_t index = 0;
		for (size_t iMsg = 0; iMsg < _NBits + _KBits; ++iMsg)
		{
			size_t cur = iMsg + 1;
			if (!(cur & (cur - 1)))
				continue;
			bitsStream[index++] = msg.m_dataFrame[iMsg];
		}
	}

};

#endif // !ECC_HAMMING_CORRECTOR_H

