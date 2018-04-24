/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-11 17:03
 * Last modified : 2018-04-11 17:03
 * Filename      : Type.h

 * Description   : 类型定义
 * *******************************************************/
#ifndef _BLING_TYPE_H_
#define _BLING_TYPE_H_

#include <stdint.h>
#include <string>
#include <assert.h>

namespace bling
{

using std::string;

/*---------------------------------------------------------------------------------
    基础类型定义
---------------------------------------------------------------------------------*/
// typedef int8_t												int8;
// typedef int16_t												int16;
// typedef int32_t												int32;
// typedef int64_t												int64;

// typedef uint8_t												uint8;
// typedef uint16_t												uint16;
// typedef uint32_t												uint32;
// typedef uint64_t												uint64;

// typedef uint16_t												WORD;
// typedef uint32_t												DWORD;


/*---------------------------------------------------------------------------------
    宏定义
---------------------------------------------------------------------------------*/
#define ARRAY_SIZE(v)				(sizeof(v) / sizeof(v[0]))

//真
#ifndef TRUE
	#define TRUE 1
#endif
//假
#ifndef FALSE
	#define FALSE 0
#endif

#ifndef NULL
	#define NULL 0
#endif

#define SAFE_RET(a, b) do { if(a == b) return; } while(0);
#define SAFE_RET_VAL(a, b, ret) do { if(a == b) return ret; } while(0);
#define SAFE_CONTINUE(a, b) if(a == b) continue;
#define SAFE_BREAK(a, b) if(a == b) break;
#define SAFE_DEL(p)           do { delete (p); (p) = NULL; } while(0);
#define SAFE_DEL_ARR(p)     do { if(p) { delete[] (p); (p) = NULL; } } while(0);

#define Float_Zero_Value 0.000001f
#define Float_Is_Zero(f) fabs(f) < Float_Zero_Value
#define Float_Is_Equal(a, b) fabs((a) - (b)) < Float_Zero_Value

//字符串是否相等，区分大小写
// #define STRING_IS_EQUAL(str1, str2) (CharUtils::strcmpA(str1, str2) == 0)
//字符串是否相等，不区分大小写
// #define STRING_NOCASE_EQUAL(str1, str2) (CharUtils::strcmpnA(str1, str2) == 0)

//
#define BLING_ASSERT(exp) assert(exp)

//判断某位是否被置
//15.14....3.2.1.0 
#define ISSET0(x) ((x)&0x1)
#define ISSET1(x) ((x)&0x2)
#define ISSET2(x) ((x)&0x4)
#define ISSET3(x) ((x)&0x8)
#define ISSET4(x) ((x)&0x10)
#define ISSET5(x) ((x)&0x20)
#define ISSET6(x) ((x)&0x40)
#define ISSET7(x) ((x)&0x80)
#define ISSET8(x) ((x)&0x100)
#define ISSET9(x) ((x)&0x200)
#define ISSET10(x) ((x)&0x400)
#define ISSET11(x) ((x)&0x800)
#define ISSET12(x) ((x)&0x1000)
#define ISSET13(x) ((x)&0x2000)
#define ISSET14(x) ((x)&0x4000)
#define ISSET15(x) ((x)&0x8000)

}; //bling


#endif //_BLING_TYPE_H_
