/**********************************************************
 * Author        : zxn
 * Email         : 176108053@qq.com
 * GitHub        : https://github.com/XuenanZhang

 * Create time   : 2018-04-11 16:32
 * Last modified : 2018-04-11 16:32
 * Filename      : noncopyable.h

 * Description   : 禁止拷贝
 * *******************************************************/

#ifndef _BLING_NOCOPYABLE_H_
#define _BLING_NOCOPYABLE_H_

namespace bling
{

class noncopyable
{
protected:  
    noncopyable() {}  
    ~noncopyable() {}  
private:  // emphasize the following members are private  
    noncopyable( const noncopyable& );  
    const noncopyable& operator=( const noncopyable& );
};

};


#endif //_BLING_NOCOPYABLE_H_
