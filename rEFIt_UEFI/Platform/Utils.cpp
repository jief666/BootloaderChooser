///*++
//
//All rights reserved. This program and the accompanying materials
//are licensed and made available under the terms and conditions of the BSD License
//which accompanies this distribution. The full text of the license may be found at
//http://opensource.org/licenses/bsd-license.php
//
//THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
//WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//Module Name:
//
//  IO.c
//
//Abstract:
//
//  the IO library function
//
//Revision History
//
//--*/
//
#include <Platform.h> // Only use angled for Platform, else, xcode project won't compile
#include <Efi.h>
//
//void LowCase (IN OUT CHAR8 *Str)
//{
//  while (*Str) {
//    if (IS_UPPER(*Str)) {
//      *Str |= 0x20;
//    }
//    Str++;
//  }
//}

UINT8 hexstrtouint8 (const CHAR8* buf)
{
	INT8 i = 0;
	if (IS_DIGIT(buf[0]))
		i = buf[0]-'0';
	else if (IS_HEX(buf[0]))
		i = (buf[0] | 0x20) - 'a' + 10;

	if (strlen(buf) == 1) {
		return i;
	}
	i <<= 4;
	if (IS_DIGIT(buf[1]))
		i += buf[1]-'0';
	else if (IS_HEX(buf[1]))
		i += (buf[1] | 0x20) - 'a' + 10;

	return i;
}

XBool IsHexDigit(char c) {
	return (IS_DIGIT(c) || (IS_HEX(c)))?true:false;
}

XBool IsHexDigit(wchar_t c) {
  return (IS_DIGIT(c) || (IS_HEX(c)))?true:false;
}

//out value is a number of byte.  out = len

// get rid of this one
//UINT32 hex2bin(IN const CHAR8 *hex, OUT UINT8 *bin, UINT32 len) //assume len = number of UINT8 values
//{
//	CHAR8	*p;
//	UINT32	i, outlen = 0;
//	CHAR8	buf[3];
//
//	if (hex == NULL || bin == NULL || len <= 0 || AsciiStrLen(hex) < len * 2) {
//    //		DBG("[ERROR] bin2hex input error\n"); //this is not error, this is empty value
//		return false;
//	}
//
//	buf[2] = '\0';
//	p = (CHAR8 *) hex;
//
//	for (i = 0; i < len; i++)
//	{
//		while ( *p == 0x20  ||  *p == ','  ||  *p == '\n'  ||  *p == '\r' ) {
//			p++; //skip spaces and commas
//		}
//		if (*p == 0) {
//			break;
//		}
//		if (!IsHexDigit(p[0]) || !IsHexDigit(p[1])) {
//			MsgLog("[ERROR] bin2hex '%s' syntax error\n", hex);
//			return 0;
//		}
//		buf[0] = *p++;
//		buf[1] = *p++;
//		bin[i] = hexstrtouint8(buf);
//		outlen++;
//	}
//	//bin[outlen] = 0;
//	return outlen;
//}

#ifdef __cplusplus

size_t hex2bin(const XBuffer<char>& buffer, uint8_t *out, size_t outlen)
{
  return hex2bin(buffer.data(), buffer.size(), out, outlen);
}

size_t hex2bin(const XString8& s, uint8_t *out, size_t outlen)
{
  return hex2bin(s.c_str(), s.length(), out, outlen);
}

size_t hex2bin(const XStringW& s, uint8_t *out, size_t outlen)
{
  return hex2bin(s.wc_str(), s.length(), out, outlen);
}
#endif

XString8 Bytes2HexStr(UINT8 *data, UINTN len)
{
  UINTN i, j, b = 0;
  XString8 result;

  result.dataSized(len*2+1);
  for (i = j = 0; i < len; i++) {
    b = data[i] >> 4;
    result += (CHAR8) (87 + b + (((b - 10) >> 31) & -39));
    b = data[i] & 0xf;
    result += (CHAR8) (87 + b + (((b - 10) >> 31) & -39));
  }
  return result;
}

UINT32 GetCrc32(UINT8 *Buffer, UINTN Size)
{
  UINT32 x = 0;
  UINT32 *Fake = (UINT32*)Buffer;
  if (!Fake) return 0;
  Size >>= 2;
  while (Size--) x+= *Fake++;
  return x;
}


XBool haveError = false;

