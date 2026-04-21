/* Copyright (c) 2025-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "xsimd_sse2.h"
#include <string.h>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#define XSIMD_X86
#include <emmintrin.h>
#endif

xsimd_int64 _xsimd_find_byte_SSE2(const unsigned char* pData, xsimd_int64 nSize, xsimd_uint8 nByte, xsimd_int64 nOffset)
{
#ifdef XSIMD_X86
    __m128i vNeedle = _mm_set1_epi8((char)nByte);
    xsimd_int64 i = 0;
    
    /* Process 64 bytes per iteration for better throughput */
    while (i + 64 <= nSize) {
        __m128i vData0 = _mm_loadu_si128((const __m128i*)(pData + i));
        __m128i vData1 = _mm_loadu_si128((const __m128i*)(pData + i + 16));
        __m128i vData2 = _mm_loadu_si128((const __m128i*)(pData + i + 32));
        __m128i vData3 = _mm_loadu_si128((const __m128i*)(pData + i + 48));
        
        __m128i vCmp0 = _mm_cmpeq_epi8(vData0, vNeedle);
        __m128i vCmp1 = _mm_cmpeq_epi8(vData1, vNeedle);
        __m128i vCmp2 = _mm_cmpeq_epi8(vData2, vNeedle);
        __m128i vCmp3 = _mm_cmpeq_epi8(vData3, vNeedle);
        
        int nMask0 = _mm_movemask_epi8(vCmp0);
        int nMask1 = _mm_movemask_epi8(vCmp1);
        int nMask2 = _mm_movemask_epi8(vCmp2);
        int nMask3 = _mm_movemask_epi8(vCmp3);
        
        if (nMask0 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask0);
            return i + nBitPos;
#else
            return i + __builtin_ctz((unsigned int)nMask0);
#endif
        }
        
        if (nMask1 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask1);
            return i + 16 + nBitPos;
#else
            return i + 16 + __builtin_ctz((unsigned int)nMask1);
#endif
        }
        
        if (nMask2 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask2);
            return i + 32 + nBitPos;
#else
            return i + 32 + __builtin_ctz((unsigned int)nMask2);
#endif
        }
        
        if (nMask3 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask3);
            return i + 48 + nBitPos;
#else
            return i + 48 + __builtin_ctz((unsigned int)nMask3);
#endif
        }
        
        i += 64;
    }
    
    /* Process remaining 16-byte chunks */
    while (i + 16 <= nSize) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pData + i));
        __m128i vCmp = _mm_cmpeq_epi8(vData, vNeedle);
        int nMask = _mm_movemask_epi8(vCmp);
        
        if (nMask != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask);
            return i + nBitPos;
#else
            return i + __builtin_ctz((unsigned int)nMask);
#endif
        }
        i += 16;
    }
    
    return i;
#else
    return 0;
#endif
}

xsimd_int64 _xsimd_find_pattern_bmh_SSE2(const char* pHay, xsimd_int64 nBufferSize, const char* pNeedle, xsimd_int64 nPatternSize, xsimd_int64 nOffset, xsimd_int64 nLimit, char nLastChar, xsimd_int64 *pPosition)
{
#ifdef XSIMD_X86
    __m128i vLast = _mm_set1_epi8((char)nLastChar);
    xsimd_int64 i = *pPosition;
    
    /* Process 64 bytes per iteration for better throughput */
    while (i + nPatternSize + 63 <= nBufferSize) {
        const char* pLastPos0 = pHay + i + nPatternSize - 1;
        const char* pLastPos1 = pLastPos0 + 16;
        const char* pLastPos2 = pLastPos0 + 32;
        const char* pLastPos3 = pLastPos0 + 48;
        
        __m128i vData0 = _mm_loadu_si128((const __m128i*)pLastPos0);
        __m128i vData1 = _mm_loadu_si128((const __m128i*)pLastPos1);
        __m128i vData2 = _mm_loadu_si128((const __m128i*)pLastPos2);
        __m128i vData3 = _mm_loadu_si128((const __m128i*)pLastPos3);
        
        __m128i vCmp0 = _mm_cmpeq_epi8(vData0, vLast);
        __m128i vCmp1 = _mm_cmpeq_epi8(vData1, vLast);
        __m128i vCmp2 = _mm_cmpeq_epi8(vData2, vLast);
        __m128i vCmp3 = _mm_cmpeq_epi8(vData3, vLast);
        
        int nMask0 = _mm_movemask_epi8(vCmp0);
        int nMask1 = _mm_movemask_epi8(vCmp1);
        int nMask2 = _mm_movemask_epi8(vCmp2);
        int nMask3 = _mm_movemask_epi8(vCmp3);
        
        /* Check all four 16-byte blocks */
        int nMasks[4] = {nMask0, nMask1, nMask2, nMask3};
        xsimd_int64 nOffsets[4] = {0, 16, 32, 48};
        
        for (int block = 0; block < 4; block++) {
            if (nMasks[block] != 0) {
                int nTempMask = nMasks[block];
                while (nTempMask != 0) {
#ifdef _MSC_VER
                    unsigned long bit;
                    _BitScanForward(&bit, (unsigned long)nTempMask);
#else
                    unsigned bit = __builtin_ctz((unsigned int)nTempMask);
#endif
                    xsimd_int64 nCheckPos = i + nOffsets[block] + (xsimd_int64)bit;
                    
                    if (nCheckPos <= nLimit) {
                        if (memcmp(pHay + nCheckPos, pNeedle, (size_t)nPatternSize) == 0) {
                            return nOffset + nCheckPos;
                        }
                    }
                    
                    nTempMask &= nTempMask - 1;
                }
            }
        }
        
        i += 64;
    }
    
    /* Process remaining 16-byte chunks */
    while (i + nPatternSize + 15 <= nBufferSize) {
        const char* pLastPos = pHay + i + nPatternSize - 1;
        __m128i vData = _mm_loadu_si128((const __m128i*)pLastPos);
        __m128i vCmp = _mm_cmpeq_epi8(vData, vLast);
        int nMask = _mm_movemask_epi8(vCmp);
        
        if (nMask != 0) {
            while (nMask != 0) {
#ifdef _MSC_VER
                unsigned long bit;
                _BitScanForward(&bit, (unsigned long)nMask);
#else
                unsigned bit = __builtin_ctz((unsigned int)nMask);
#endif
                xsimd_int64 nCheckPos = i + (xsimd_int64)bit;
                
                if (nCheckPos <= nLimit) {
                    if (memcmp(pHay + nCheckPos, pNeedle, (size_t)nPatternSize) == 0) {
                        return nOffset + nCheckPos;
                    }
                }
                
                nMask &= nMask - 1;
            }
        }
        
        i += 16;
    }
    
    /* Update position and return -1 to signal no match found by SIMD */
    *pPosition = i;
    return -1;
#else
    return -1;
#endif
}

xsimd_int64 _xsimd_find_pattern_bmh_3byte_SSE2(const char* pHay, xsimd_int64 nBufferSize, const char* pNeedle, xsimd_int64 nOffset)
{
#ifdef XSIMD_X86
    __m128i vFirst = _mm_set1_epi8(pNeedle[0]);
    const char nSecond = pNeedle[1];
    const char nThird = pNeedle[2];
    xsimd_int64 i = 0;
    
    /* Process 16 bytes per iteration - filter by first byte */
    while (i + 15 + 2 < nBufferSize) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pHay + i));
        __m128i vCmp = _mm_cmpeq_epi8(vData, vFirst);
        int nMask = _mm_movemask_epi8(vCmp);
        
        if (nMask != 0) {
            /* Check each candidate */
            int nTempMask = nMask;
            while (nTempMask != 0) {
#ifdef _MSC_VER
                unsigned long bit;
                _BitScanForward(&bit, (unsigned long)nTempMask);
#else
                unsigned bit = __builtin_ctz((unsigned int)nTempMask);
#endif
                xsimd_int64 pos = i + (xsimd_int64)bit;
                
                /* Verify remaining 2 bytes */
                if (pos + 2 < nBufferSize) {
                    if (pHay[pos + 1] == nSecond && pHay[pos + 2] == nThird) {
                        return nOffset + pos;
                    }
                }
                
                nTempMask &= nTempMask - 1;
            }
        }
        
        i += 16;
    }
    
    /* Scalar fallback for remaining bytes */
    while (i + 2 < nBufferSize) {
        if (pHay[i] == pNeedle[0] && pHay[i + 1] == nSecond && pHay[i + 2] == nThird) {
            return nOffset + i;
        }
        i++;
    }
