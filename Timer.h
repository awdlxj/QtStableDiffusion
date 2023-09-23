#pragma once
#include <chrono>

//֧��linux

class Timer
{
public:
    /// <summary>
    /// ˢ�µ�ǰʱ���
    /// </summary>
    /// 

    inline void starttimer()
    {
        m_begin = std::chrono::high_resolution_clock::now();
    }

    /// <summary>
    /// ��ȡʱ��΢��
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
    std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;  //ʱ�俪ʼ
};
