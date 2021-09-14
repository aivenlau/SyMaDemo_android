#pragma once
#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <malloc.h>
#endif

typedef unsigned char  BYTE;
typedef unsigned long  DWORD;
class CBuffer
{
public:
	CBuffer(void)
	{
		m_pBuf = nullptr;
		m_nBufSize = 0;
		m_nCurWritePos = 0;
		m_nCurReadPos = 0;
	}

	CBuffer(int nBufSize)
	{
		Init(nBufSize);
	}

	~CBuffer(void)
	{
		if(nullptr != m_pBuf)
		{
			free(m_pBuf);
			m_pBuf = nullptr;
		}
	}

public:
	int Init(int nBufSize)
	{
		if(nullptr != m_pBuf)
		{
			free(m_pBuf);
			m_pBuf = nullptr;
		}

		if(nBufSize > 0) 
		{
			m_pBuf = (char *)malloc(nBufSize);

			if(nullptr == m_pBuf)
				return (-1);

			m_nBufSize = nBufSize;
			m_nCurWritePos = 0;
			m_nCurReadPos = 0;
		}

		return (1);
	}

	void Reset(void)
	{
		m_nCurReadPos = 0;
		m_nCurWritePos = 0;
	}

	int Add(char* pData, int nDataLen)
	{
		if(nullptr == m_pBuf)
			return (-1);

		if(nullptr == pData || nDataLen <= 0)
			return (0);

		if(m_nCurWritePos + nDataLen > m_nBufSize)
		{
			char* pNewBuf = (char *)realloc(m_pBuf, m_nCurWritePos + nDataLen);
			
			if(nullptr == pNewBuf)
				return (-1);

			m_pBuf = pNewBuf;
			m_nBufSize = m_nCurWritePos + nDataLen;
		}

		memcpy(m_pBuf + m_nCurWritePos, pData, nDataLen);
		m_nCurWritePos += nDataLen;

		return nDataLen;
	}

	int GetDataLen()  
	{
		return (m_nCurWritePos - m_nCurReadPos);  
	}

	char* GetDataPtr()
	{
		if(nullptr == m_pBuf)
			return nullptr;

		return (m_pBuf + m_nCurReadPos);
	}

	void SetUsed(int nUsedLen)
	{
		if(nullptr == m_pBuf)
			return;

		m_nCurReadPos += nUsedLen;

		if(m_nCurReadPos >= m_nCurWritePos)
		{
			Reset();
			return;
		}

		int nLeftLen = GetDataLen();
		memcpy(m_pBuf, m_pBuf + m_nCurReadPos, nLeftLen);
			
		m_nCurReadPos = 0;
		m_nCurWritePos = nLeftLen;
	} 

private:	
	char* m_pBuf;
	int m_nBufSize;
	int m_nCurWritePos;
	int m_nCurReadPos;
};

#endif // __BUFFER_H__
