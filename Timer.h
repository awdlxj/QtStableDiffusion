#pragma once
#include <chrono>

//支持linux

class Timer
{
public:
    /// <summary>
    /// 刷新当前时间点
    /// </summary>
    /// 

    inline void starttimer()
    {
        m_begin = std::chrono::high_resolution_clock::now();
    }

    /// <summary>
    /// 获取时间微秒
    /// </summary>
    /// <returns></returns>
    inline long long getthroughtime()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_begin).count();
    }
    inline long long endtimer()
    {
        return getthroughtime();
    }

protected:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;  //时间开始
};
