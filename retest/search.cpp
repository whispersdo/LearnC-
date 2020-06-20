#include<iostream>
#include<regex>
#include<vector>
#include<tuple>
#include<chrono>
#include<thread>

using namespace std;

void csearchall(const char* str, const char* pattern)
{
    regex r(pattern);
    cmatch m;

    if (regex_search(str, m, r))
    {

    }
}

auto wsearchall(const wchar_t* str, const wchar_t* pattern)
{
    wregex r(pattern);
    wcmatch m;

    size_t currindex = 0;
    vector<pair<size_t, size_t>> bes;
    wchar_t* b = const_cast<wchar_t*>(str);
    while (regex_search(b, m, r))
    {
        size_t pos = m.position();
        size_t len = m.length();
        
        currindex += m.position();
        bes.push_back({ currindex, currindex + m.length() });
        currindex += m.length();
        b += pos + len;
    }

    return bes;
}

void wssearchall(const wstring& str, const wstring& pattern)
{
    wregex r(pattern);
    wsmatch m;

    if (regex_search(str, m, r))
    {
        cout << m.empty() << endl;
        cout << m.size() << endl;
        cout << m.position() << endl;
        cout << m.length() << endl;
        wcout << m.str() << endl;
        
        wcout << L"====================" << endl;

        for (size_t i = 0; i < m.size(); i++)
        {
            wcout << m[i].str() << endl;
            wcout << m.position(i) << endl;
            wcout << m.length(i) << endl;
        }

        for (auto pos = m.begin(); pos != m.end(); ++pos)
        {
            wcout << L"" << *pos << L"";
            wcout << pos->length() << endl;
        }
    }
}

auto ssearchall(const string& str, const string& pattern)
{
    regex r(pattern);
    smatch m;

    auto b = str.begin();
    auto e = str.end();
    string temp;
    size_t currindex = 0;
    vector<pair<size_t, size_t>> bes;
    while (regex_search(b, e, m, r))
    {
        temp = m[0];
        b = m[0].second;
        currindex += m.position();
        bes.push_back({ currindex, currindex + m.length() });
        currindex += m.length();
        /*
        cout <<"empty:"<<m.empty() << endl;
        cout <<"size:"<<m.size() << endl;
        cout <<"position:"<<m.position() << endl;
        cout <<"length:"<<m.length() << endl;
        cout <<"str:"<<m.str() << endl;
        */
        /*
        for (size_t i = 0; i < m.size(); i++)
        {
            cout << m[i].str() << endl;
            cout << m.position(i) << endl;
            cout << m.length(i) << endl;
        }

        for (auto pos = m.begin(); pos != m.end(); ++pos)
        {
            cout << L"" << *pos << L"";
            cout << pos->length() << endl;
        }
        */
    }

    return bes;
}

auto mysearch(const wchar_t* str, const wchar_t* pattern, size_t& begin, size_t& end)
{
    size_t si = 0, pi = 0;
    bool found = false;
    bool firstmatch = false;
    while (str[si])
    {
        if (str[si] == pattern[pi])
        {
            if ( firstmatch == false )
            {
                begin = si;
                firstmatch = true;
            }
            
            if (pattern[++pi] == L'\0')
            { 
                found = true;
                end = si;
                break;
            }
        }

        si ++;
    }

    return found;
}

#if UNICODE
#define searchall wsearchall  
#else
#define searchall csearchall  
#endif

struct processtime {
    chrono::time_point<chrono::system_clock> tp;
    processtime() 
    {
        tp = chrono::system_clock::now();
    }
    ~processtime()
    {
        auto diff = chrono::system_clock::now() - tp;
        auto msec = chrono::duration_cast<chrono::milliseconds>(diff);
        cout << "cost:" << msec.count() << endl;
    }
};

bool replace(wchar_t* str, const wchar_t* pattern, size_t& end, bool& found)
{
    size_t si = 0, pi = 0, begin = 0;
    bool firstmatch = false;
    bool goon = true;
    found = false;
    while (str[si] != L'\0')
    {
        if (str[si] == pattern[pi] && str[si] != '*')
        {
            if (firstmatch == false)
            {
                begin = si;
                firstmatch = true;
            }

            if (pattern[++pi] == L'\0')
            {
                found = true;
                end = si;

                //直接替换
                for (size_t i = begin; i <= end; i++)
                    str[i] = L'*';

                break;
            }
        }

        si++;
    }

    if (str[si] == L'\0')
        goon = false;

    return goon;
}

void Replace(wchar_t* str, const wchar_t* pattern)
{
    size_t e;
    bool found;
    while (replace(str, pattern, e, found)) // 这里可以做裁剪
        str += e + 1;
}

int main()
{
{
    auto str = L"what the fuck is going on?";
    auto pattern(L"f.*k");

    processtime p;
    size_t i = 0;
    while (i++ < 100)
    { 
        wsearchall(str, pattern);
        /*
        for (auto& e : wsearchall(test, pattern))
            cout << e.first << e.second << endl;
        */
    }
}

{
    string str("what the fuck is going on?");
    string pattern("f.*k");
    processtime p;

    size_t i = 0;
    while (i++ < 100)
    {
        ssearchall(str, pattern);
    }
}

{
    auto str = L"what the fuck is going on?";
    auto pattern(L"fk");

    processtime p;
    size_t i = 0;
    while (i++ < 100000)
    {
        size_t a, b;
        mysearch(str, pattern, a, b);
        /*
        for (auto& e : wsearchall(test, pattern))
            cout << e.first << e.second << endl;
        */
    }
}
{
    auto str = L"what the fuck is going on? fxxk w fffk fw f eff-k";
    auto pattern(L"fk");

    size_t l = wstring(str).length();

    auto str2 = new wchar_t[l];
    for (size_t i = 0; i <= l; i++)
    {
        str2[i] = str[i];
    }

    Replace(str2, pattern);
    wcout << str2 << endl;
}
    return 0;
};
