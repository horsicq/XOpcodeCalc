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
#include "xsimd.h"
#include "xsimd_sse2.h"
#include "xsimd_avx2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#define XSIMD_X86
#include <immintrin.h>
#endif

static int g_bInitialized = 0;
static xsimd_uint32 g_nFeatures = XSIMD_FEATURE_NONE;
static xsimd_uint32 g_nEnabledFeatures = XSIMD_FEATURE_NONE;

#ifdef XSIMD_X86
static void xsimd_detect_features(void)
{
    int nInfo[4];
    
#ifdef _MSC_VER
    /* CPUID function 1: Basic processor info */
    __cpuid(nInfo, 1);
    
    /* EDX register (bits in nInfo[3]) */
    if (nInfo[3] & (1 << 26)) g_nFeatures |= XSIMD_FEATURE_SSE2;   /* SSE2 */
    
    /* Check AVX support (requires both CPUID and OS support) */
    int bOSXSAVE = nInfo[2] & (1 << 27);  /* OS supports XSAVE (bit 27) */
    int bAVXCPUID = nInfo[2] & (1 << 28); /* CPU supports AVX (bit 28) */
    
    if (bOSXSAVE && bAVXCPUID) {
        /* Check if OS has enabled AVX state saving via XCR0 */
        unsigned long long nXCR0 = _xgetbv(0);
        
        if ((nXCR0 & 0x6) == 0x6) {  /* XMM (bit 1) and YMM (bit 2) state enabled */
            /* Check AVX2 (CPUID function 7, subleaf 0, EBX bit 5) */
            __cpuidex(nInfo, 7, 0);
            
            if (nInfo[1] & (1 << 5)) {
                g_nFeatures |= XSIMD_FEATURE_AVX2;
            }
        }
    }
#else
    /* GCC/Clang builtin CPU detection */
    __builtin_cpu_init();
    
    if (__builtin_cpu_supports("sse2"))   g_nFeatures |= XSIMD_FEATURE_SSE2;
    if (__builtin_cpu_supports("avx2"))   g_nFeatures |= XSIMD_FEATURE_AVX2;
#endif
}
#endif

int xsimd_init(void)
{
    if (g_bInitialized) {
        return 0;
    }
    
#ifdef XSIMD_X86
    xsimd_detect_features();
#endif
    
    /* Enable all detected features by default */
    g_nEnabledFeatures = g_nFeatures;
    
    g_bInitialized = 1;
    return 0;
}

const char* xsimd_version(void)
{
    static char sVersion[32];
#ifdef _MSC_VER
    _snprintf(sVersion, sizeof(sVersion), "%d.%d.%d",
              XSIMD_VERSION_MAJOR,
              XSIMD_VERSION_MINOR,
              XSIMD_VERSION_PATCH);
#else
    snprintf(sVersion, sizeof(sVersion), "%d.%d.%d",
             XSIMD_VERSION_MAJOR,
             XSIMD_VERSION_MINOR,
             XSIMD_VERSION_PATCH);
#endif
    return sVersion;
}

xsimd_uint32 xsimd_get_features(void)
{
    return g_nFeatures;
}

xsimd_uint32 xsimd_get_enabled_features(void)
{
    return g_nEnabledFeatures;
}

int xsimd_enable_features(xsimd_uint32 nFeatures)
{
    /* Check if trying to enable features that aren't detected */
    if ((nFeatures & ~g_nFeatures) != 0) {
        return -1;  /* Error: trying to enable unavailable features */
    }
    
    g_nEnabledFeatures |= nFeatures;
    return 0;
}

void xsimd_disable_features(xsimd_uint32 nFeatures)
{
    g_nEnabledFeatures &= ~nFeatures;
}

void xsimd_enable_all_features(void)
{
    g_nEnabledFeatures = g_nFeatures;
}

void xsimd_disable_all_features(void)
{
    g_nEnabledFeatures = XSIMD_FEATURE_NONE;
}

int xsimd_is_sse2_enabled(void)
{
    return (g_nEnabledFeatures & XSIMD_FEATURE_SSE2) != 0;
}

int xsimd_is_avx2_enabled(void)
{
    return (g_nEnabledFeatures & XSIMD_FEATURE_AVX2) != 0;
}

int xsimd_is_sse2_present(void)
{
    return (g_nFeatures & XSIMD_FEATURE_SSE2) != 0;
}

int xsimd_is_avx2_present(void)
{
    return (g_nFeatures & XSIMD_FEATURE_AVX2) != 0;
}

void xsimd_set_sse2(int bState)
{
    if (bState) {
        xsimd_enable_features(XSIMD_FEATURE_SSE2);
    } else {
        xsimd_disable_features(XSIMD_FEATURE_SSE2);
    }
}

