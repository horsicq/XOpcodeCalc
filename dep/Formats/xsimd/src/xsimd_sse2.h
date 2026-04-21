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
#ifndef XSIMD_SSE2_H
#define XSIMD_SSE2_H

#include "xsimd.h"

#ifdef __cplusplus
extern "C" {
#endif

/* SSE2 internal functions */
xsimd_int64 _xsimd_find_byte_SSE2(const unsigned char* pData, xsimd_int64 nSize, xsimd_uint8 nByte, xsimd_int64 nOffset);
xsimd_int64 _xsimd_find_pattern_bmh_SSE2(const char* pHay, xsimd_int64 nBufferSize, const char* pNeedle, xsimd_int64 nPatternSize, xsimd_int64 nOffset,
                                         xsimd_int64 nLimit, char nLastChar, xsimd_int64* pPosition);
xsimd_int64 _xsimd_find_pattern_bmh_3byte_SSE2(const char* pHay, xsimd_int64 nBufferSize, const char* pNeedle, xsimd_int64 nOffset);
xsimd_int64 _xsimd_find_pattern_bmh_2byte_SSE2(const char* pHay, xsimd_int64 nBufferSize, xsimd_uint16 pattern16, xsimd_int64 nOffset);
xsimd_int64 _xsimd_find_pattern_bmh_1byte_SSE2(const char* pHay, xsimd_int64 nBufferSize, char nByte, xsimd_int64 nOffset);
xsimd_int64 _xsimd_find_ansi_SSE2(const unsigned char* pData, xsimd_int64 nBufferSize, xsimd_int64 nMinLength, xsimd_int64 nOffset);
xsimd_int64 _xsimd_find_ansi_SSE2_2(const unsigned char* pData, xsimd_int64 nBufferSize, xsimd_int64 nMinLength, xsimd_int64 nOffset);
xsimd_int64 _xsimd_find_notnull_SSE2(const unsigned char* pData, xsimd_int64 nBufferSize, xsimd_int64 nMinLength, xsimd_int64 nOffset, xsimd_int64 j,
                                     xsimd_int64 runStart);
xsimd_int64 _xsimd_find_not_ansi_SSE2(const unsigned char* pData, xsimd_int64 nBufferSize, xsimd_int64 nMinLength, xsimd_int64 nOffset);
int _xsimd_is_not_null_SSE2(const char* ptr, xsimd_int64 nSize);
int _xsimd_is_ansi_SSE2(const char* ptr, xsimd_int64 nSize);
int _xsimd_is_not_ansi_SSE2(const char* ptr, xsimd_int64 nSize);
int _xsimd_compare_memory_SSE2(const char* ptr1, const char* ptr2, xsimd_int64 nSize);
xsimd_int64 _xsimd_find_not_ansi_and_null_SSE2(const unsigned char* hay, xsimd_int64 hayLen, xsimd_int64 nOffset, xsimd_int64 m, const xsimd_int64 limit,
                                               const int* ansiTable, xsimd_int64* pj);
int _xsimd_is_not_ansi_and_null_SSE2(const char* ptr, xsimd_int64 nSize);
xsimd_int64 _xsimd_find_ansi_number_SSE2(const unsigned char* hay, xsimd_int64 hayLen, xsimd_int64 nOffset, xsimd_int64 m, const xsimd_int64 limit, xsimd_int64* pj);
int _xsimd_is_ansi_number_SSE2(const char* ptr, xsimd_int64 nSize);
xsimd_int64 _xsimd_find_first_non_ansi_SSE2(const xsimd_uint8* pData, xsimd_int64 i, xsimd_int64 nSize);
xsimd_int64 _xsimd_find_null_byte_SSE2(const xsimd_uint8* pData, xsimd_int64 nSize, xsimd_int64* pi);
xsimd_int64 _xsimd_count_unicode_prefix_SSE2(const xsimd_uint16* pData, xsimd_int64 nChars, xsimd_int64* pi);
void _xsimd_count_char_SSE2(const xsimd_uint8* pData, xsimd_int64 nSize, xsimd_uint8 nByte, xsimd_int64* pi, xsimd_int64* pnCount);
void _xsimd_create_ansi_mask_SSE2(const xsimd_uint8* pData, xsimd_int64 nSize, xsimd_uint8* pMaskData, xsimd_int64* pi, xsimd_int64* pnAnsiCount);
int _xsimd_compare_sigbytes_SSE2(const xsimd_uint8* pSigBytes, xsimd_int64 nSigBytesSize, const xsimd_uint8* pData, xsimd_int64 nDataSize,
                                 const xsimd_uint8* pAlphaNumTable);
xsimd_int64 _xsimd_find_sigbytes_SSE2(const xsimd_uint8* pData, xsimd_int64 nDataSize, const xsimd_uint8* pSigBytes, xsimd_int64 nSigBytesSize,
                                      const xsimd_uint8* pAlphaNumTable);

#ifdef __cplusplus
}
#endif

#endif /* XSIMD_SSE2_H */