#endif
    
    return -1;
}

xsimd_int64 _xsimd_find_pattern_bmh_2byte_SSE2(const char* pHay, xsimd_int64 nBufferSize, xsimd_uint16 pattern16, xsimd_int64 nOffset)
{
#ifdef XSIMD_X86
    const char nFirstByte = (char)(pattern16 & 0xFF);
    const char nSecondByte = (char)((pattern16 >> 8) & 0xFF);
    __m128i vFirst = _mm_set1_epi8(nFirstByte);
    xsimd_int64 i = 0;
    
    /* Process 64 bytes per iteration - filter by first byte */
    while (i + 64 <= nBufferSize) {
        __m128i vData0 = _mm_loadu_si128((const __m128i*)(pHay + i));
        __m128i vData1 = _mm_loadu_si128((const __m128i*)(pHay + i + 16));
        __m128i vData2 = _mm_loadu_si128((const __m128i*)(pHay + i + 32));
        __m128i vData3 = _mm_loadu_si128((const __m128i*)(pHay + i + 48));
        
        __m128i vCmp0 = _mm_cmpeq_epi8(vData0, vFirst);
        __m128i vCmp1 = _mm_cmpeq_epi8(vData1, vFirst);
        __m128i vCmp2 = _mm_cmpeq_epi8(vData2, vFirst);
        __m128i vCmp3 = _mm_cmpeq_epi8(vData3, vFirst);
        
        int nMask0 = _mm_movemask_epi8(vCmp0);
        int nMask1 = _mm_movemask_epi8(vCmp1);
        int nMask2 = _mm_movemask_epi8(vCmp2);
        int nMask3 = _mm_movemask_epi8(vCmp3);
        
        /* Fast skip if no matches */
        if ((nMask0 | nMask1 | nMask2 | nMask3) == 0) {
            i += 64;
            continue;
        }
        
        /* Check first block */
        if (nMask0 != 0) {
            int nTempMask = nMask0;
            while (nTempMask != 0) {
#ifdef _MSC_VER
                unsigned long bit;
                _BitScanForward(&bit, (unsigned long)nTempMask);
#else
                unsigned bit = __builtin_ctz((unsigned int)nTempMask);
#endif
                xsimd_int64 pos = i + (xsimd_int64)bit;
                if (pos + 1 < nBufferSize && pHay[pos + 1] == nSecondByte) {
                    return nOffset + pos;
                }
                nTempMask &= nTempMask - 1;
            }
        }
        
        /* Check second block */
        if (nMask1 != 0) {
            int nTempMask = nMask1;
            while (nTempMask != 0) {
#ifdef _MSC_VER
                unsigned long bit;
                _BitScanForward(&bit, (unsigned long)nTempMask);
#else
                unsigned bit = __builtin_ctz((unsigned int)nTempMask);
#endif
                xsimd_int64 pos = i + 16 + (xsimd_int64)bit;
                if (pos + 1 < nBufferSize && pHay[pos + 1] == nSecondByte) {
                    return nOffset + pos;
                }
                nTempMask &= nTempMask - 1;
            }
        }
        
        /* Check third block */
        if (nMask2 != 0) {
            int nTempMask = nMask2;
            while (nTempMask != 0) {
#ifdef _MSC_VER
                unsigned long bit;
                _BitScanForward(&bit, (unsigned long)nTempMask);
#else
                unsigned bit = __builtin_ctz((unsigned int)nTempMask);
#endif
                xsimd_int64 pos = i + 32 + (xsimd_int64)bit;
                if (pos + 1 < nBufferSize && pHay[pos + 1] == nSecondByte) {
                    return nOffset + pos;
                }
                nTempMask &= nTempMask - 1;
            }
        }
        
        /* Check fourth block */
        if (nMask3 != 0) {
            int nTempMask = nMask3;
            while (nTempMask != 0) {
#ifdef _MSC_VER
                unsigned long bit;
                _BitScanForward(&bit, (unsigned long)nTempMask);
#else
                unsigned bit = __builtin_ctz((unsigned int)nTempMask);
#endif
                xsimd_int64 pos = i + 48 + (xsimd_int64)bit;
                if (pos + 1 < nBufferSize && pHay[pos + 1] == nSecondByte) {
                    return nOffset + pos;
                }
                nTempMask &= nTempMask - 1;
            }
        }
        
        i += 64;
    }
    
    /* Process remaining 16-byte chunks */
    while (i + 16 <= nBufferSize) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pHay + i));
        __m128i vCmp = _mm_cmpeq_epi8(vData, vFirst);
        int nMask = _mm_movemask_epi8(vCmp);
        
        if (nMask != 0) {
            int nTempMask = nMask;
            while (nTempMask != 0) {
#ifdef _MSC_VER
                unsigned long bit;
                _BitScanForward(&bit, (unsigned long)nTempMask);
#else
                unsigned bit = __builtin_ctz((unsigned int)nTempMask);
#endif
                xsimd_int64 pos = i + (xsimd_int64)bit;
                if (pos + 1 < nBufferSize && pHay[pos + 1] == nSecondByte) {
                    return nOffset + pos;
                }
                nTempMask &= nTempMask - 1;
            }
        }
        
        i += 16;
    }
    
    /* Scalar fallback for remaining bytes using memchr for speed */
    while (i + 1 < nBufferSize) {
        const char* pFound = (const char*)memchr(pHay + i, nFirstByte, (size_t)(nBufferSize - i));
        if (!pFound) break;
        
        xsimd_int64 pos = pFound - pHay;
        if (pos + 1 < nBufferSize && pHay[pos + 1] == nSecondByte) {
            return nOffset + pos;
        }
        i = pos + 1;
    }
#endif
    
    return -1;
}

xsimd_int64 _xsimd_find_pattern_bmh_1byte_SSE2(const char* pHay, xsimd_int64 nBufferSize, char nByte, xsimd_int64 nOffset)
{
#ifdef XSIMD_X86
    __m128i vPattern = _mm_set1_epi8(nByte);
    xsimd_int64 i = 0;
    
    /* Process 16 bytes per iteration */
    while (i + 15 < nBufferSize) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pHay + i));
        __m128i vCmp = _mm_cmpeq_epi8(vData, vPattern);
        int nMask = _mm_movemask_epi8(vCmp);
        
        if (nMask != 0) {
            /* Find first match using bit scan */
#ifdef _MSC_VER
            unsigned long bit;
            _BitScanForward(&bit, (unsigned long)nMask);
#else
            unsigned bit = __builtin_ctz((unsigned int)nMask);
#endif
            return nOffset + i + (xsimd_int64)bit;
        }
        
        i += 16;
    }
    
    /* Scalar fallback for remaining bytes */
    while (i < nBufferSize) {
        if (pHay[i] == nByte) {
            return nOffset + i;
        }
        i++;
    }
#endif
    
    return -1;
}