void xsimd_set_avx2(int bState)
{
    if (bState) {
        xsimd_enable_features(XSIMD_FEATURE_AVX2);
    } else {
        xsimd_disable_features(XSIMD_FEATURE_AVX2);
    }
}

xsimd_int64 xsimd_find_byte(const void* pBuffer, xsimd_int64 nSize, xsimd_uint8 nByte, xsimd_int64 nOffset)
{
    const unsigned char* pData = (const unsigned char*)pBuffer;
    xsimd_int64 i = 0;
    
#ifdef XSIMD_X86
    /* AVX2: Process 32 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && nSize >= 32) {
        i = _xsimd_find_byte_AVX2(pData, nSize, nByte, nOffset);

        if (i < nSize && pData[i] == nByte) {
            return nOffset + i;
        }
    }
    /* SSE2: Process 16 bytes at a time */
    else if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && nSize >= 16) {
        i = _xsimd_find_byte_SSE2(pData, nSize, nByte, nOffset);

        if (i < nSize && pData[i] == nByte) {
            return nOffset + i;
        }
    }
#endif
    
    /* Fallback for remaining bytes */
    const void* pFound = memchr(pData + i, nByte, (size_t)(nSize - i));
    if (pFound) {
        return nOffset + ((const char*)pFound - (const char*)pBuffer);
    }
    
    return -1;
}

xsimd_int64 xsimd_find_pattern_bmh(const void* pBuffer, xsimd_int64 nBufferSize, 
                                   const void* pPattern, xsimd_int64 nPatternSize, 
                                   xsimd_int64 nOffset)
{
    if (nPatternSize == 0 || nBufferSize < nPatternSize) {
        return -1;
    }
    
    const char* pHay = (const char*)pBuffer;
    const char* pNeedle = (const char*)pPattern;
    const xsimd_int64 nLimit = nBufferSize - nPatternSize;
    const char nLastChar = pNeedle[nPatternSize - 1];
    xsimd_int64 i = 0;
    
#ifdef XSIMD_X86
    /* Special handling for 1-byte patterns using SIMD */
    if (nPatternSize == 1 && (g_nEnabledFeatures & XSIMD_FEATURE_AVX2)) {
        return _xsimd_find_pattern_bmh_1byte_AVX2(pHay, nBufferSize, pNeedle[0], nOffset);
    }
    
    /* Special handling for 2-byte patterns using SIMD */
    if (nPatternSize == 2 && (g_nEnabledFeatures & XSIMD_FEATURE_AVX2)) {
        xsimd_uint16 pattern16 = *(const xsimd_uint16*)pNeedle;
        return _xsimd_find_pattern_bmh_2byte_AVX2(pHay, nBufferSize, pattern16, nOffset);
    }
    
    /* Special handling for 3-byte patterns using hybrid SIMD + scalar */
    if (nPatternSize == 3 && (g_nEnabledFeatures & XSIMD_FEATURE_AVX2)) {
        return _xsimd_find_pattern_bmh_3byte_AVX2(pHay, nBufferSize, pNeedle, nOffset);
    }
    
    /* AVX2: Process 64 bytes at a time (2 × 32-byte vectors) */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && nPatternSize >= 4) {
        xsimd_int64 result = _xsimd_find_pattern_bmh_AVX2(pHay, nBufferSize, pNeedle, nPatternSize, nOffset, nLimit, nLastChar, &i);
        if (result != -1) {
            return result;
        }
        /* SIMD didn't find it, continue with scalar fallback from position i */
    }
    /* SSE2: Special handling for 1-byte patterns */
    else if (nPatternSize == 1 && (g_nEnabledFeatures & XSIMD_FEATURE_SSE2)) {
        return _xsimd_find_pattern_bmh_1byte_SSE2(pHay, nBufferSize, pNeedle[0], nOffset);
    }
    /* SSE2: Special handling for 2-byte patterns */
    else if (nPatternSize == 2 && (g_nEnabledFeatures & XSIMD_FEATURE_SSE2)) {
        xsimd_uint16 pattern16 = *(const xsimd_uint16*)pNeedle;
        return _xsimd_find_pattern_bmh_2byte_SSE2(pHay, nBufferSize, pattern16, nOffset);
    }
    /* SSE2: Special handling for 3-byte patterns */
    else if (nPatternSize == 3 && (g_nEnabledFeatures & XSIMD_FEATURE_SSE2)) {
        return _xsimd_find_pattern_bmh_3byte_SSE2(pHay, nBufferSize, pNeedle, nOffset);
    }
    /* SSE2: Process 64 bytes at a time (4 × 16-byte vectors) */
    else if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && nPatternSize >= 4) {
        xsimd_int64 result = _xsimd_find_pattern_bmh_SSE2(pHay, nBufferSize, pNeedle, nPatternSize, nOffset, nLimit, nLastChar, &i);
        if (result != -1) {
            return result;
        }
        /* SIMD didn't find it, continue with scalar fallback from position i */
    }
