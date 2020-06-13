#include <iostream>
#include <regex>

using namespace std;

void csearchall(const char* str, const char* pattern)
{
    regex r(pattern);
    cmatch m;

    if (regex_search(str, m, r))
    {

    }
}

void wsearchall(const wchar_t* str, const wchar_t* pattern)
{
    wregex r(pattern);
    wcmatch m;

    if (regex_search(str, m, r))
    {
        cout << m.empty() << endl;
        cout << m.size() << endl;
        cout << L"" << endl;
        cout << m.position() << endl;
        cout << m.length() << endl;
    }
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

void ssearchall(const string& str, const string& pattern)
{
    regex r(pattern);
    smatch m;

    auto b = str.begin();
    auto e = str.end();
    string temp;
    while (regex_search(b, e, m, r))
    {
        temp = m[0];
        cout << temp << endl;
        b = m[0].second;

        cout << "====================" << endl;
        cout <<"empty:"<<m.empty() << endl;
        cout <<"size:"<<m.size() << endl;
        cout <<"position:"<<m.position() << endl;
        cout <<"length:"<<m.length() << endl;
        cout <<"str:"<<m.str() << endl;

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
}

#if UNICODE
#define searchall wsearchall  
#else
#define searchall csearchall  
#endif

auto pattern(L"ei");
auto test = L"what noei cei what ur cei neei n!!eei";

int main()
{
    //wssearchall(test, pattern);
    ssearchall(string("what noei cei what ur cei neei n!!eei"), string("[^c]ei"));
    return 0;
};