xsimd_int64 _xsimd_find_ansi_SSE2(const unsigned char* pData, xsimd_int64 nBufferSize, xsimd_int64 nMinLength, xsimd_int64 nOffset)
{
#ifdef XSIMD_X86
    const __m128i vLowerMinus1 = _mm_set1_epi8(0x1F);
    const __m128i vUpperPlus1 = _mm_set1_epi8(0x7F);
    xsimd_int64 j = 0;
    
    /* Process 64 bytes per iteration for better throughput */
    while (j + 64 <= nBufferSize) {
        __m128i vData0 = _mm_loadu_si128((const __m128i*)(pData + j));
        __m128i vData1 = _mm_loadu_si128((const __m128i*)(pData + j + 16));
        __m128i vData2 = _mm_loadu_si128((const __m128i*)(pData + j + 32));
        __m128i vData3 = _mm_loadu_si128((const __m128i*)(pData + j + 48));
        
        __m128i vGe0 = _mm_cmpgt_epi8(vData0, vLowerMinus1);
        __m128i vLe0 = _mm_cmpgt_epi8(vUpperPlus1, vData0);
        __m128i vAnsi0 = _mm_and_si128(vGe0, vLe0);
        
        __m128i vGe1 = _mm_cmpgt_epi8(vData1, vLowerMinus1);
        __m128i vLe1 = _mm_cmpgt_epi8(vUpperPlus1, vData1);
        __m128i vAnsi1 = _mm_and_si128(vGe1, vLe1);
        
        __m128i vGe2 = _mm_cmpgt_epi8(vData2, vLowerMinus1);
        __m128i vLe2 = _mm_cmpgt_epi8(vUpperPlus1, vData2);
        __m128i vAnsi2 = _mm_and_si128(vGe2, vLe2);
        
        __m128i vGe3 = _mm_cmpgt_epi8(vData3, vLowerMinus1);
        __m128i vLe3 = _mm_cmpgt_epi8(vUpperPlus1, vData3);
        __m128i vAnsi3 = _mm_and_si128(vGe3, vLe3);
        
        int nMask0 = _mm_movemask_epi8(vAnsi0);
        int nMask1 = _mm_movemask_epi8(vAnsi1);
        int nMask2 = _mm_movemask_epi8(vAnsi2);
        int nMask3 = _mm_movemask_epi8(vAnsi3);
        
        if (nMask0 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask0);
            xsimd_int64 start = j + nBitPos;
#else
            xsimd_int64 start = j + __builtin_ctz((unsigned int)nMask0);
#endif
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && pData[start + runLen] >= 0x20 && pData[start + runLen] <= 0x7E) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
            continue;
        }
        
        if (nMask1 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask1);
            xsimd_int64 start = j + 16 + nBitPos;
#else
            xsimd_int64 start = j + 16 + __builtin_ctz((unsigned int)nMask1);
#endif
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && pData[start + runLen] >= 0x20 && pData[start + runLen] <= 0x7E) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
            continue;
        }
        
        if (nMask2 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask2);
            xsimd_int64 start = j + 32 + nBitPos;
#else
            xsimd_int64 start = j + 32 + __builtin_ctz((unsigned int)nMask2);
#endif
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && pData[start + runLen] >= 0x20 && pData[start + runLen] <= 0x7E) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
            continue;
        }
        
        if (nMask3 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask3);
            xsimd_int64 start = j + 48 + nBitPos;
#else
            xsimd_int64 start = j + 48 + __builtin_ctz((unsigned int)nMask3);
#endif
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && pData[start + runLen] >= 0x20 && pData[start + runLen] <= 0x7E) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
            continue;
        }
        
        j += 64;
    }
    
    /* Process remaining 16-byte chunks */
    while (j + 16 <= nBufferSize) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pData + j));
        __m128i vGe = _mm_cmpgt_epi8(vData, vLowerMinus1);
        __m128i vLe = _mm_cmpgt_epi8(vUpperPlus1, vData);
        __m128i vAnsi = _mm_and_si128(vGe, vLe);
        xsimd_int32 nMask = _mm_movemask_epi8(vAnsi);
        
        if (nMask != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask);
            xsimd_int64 start = j + nBitPos;
#else
            xsimd_int64 start = j + __builtin_ctz((unsigned int)nMask);
#endif
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && pData[start + runLen] >= 0x20 && pData[start + runLen] <= 0x7E) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
        } else {
            j += 16;
        }
    }
    
    return j;
#else
    return 0;
#endif
}

/* Optimized version using combined 64-bit bitmask analysis */
xsimd_int64 _xsimd_find_ansi_SSE2_2(const unsigned char* pData, xsimd_int64 nBufferSize, xsimd_int64 nMinLength, xsimd_int64 nOffset)
{
#ifdef XSIMD_X86
    const __m128i vLowerMinus1 = _mm_set1_epi8(0x1F);
    const __m128i vUpperPlus1 = _mm_set1_epi8(0x7F);
    xsimd_int64 j = 0;
    
    /* Process 64 bytes per iteration using combined 64-bit bitmask */
    while (j + 64 <= nBufferSize) {
        __m128i vData0 = _mm_loadu_si128((const __m128i*)(pData + j));
        __m128i vData1 = _mm_loadu_si128((const __m128i*)(pData + j + 16));
        __m128i vData2 = _mm_loadu_si128((const __m128i*)(pData + j + 32));
        __m128i vData3 = _mm_loadu_si128((const __m128i*)(pData + j + 48));
        
        __m128i vGe0 = _mm_cmpgt_epi8(vData0, vLowerMinus1);
        __m128i vLe0 = _mm_cmpgt_epi8(vUpperPlus1, vData0);
        __m128i vAnsi0 = _mm_and_si128(vGe0, vLe0);
        
        __m128i vGe1 = _mm_cmpgt_epi8(vData1, vLowerMinus1);
        __m128i vLe1 = _mm_cmpgt_epi8(vUpperPlus1, vData1);
        __m128i vAnsi1 = _mm_and_si128(vGe1, vLe1);
        
        __m128i vGe2 = _mm_cmpgt_epi8(vData2, vLowerMinus1);
        __m128i vLe2 = _mm_cmpgt_epi8(vUpperPlus1, vData2);
        __m128i vAnsi2 = _mm_and_si128(vGe2, vLe2);
        
        __m128i vGe3 = _mm_cmpgt_epi8(vData3, vLowerMinus1);
        __m128i vLe3 = _mm_cmpgt_epi8(vUpperPlus1, vData3);
        __m128i vAnsi3 = _mm_and_si128(vGe3, vLe3);
        
        xsimd_uint32 nMask0 = (xsimd_uint32)(unsigned int)_mm_movemask_epi8(vAnsi0);
        xsimd_uint32 nMask1 = (xsimd_uint32)(unsigned int)_mm_movemask_epi8(vAnsi1);
        xsimd_uint32 nMask2 = (xsimd_uint32)(unsigned int)_mm_movemask_epi8(vAnsi2);
        xsimd_uint32 nMask3 = (xsimd_uint32)(unsigned int)_mm_movemask_epi8(vAnsi3);
        
        /* Combine four 16-bit masks into one 64-bit mask */
        xsimd_uint64 nCombinedMask = (xsimd_uint64)(nMask0 & 0xFFFF)
                                   | ((xsimd_uint64)(nMask1 & 0xFFFF) << 16)
                                   | ((xsimd_uint64)(nMask2 & 0xFFFF) << 32)
                                   | ((xsimd_uint64)(nMask3 & 0xFFFF) << 48);
        
        if (nCombinedMask == 0) {
            j += 64;
            continue;
        }
        
        /* Find first ANSI byte using combined bitmask */
        {
#ifdef _MSC_VER
            unsigned long nBitPos;
#if defined(_WIN64)
            _BitScanForward64(&nBitPos, nCombinedMask);
#else
            /* 32-bit: check lower 32 bits first, then upper 32 bits */
            if ((unsigned long)nCombinedMask != 0) {
                _BitScanForward(&nBitPos, (unsigned long)nCombinedMask);
            } else {
                _BitScanForward(&nBitPos, (unsigned long)(nCombinedMask >> 32));
                nBitPos += 32;
            }
#endif
            xsimd_int64 start = j + (xsimd_int64)nBitPos;
#else
            xsimd_int64 start = j + __builtin_ctzll(nCombinedMask);
#endif
            /* Always scalar-extend the run for exact compatibility */
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && pData[start + runLen] >= 0x20 && pData[start + runLen] <= 0x7E) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
            continue;
        }
    }
    
    /* Process remaining 16-byte chunks */
    while (j + 16 <= nBufferSize) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pData + j));
        __m128i vGe = _mm_cmpgt_epi8(vData, vLowerMinus1);
        __m128i vLe = _mm_cmpgt_epi8(vUpperPlus1, vData);
        __m128i vAnsi = _mm_and_si128(vGe, vLe);
        xsimd_uint32 nMask = (xsimd_uint32)(unsigned int)_mm_movemask_epi8(vAnsi);
        
        if (nMask != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask);
            xsimd_int64 start = j + (xsimd_int64)nBitPos;
#else
            xsimd_int64 start = j + __builtin_ctz((unsigned int)nMask);
#endif
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && pData[start + runLen] >= 0x20 && pData[start + runLen] <= 0x7E) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
        } else {
            j += 16;
        }
    }
    
    return j;
#else
    return 0;
#endif
}