#endif
    
    /* Fallback: simple search for remaining bytes */
    while (i <= nLimit) {
        if (pHay[i + nPatternSize - 1] == nLastChar) {
            if (memcmp(pHay + i, pNeedle, (size_t)nPatternSize) == 0) {
                return nOffset + i;
            }
        }
        i++;
    }
    
    return -1;
}

xsimd_int64 xsimd_find_ansi(const void* pBuffer, xsimd_int64 nBufferSize, 
                            xsimd_int64 nMinLength, xsimd_int64 nOffset)
{
    const unsigned char* pData = (const unsigned char*)pBuffer;
    const xsimd_int64 nLimit = nBufferSize - (nMinLength - 1);
    xsimd_int64 j = 0;
    
#ifdef XSIMD_X86
    /* AVX2: Process 32 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && nBufferSize >= 32) {
        j = _xsimd_find_ansi_AVX2(pData, nBufferSize, nMinLength, nOffset);
        if (j >= 0 && j < nBufferSize) {
            return j;
        }
    }
    /* SSE2: Process 16 bytes at a time */
    else if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && nBufferSize >= 16) {
        j = _xsimd_find_ansi_SSE2(pData, nBufferSize, nMinLength, nOffset);
        if (j >= 0 && j < nBufferSize) {
            return j;
        }
    }
#endif
    
    /* Fallback */
    while (j < nLimit) {
        while (j < nBufferSize && (pData[j] < 0x20 || pData[j] > 0x7E)) j++;
        if (j >= nLimit) break;
        
        xsimd_int64 start = j;
        while (j < nBufferSize && pData[j] >= 0x20 && pData[j] <= 0x7E) j++;
        
        if ((j - start) >= nMinLength) {
            return nOffset + start;
        }
    }
    
    return -1;
}

xsimd_int64 xsimd_find_notnull(const void* pBuffer, xsimd_int64 nBufferSize, 
                               xsimd_int64 nMinLength, xsimd_int64 nOffset)
{
    const unsigned char* pData = (const unsigned char*)pBuffer;
    xsimd_int64 j = 0;
    xsimd_int64 runStart = -1;  // Track start of current non-null run
    
    /* Fast path for very short minimum lengths (1-3 bytes) */
    /* SIMD overhead outweighs benefits for tiny searches */
    if (nMinLength <= 3) {
        /* Skip leading nulls */
        while (j < nBufferSize && pData[j] == 0) j++;
        if (j >= nBufferSize) return -1;
        
        /* For length 1, we're done - found first non-null */
        if (nMinLength == 1) return nOffset + j;
        
        /* For length 2-3, verify consecutive non-null bytes */
        runStart = j;
        j++;
        
        while (j < nBufferSize) {
            if (pData[j] == 0) {
                xsimd_int64 runLen = j - runStart;
                if (runLen >= nMinLength) return nOffset + runStart;
                
                /* Skip past null byte(s) and find next non-null */
                j++;
                while (j < nBufferSize && pData[j] == 0) j++;
                if (j >= nBufferSize) return -1;
                runStart = j;
            }
            j++;
        }
        
        /* Check final run */
        xsimd_int64 runLen = j - runStart;
        if (runLen >= nMinLength) return nOffset + runStart;
        return -1;
    }
    
#ifdef XSIMD_X86
    /* AVX2: Process 32 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && nBufferSize >= 32) {
        xsimd_int64 result = _xsimd_find_notnull_AVX2(pData, nBufferSize, nMinLength, nOffset, j, runStart);
        if (result < 0) return result;
        j = result & 0xFFFFFFFF;
        runStart = (result >> 32) & 0xFFFFFFFF;
    }
    /* SSE2: Process 16 bytes at a time */
    else if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && nBufferSize >= 16) {
        xsimd_int64 result = _xsimd_find_notnull_SSE2(pData, nBufferSize, nMinLength, nOffset, j, runStart);
        if (result < 0) return result;
        j = result & 0xFFFFFFFF;
        runStart = (result >> 32) & 0xFFFFFFFF;
    }
#endif
    
    /* Fallback: Process remaining bytes scalar */
    if (runStart == -1) {
        /* Haven't started processing yet, skip leading nulls */
        while (j < nBufferSize && pData[j] == 0) j++;
        if (j >= nBufferSize) return -1;
        runStart = j;
    }
    
    /* Check remaining bytes */
    while (j < nBufferSize) {
        if (pData[j] == 0) {
            xsimd_int64 runLen = j - runStart;
            if (runLen >= nMinLength) {
                return nOffset + runStart;
            }
            /* Skip past null byte(s) */
            j++;
            while (j < nBufferSize && pData[j] == 0) j++;
            if (j >= nBufferSize) return -1;
            runStart = j;
        } else {
            j++;
        }
    }
    
    /* Check final run */
    xsimd_int64 runLen = j - runStart;
    if (runLen >= nMinLength) {
        return nOffset + runStart;
    }
    
    return -1;
}

