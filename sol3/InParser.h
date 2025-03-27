#pragma once

#include <cstdio>
#include <cctype>

class InParser{
private:
    const static size_t BLOCK_SIZE = 4096;

    FILE *fin;
    char *buffer;
    size_t current_index;

    inline char read_char()
    {
        ++current_index;
        if (current_index == BLOCK_SIZE)
        {
            current_index = 0;
            fread(buffer, 1, BLOCK_SIZE, fin);
        }
        return buffer[current_index];
    }

public:
    explicit InParser(const char* file_name)
    {
        fin = fopen(file_name, "r");
        if (fin != nullptr)
        {
            buffer = new char[BLOCK_SIZE]();
            current_index = BLOCK_SIZE - 1;
        }
    }

    ~InParser()
    {
        delete[] buffer;
    }

    bool file_exists() const
    {
        return fin != nullptr;
    }

    InParser& operator >> (int &num)
    {
        read_signed(num);
        return *this;
    }

    InParser& operator >> (long long &num)
    {
        read_signed(num);
        return *this;
    }

    template<class T>
    void read_signed(T& num)
    {
        char ch;
        num = 0;
        while (!isdigit(ch = read_char()) && ch != '-');
        int sign = 1;
        if (ch == '-')
        {
            num = 0;
            sign = -1;
        }
        else
        {
            num = ch - '0';
        }
        while (isdigit(ch = read_char()))
        {
            num = 10 * num + ch - '0';
        }
        num *= sign;
    }
};