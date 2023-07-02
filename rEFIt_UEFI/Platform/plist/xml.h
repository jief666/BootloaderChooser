/*
 * plist.h
 *
 *  Created on: 31 Mar 2020
 *      Author: jief
 */

#ifndef __xml_h__
#define __xml_h__

/* XML Tags */
#define kXMLTagPList     "plist"
#define kXMLTagDict      "dict"
#define kXMLTagKey       "key"
#define kXMLTagString    "string"
#define kXMLTagInteger   "integer"
#define kXMLTagData      "data"
#define kXMLTagDate      "date"
#define kXMLTagFalse     "false/"
#define kXMLTagTrue      "true/"
#define kXMLTagArray     "array"
#define kXMLTagReference "reference"
#define kXMLTagID        "ID="
#define kXMLTagIDREF     "IDREF="
#define kXMLTagFloat     "real"


size_t XMLDecode (const char* src, size_t srclen, char* out, size_t outlen);
char* XMLDecode (char* src);

#endif /* __xml_h__ */