xsimd_int64 xsimd_find_not_ansi(const void* pBuffer, xsimd_int64 nBufferSize, 
                                xsimd_int64 nMinLength, xsimd_int64 nOffset)
{
    const unsigned char* pData = (const unsigned char*)pBuffer;
    const xsimd_int64 nLimit = nBufferSize - (nMinLength - 1);
    xsimd_int64 j = 0;
    
#ifdef XSIMD_X86
    /* AVX2: Process 32 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && nBufferSize >= 32) {
        j = _xsimd_find_not_ansi_AVX2(pData, nBufferSize, nMinLength, nOffset);
        if (j >= 0 && j < nBufferSize) {
            return j;
        }
    }
    /* SSE2: Process 16 bytes at a time */
    else if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && nBufferSize >= 16) {
        j = _xsimd_find_not_ansi_SSE2(pData, nBufferSize, nMinLength, nOffset);
        if (j >= 0 && j < nBufferSize) {
            return j;
        }
    }
#endif
    
    /* Fallback */
    while (j < nLimit) {
        while (j < nBufferSize && (pData[j] >= 0x20 && pData[j] <= 0x7E)) j++;
        if (j >= nLimit) break;
        
        xsimd_int64 start = j;
        while (j < nBufferSize && (pData[j] < 0x20 || pData[j] > 0x7E)) j++;
        
        if ((j - start) >= nMinLength) {
            return nOffset + start;
        }
    }
    
    return -1;
}

#ifdef XSIMD_X86
#endif

int xsimd_is_not_null(const void* pBuffer, xsimd_int64 nSize)
{
    const char* ptr = (const char*)pBuffer;
    
#ifdef XSIMD_X86
    /* AVX2: Process 32 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && nSize >= 32) {
        int result = _xsimd_is_not_null_AVX2(ptr, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update ptr and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 32) * 32;
        ptr += nProcessed;
        nSize -= nProcessed;
    }
    /* SSE2: Process 16 bytes at a time */
    else if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && nSize >= 16) {
        int result = _xsimd_is_not_null_SSE2(ptr, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update ptr and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 16) * 16;
        ptr += nProcessed;
        nSize -= nProcessed;
    }
#endif
    
    /* Fallback */
    while (nSize > 0) {
        if (*ptr == 0) {
            return 0;
        }
        ptr++;
        nSize--;
    }
    
    return 1;
}

int xsimd_is_ansi(const void* pBuffer, xsimd_int64 nSize)
{
    const char* ptr = (const char*)pBuffer;
    
#ifdef XSIMD_X86
    /* AVX2: Process 32 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && nSize >= 32) {
        int result = _xsimd_is_ansi_AVX2(ptr, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update ptr and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 32) * 32;
        ptr += nProcessed;
        nSize -= nProcessed;
    }
    /* SSE2: Process 16 bytes at a time */
    else if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && nSize >= 16) {
        int result = _xsimd_is_ansi_SSE2(ptr, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update ptr and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 16) * 16;
        ptr += nProcessed;
        nSize -= nProcessed;
    }
#endif
    
    /* Fallback */
    const xsimd_uint8* pData = (const xsimd_uint8*)ptr;
    while (nSize > 0) {
        if (*pData < 0x20 || *pData >= 0x80) {
            return 0;
        }
        pData++;
        nSize--;
    }
    
    return 1;
}

int xsimd_is_not_ansi(const void* pBuffer, xsimd_int64 nSize)
{
    const char* ptr = (const char*)pBuffer;
    
#ifdef XSIMD_X86
    /* AVX2: Process 32 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && nSize >= 32) {
        int result = _xsimd_is_not_ansi_AVX2(ptr, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update ptr and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 32) * 32;
        ptr += nProcessed;
        nSize -= nProcessed;
    }
    /* SSE2: Process 16 bytes at a time */
    else if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && nSize >= 16) {
        int result = _xsimd_is_not_ansi_SSE2(ptr, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update ptr and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 16) * 16;
        ptr += nProcessed;
        nSize -= nProcessed;
    }
#endif
    
    /* Fallback */
    const xsimd_uint8* pData = (const xsimd_uint8*)ptr;
    while (nSize > 0) {
        if (*pData >= 0x20 && *pData < 0x80) {
            return 0;
        }
        pData++;
        nSize--;
    }
    
    return 1;
}

#ifdef XSIMD_X86
#endif

