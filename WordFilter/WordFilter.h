#pragma once
#include<iostream>
#include<regex>
#include<vector>
#include<tuple>
#include<chrono>
#include<thread>

using namespace std;

class Pattern {
public:
    wchar_t   curr;
    vector<Pattern*> nexts;

    Pattern():curr(0) {};
    Pattern(wchar_t word) :curr(word) {};
    ~Pattern() {
    
    };

    void insert(Pattern* p, const wchar_t* words, size_t index, size_t maxi)
    {
        if (index == maxi)
            return;

        if (p->curr == words[index])
        {
            for (Pattern* next : p->nexts)
                insert(next, words, index + 1, maxi);
        }
        else
        {
            p->nexts.push_back(new Pattern(words[index]));
            insert(p->nexts.back(), words, index + 1, maxi);
        }
    }

    void add(const wchar_t* word)
    {
        size_t length = wcslen(word);
        insert(this, word, 0, length);
    }

    void verbose(size_t depth = 0) const
    {
        for (size_t i = 0; i < depth; i++)
            wcout << "  ";

        wcout << curr << endl;

        for (auto p : nexts)
            verbose(depth+1);
    }
};

/*
template<class T>
class Pattern {
    Pattern() {};
    ~Pattern() {};
public:
    T   curr;
    vector<Pattern*> nexts;

    void add(const T* word)
    {
        size_t length = wstring(word).length();
        for (size_t i = 0; i < length)
    }
};
*/

template<class T>
class Filter {
public:
	Filter() {};
	~Filter() {};
public:
    // using T0 = typename std::remove_pointer_t<T>;
	void addWord(const T* word)
	{ 
		words_.push_back(word);	
	};

	void filter(T* str) 
	{
        for (auto& word : words_ )
        {
            Replace(str, word);
        }
	};

private:

    bool replace(T* str, const T* pattern, size_t& end, bool& found)
    {
        size_t si = 0, pi = 0, begin = 0;
        bool firstmatch = false;
        bool goon = true;
        found = false;
        while (str[si] != end_)
        {
            if (str[si] == pattern[pi] && str[si] != replacer_)
            {
                if (firstmatch == false)
                {
                    begin = si;
                    firstmatch = true;
                }

                if (pattern[++pi] == end_)
                {
                    found = true;
                    end = si;

                    //直接替换
                    for (size_t i = begin; i <= end; i++)
                        str[i] = replacer_;

                    break;
                }
            }

            si++;
        }

        if (str[si] == end_)
            goon = false;

        return goon;
    }

    void Replace(T* str, const T* pattern)
    {
        size_t e;
        bool found;
        while (replace(str, pattern, e, found)) // 这里可以做裁剪
            str += e + 1;
    }

private:
	vector<const T*> words_;
 
	static T replacer_;
    const static T end_ = T(0);
};

using wfilter = Filter<wchar_t>;
using cfilter = Filter<char>;
wchar_t wfilter::replacer_ = L'*';
char cfilter::replacer_ = '*';