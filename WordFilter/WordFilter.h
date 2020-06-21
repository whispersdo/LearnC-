#pragma once
#include<iostream>
#include<regex>
#include<vector>
#include<tuple>
#include<chrono>
#include<thread>

using namespace std;

template<class T>
class Pattern {
public:
    T   curr;
    vector<Pattern*> nexts;

    // 缓存，空间换时间
    Pattern* parent = nullptr;
    Pattern* pre = nullptr;
    Pattern* next = nullptr;

    // 这个模式在当前string里匹配的位置
    size_t  pos = -1;

    Pattern():curr(0) {};
    Pattern(T word) :curr(word) {};
    ~Pattern() {
        // remove all nodes;
    };

    class iterator {
        Pattern* parent;
    };

    Pattern* GetNext()
    {
        if (next)
            return next;
        else if (parent)
            return parent->GetNext();
        else
            return nullptr;
    }

    Pattern* GetRoot()
    {
        if (parent)
        {
            if (parent->curr != 0)
                return parent->GetRoot();
            else
                return this;
        }
        else
        {
            return nullptr;
        }
    }

    bool IsTerminal() const
    {
        return nexts.empty();
    }

    void insert(Pattern* p, const T* words, size_t index, size_t maxi)
    {
        if (index == maxi)
            return;

        bool match = false;
        for (Pattern* next : p->nexts)
        {
            if (next->curr == words[index])
            {
                insert(next, words, index + 1, maxi);
                match = true;
                break;
            }

        }

        if (!match)
        {
            Pattern* pattern = new Pattern(words[index]);
            pattern->parent = p;
            if (!p->nexts.empty())
            {
                pattern->pre = p->nexts.back();
                p->nexts.back()->next = pattern;
            }

            p->nexts.push_back(pattern);
            insert(pattern, words, index + 1, maxi);
        }
    }

    void add(const T* word)
    {
        size_t length = wcslen(word);
        insert(this, word, 0, length);
    }

    void verbose(const Pattern* p, size_t depth = 0) const
    {
        for (size_t i = 0; i < depth; i++)
            wcout << "  ";

        wcout << "|";

        //cout << p->curr << endl;
        wcout << p->curr << " pre: " <<(uint64_t) p->pre << " next: " << (uint64_t) p->next << endl;


        for (auto p : p->nexts)
            verbose(p, depth+1);
    }

    void dump() const
    {
        wcout << L"=========dump==========" << endl;
        verbose(this);
    }
};

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
        counter_ = 0;
        for (auto& word : words_ )
        {
            Replace(str, word);
        }
	};

    void filter(T* str, Pattern<T>* pattern)
    {
        counter_ = 0;
        replace(str, pattern, 0);
    };

    uint32_t counter() const
    {
        return counter_;
    }

private:

    bool replace(T* str, const T* pattern, size_t& end, bool& found)
    {
        size_t si = 0, pi = 0, begin = 0;
        bool firstmatch = false;
        bool goon = true;
        found = false;
        while (str[si] != end_)
        {
            if (str[si] != replacer_)
                counter_++;

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
                    {
                        str[i] = replacer_;
                    }

                    break;
                }
            }

            si++;
        }

        if (str[si] == end_)
            goon = false;

        return goon;
    }

    void replace(T* str, Pattern<T>* pattern, size_t strIndex)
    {
        Pattern<T>* currentPattern = pattern;
        if (currentPattern->curr == 0)
            currentPattern = currentPattern->nexts.front();

        while (str[strIndex] != end_)
        {
            //wcout << str[strIndex] << strIndex;
            if (str[strIndex] == replacer_)
            {
                strIndex++;
                continue;
            }

            //wcout << "[" << currentPattern->pos << L"," << currentPattern->curr << "]";
            if (str[strIndex] == currentPattern->curr)// 匹配上了，记录位置
            {
                counter_++;

                currentPattern->pos = strIndex;
                strIndex++;
                if (currentPattern->IsTerminal())// pattern到头了，匹配成功了，从当前位置重新匹配最高的parent
                {
                    // 替换
                    size_t begin = currentPattern->GetRoot()->pos;
                    size_t end = currentPattern->pos;
                    for (size_t i = begin; i <= end; i++)
                    {
                        str[i] = replacer_;
                    }

                    currentPattern = currentPattern->GetRoot();

                    // wcout << endl;

                } 
                else if (str[strIndex] == end_)// str到头了，匹配失败了，回溯
                {
                    currentPattern = currentPattern->GetNext();

                    if (currentPattern) // 到头-1+1=0
                    {
                        strIndex = currentPattern->parent->pos + 1;
                    }
                    else // 后面没有了，全部完成
                    {
                        return;
                    }

                    wcout << endl;
                }
                else //pattern还有，就匹配下一个
                {
                    currentPattern = currentPattern->nexts.front();

                    /*
                    for (auto& p : currentPattern->nexts)
                        replace(str, p, strIndex);
                    */
                }
            }
            else
            {
                strIndex++;
            }
        }
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
    static uint32_t counter_;
};

using wfilter = Filter<wchar_t>;
using cfilter = Filter<char>;
uint32_t wfilter::counter_ = 0;
wchar_t wfilter::replacer_ = L'*';
char cfilter::replacer_ = '*';