xsimd_int64 _xsimd_find_notnull_SSE2(const unsigned char* pData, xsimd_int64 nBufferSize, xsimd_int64 nMinLength, xsimd_int64 nOffset, xsimd_int64 j, xsimd_int64 runStart)
{
#ifdef XSIMD_X86
    const __m128i vZero = _mm_setzero_si128();
    
    /* Skip leading nulls */
    while (j < nBufferSize && pData[j] == 0) j++;
    if (j >= nBufferSize) return -1;
    runStart = j;
    
    while (j + 16 <= nBufferSize) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pData + j));
        __m128i vCmpZero = _mm_cmpeq_epi8(vData, vZero);
        xsimd_int32 nMask = _mm_movemask_epi8(vCmpZero);
        
        if (nMask != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask);
            xsimd_int64 nullPos = j + nBitPos;
#else
            xsimd_int64 nullPos = j + __builtin_ctz((unsigned int)nMask);
#endif
            xsimd_int64 runLen = nullPos - runStart;
            if (runLen >= nMinLength) {
                return nOffset + runStart;
            }
            
            j = nullPos + 1;
            while (j < nBufferSize && pData[j] == 0) j++;
            if (j >= nBufferSize) return -1;
            runStart = j;
        } else {
            j += 16;
        }
    }
    
    return j | (runStart << 32);
#else
    return j | (runStart << 32);
#endif
}

xsimd_int64 _xsimd_find_not_ansi_SSE2(const unsigned char* pData, xsimd_int64 nBufferSize, xsimd_int64 nMinLength, xsimd_int64 nOffset)
{
#ifdef XSIMD_X86
    const __m128i vLower = _mm_set1_epi8(0x20);
    const __m128i vUpper = _mm_set1_epi8(0x7E);
    xsimd_int64 j = 0;
    
    /* Process 64 bytes per iteration for better throughput */
    while (j + 64 <= nBufferSize) {
        __m128i vData0 = _mm_loadu_si128((const __m128i*)(pData + j));
        __m128i vData1 = _mm_loadu_si128((const __m128i*)(pData + j + 16));
        __m128i vData2 = _mm_loadu_si128((const __m128i*)(pData + j + 32));
        __m128i vData3 = _mm_loadu_si128((const __m128i*)(pData + j + 48));
        
        __m128i vLt0 = _mm_cmpgt_epi8(vLower, vData0);
        __m128i vGt0 = _mm_cmpgt_epi8(vData0, vUpper);
        __m128i vNotAnsi0 = _mm_or_si128(vLt0, vGt0);
        
        __m128i vLt1 = _mm_cmpgt_epi8(vLower, vData1);
        __m128i vGt1 = _mm_cmpgt_epi8(vData1, vUpper);
        __m128i vNotAnsi1 = _mm_or_si128(vLt1, vGt1);
        
        __m128i vLt2 = _mm_cmpgt_epi8(vLower, vData2);
        __m128i vGt2 = _mm_cmpgt_epi8(vData2, vUpper);
        __m128i vNotAnsi2 = _mm_or_si128(vLt2, vGt2);
        
        __m128i vLt3 = _mm_cmpgt_epi8(vLower, vData3);
        __m128i vGt3 = _mm_cmpgt_epi8(vData3, vUpper);
        __m128i vNotAnsi3 = _mm_or_si128(vLt3, vGt3);
        
        int nMask0 = _mm_movemask_epi8(vNotAnsi0);
        int nMask1 = _mm_movemask_epi8(vNotAnsi1);
        int nMask2 = _mm_movemask_epi8(vNotAnsi2);
        int nMask3 = _mm_movemask_epi8(vNotAnsi3);
        
        if (nMask0 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask0);
            xsimd_int64 start = j + nBitPos;
#else
            xsimd_int64 start = j + __builtin_ctz((unsigned int)nMask0);
#endif
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && (pData[start + runLen] < 0x20 || pData[start + runLen] > 0x7E)) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
            continue;
        }
        
        if (nMask1 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask1);
            xsimd_int64 start = j + 16 + nBitPos;
#else
            xsimd_int64 start = j + 16 + __builtin_ctz((unsigned int)nMask1);
#endif
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && (pData[start + runLen] < 0x20 || pData[start + runLen] > 0x7E)) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
            continue;
        }
        
        if (nMask2 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask2);
            xsimd_int64 start = j + 32 + nBitPos;
#else
            xsimd_int64 start = j + 32 + __builtin_ctz((unsigned int)nMask2);
#endif
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && (pData[start + runLen] < 0x20 || pData[start + runLen] > 0x7E)) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
            continue;
        }
        
        if (nMask3 != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask3);
            xsimd_int64 start = j + 48 + nBitPos;
#else
            xsimd_int64 start = j + 48 + __builtin_ctz((unsigned int)nMask3);
#endif
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && (pData[start + runLen] < 0x20 || pData[start + runLen] > 0x7E)) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
            continue;
        }
        
        j += 64;
    }
    
    /* Process remaining 16-byte chunks */
    while (j + 16 <= nBufferSize) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pData + j));
        __m128i vLt = _mm_cmpgt_epi8(vLower, vData);
        __m128i vGt = _mm_cmpgt_epi8(vData, vUpper);
        __m128i vNotAnsi = _mm_or_si128(vLt, vGt);
        xsimd_int32 nMask = _mm_movemask_epi8(vNotAnsi);
        
        if (nMask != 0) {
#ifdef _MSC_VER
            unsigned long nBitPos;
            _BitScanForward(&nBitPos, (unsigned long)nMask);
            xsimd_int64 start = j + nBitPos;
#else
            xsimd_int64 start = j + __builtin_ctz((unsigned int)nMask);
#endif
            xsimd_int64 runLen = 0;
            while ((start + runLen) < nBufferSize && (pData[start + runLen] < 0x20 || pData[start + runLen] > 0x7E)) {
                runLen++;
            }
            
            if (runLen >= nMinLength) {
                return nOffset + start;
            }
            
            j = start + runLen + 1;
        } else {
            j += 16;
        }
    }
    
    return j;
#else
    return 0;
#endif
}

int _xsimd_is_not_null_SSE2(const char* ptr, xsimd_int64 nSize)
{
#ifdef XSIMD_X86
    const __m128i zero = _mm_setzero_si128();
    
    /* Process 128 bytes per iteration for better throughput */
    while (nSize >= 128) {
        __m128i chunk0 = _mm_loadu_si128((const __m128i*)ptr);
        __m128i chunk1 = _mm_loadu_si128((const __m128i*)(ptr + 16));
        __m128i chunk2 = _mm_loadu_si128((const __m128i*)(ptr + 32));
        __m128i chunk3 = _mm_loadu_si128((const __m128i*)(ptr + 48));
        __m128i chunk4 = _mm_loadu_si128((const __m128i*)(ptr + 64));
        __m128i chunk5 = _mm_loadu_si128((const __m128i*)(ptr + 80));
        __m128i chunk6 = _mm_loadu_si128((const __m128i*)(ptr + 96));
        __m128i chunk7 = _mm_loadu_si128((const __m128i*)(ptr + 112));
        
        __m128i cmp0 = _mm_cmpeq_epi8(chunk0, zero);
        __m128i cmp1 = _mm_cmpeq_epi8(chunk1, zero);
        __m128i cmp2 = _mm_cmpeq_epi8(chunk2, zero);
        __m128i cmp3 = _mm_cmpeq_epi8(chunk3, zero);
        __m128i cmp4 = _mm_cmpeq_epi8(chunk4, zero);
        __m128i cmp5 = _mm_cmpeq_epi8(chunk5, zero);
        __m128i cmp6 = _mm_cmpeq_epi8(chunk6, zero);
        __m128i cmp7 = _mm_cmpeq_epi8(chunk7, zero);
        
        int mask0 = _mm_movemask_epi8(cmp0);
        int mask1 = _mm_movemask_epi8(cmp1);
        int mask2 = _mm_movemask_epi8(cmp2);
        int mask3 = _mm_movemask_epi8(cmp3);
        int mask4 = _mm_movemask_epi8(cmp4);
        int mask5 = _mm_movemask_epi8(cmp5);
        int mask6 = _mm_movemask_epi8(cmp6);
        int mask7 = _mm_movemask_epi8(cmp7);
        
        if (mask0 != 0 || mask1 != 0 || mask2 != 0 || mask3 != 0 ||
            mask4 != 0 || mask5 != 0 || mask6 != 0 || mask7 != 0) {
            return 0;
        }
        
        ptr += 128;
        nSize -= 128;
    }
    
    /* Process remaining 16-byte chunks */
    while (nSize >= 16) {
        __m128i chunk = _mm_loadu_si128((const __m128i*)ptr);
        __m128i cmp = _mm_cmpeq_epi8(chunk, zero);
        int mask = _mm_movemask_epi8(cmp);
        
        if (mask != 0) {
            return 0;
        }
        
        ptr += 16;
        nSize -= 16;
    }
    
    return -1;  /* Continue with fallback */
#else
    return -1;
#endif
}

