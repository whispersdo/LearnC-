#include"WordFilter.h"

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

struct wstr {
    wchar_t* ptr;
    wstr(const wchar_t* ws) {
        size_t strlen = wcslen( ws ) + 1;
        ptr = new wchar_t[strlen];
        wcscpy_s(ptr, strlen, ws);
    };
    ~wstr()
    {
        delete ptr;
    };
};

int main()
{
    // Êä³öÖÐÎÄ
    wcout.imbue(locale("chs"));

    wfilter wf;
    wf.addWord(L"ABCDEFG");
    wf.addWord(L"ABCDEFH");
    wf.addWord(L"ABCDEFJ");
    wf.addWord(L"ABCDEFX");

    wstr p(L"ABCDEFG ABCDEFH ABCDEFJ ABCDEFXxx");
    wchar_t* str = p.ptr;

    wf.filter(str);
    

    wcout << str <<endl;
    wcout << "c:" << wf.counter() << endl;
    wcout << L"===================" << endl;

    Pattern<wchar_t> pat(0);
    
    pat.add(L"ABCDEFG");
    pat.add(L"ABCDEFH");
    pat.add(L"ABCDEFJ");
    pat.add(L"ABCDEFX");
    pat.dump();

    wstr p2(L"ABCDEFG ABCDEFH ABCDEFJ ABCDEFXxx");
    str = p2.ptr;

    wf.filter(str, &pat);
    wcout << "c:" << wf.counter() << endl;
    wcout << str << endl;
    wcout << L"===================" << endl;

    return 0;
}