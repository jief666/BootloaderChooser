/*
 *
 * Created by jief in 1997.
 * Copyright (c) 2020 Jief
 * All rights reserved.
 *
 */

#if !defined(__XSTRINGARRAY_H__)
#define __XSTRINGARRAY_H__

#include <XToolsConf.h>
#include "XToolsCommon.h"
#include "XObjArray.h"
#include "XString.h"




//#define XStringArraySuper XObjArray<XStringClass>

template<class XStringClass_, class XStringArrayClass>
class XStringArray_/* : public XStringArraySuper*/
{
protected:
  XObjArray<XStringClass_> array;
  
public:
  typedef XStringClass_ XStringClass;

  XStringArray_() : array() {};
 
  template<typename OtherXStringClass_, class OtherXStringArrayClass>
  XStringArray_(const XStringArray_<OtherXStringClass_, OtherXStringArrayClass>& other) : array() { *this = other; }

  template<typename OtherXStringClass_, class OtherXStringArrayClass>
  XStringArray_& operator = (const XStringArray_<OtherXStringClass_, OtherXStringArrayClass>& other) {
    for ( size_t idx = 0 ; idx < other.size() ; ++idx ) {
      array.AddCopy(other[idx]);
    }
    return *this;
//    return *((XStringArrayClass*)this);
  }

//  template<typename OtherXStringClass_, class OtherXStringArrayClass>
//  XStringArrayClass& operator =(const XStringArray_<OtherXStringClass_, OtherXStringArrayClass>& other)  { array = other.array; return *((XStringArrayClass*)this); }

  size_t size() const { return array.size(); }
  void setEmpty() { array.setEmpty(); }
  bool isEmpty() const { return this->size() == 0 ; }
  bool notEmpty() const { return this->size() > 0 ; }
  
//  #define enable_if _xtools_enable_if_t
  /* [] */
  template<typename IntegralType, enable_if(is_integral(IntegralType))>
  const XStringClass& operator [](IntegralType i) const { return array[i]; }
  template<typename IntegralType, enable_if(is_integral(IntegralType))>
  XStringClass& operator [](IntegralType i) { return array[i]; }
  /* ElementAt */
  template<typename IntegralType, enable_if(is_integral(IntegralType))>
  const XStringClass& elementAt(IntegralType i) const { return array[i]; }
  template<typename IntegralType, enable_if(is_integral(IntegralType))>
  XStringClass& elementAt(IntegralType i) { return array[i]; }

  template<class Type1, class Type2, class Type3,
      enable_if(
                (  is_char(Type1) || is_char_ptr(Type1) || is___String(Type1)  ) &&
                (  is_char(Type2) || is_char_ptr(Type2) || is___String(Type2)  ) &&
                (  is_char(Type3) || is_char_ptr(Type3) || is___String(Type3)  )
               )
          >
  XStringClass ConcatAll(const Type1& Separator, const Type2& Prefix, const Type3& Suffix) const
  {
      size_t i;
      remove_const(XStringClass) s;

      if ( array.size() > 0 ) {
          s.takeValueFrom(Prefix);
          s += array.ElementAt(0);
          for ( i=1 ; i<array.size() ; i+=1 ) {
              s += Separator;
              s += array.ElementAt(i);
          }
          s += Suffix;
      }
      return s;
  }

  XStringClass ConcatAll() const
  {
    return ConcatAll(", ", "", "");
  }

  template<class Type1, enable_if(is_char(Type1) || is_char_ptr(Type1) || is___String(Type1))>
  XStringClass ConcatAll(const Type1& Separator) const
  {
      return ConcatAll(Separator, "", "");
  }


  template<class OtherXStringArrayClass>
  bool Equal(const OtherXStringArrayClass &aStrings) const
  {
    size_t ui;
    
    if ( array.size() != aStrings.size() ) return false;
    for ( ui=0 ; ui<array.size() ; ui+=1 ) {
      if ( array.ElementAt(ui) != aStrings[ui] ) return false;
    }
    return true;
  }
  template<class OtherXStringArrayClass>
  bool operator ==(const OtherXStringArrayClass &aXStrings) const { return Equal(aXStrings); }
  template<class OtherXStringArrayClass>
  bool operator !=(const OtherXStringArrayClass& aXStrings) const { return !Equal(aXStrings); }
  