int _xsimd_is_ansi_SSE2(const char* ptr, xsimd_int64 nSize)
{
#ifdef XSIMD_X86
    const __m128i low_bound = _mm_set1_epi8(0x20 - 1);
    const __m128i high_bound = _mm_set1_epi8(0x7F);
    
    /* Process 128 bytes per iteration for better throughput */
    while (nSize >= 128) {
        __m128i chunk0 = _mm_loadu_si128((const __m128i*)ptr);
        __m128i chunk1 = _mm_loadu_si128((const __m128i*)(ptr + 16));
        __m128i chunk2 = _mm_loadu_si128((const __m128i*)(ptr + 32));
        __m128i chunk3 = _mm_loadu_si128((const __m128i*)(ptr + 48));
        __m128i chunk4 = _mm_loadu_si128((const __m128i*)(ptr + 64));
        __m128i chunk5 = _mm_loadu_si128((const __m128i*)(ptr + 80));
        __m128i chunk6 = _mm_loadu_si128((const __m128i*)(ptr + 96));
        __m128i chunk7 = _mm_loadu_si128((const __m128i*)(ptr + 112));
        
        __m128i too_low0 = _mm_cmpgt_epi8(low_bound, chunk0);
        __m128i too_high0 = _mm_cmpgt_epi8(chunk0, high_bound);
        __m128i invalid0 = _mm_or_si128(too_low0, too_high0);
        
        __m128i too_low1 = _mm_cmpgt_epi8(low_bound, chunk1);
        __m128i too_high1 = _mm_cmpgt_epi8(chunk1, high_bound);
        __m128i invalid1 = _mm_or_si128(too_low1, too_high1);
        
        __m128i too_low2 = _mm_cmpgt_epi8(low_bound, chunk2);
        __m128i too_high2 = _mm_cmpgt_epi8(chunk2, high_bound);
        __m128i invalid2 = _mm_or_si128(too_low2, too_high2);
        
        __m128i too_low3 = _mm_cmpgt_epi8(low_bound, chunk3);
        __m128i too_high3 = _mm_cmpgt_epi8(chunk3, high_bound);
        __m128i invalid3 = _mm_or_si128(too_low3, too_high3);
        
        __m128i too_low4 = _mm_cmpgt_epi8(low_bound, chunk4);
        __m128i too_high4 = _mm_cmpgt_epi8(chunk4, high_bound);
        __m128i invalid4 = _mm_or_si128(too_low4, too_high4);
        
        __m128i too_low5 = _mm_cmpgt_epi8(low_bound, chunk5);
        __m128i too_high5 = _mm_cmpgt_epi8(chunk5, high_bound);
        __m128i invalid5 = _mm_or_si128(too_low5, too_high5);
        
        __m128i too_low6 = _mm_cmpgt_epi8(low_bound, chunk6);
        __m128i too_high6 = _mm_cmpgt_epi8(chunk6, high_bound);
        __m128i invalid6 = _mm_or_si128(too_low6, too_high6);
        
        __m128i too_low7 = _mm_cmpgt_epi8(low_bound, chunk7);
        __m128i too_high7 = _mm_cmpgt_epi8(chunk7, high_bound);
        __m128i invalid7 = _mm_or_si128(too_low7, too_high7);
        
        if (_mm_movemask_epi8(invalid0) != 0 || _mm_movemask_epi8(invalid1) != 0 ||
            _mm_movemask_epi8(invalid2) != 0 || _mm_movemask_epi8(invalid3) != 0 ||
            _mm_movemask_epi8(invalid4) != 0 || _mm_movemask_epi8(invalid5) != 0 ||
            _mm_movemask_epi8(invalid6) != 0 || _mm_movemask_epi8(invalid7) != 0) {
            return 0;
        }
        
        ptr += 128;
        nSize -= 128;
    }
    
    /* Process remaining 16-byte chunks */
    while (nSize >= 16) {
        __m128i chunk = _mm_loadu_si128((const __m128i*)ptr);
        __m128i too_low = _mm_cmpgt_epi8(low_bound, chunk);
        __m128i too_high = _mm_cmpgt_epi8(chunk, high_bound);
        __m128i invalid = _mm_or_si128(too_low, too_high);
        
        if (_mm_movemask_epi8(invalid) != 0) {
            return 0;
        }
        
        ptr += 16;
        nSize -= 16;
    }
    
    return -1;  /* Continue with fallback */
#else
    return -1;
#endif
}

int _xsimd_is_not_ansi_SSE2(const char* ptr, xsimd_int64 nSize)
{
#ifdef XSIMD_X86
    /* Process 128 bytes per iteration for better throughput */
    while (nSize >= 128) {
        __m128i chunk0 = _mm_loadu_si128((const __m128i*)ptr);
        __m128i chunk1 = _mm_loadu_si128((const __m128i*)(ptr + 16));
        __m128i chunk2 = _mm_loadu_si128((const __m128i*)(ptr + 32));
        __m128i chunk3 = _mm_loadu_si128((const __m128i*)(ptr + 48));
        __m128i chunk4 = _mm_loadu_si128((const __m128i*)(ptr + 64));
        __m128i chunk5 = _mm_loadu_si128((const __m128i*)(ptr + 80));
        __m128i chunk6 = _mm_loadu_si128((const __m128i*)(ptr + 96));
        __m128i chunk7 = _mm_loadu_si128((const __m128i*)(ptr + 112));
        
        __m128i ge_low0 = _mm_cmpgt_epi8(chunk0, _mm_set1_epi8(0x1F));
        __m128i le_high0 = _mm_cmpgt_epi8(_mm_set1_epi8((char)0x80), chunk0);
        __m128i printable0 = _mm_and_si128(ge_low0, le_high0);
        
        __m128i ge_low1 = _mm_cmpgt_epi8(chunk1, _mm_set1_epi8(0x1F));
        __m128i le_high1 = _mm_cmpgt_epi8(_mm_set1_epi8((char)0x80), chunk1);
        __m128i printable1 = _mm_and_si128(ge_low1, le_high1);
        
        __m128i ge_low2 = _mm_cmpgt_epi8(chunk2, _mm_set1_epi8(0x1F));
        __m128i le_high2 = _mm_cmpgt_epi8(_mm_set1_epi8((char)0x80), chunk2);
        __m128i printable2 = _mm_and_si128(ge_low2, le_high2);
        
        __m128i ge_low3 = _mm_cmpgt_epi8(chunk3, _mm_set1_epi8(0x1F));
        __m128i le_high3 = _mm_cmpgt_epi8(_mm_set1_epi8((char)0x80), chunk3);
        __m128i printable3 = _mm_and_si128(ge_low3, le_high3);
        
        __m128i ge_low4 = _mm_cmpgt_epi8(chunk4, _mm_set1_epi8(0x1F));
        __m128i le_high4 = _mm_cmpgt_epi8(_mm_set1_epi8((char)0x80), chunk4);
        __m128i printable4 = _mm_and_si128(ge_low4, le_high4);
        
        __m128i ge_low5 = _mm_cmpgt_epi8(chunk5, _mm_set1_epi8(0x1F));
        __m128i le_high5 = _mm_cmpgt_epi8(_mm_set1_epi8((char)0x80), chunk5);
        __m128i printable5 = _mm_and_si128(ge_low5, le_high5);
        
        __m128i ge_low6 = _mm_cmpgt_epi8(chunk6, _mm_set1_epi8(0x1F));
        __m128i le_high6 = _mm_cmpgt_epi8(_mm_set1_epi8((char)0x80), chunk6);
        __m128i printable6 = _mm_and_si128(ge_low6, le_high6);
        
        __m128i ge_low7 = _mm_cmpgt_epi8(chunk7, _mm_set1_epi8(0x1F));
        __m128i le_high7 = _mm_cmpgt_epi8(_mm_set1_epi8((char)0x80), chunk7);
        __m128i printable7 = _mm_and_si128(ge_low7, le_high7);
        
        if (_mm_movemask_epi8(printable0) != 0 || _mm_movemask_epi8(printable1) != 0 ||
            _mm_movemask_epi8(printable2) != 0 || _mm_movemask_epi8(printable3) != 0 ||
            _mm_movemask_epi8(printable4) != 0 || _mm_movemask_epi8(printable5) != 0 ||
            _mm_movemask_epi8(printable6) != 0 || _mm_movemask_epi8(printable7) != 0) {
            return 0;
        }
        
        ptr += 128;
        nSize -= 128;
    }
    
    /* Process remaining 16-byte chunks */
    while (nSize >= 16) {
        __m128i chunk = _mm_loadu_si128((const __m128i*)ptr);
        __m128i ge_low = _mm_cmpgt_epi8(chunk, _mm_set1_epi8(0x1F));
        __m128i le_high = _mm_cmpgt_epi8(_mm_set1_epi8((char)0x80), chunk);
        __m128i printable = _mm_and_si128(ge_low, le_high);
        
        if (_mm_movemask_epi8(printable) != 0) {
            return 0;
        }
        
        ptr += 16;
        nSize -= 16;
    }
    
    return -1;  /* Continue with fallback */
#else
    return -1;
#endif
}