int xsimd_compare_memory(const void* pBuffer1, const void* pBuffer2, xsimd_int64 nSize)
{
    const char* ptr1 = (const char*)pBuffer1;
    const char* ptr2 = (const char*)pBuffer2;
    
#ifdef XSIMD_X86
    /* AVX2: Process 32 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && nSize >= 32) {
        int result = _xsimd_compare_memory_AVX2(ptr1, ptr2, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update pointers and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 32) * 32;
        ptr1 += nProcessed;
        ptr2 += nProcessed;
        nSize -= nProcessed;
    }
    /* SSE2: Process 16 bytes at a time */
    else if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && nSize >= 16) {
        int result = _xsimd_compare_memory_SSE2(ptr1, ptr2, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update pointers and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 16) * 16;
        ptr1 += nProcessed;
        ptr2 += nProcessed;
        nSize -= nProcessed;
    }
#endif
    
    /* Fallback: use memcmp */
    return (memcmp(ptr1, ptr2, (size_t)nSize) == 0) ? 1 : 0;
}

int xsimd_compare_sigbytes(const void* pSigBytes, xsimd_int64 nSigBytesSize, const void* pData, xsimd_int64 nDataSize, const void* pAlphaNumTable)
{
    if (!pSigBytes || !pData || !pAlphaNumTable) {
        return 0;  /* Invalid input */
    }
    
    /* SigBytes must be even size (type+value pairs) */
    if ((nSigBytesSize % 2) != 0) {
        return 0;
    }
    
    /* Pattern length = nSigBytesSize / 2 */
    xsimd_int64 nPatternLength = nSigBytesSize / 2;
    
    /* Data must be at least as long as pattern */
    if (nDataSize < nPatternLength) {
        return 0;
    }
    
#ifdef XSIMD_X86
    /* Use SIMD if pattern is large enough */
    if (nPatternLength >= 8) {
        /* Try AVX2 first (processes 32 bytes at a time) */
        if (g_nEnabledFeatures & XSIMD_FEATURE_AVX2) {
            return _xsimd_compare_sigbytes_AVX2((const xsimd_uint8*)pSigBytes, nSigBytesSize, 
                                                 (const xsimd_uint8*)pData, nDataSize, 
                                                 (const xsimd_uint8*)pAlphaNumTable);
        }
        /* Fall back to SSE2 (processes 16 bytes at a time) */
        else if (g_nEnabledFeatures & XSIMD_FEATURE_SSE2) {
            return _xsimd_compare_sigbytes_SSE2((const xsimd_uint8*)pSigBytes, nSigBytesSize, 
                                                 (const xsimd_uint8*)pData, nDataSize, 
                                                 (const xsimd_uint8*)pAlphaNumTable);
        }
    }
#endif
    
    /* For small patterns or no SIMD, use scalar implementation */
    /* This fallback is handled in xbinary.cpp _compareSigBytes */
    return -1;  /* Indicates caller should use scalar path */
}

xsimd_int64 xsimd_find_sigbytes(const void* pData, xsimd_int64 nDataSize, const void* pSigBytes, xsimd_int64 nSigBytesSize, const void* pAlphaNumTable)
{
    if (!pData || !pSigBytes || !pAlphaNumTable) {
        return -1;  /* Invalid input */
    }
    
    /* SigBytes must be even size (type+value pairs) */
    if ((nSigBytesSize % 2) != 0) {
        return -1;
    }
    
    /* Pattern length = nSigBytesSize / 2 */
    xsimd_int64 nPatternLength = nSigBytesSize / 2;
    
    /* Data must be at least as long as pattern */
    if (nDataSize < nPatternLength) {
        return -1;
    }
    
#ifdef XSIMD_X86
    /* Use SIMD if pattern is large enough (>= 4 bytes) */
    if (nPatternLength >= 4) {
        /* Try AVX2 first (faster for larger data) */
        if (g_nEnabledFeatures & XSIMD_FEATURE_AVX2) {
            return _xsimd_find_sigbytes_AVX2((const xsimd_uint8*)pData, nDataSize, 
                                              (const xsimd_uint8*)pSigBytes, nSigBytesSize, 
                                              (const xsimd_uint8*)pAlphaNumTable);
        }
        /* Fall back to SSE2 */
        else if (g_nEnabledFeatures & XSIMD_FEATURE_SSE2) {
            return _xsimd_find_sigbytes_SSE2((const xsimd_uint8*)pData, nDataSize, 
                                              (const xsimd_uint8*)pSigBytes, nSigBytesSize, 
                                              (const xsimd_uint8*)pAlphaNumTable);
        }
    }
#endif
    
    /* For small patterns or no SIMD, use scalar implementation */
    /* This fallback is handled in xbinary.cpp _findSigBytes */
    return -1;  /* Indicates caller should use scalar path */
}