  /* contains */
  template<typename CharType, enable_if(is_char(CharType))>
  bool contains(const CharType* s) const
  {
    for ( size_t i=0 ; i<array.size() ; i+=1 ) {
      if ( array.ElementAt(i).isEqual(s) ) return true;
    }
    return false;
  }
  template<class OtherXStringClass, enable_if(!is_char(OtherXStringClass) && !is_char_ptr(OtherXStringClass))>
  bool contains(const OtherXStringClass &S) const
  {
    return contains(S.s());
  }
  /* containsIC */
  template<typename CharType, enable_if(is_char(CharType))>
  bool containsIC(const CharType* s) const
  {
    for ( size_t i=0 ; i<array.size() ; i+=1 ) {
      if ( array.ElementAt(i).isEqualIC(s) ) return true;
    }
    return false;
  }
  template<class OtherXStringClass, enable_if(!is_char(OtherXStringClass) && !is_char_ptr(OtherXStringClass))>
  bool containsIC(const OtherXStringClass &S) const
  {
    return containsIC(S.s());
  }
  /* ContainsStartWithIC */
  template<typename CharType, enable_if(is_char(CharType))>
  bool containsStartWithIC(const CharType* s) const
  {
    for ( size_t i=0 ; i<array.size() ; i+=1 ) {
      if ( array.ElementAt(i).startWithIC(s) ) return true;
    }
    return false;
  }
  template<class OtherXStringClass, enable_if(!is_char(OtherXStringClass) && !is_char_ptr(OtherXStringClass))>
  bool containsStartWithIC(const OtherXStringClass &S) const
  {
    return ContainsStartWithIC(S.s());
  }


  template<class OtherXStringArrayClass>
  bool Same(const OtherXStringArrayClass &aStrings) const
  {
    size_t i;
    
    for ( i=0 ; i<this->size() ; i+=1 ) {
      if ( !aStrings.contains(array.ElementAt(i)) ) return false;
    }
    for ( i=0 ; i<aStrings.size() ; i+=1 ) {
      if ( !contains(aStrings[i]) ) return false;
    }
    return true;
  }

    // Add
  template<typename CharType>
  void AddStrings(const CharType* Val1, ...)
  {
    XTOOLS_VA_LIST va;
    const wchar_t *p;
    
    {
      remove_const(XStringClass)* newS = new remove_const(XStringClass);
      newS->takeValueFrom(Val1);
      AddReference(newS, true);
    }
    XTOOLS_VA_START(va, Val1);
    p = XTOOLS_VA_ARG(va, const CharType*);
    while ( p != nullptr ) {
      remove_const(XStringClass)* newS = new remove_const(XStringClass);
      newS->takeValueFrom(p);
      AddReference(newS, true);
      p = XTOOLS_VA_ARG(va, const CharType*);
    }
    XTOOLS_VA_END(va);
  }

  void AddNoNull(const XStringClass &aString) { if ( !aString.isEmpty() ) array.AddCopy(aString); }
  void AddEvenNull(const XStringClass &aString) { array.AddCopy(aString); }

  template<typename CharType, enable_if(is_char(CharType))>
  void Add(const CharType* s)
  {
    remove_const(XStringClass)* xstr = new remove_const(XStringClass);
    xstr->strcpy(s);
    array.AddReference(xstr, true);
  }

  template<typename XStringClass1, enable_if(is___String(XStringClass1))>
  void Add(const XStringClass1 &aString) { Add(aString.s()); }

  template<typename XStringClass1, enable_if(is___String(XStringClass1))>
  void insertAtPos(const XStringClass1 &aString, size_t pos) { array.InsertRef(new remove_const(XStringClass1)(aString), pos, true); }

  template<typename CharType, enable_if(is_char(CharType))>
  void AddReference(CharType* newElement, bool FreeIt) { array.AddReference(new XStringClass_(newElement), FreeIt); }

  void AddReference(XStringClass* newElement, bool FreeIt) { array.AddReference(newElement, FreeIt); }

  void insertReferenceAtPos(XStringClass* newElement, size_t pos, bool FreeIt) { array.InsertRef(newElement, pos, FreeIt); }

  template<typename IntegralType, enable_if(is_integral(IntegralType))>
  XStringClass* ExtractFromPos(IntegralType i) { return array.RemoveWithoutFreeingAtIndex(i); }

  template<class OtherXStringClass, class OtherXStringArrayClass>
  void import(const XStringArray_<OtherXStringClass, OtherXStringArrayClass> &aStrings)
  {
    size_t i;
    
    for ( i=0 ; i<aStrings.size() ; i+=1 ) {
      array.AddCopy(aStrings[i]);
    }
  }