int _xsimd_compare_memory_SSE2(const char* ptr1, const char* ptr2, xsimd_int64 nSize)
{
#ifdef XSIMD_X86
    /* Process 128 bytes per iteration for better throughput */
    while (nSize >= 128) {
        __m128i chunk1_0 = _mm_loadu_si128((const __m128i*)ptr1);
        __m128i chunk2_0 = _mm_loadu_si128((const __m128i*)ptr2);
        __m128i chunk1_1 = _mm_loadu_si128((const __m128i*)(ptr1 + 16));
        __m128i chunk2_1 = _mm_loadu_si128((const __m128i*)(ptr2 + 16));
        __m128i chunk1_2 = _mm_loadu_si128((const __m128i*)(ptr1 + 32));
        __m128i chunk2_2 = _mm_loadu_si128((const __m128i*)(ptr2 + 32));
        __m128i chunk1_3 = _mm_loadu_si128((const __m128i*)(ptr1 + 48));
        __m128i chunk2_3 = _mm_loadu_si128((const __m128i*)(ptr2 + 48));
        __m128i chunk1_4 = _mm_loadu_si128((const __m128i*)(ptr1 + 64));
        __m128i chunk2_4 = _mm_loadu_si128((const __m128i*)(ptr2 + 64));
        __m128i chunk1_5 = _mm_loadu_si128((const __m128i*)(ptr1 + 80));
        __m128i chunk2_5 = _mm_loadu_si128((const __m128i*)(ptr2 + 80));
        __m128i chunk1_6 = _mm_loadu_si128((const __m128i*)(ptr1 + 96));
        __m128i chunk2_6 = _mm_loadu_si128((const __m128i*)(ptr2 + 96));
        __m128i chunk1_7 = _mm_loadu_si128((const __m128i*)(ptr1 + 112));
        __m128i chunk2_7 = _mm_loadu_si128((const __m128i*)(ptr2 + 112));
        
        __m128i cmp0 = _mm_cmpeq_epi8(chunk1_0, chunk2_0);
        __m128i cmp1 = _mm_cmpeq_epi8(chunk1_1, chunk2_1);
        __m128i cmp2 = _mm_cmpeq_epi8(chunk1_2, chunk2_2);
        __m128i cmp3 = _mm_cmpeq_epi8(chunk1_3, chunk2_3);
        __m128i cmp4 = _mm_cmpeq_epi8(chunk1_4, chunk2_4);
        __m128i cmp5 = _mm_cmpeq_epi8(chunk1_5, chunk2_5);
        __m128i cmp6 = _mm_cmpeq_epi8(chunk1_6, chunk2_6);
        __m128i cmp7 = _mm_cmpeq_epi8(chunk1_7, chunk2_7);
        
        if (_mm_movemask_epi8(cmp0) != 0xFFFF ||
            _mm_movemask_epi8(cmp1) != 0xFFFF ||
            _mm_movemask_epi8(cmp2) != 0xFFFF ||
            _mm_movemask_epi8(cmp3) != 0xFFFF ||
            _mm_movemask_epi8(cmp4) != 0xFFFF ||
            _mm_movemask_epi8(cmp5) != 0xFFFF ||
            _mm_movemask_epi8(cmp6) != 0xFFFF ||
            _mm_movemask_epi8(cmp7) != 0xFFFF) {
            return 0;
        }
        
        ptr1 += 128;
        ptr2 += 128;
        nSize -= 128;
    }
    
    /* Process remaining 16-byte chunks */
    while (nSize >= 16) {
        __m128i chunk1 = _mm_loadu_si128((const __m128i*)ptr1);
        __m128i chunk2 = _mm_loadu_si128((const __m128i*)ptr2);
        __m128i cmp = _mm_cmpeq_epi8(chunk1, chunk2);
        
        if (_mm_movemask_epi8(cmp) != 0xFFFF) {
            return 0;
        }
        
        ptr1 += 16;
        ptr2 += 16;
        nSize -= 16;
    }
    
    return -1;  /* Continue with fallback */
#else
    return -1;
#endif
}

xsimd_int64 _xsimd_find_not_ansi_and_null_SSE2(const unsigned char* hay, xsimd_int64 hayLen, xsimd_int64 nOffset, xsimd_int64 m, const xsimd_int64 limit, const int* ansiTable, xsimd_int64* pj)
{
#ifdef XSIMD_X86
    const xsimd_int64 vectorSize = 16;
    __m128i vMin = _mm_set1_epi8(0x20);
    __m128i vMax = _mm_set1_epi8(0x7E);
    __m128i vZero = _mm_setzero_si128();
    xsimd_int64 j = *pj;
    
    while (j + vectorSize <= hayLen) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(hay + j));
        
        __m128i vLt = _mm_cmpgt_epi8(vMin, vData);
        __m128i vNotNull = _mm_cmpeq_epi8(vData, vZero);
        vLt = _mm_andnot_si128(vNotNull, vLt);
        
        __m128i vGt = _mm_cmpgt_epi8(vData, vMax);
        
        __m128i vNotAnsiAndNull = _mm_or_si128(vLt, vGt);
        
        xsimd_uint32 mask = _mm_movemask_epi8(vNotAnsiAndNull);
        
        if (mask != 0) {
            xsimd_int32 k;
            for (k = 0; k < vectorSize && j + k < limit; k++) {
                if (mask & (1U << k)) {
                    xsimd_int64 start = j + k;
                    xsimd_int64 runLen = 0;
                    xsimd_int64 r;
                    for (r = start; r < hayLen; r++) {
                        unsigned char c = hay[r];
                        if ((ansiTable[c]) || (c == 0)) break;
                        runLen++;
                        if (runLen >= m) {
                            return nOffset + start;
                        }
                    }
                }
            }
        }
        j += vectorSize;
    }
    
    *pj = j;
#endif
    return -1;
}

int _xsimd_is_not_ansi_and_null_SSE2(const char* ptr, xsimd_int64 nSize)
{
#ifdef XSIMD_X86
    const __m128i zero = _mm_setzero_si128();
    
    while (nSize >= 16) {
        __m128i chunk = _mm_loadu_si128((const __m128i*)ptr);
        
        /* Check for zero bytes */
        __m128i is_zero = _mm_cmpeq_epi8(chunk, zero);
        
        /* Check for printable ASCII */
        __m128i ge_low = _mm_cmpgt_epi8(chunk, _mm_set1_epi8(0x1F));
        __m128i le_high = _mm_cmpgt_epi8(_mm_set1_epi8((char)0x80), chunk);
        __m128i is_printable = _mm_and_si128(ge_low, le_high);
        
        __m128i invalid = _mm_or_si128(is_zero, is_printable);
        
        if (_mm_movemask_epi8(invalid) != 0) {
            return 0;
        }
        
        ptr += 16;
        nSize -= 16;
    }
    
    return -1;  /* Continue with fallback */
#else
    return -1;
#endif
}

