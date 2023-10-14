#include "oddEvenChecker.h"
#include "noiseGenerator.h"
#include "hammingCorrector.h"
#include <iostream>
#include <fstream>

template<size_t _NBits>
void getRandomBitsStream(std::bitset<_NBits>& bitsStream)
{
	std::mt19937 eGen(std::random_device{}());
	std::uniform_int_distribution<uint16_t> uGen(0, 1);
	for (size_t i = 0; i < _NBits; ++i)
		bitsStream[i] = uGen(eGen);
}

void testOddEvenChecker()
{
	std::ofstream ofs("OddEvenCheckerLog.txt");
	bool isError = false;
	ODD_EVEN_TYPE testType = EVEN;
	size_t testRound = 100, errFreq = 10;
	const size_t testNBits = 16;
	std::bitset<testNBits> sendBitsStream;
	std::bitset<testNBits> receiveBitsStream;
	DataFrame<testNBits, 1> dataFrame;
	size_t updateIndex = 0;

	for (size_t iRound = 0; iRound < testRound; ++iRound)
	{
		ofs << "Test round " << iRound << "====>" << std::endl;

		isError = false;
		sendBitsStream.reset();
		receiveBitsStream.reset();
		dataFrame.clear();

		ofs << "Step 1. Initialize the original bit-stream, which is: " << std::endl;
		getRandomBitsStream(sendBitsStream);
		ofs << sendBitsStream << std::endl;
		OddEvenChecker<testNBits>::encoding(dataFrame, sendBitsStream, testType);
		ofs << "Step 2. Encoding and sending the message-frame to receiver, which is" << std::endl;
		ofs << dataFrame << std::endl;

		if (0 == iRound % errFreq)
		{
			/*
			* Import 1-bit noise/error from channel
			*/
			isError = true;
			updateIndex = NoiseGenerator::setNoise(dataFrame);
		}
		if (OddEvenChecker<testNBits>::isErrorWithSingleBit(dataFrame, testType))
		{
			if (!isError)
				std::cerr << "OddEvenChecker Error, the sample is correct but checker detects it is incorrect." << std::endl;
			ofs << "Error: the bit-stream is error with single bit, which is: " << std::endl;
			ofs << dataFrame << std::endl;
			ofs << "After modifing......, which is: " << std::endl;
			dataFrame.m_dataFrame.flip(updateIndex);
			ofs << dataFrame << std::endl;
			if (OddEvenChecker<testNBits>::isErrorWithSingleBit(dataFrame, testType))
				ofs << "It is still error......" << std::endl;
			else
			{
				ofs << "Now, it is correct after modifing, which is: " << std::endl;
				OddEvenChecker<testNBits>::decoding(receiveBitsStream, dataFrame);
				ofs << receiveBitsStream << std::endl;
			}
		}
		else
		{
			if (isError)
				ofs << "OddEvenChecker Error, the sample is incorrect but "
				<< "checker detects it is correct." << std::endl;
			OddEvenChecker<testNBits>::decoding(receiveBitsStream, dataFrame);
			ofs << "Step 3. Receiving and decoding the data from sender, which is: " << std::endl;
			ofs << receiveBitsStream << std::endl;
		}
		if (sendBitsStream == receiveBitsStream)
			ofs << "CASE : " << iRound << " is CORRECT!" << std::endl;
		else
			ofs << "CASE : " << iRound << " is INCORRECT!" << std::endl;
	}
}

void testHammingCorrectCode()
{
	std::ofstream ofs("HammingCorrectorLog.txt");
	bool isError = false;
	ODD_EVEN_TYPE testType = EVEN;
	size_t testRound = 100, errFreq = 10;
	const size_t testHamNBits = 16;
	const size_t testHamKBits = 5;
	std::bitset<testHamNBits> sendBitsStream;
	std::bitset<testHamNBits> receiveBitsStream;
	DataFrame<testHamNBits, testHamKBits> dataFrame;
	using HAM = HammingCorrector<testHamNBits, testHamKBits>;
	size_t updateIndex = 0;

	for (size_t iRound = 0; iRound < testRound; ++iRound)
	{
		ofs << "Test round " << iRound << "====>" << std::endl;

		isError = false;
		sendBitsStream.reset();
		receiveBitsStream.reset();
		dataFrame.clear();

		ofs << "Step 1. Initialize the original bit-stream, which is: " << std::endl;
		getRandomBitsStream(sendBitsStream);
		ofs << sendBitsStream << std::endl;
		HAM::encoding(dataFrame, sendBitsStream, testType);
		ofs << "Step 2. Encoding and sending the message-frame to receiver, which is" << std::endl;
		ofs << dataFrame << std::endl;

		if (0 == iRound % errFreq)
		{
			/*
			* Import 1-bit noise/error from channel
			*/
			isError = true;
			while (true)
			{
				updateIndex = NoiseGenerator::setNoise(dataFrame);
				size_t cur = updateIndex + 1;
				if (!(cur & (cur - 1)))
				{
					dataFrame.m_dataFrame.flip(updateIndex);
					continue;
				}
				break;
			}
		}
		if (isError)
		{
			ofs << "Error: the bit-stream is error with single bit, which is: " << std::endl;
			ofs << dataFrame << std::endl;
		}
		if (HAM::isErrorWithSingleBitAndTakeCorrect(dataFrame, testType))
		{
			if (!isError)
				std::cerr << "Hamming-Correcting failure, the sample is correct "
				<< "but checker detects it is incorrect." << std::endl;
			ofs << "After correcting......, which is: " << std::endl;
			ofs << dataFrame << std::endl;
			if (HAM::isErrorWithSingleBitAndTakeCorrect(dataFrame, testType))
				ofs << "It is still error......" << std::endl;
			else
			{
				ofs << "Now, it is correct after modifing, which is: " << std::endl;
				HAM::decoding(receiveBitsStream, dataFrame);
				ofs << receiveBitsStream << std::endl;
			}
		}
		else
		{
			if (isError)
				std::cerr << "Hamming-Correcting failure, the sample is incorrect "
				<< "but checker detects it is correct." << std::endl;
			HAM::decoding(receiveBitsStream, dataFrame);
			ofs << "Step 3. Receiving and decoding the data from sender, which is: " << std::endl;
			ofs << receiveBitsStream << std::endl;
		}
		if (sendBitsStream == receiveBitsStream)
			ofs << "CASE : " << iRound << " is CORRECT!" << std::endl;
		else
			ofs << "CASE : " << iRound << " is INCORRECT!" << std::endl;
	}
}

int main()
{
	//testOddEvenChecker();
	testHammingCorrectCode();
	//std::cout << getHammingCodeLength(16) << std::endl; // 5
	return 0;
}