  void AddID(const XStringClass &aString) /* ignore Duplicate */
  {
    if ( !contains(aString) ) array.AddCopy(aString);
  }
  void importID(const XStringArray_ &aStrings) /* ignore Duplicate */
  {
    size_t i;
    
    for ( i=0 ; i<aStrings.size() ; i+=1 ) {
      if ( !Contains(aStrings[i]) ) AddCopy(aStrings[i]);
    }
  }
  void remove(const XStringClass &aString)
  {
    if ( array.size() > 0 )
    {
      size_t i = array.size();
      do {
        i--;
        if ( array[i] == aString ) {
          array.RemoveAtIndex(i);
          break;
        }
      } while ( i > 0 );
    }
  }
  void removeAtPos(size_t pos)
  {
    array.RemoveAtIndex(pos);
  }
  void removeIC(const XStringClass &aString)
  {
    if ( array.size() > 0 )
    {
      size_t i = array.size();
      do {
        i--;
        if ( array[i].isEqualIC(aString) ) {
          array.RemoveAtIndex(i);
          break;
        }
      } while ( i > 0 );
    }
  }
  XStringArrayClass trimEachString()
  {
    for ( size_t i=0 ; i<array.size() ; i+=1 ) {
      array.ElementAt(i).trim();
    }
    return *((XStringArrayClass*)this);
  }

};

class XString8Array : public XStringArray_<XString8, XString8Array>
{
public:
  using super = XStringArray_<XString8, XString8Array>;
  
  XString8Array() : super() {}

  template<typename OtherXStringClass_, class OtherXStringArrayClass>
  XString8Array(const XStringArray_<OtherXStringClass_, OtherXStringArrayClass>& other) : super(other) {}

//  template<typename OtherXStringClass_, class OtherXStringArrayClass>
//  XStringArrayClass& operator =(const XStringArray_<OtherXStringClass_, OtherXStringArrayClass>& other)  { array = other.array; return *((XStringArrayClass*)this); }
};
extern const XString8Array NullXString8Array;

class XString16Array : public XStringArray_<XString16, XString16Array>
{
public:
  using super = XStringArray_<XString16, XString16Array>;

  XString16Array() : super() {}

  template<typename OtherXStringClass_, class OtherXStringArrayClass>
  XString16Array(const XStringArray_<OtherXStringClass_, OtherXStringArrayClass>& other) : super(other) {}
};
extern const XString16Array NullXString16Array;

class XString32Array : public XStringArray_<XString32, XString32Array>
{
public:
  using super = XStringArray_<XString32, XString32Array>;
  
  XString32Array() : super() {}

  template<typename OtherXStringClass_, class OtherXStringArrayClass>
  XString32Array(const XStringArray_<OtherXStringClass_, OtherXStringArrayClass>& other) : super(other) {}
};
extern const XString32Array NullXString32Array;

class XStringWArray : public XStringArray_<XStringW, XStringWArray>
{
public:
  using super = XStringArray_<XStringW, XStringWArray>;
  
  XStringWArray() : super() {}

  template<typename OtherXStringClass_, class OtherXStringArrayClass>
  XStringWArray(const XStringArray_<OtherXStringClass_, OtherXStringArrayClass>& other) : super(other) {}
};
extern const XStringWArray NullXStringWArray;


class ConstXString8Array : public XStringArray_<const XString8, ConstXString8Array>
{
public:
  using super = XStringArray_<const XString8, ConstXString8Array>;
  
  ConstXString8Array() : super() {}

  template<typename OtherXStringClass_, class OtherXStringArrayClass>
  ConstXString8Array(const XStringArray_<OtherXStringClass_, OtherXStringArrayClass>& other) : super(other) {}
};
extern const ConstXString8Array NullConstXString8Array;

class ConstXStringWArray : public XStringArray_<const XStringW, ConstXStringWArray>
{
public:
  using super = XStringArray_<const XStringW, ConstXStringWArray>;
  
  ConstXStringWArray() : super() {}

  template<typename OtherXStringClass_, class OtherXStringArrayClass>
  ConstXStringWArray(const XStringArray_<OtherXStringClass_, OtherXStringArrayClass>& other) : super(other) {}
};
extern const ConstXStringWArray NullConstXStringWArray;