xsimd_int64 _xsimd_find_ansi_number_SSE2(const unsigned char* hay, xsimd_int64 hayLen, xsimd_int64 nOffset, xsimd_int64 m, const xsimd_int64 limit, xsimd_int64* pj)
{
#ifdef XSIMD_X86
    const xsimd_int64 nVectorSize = 16;
    __m128i vMin = _mm_set1_epi8(0x30);
    __m128i vMax = _mm_set1_epi8(0x39);
    __m128i vOne = _mm_set1_epi8(1);
    xsimd_int64 j = *pj;
    
    while (j < limit) {
        int bFoundDigit = 0;
        
        for (; j + nVectorSize <= hayLen; j += nVectorSize) {
            __m128i vData = _mm_loadu_si128((const __m128i*)(hay + j));
            
            __m128i vGe = _mm_cmpgt_epi8(vData, _mm_sub_epi8(vMin, vOne));
            __m128i vLe = _mm_cmpgt_epi8(_mm_add_epi8(vMax, vOne), vData);
            __m128i vDigit = _mm_and_si128(vGe, vLe);
            
            xsimd_uint32 nMask = _mm_movemask_epi8(vDigit);
            
            if (nMask != 0) {
                xsimd_int32 i;
                for (i = 0; i < nVectorSize && j + i < hayLen; i++) {
                    if (nMask & (1U << i)) {
                        j += i;
                        bFoundDigit = 1;
                        break;
                    }
                }
                if (bFoundDigit) break;
            }
        }
        
        if (!bFoundDigit) {
            while (j < hayLen) {
                unsigned char c = hay[j];
                if ((c >= 0x30) && (c <= 0x39)) {
                    bFoundDigit = 1;
                    break;
                }
                j++;
            }
        }
        
        if (!bFoundDigit || j >= limit) break;
        
        xsimd_int64 start = j;
        
        while (j < hayLen) {
            unsigned char c = hay[j];
            if ((c < 0x30) || (c > 0x39)) break;
            j++;
        }
        
        xsimd_int64 runLen = j - start;
        if (runLen >= m) {
            return nOffset + start;
        }
    }
    
    *pj = j;
#endif
    return -1;
}

int _xsimd_is_ansi_number_SSE2(const char* ptr, xsimd_int64 nSize)
{
#ifdef XSIMD_X86
    const __m128i digit_low = _mm_set1_epi8(0x30);   /* '0' */
    const __m128i digit_high = _mm_set1_epi8(0x39);  /* '9' */
    
    while (nSize >= 16) {
        __m128i chunk = _mm_loadu_si128((const __m128i*)ptr);
        
        /* Check if any byte < '0' or > '9' */
        __m128i too_low = _mm_cmplt_epi8(chunk, digit_low);
        __m128i too_high = _mm_cmpgt_epi8(chunk, digit_high);
        __m128i invalid = _mm_or_si128(too_low, too_high);
        
        if (_mm_movemask_epi8(invalid) != 0) {
            return 0;
        }
        
        ptr += 16;
        nSize -= 16;
    }
    
    return -1;  /* Continue with fallback */
#else
    return -1;
#endif
}

xsimd_int64 _xsimd_find_first_non_ansi_SSE2(const xsimd_uint8* pData, xsimd_int64 i, xsimd_int64 nSize)
{
#ifdef XSIMD_X86
    __m128i vMin = _mm_set1_epi8(0x20);
    __m128i vMax = _mm_set1_epi8(0x7F);
    
    /* Process 16 bytes at a time */
    for (; i + 16 <= nSize; i += 16) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pData + i));
        
        __m128i vLessThanMin = _mm_cmplt_epi8(vData, vMin);
        __m128i vGreaterThanMax = _mm_cmpgt_epi8(vData, vMax);
        __m128i vNonAnsi = _mm_or_si128(vLessThanMin, vGreaterThanMax);
        
        int nMask = _mm_movemask_epi8(vNonAnsi);
        
        if (nMask != 0) {
            int nPos = 0;
            while (((nMask >> nPos) & 1) == 0) nPos++;
            return i + nPos;
        }
    }
    
    return -1;  /* Not found in SIMD portion */
#else
    return -1;
#endif
}

xsimd_int64 _xsimd_find_null_byte_SSE2(const xsimd_uint8* pData, xsimd_int64 nSize, xsimd_int64* pi)
{
#ifdef XSIMD_X86
    __m128i vZero = _mm_setzero_si128();
    xsimd_int64 i = *pi;
    
    for (; i + 16 <= nSize; i += 16) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pData + i));
        __m128i vCmp = _mm_cmpeq_epi8(vData, vZero);
        xsimd_uint16 nMask = _mm_movemask_epi8(vCmp);
        
        if (nMask != 0) {
            unsigned long nBitPos;
#ifdef _MSC_VER
            _BitScanForward(&nBitPos, nMask);
#else
            nBitPos = __builtin_ctz(nMask);
#endif
            return i + nBitPos;
        }
    }
    
    *pi = i;
#endif
    return -1;
}

xsimd_int64 _xsimd_count_unicode_prefix_SSE2(const xsimd_uint16* pData, xsimd_int64 nChars, xsimd_int64* pi)
{
#ifdef XSIMD_X86
    __m128i vMin1 = _mm_set1_epi16(0x0020);
    __m128i vMax1 = _mm_set1_epi16(0x00FF);
    __m128i vMin2 = _mm_set1_epi16(0x0400);
    __m128i vMax2 = _mm_set1_epi16(0x04FF);
    xsimd_int64 i = *pi;
    
    for (; i + 8 <= nChars; i += 8) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pData + i));
        
        __m128i vGe1 = _mm_cmpgt_epi16(vData, _mm_sub_epi16(vMin1, _mm_set1_epi16(1)));
        __m128i vLe1 = _mm_cmpgt_epi16(_mm_add_epi16(vMax1, _mm_set1_epi16(1)), vData);
        __m128i vRange1 = _mm_and_si128(vGe1, vLe1);
        
        __m128i vGe2 = _mm_cmpgt_epi16(vData, _mm_sub_epi16(vMin2, _mm_set1_epi16(1)));
        __m128i vLe2 = _mm_cmpgt_epi16(_mm_add_epi16(vMax2, _mm_set1_epi16(1)), vData);
        __m128i vRange2 = _mm_and_si128(vGe2, vLe2);
        
        __m128i vValid = _mm_or_si128(vRange1, vRange2);
        
        xsimd_uint16 nMask = _mm_movemask_epi8(vValid);
        
        if (nMask != 0xFFFF) {
            xsimd_int64 j;
            for (j = 0; j < 8; j++) {
                xsimd_uint16 nChar = pData[i + j];
                int bValid = ((nChar >= 0x0020 && nChar <= 0x00FF) || 
                             (nChar >= 0x0400 && nChar <= 0x04FF));
                if (!bValid) {
                    *pi = i + j;
                    return (i + j) * 2;
                }
            }
        }
    }
    
    *pi = i;
#endif
    return -1;
}

void _xsimd_count_char_SSE2(const xsimd_uint8* pData, xsimd_int64 nSize, xsimd_uint8 nByte, xsimd_int64* pi, xsimd_int64* pnCount)
{
#ifdef XSIMD_X86
    __m128i vNeedle = _mm_set1_epi8(nByte);
    xsimd_int64 i = *pi;
    
    for (; i + 16 <= nSize; i += 16) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pData + i));
        __m128i vCmp = _mm_cmpeq_epi8(vData, vNeedle);
        xsimd_uint16 nMask = _mm_movemask_epi8(vCmp);
        
#ifdef _MSC_VER
        *pnCount += __popcnt(nMask);
#else
        *pnCount += __builtin_popcount(nMask);
#endif
    }
    
    *pi = i;
#endif
}