xsimd_int64 xsimd_find_not_ansi_and_null(const void* pBuffer, xsimd_int64 nBufferSize, 
                                         xsimd_int64 nMinLength, xsimd_int64 nOffset)
{
    if (!pBuffer || nBufferSize <= 0 || nMinLength <= 0) {
        return -1;
    }
    
    const unsigned char* hay = (const unsigned char*)pBuffer;
    xsimd_int64 hayLen = nBufferSize;
    xsimd_int64 m = nMinLength;
    const xsimd_int64 limit = hayLen - (m - 1);
    xsimd_int64 j = 0;
    
    /* ANSI table for quick lookups */
    int ansiTable[256];
    int i;
    for (i = 0; i < 256; i++) {
        ansiTable[i] = (i >= 0x20 && i <= 0x7E) ? 1 : 0;
    }
    
#ifdef XSIMD_X86
    xsimd_int64 nResult;
    
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && hayLen >= 32) {
        nResult = _xsimd_find_not_ansi_and_null_AVX2(hay, hayLen, nOffset, m, limit, ansiTable, &j);
        if (nResult != -1) {
            return nResult;
        }
    }
    
    if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && hayLen >= 16) {
        nResult = _xsimd_find_not_ansi_and_null_SSE2(hay, hayLen, nOffset, m, limit, ansiTable, &j);
        if (nResult != -1) {
            return nResult;
        }
    }
#endif
    
    /* Fallback scalar loop */
    while (j < limit) {
        /* Skip bytes that are ANSI or zero to the start of a desired run */
        while (j < hayLen) {
            unsigned char c = hay[j];
            if ((!ansiTable[c]) && (c != 0)) break;
            j++;
        }
        if (j >= limit) break;
        xsimd_int64 start = j;
        /* Extend run of non-ANSI and non-zero bytes */
        while (j < hayLen) {
            unsigned char c = hay[j];
            if ((ansiTable[c]) || (c == 0)) break;
            j++;
        }
        xsimd_int64 runLen = j - start;
        if (runLen >= m) {
            return nOffset + start;
        }
    }
    
    return -1;
}

int xsimd_is_not_ansi_and_null(const void* pBuffer, xsimd_int64 nSize)
{
    if (!pBuffer || nSize <= 0) {
        return 0;
    }
    
    const char* ptr = (const char*)pBuffer;
    
#ifdef XSIMD_X86
    /* AVX2 optimization: process 32 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && nSize >= 32) {
        int result = _xsimd_is_not_ansi_and_null_AVX2(ptr, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update ptr and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 32) * 32;
        ptr += nProcessed;
        nSize -= nProcessed;
    }
    
    /* SSE2 optimization: process 16 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && nSize >= 16) {
        int result = _xsimd_is_not_ansi_and_null_SSE2(ptr, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update ptr and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 16) * 16;
        ptr += nProcessed;
        nSize -= nProcessed;
    }
#endif
    
    /* Process remaining bytes */
    const unsigned char* pRemaining = (const unsigned char*)ptr;
    while (nSize > 0) {
        if (*pRemaining == 0 || (*pRemaining >= 0x20 && *pRemaining < 0x80)) {
            return 0;
        }
        pRemaining++;
        nSize--;
    }
    
    return 1;
}

xsimd_int64 xsimd_find_ansi_number(const void* pBuffer, xsimd_int64 nBufferSize, 
                                   xsimd_int64 nMinLength, xsimd_int64 nOffset)
{
    if (!pBuffer || nBufferSize <= 0 || nMinLength <= 0) {
        return -1;
    }
    
    const unsigned char* hay = (const unsigned char*)pBuffer;
    xsimd_int64 hayLen = nBufferSize;
    xsimd_int64 m = nMinLength;
    const xsimd_int64 limit = hayLen - (m - 1);
    xsimd_int64 j = 0;
    
#ifdef XSIMD_X86
    xsimd_int64 nResult;
    
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && hayLen >= 32) {
        nResult = _xsimd_find_ansi_number_AVX2(hay, hayLen, nOffset, m, limit, &j);
        if (nResult != -1) {
            return nResult;
        }
    }
    
    if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && hayLen >= 16) {
        nResult = _xsimd_find_ansi_number_SSE2(hay, hayLen, nOffset, m, limit, &j);
        if (nResult != -1) {
            return nResult;
        }
    }
#endif
    
    /* Fallback scalar implementation */
    while (j < limit) {
        /* Skip bytes that are not digits to the start of a desired run */
        while (j < hayLen) {
            unsigned char c = hay[j];
            if ((c >= 0x30) && (c <= 0x39)) break;
            j++;
        }
        if (j >= limit) break;
        xsimd_int64 start = j;
        /* Extend run of digit bytes */
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
    
    return -1;
}