//
//template<class XStringArrayClass, class XStringClass1, enable_if(!is_char(XStringClass1) && !is_char_ptr(XStringClass1))>
//XStringArrayClass Split(const XStringClass1& S)
//{
//  return Split<XStringArrayClass>(S, ", "_XS8);
//};




//template<class XStringArrayClass, typename CharType1, typename CharType2, enable_if(is_char(CharType1) && is_char(CharType2))>
template<class XStringArrayClass, typename Type1, typename Type2,
    enable_if(
              (  is_char_ptr(Type1) || is___String(Type1) || is___LString(Type1)  ) &&
              (  is_char_ptr(Type2) || is___String(Type2) || is___LString(Type2)  )
             )
        >
XStringArrayClass Split(Type1 S, const Type2 Separator)
{
  XStringArrayClass xsArray;

    auto s = _xstringarray__char_type<Type1>::getCharPtr(S);
    auto separator = _xstringarray__char_type<Type2>::getCharPtr(Separator);
//    typename _xstringarray__char_type<Type2>::type separator = Separator;

  size_t separatorLength = length_of_utf_string(separator);
  
  if ( separatorLength == 0 ) {
    typename XStringArrayClass::XStringClass* xstr;
    xstr = new typename XStringArrayClass::XStringClass;
    xstr->takeValueFrom(S);
    xsArray.AddReference(xstr, true);
    return xsArray;
  }

//    typename _xstringarray__char_type<Type1>::type s = S;
//  const CharType1* s = S;
  char32_t char32 = 1;
  
  do
  {
    while ( XStringAbstract__ncompare(s, separator, separatorLength, false) == 0 ) {
      // I have to implement a move_forward_one_char in unicode_conversions, as we don't care about char32
      for ( size_t i = 0 ; i < separatorLength ; i++ ) s = get_char32_from_string(s, &char32);
    }
        if ( !*s ) return xsArray;
        auto t = s;
//        typename _xstringarray__char_type<Type1>::type t = s;
//    const CharType1* t = s;
    size_t nb = 0;
    while ( char32 && XStringAbstract__ncompare(t, separator, separatorLength, false) != 0 ) {
      nb++;
      t = get_char32_from_string(t, &char32);
    }
    typename XStringArrayClass::XStringClass* xstr;
    xstr = new typename XStringArrayClass::XStringClass;
    xstr->strncpy(s, nb);
    xsArray.AddReference(xstr, true);
//    s = get_char32_from_string(t, &char32);
    s = t;
    // Consume the separator we found
    for ( size_t i = 0 ; i < separatorLength ; i++ ) s = get_char32_from_string(s, &char32);
  } while ( char32 );
  
  return xsArray;
//
//
//  // TODO : Allocating temporary strings could be avoided by using low level function from unicode_conversions
//  typename XStringArrayClass::XStringClass SS;
//  SS.takeValueFrom(S);
//  typename XStringArrayClass::XStringClass XSeparator;
//  SS.takeValueFrom(Separator);
//  return Split<XStringArrayClass>(SS, XSeparator);
};


template<class XStringArrayClass, class Type1, enable_if( is_char_ptr(Type1)  ||  is___String(Type1) ) >
XStringArrayClass Split(Type1 S)
{
    return Split<XStringArrayClass>(S, ", ");
};





//
//template<class XStringArrayClass, class XStringClass1, class XStringClass2, enable_if(is___String(XStringClass1) && is___String(XStringClass2))>
//XStringArrayClass Split(const XStringClass1& S, const XStringClass2& Separator)
//{
//  return Split<XStringArrayClass>(S.s(), Separator.s());
////
////  XStringArrayClass Ss;
////  size_t idxB, idxE;
////
////  if ( Separator.length() == 0 ) {
////    Ss.Add(S);
////    return Ss;
////  }
////  idxB = 0;
////  idxE = S.indexOf(Separator, idxB);
////  while ( idxE != MAX_XSIZE ) {
////    Ss.Add(S.subString(idxB, idxE-idxB));
////    idxB = idxE + Separator.length();
////    idxE = S.indexOf(Separator, idxB);
////  }
////  if ( idxB < S.length() ) Ss.Add(S.subString(idxB, S.length()-idxB));
////  return Ss;
//};
//
//
//template<class XStringArrayClass, class XStringClass1, enable_if(!is_char(XStringClass1) && !is_char_ptr(XStringClass1))>
//XStringArrayClass Split(const XStringClass1& S)
//{
//  return Split<XStringArrayClass>(S, ", "_XS8);
//};

#endif
