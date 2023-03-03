#ifndef HELPERS_H
#define HELPERS_H
#include <string>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <locale>

class Helpers
{
public:
    char *ltrim_char(char *szX)
    {
        if (' ' == szX[0])
            while (' ' == (++szX)[0])
                ;
        return szX;
    }

    char *rtrim_char(char *szX)
    {
        int i = std::strlen(szX);
        while (' ' == szX[--i])
            szX[i] = 0;
        return szX;
    }

    char *trim_char(char *szX)
    {
        szX = ltrim_char(szX);
        szX = rtrim_char(szX);
        return szX;
    }

    static inline void ltrim(std::string &s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                        { return !std::isspace(ch); }));
    }

    // trim from end (in place)
    static inline void rtrim(std::string &s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                             { return !std::isspace(ch); })
                    .base(),
                s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string &s)
    {
        rtrim(s);
        ltrim(s);
    }

    // trim from start (copying)
    static inline std::string ltrim_copy(std::string s)
    {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    static inline std::string rtrim_copy(std::string s)
    {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    static inline std::string trim_copy(std::string s)
    {
        trim(s);
        return s;
    }
};
#endif