int xsimd_is_ansi_number(const void* pBuffer, xsimd_int64 nSize)
{
    if (!pBuffer || nSize <= 0) {
        return 0;
    }
    
    const char* ptr = (const char*)pBuffer;
    
#ifdef XSIMD_X86
    /* AVX2 optimization: process 32 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_AVX2) && nSize >= 32) {
        int result = _xsimd_is_ansi_number_AVX2(ptr, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update ptr and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 32) * 32;
        ptr += nProcessed;
        nSize -= nProcessed;
    }
    
    /* SSE2 optimization: process 16 bytes at a time */
    if ((g_nEnabledFeatures & XSIMD_FEATURE_SSE2) && nSize >= 16) {
        int result = _xsimd_is_ansi_number_SSE2(ptr, nSize);
        if (result == 0) {
            return 0;
        }
        /* Update ptr and nSize for remaining bytes */
        xsimd_int64 nProcessed = (nSize / 16) * 16;
        ptr += nProcessed;
        nSize -= nProcessed;
    }
#endif
    
    /* Process remaining bytes */
    const unsigned char* pRemaining = (const unsigned char*)ptr;
    while (nSize > 0) {
        if (*pRemaining < 0x30 || *pRemaining > 0x39) {
            return 0;
        }
        pRemaining++;
        nSize--;
    }
    
    return 1;
}

xsimd_int64 xsimd_find_ansi_string_i(const void* pBuffer, xsimd_int64 nBufferSize,
                                     const void* pString, xsimd_int64 nStringSize,
                                     xsimd_int64 nOffset)
{
    if (!pBuffer || !pString || nBufferSize <= 0 || nStringSize <= 0 || nStringSize > nBufferSize) {
        return -1;
    }
    
    /* Prepare upper and lower case versions of search string */
    unsigned char* pUpperData = (unsigned char*)malloc((size_t)nStringSize);
    unsigned char* pLowerData = (unsigned char*)malloc((size_t)nStringSize);
    if (!pUpperData || !pLowerData) {
        if (pUpperData) free(pUpperData);
        if (pLowerData) free(pLowerData);
        return -1;
    }
    
    const unsigned char* pStr = (const unsigned char*)pString;
    xsimd_int64 i;
    for (i = 0; i < nStringSize; i++) {
        unsigned char c = pStr[i];
        pUpperData[i] = (c >= 'a' && c <= 'z') ? (c - 32) : c;
        pLowerData[i] = (c >= 'A' && c <= 'Z') ? (c + 32) : c;
    }
    
    const unsigned char* pData = (const unsigned char*)pBuffer;
    xsimd_int64 nSearchEnd = nBufferSize - (nStringSize - 1);
    
    /* Fallback scalar implementation */
    for (i = 0; i < nSearchEnd; i++) {
        int bMatch = 1;
        xsimd_int64 j;
        for (j = 0; j < nStringSize; j++) {
            if ((pData[i + j] != pUpperData[j]) && (pData[i + j] != pLowerData[j])) {
                bMatch = 0;
                break;
            }
        }
        if (bMatch) {
            free(pUpperData);
            free(pLowerData);
            return nOffset + i;
        }
    }
    
    free(pUpperData);
    free(pLowerData);
    return -1;
}

#ifdef XSIMD_X86
#endif

xsimd_int64 xsimd_find_first_non_ansi(const void* pBuffer, xsimd_int64 nSize)
{
    const xsimd_uint8* pData = (const xsimd_uint8*)pBuffer;
    xsimd_int64 i = 0;
    
    if (!g_bInitialized) {
        xsimd_init();
    }
    
#ifdef XSIMD_X86
    if (g_nEnabledFeatures & XSIMD_FEATURE_AVX2) {
        xsimd_int64 result = _xsimd_find_first_non_ansi_AVX2(pData, i, nSize);
        if (result >= 0) {
            return result;
        }
        i = (nSize / 32) * 32;
    } else if (g_nEnabledFeatures & XSIMD_FEATURE_SSE2) {
        xsimd_int64 result = _xsimd_find_first_non_ansi_SSE2(pData, i, nSize);
        if (result >= 0) {
            return result;
        }
        i = (nSize / 16) * 16;
    }
#endif
    
    /* Scalar fallback for remaining bytes */
    for (; i < nSize; i++) {
        xsimd_uint8 nByte = pData[i];
        if (nByte < 0x20 || nByte >= 0x80) {
            return i;
        }
    }
    
    return -1;
}

xsimd_int64 xsimd_find_null_byte(const void* pBuffer, xsimd_int64 nSize)
{
    const xsimd_uint8* pData = (const xsimd_uint8*)pBuffer;
    xsimd_int64 i = 0;
    
    if (!g_bInitialized) {
        xsimd_init();
    }
    
#ifdef XSIMD_X86
    xsimd_int64 nResult;
    
    if (g_nEnabledFeatures & XSIMD_FEATURE_AVX2) {
        nResult = _xsimd_find_null_byte_AVX2(pData, nSize, &i);
        if (nResult != -1) {
            return nResult;
        }
    } else if (g_nEnabledFeatures & XSIMD_FEATURE_SSE2) {
        nResult = _xsimd_find_null_byte_SSE2(pData, nSize, &i);
        if (nResult != -1) {
            return nResult;
        }
    }
#endif
    
    /* Scalar fallback */
    for (; i < nSize; i++) {
        if (pData[i] == 0) {
            return i;
        }
    }
    
    return -1;
}

