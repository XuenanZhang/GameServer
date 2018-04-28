/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-27 14:54
 * Last modified : 2018-04-27 14:54
 * Filename      : Endian.h

 * Description   : 字节序转换
 * *******************************************************/
#ifndef _BLING_ENDIAN_H_
#define _BLING_ENDIAN_H_

#include "common/Type.h"
#include <endian.h>

namespace bling
{

namespace net
{

namespace sockets 
{

inline uint64_t hostToNet64(uint64_t host64)
{
    return htobe64(host64);
}

inline uint32_t hostToNet32(uint64_t host32)
{
    return htobe32(host32);
}

inline uint16_t hostToNet16(uint16_t host16)
{
    return htobe16(host16);
}

inline uint64_t netToHost64(uint64_t net64)
{
    return be64toh(net64);
}

inline uint32_t netToHost32(uint32_t net32)
{
    return be32toh(net32);
}

inline uint16_t netToHost16(uint64_t net16)
{
    return be16toh(net16);
}

}; // ns sockets

}; // ns net

}; //ns bling


#endif // _BLING_ENDIAN_H_
