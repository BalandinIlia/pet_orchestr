#include "pch.h"
#include "messages.h"

namespace MS
{

	char codeType(ETypeMes t)
	{
		switch (t)
		{
		case ETypeMes::eReq: return 'a';
		case ETypeMes::eAnsNo: return 'b';
		case ETypeMes::eAnsYes: return 'c';
		case ETypeMes::eError: return 0;
		}
	}

	ETypeMes decodeType(char c)
	{
		switch (c)
		{
		case 'a': return ETypeMes::eReq;
		case 'b': return ETypeMes::eAnsNo;
		case 'c': return ETypeMes::eAnsYes;
		default: return ETypeMes::eError;
		}
	}

	int length(ETypeMes t)
	{
		switch (t)
		{
		case ETypeMes::eReq: return 10;
		case ETypeMes::eAnsNo: return 2;
		case ETypeMes::eAnsYes: return -1;
		case ETypeMes::eError: return -2;
		}
	}

	std::array<char, 11> serializeRequest(number req, short idRequest)
	{
		// In this function I serialize the following fields:
		// 1. Message code
		// 2. Request id
		// 3. Number (request body)
		// So, first I introduce pointers to all these 3 parts

		// This is a pointer to the message code field
		char* pCode = nullptr;
		// a pointer to the id field
		uint16_t* pId = nullptr;
		// a pointer to the body field
		uint64_t* pNum = nullptr;

		// This is a memory chunk I will use for message serialization
		// This chunk contains 16 bytes, while I need to serialize only 11 bytes.
		// I still reserve buffer of 16 bytes due to data alignment.
		// The number must be written at address multiple of 8. So first 8 bytes
		// are for code, id. Last 8 bytes (which start at address multiple of 8 due to reservation way)
		// are for the number.
		std::array<uint64_t, 2> buf;
		// pointer to the place where serialization starts
		char* pSer = reinterpret_cast<char*>(buf.data()) + 5;
		pCode = pSer;
		pId = reinterpret_cast<uint16_t*>(pSer + 1);
		pNum = reinterpret_cast<uint64_t*>(pSer + 3);

		*pCode = codeType(ETypeMes::eReq);
		*pId = static_cast<uint16_t>(idRequest);
		*pNum = static_cast<uint64_t>(req);

		// here I return the serialization
		std::array<char, 11> ans;
		for (int i = 0; i < 11; i++)
			ans[i] = pSer[i];
		return ans;
	}

	std::vector<char> serializeAnsYes(const std::vector<number> aNum, short id)
	{
		// quantity of elements
		const int N = static_cast<int>(aNum.size());

		// In this function I serialize the following fields:
		// 1. Message code
		// 2. Quantity of elements
		// 3. Request id
		// 4. Numbers
		// So, first I introduce pointers to all these 4 parts

		// pointer to the message code in the buffer
		char* pCode = nullptr;
		// pointer to quantity of numbers in the buffer
		uint8_t* pQuant = nullptr;
		// pointer to the id
		uint16_t* pId = nullptr;
		// pointer of the array of numbers in the buffer
		uint64_t* aVal = nullptr;

		// Here I reserve a memory chunk to serialize the message. 
		// Data alignment is taken into account.
		std::vector<uint64_t> buf;
		buf.resize(N + 1);
		char* pSer = reinterpret_cast<char*>(buf.data()) + 4;
		pCode = pSer;
		pQuant = reinterpret_cast<uint8_t*>(pSer) + 1;
		pId = reinterpret_cast<uint16_t*>(pSer + 2);
		aVal = buf.data() + 1;

		// fill the buffer
		*pCode = codeType(ETypeMes::eAnsYes);
		*pQuant = static_cast<uint8_t>(N);
		*pId = static_cast<uint16_t>(id);
		for (int i = 0; i < N; i++)
			aVal[i] = aNum[i];

		// return the serialization
		std::vector<char> ans;
		const int lenBuf = N * 8 + 4;
		ans.reserve(lenBuf);
		for (int i = 0; i < lenBuf; i++)
			ans.push_back(pSer[i]);
		return ans;
	}

	std::array<char, 3> serializeAnsNo(short id)
	{
		// In this function I serialize the following fields:
		// 1. Message code
		// 2. Request id
		// So, first I introduce pointers to all these 2 parts

		// pointer to the message code in the buffer
		char* pCode = nullptr;
		// pointer to id
		uint16_t* pId = nullptr;

		// Here I reserve a memory chunk to serialize the message.
		// Data alignment is taken into account.
		std::array<uint16_t, 2> buf;
		char* pSer = reinterpret_cast<char*>(buf.data()) + 1;
		pCode = pSer;
		pId = reinterpret_cast<uint16_t*>(pSer + 1);

		// fill the buffer
		*pCode = codeType(ETypeMes::eAnsNo);
		*pId = static_cast<uint16_t>(id);

		// return the serialization
		std::array<char, 3> ans;
		for (int i = 0; i < 3; i++)
			ans[i] = pSer[i];
		return ans;
	}

	std::pair<short, number> deserializeRequest(const std::array<char, 10>& rawData)
	{
		// In this function I deserialize the request
		uint16_t id = 0;
		uint64_t num = 0;

		{
			// fill id: first two bytes in raw data
			char* pId = reinterpret_cast<char*>(&id);
			for (int i = 0; i < 2; i++)
				pId[i] = rawData[i];
		}
		{
			// fill num: last 8 bytes in raw data
			char* pNum = reinterpret_cast<char*>(&num);
			for (int i = 0; i < 8; i++)
				pNum[i] = rawData[i + 2];
		}

		return std::pair<short, number>
			(
				static_cast<short>(id),
				static_cast<number>(num)
			);
	}

	int bufSizeAnsYes(char c)
	{
		uint8_t* p = reinterpret_cast<uint8_t*>(&c);
		return static_cast<int>(*p) * 8 + 2;
	}

	std::pair<short, std::vector<number>> deserializeAnsYes(const std::vector<char>& rawData)
	{
		// quantity of elements
		const int N = static_cast<int>(rawData.size() - 2) / 8;

		uint16_t id = 0;
		std::vector<uint64_t> aNum;

		{
			// fill id: first two bytes in raw data
			char* pId = reinterpret_cast<char*>(&id);
			for (int i = 0; i < 2; i++)
				pId[i] = rawData[i];
		}
		{
			// fill aNum: all other bytes
			aNum.resize(N);
			char* pNum = reinterpret_cast<char*>(aNum.data());
			for (int i = 0; i < 8 * N; i++)
				pNum[i] = rawData[i + 2];
		}

		short idRet = static_cast<short>(id);
		std::vector<number> aNumRet;
		for (const uint64_t& i : aNum)
			aNumRet.push_back(static_cast<number>(i));

		return std::pair<short, std::vector<number>>(idRet, aNumRet);
	}

	short deserializeAnsNo(const std::array<char, 2>& rawData)
	{
		uint16_t id = 0;

		// deserialize id: two bytes in id
		char* pId = reinterpret_cast<char*>(&id);
		for (int i = 0; i < 2; i++)
			pId[i] = rawData[i];

		return static_cast<short>(id);
	}
}