xsimd_int64 xsimd_count_unicode_prefix(const void* pBuffer, xsimd_int64 nSize)
{
    const xsimd_uint16* pData = (const xsimd_uint16*)pBuffer;
    xsimd_int64 nChars = nSize / 2;
    xsimd_int64 i = 0;
    
    if (!g_bInitialized) {
        xsimd_init();
    }
    
    /* Must be even size for 16-bit characters */
    if (nSize % 2 != 0) {
        return 0;
    }
    
#ifdef XSIMD_X86
    xsimd_int64 nResult;
    
    if (g_nEnabledFeatures & XSIMD_FEATURE_AVX2) {
        nResult = _xsimd_count_unicode_prefix_AVX2(pData, nChars, &i);
        if (nResult != -1) {
            return nResult;
        }
    } else if (g_nEnabledFeatures & XSIMD_FEATURE_SSE2) {
        nResult = _xsimd_count_unicode_prefix_SSE2(pData, nChars, &i);
        if (nResult != -1) {
            return nResult;
        }
    }
#endif
    
    /* Scalar fallback */
    for (; i < nChars; i++) {
        xsimd_uint16 nChar = pData[i];
        int bValid = ((nChar >= 0x0020 && nChar <= 0x00FF) || 
                     (nChar >= 0x0400 && nChar <= 0x04FF));
        if (!bValid) {
            return i * 2;  /* Return byte offset */
        }
    }
    
    return nSize;  /* All characters are valid */
}

int xsimd_is_valid_unicode(const void* pBuffer, xsimd_int64 nSize)
{
    xsimd_int64 nValidBytes = xsimd_count_unicode_prefix(pBuffer, nSize);
    return (nValidBytes == nSize) ? 1 : 0;
}

xsimd_int64 xsimd_count_char(const void* pBuffer, xsimd_int64 nSize, xsimd_uint8 nByte)
{
    const xsimd_uint8* pData = (const xsimd_uint8*)pBuffer;
    xsimd_int64 nCount = 0;
    xsimd_int64 i = 0;
    
    if (!g_bInitialized) {
        xsimd_init();
    }
    
#ifdef XSIMD_X86
    if (g_nEnabledFeatures & XSIMD_FEATURE_AVX2) {
        _xsimd_count_char_AVX2(pData, nSize, nByte, &i, &nCount);
    } else if (g_nEnabledFeatures & XSIMD_FEATURE_SSE2) {
        _xsimd_count_char_SSE2(pData, nSize, nByte, &i, &nCount);
    }
#endif
    
    /* Scalar fallback */
    for (; i < nSize; i++) {
        if (pData[i] == nByte) {
            nCount++;
        }
    }
    
    return nCount;
}

xsimd_int64 xsimd_count_ansi_prefix(const void* pBuffer, xsimd_int64 nSize)
{
    xsimd_int64 nNonAnsiPos = xsimd_find_first_non_ansi(pBuffer, nSize);
    
    if (nNonAnsiPos == -1) {
        return nSize;  /* All bytes are ANSI */
    }
    
    return nNonAnsiPos;
}

xsimd_int64 xsimd_create_ansi_mask(const void* pBuffer, xsimd_int64 nSize, void* pMask)
{
    const xsimd_uint8* pData = (const xsimd_uint8*)pBuffer;
    xsimd_uint8* pMaskData = (xsimd_uint8*)pMask;
    xsimd_int64 nAnsiCount = 0;
    xsimd_int64 i = 0;
    
    if (!g_bInitialized) {
        xsimd_init();
    }
    
    /* Initialize mask to zero */
    memset(pMaskData, 0, (nSize + 7) / 8);
    
#ifdef XSIMD_X86
    if (g_nEnabledFeatures & XSIMD_FEATURE_AVX2) {
        _xsimd_create_ansi_mask_AVX2(pData, nSize, pMaskData, &i, &nAnsiCount);
    } else if (g_nEnabledFeatures & XSIMD_FEATURE_SSE2) {
        _xsimd_create_ansi_mask_SSE2(pData, nSize, pMaskData, &i, &nAnsiCount);
    }
#endif
    
    /* Scalar fallback for remaining bytes */
    for (; i < nSize; i++) {
        xsimd_uint8 nByte = pData[i];
        if (nByte >= 0x20 && nByte < 0x80) {
            pMaskData[i / 8] |= (1 << (i % 8));
            nAnsiCount++;
        }
    }
    
    return nAnsiCount;
}

void xsimd_cleanup(void)
{
    g_bInitialized = 0;
    g_nFeatures = XSIMD_FEATURE_NONE;
    g_nEnabledFeatures = XSIMD_FEATURE_NONE;
}