void _xsimd_create_ansi_mask_SSE2(const xsimd_uint8* pData, xsimd_int64 nSize, xsimd_uint8* pMaskData, xsimd_int64* pi, xsimd_int64* pnAnsiCount)
{
#ifdef XSIMD_X86
    __m128i vMin = _mm_set1_epi8(0x20);
    __m128i vMax = _mm_set1_epi8(0x7F);
    xsimd_int64 i = *pi;
    
    for (; i + 16 <= nSize; i += 16) {
        __m128i vData = _mm_loadu_si128((const __m128i*)(pData + i));
        
        __m128i vGreaterEqMin = _mm_cmpgt_epi8(vData, _mm_sub_epi8(vMin, _mm_set1_epi8(1)));
        __m128i vLessThanMax = _mm_cmpgt_epi8(vMax, vData);
        __m128i vIsAnsi = _mm_and_si128(vGreaterEqMin, vLessThanMax);
        
        xsimd_uint16 nMask = _mm_movemask_epi8(vIsAnsi);
        
        pMaskData[i / 8] = (xsimd_uint8)(nMask & 0xFF);
        pMaskData[i / 8 + 1] = (xsimd_uint8)((nMask >> 8) & 0xFF);
        
#ifdef _MSC_VER
        *pnAnsiCount += __popcnt(nMask);
#else
        *pnAnsiCount += __builtin_popcount(nMask);
#endif
    }
    
    *pi = i;
#endif
}

xsimd_int32 _xsimd_compare_sigbytes_SSE2(const xsimd_uint8 *pSigBytes, xsimd_int64 nSigBytesSize, const xsimd_uint8 *pData, xsimd_int64 nDataSize, const xsimd_uint8 *pAlphaNumTable)
{
#ifdef USE_SSE2
    if (!pData || !pSigBytes || !pAlphaNumTable || nSigBytesSize < 2 || (nSigBytesSize % 2) != 0) {
        return 0;
    }
    
    xsimd_int64 nPatternLength = nSigBytesSize / 2;
    
    if (nDataSize < nPatternLength) {
        return 0;
    }
    
    // Constants for ANSI range check (0x20 to 0x7E)
    const __m128i vAnsiMin = _mm_set1_epi8(0x20);
    const __m128i vAnsiMax = _mm_set1_epi8(0x7E);
    const __m128i vZero = _mm_setzero_si128();
    
    // Process signature pattern byte by byte
    for (xsimd_int64 i = 0; i < nPatternLength; i++) {
        xsimd_uint8 nType = pSigBytes[i * 2];
        xsimd_uint8 nValue = pSigBytes[i * 2 + 1];
        xsimd_uint8 nDataByte = pData[i];
        
        switch (nType) {
            case 0:  // HEX - exact match
                if (nDataByte != nValue) {
                    return 0;
                }
                break;
                
            case 1:  // WILDCARD - always matches
                break;
                
            case 2:  // ANSI - check if in range 0x20-0x7E
            {
                // Use SIMD for single byte check (will be optimized by compiler)
                __m128i vData = _mm_set1_epi8(nDataByte);
                __m128i vGte = _mm_cmpgt_epi8(vData, _mm_subs_epi8(vAnsiMin, _mm_set1_epi8(1)));
                __m128i vLte = _mm_cmpgt_epi8(_mm_adds_epi8(vAnsiMax, _mm_set1_epi8(1)), vData);
                __m128i vInRange = _mm_and_si128(vGte, vLte);
                xsimd_int32 nMask = _mm_movemask_epi8(vInRange);
                if (nMask != 0xFFFF) {
                    return 0;
                }
                break;
            }
            
            case 3:  // NOT_ANSI - check if NOT in range 0x20-0x7E
            {
                __m128i vData = _mm_set1_epi8(nDataByte);
                __m128i vGte = _mm_cmpgt_epi8(vData, _mm_subs_epi8(vAnsiMin, _mm_set1_epi8(1)));
                __m128i vLte = _mm_cmpgt_epi8(_mm_adds_epi8(vAnsiMax, _mm_set1_epi8(1)), vData);
                __m128i vInRange = _mm_and_si128(vGte, vLte);
                xsimd_int32 nMask = _mm_movemask_epi8(vInRange);
                if (nMask == 0xFFFF) {
                    return 0;  // It IS in ANSI range, but we want NOT_ANSI
                }
                break;
            }
            
            case 4:  // NOT_ANSI_AND_NOT_NULL
            {
                if (nDataByte == 0x00) {
                    return 0;  // Is null
                }
                __m128i vData = _mm_set1_epi8(nDataByte);
                __m128i vGte = _mm_cmpgt_epi8(vData, _mm_subs_epi8(vAnsiMin, _mm_set1_epi8(1)));
                __m128i vLte = _mm_cmpgt_epi8(_mm_adds_epi8(vAnsiMax, _mm_set1_epi8(1)), vData);
                __m128i vInRange = _mm_and_si128(vGte, vLte);
                xsimd_int32 nMask = _mm_movemask_epi8(vInRange);
                if (nMask == 0xFFFF) {
                    return 0;  // Is in ANSI range
                }
                break;
            }
            
            case 5:  // ANSI_ALPHANUMERIC
            {
                // Use lookup table for fast check
                if (!pAlphaNumTable[nDataByte]) {
                    return 0;
                }
                break;
            }
            
            case 6:  // NOT_NULL
                if (nDataByte == 0x00) {
                    return 0;
                }
                break;
                
            default:
                return 0;  // Unknown type
        }
    }
    
    return 1;  // All checks passed
#else
    return -1;  // Not available
#endif
}

xsimd_int64 _xsimd_find_sigbytes_SSE2(const xsimd_uint8 *pData, xsimd_int64 nDataSize, const xsimd_uint8 *pSigBytes, xsimd_int64 nSigBytesSize, const xsimd_uint8 *pAlphaNumTable)
{
#ifdef USE_SSE2
    if (!pData || !pSigBytes || !pAlphaNumTable || nSigBytesSize < 2 || (nSigBytesSize % 2) != 0) {
        return -1;
    }
    
    xsimd_int64 nPatternLength = nSigBytesSize / 2;
    
    if (nDataSize < nPatternLength) {
        return -1;
    }
    
    xsimd_int64 nSearchEnd = nDataSize - nPatternLength + 1;
    
    /* Check first signature byte type to determine scan strategy */
    xsimd_uint8 nFirstType = pSigBytes[0];
    xsimd_uint8 nFirstValue = pSigBytes[1];
    
    /* If first byte is HEX, use SIMD to scan for that specific byte */
    if (nFirstType == 0) {  /* HEX type */
        __m128i vFirstByte = _mm_set1_epi8(nFirstValue);
        xsimd_int64 i = 0;
        
        /* Process 16 bytes at a time with SSE2 */
        for (; i + 16 <= nSearchEnd; i += 16) {
            __m128i vData = _mm_loadu_si128((const __m128i*)(pData + i));
            __m128i vCmp = _mm_cmpeq_epi8(vData, vFirstByte);
            xsimd_int32 nMask = _mm_movemask_epi8(vCmp);
            
            /* Check each matching position */
            while (nMask != 0) {
                xsimd_int32 nBitPos = 0;
#ifdef _MSC_VER
                unsigned long nIndex;
                _BitScanForward(&nIndex, nMask);
                nBitPos = (xsimd_int32)nIndex;
#else
                nBitPos = __builtin_ctz(nMask);
#endif
                xsimd_int64 nCandidatePos = i + nBitPos;
                
                /* Verify full pattern at this position */
                if (nCandidatePos + nPatternLength <= nDataSize) {
                    if (_xsimd_compare_sigbytes_SSE2(pSigBytes, nSigBytesSize, 
                                                      pData + nCandidatePos, nPatternLength, 
                                                      pAlphaNumTable) == 1) {
                        return nCandidatePos;
                    }
                }
                
                /* Clear this bit and continue */
                nMask &= nMask - 1;
            }
        }
        
        /* Handle remaining bytes with scalar scan */
        for (; i < nSearchEnd; i++) {
            if (pData[i] == nFirstValue) {
                if (_xsimd_compare_sigbytes_SSE2(pSigBytes, nSigBytesSize, 
                                                  pData + i, nPatternLength, 
                                                  pAlphaNumTable) == 1) {
                    return i;
                }
            }
        }
    } else {
        /* For non-HEX first byte (wildcard, ANSI, etc.), do linear scan */
        /* This is still faster than scalar due to SIMD comparison */
        for (xsimd_int64 i = 0; i < nSearchEnd; i++) {
            if (_xsimd_compare_sigbytes_SSE2(pSigBytes, nSigBytesSize, 
                                              pData + i, nPatternLength, 
                                              pAlphaNumTable) == 1) {
                return i;
            }
        }
    }
    
    return -1;  /* Not found */
#else
    return -1;  /* Not available */
#endif
}

