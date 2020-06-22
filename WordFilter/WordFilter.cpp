#include"WordFilter.h"

#define SrcCount 100
#define FilterCount 100
#define LoopCount 10000

const wchar_t* prefix = L"";

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
    // 输出中文
    wcout.imbue(locale("chs"));
    
    // 生成被过滤字符串
    wstring wssrcstr(L"");
    for (size_t i = 0; i < SrcCount; i++)
    {
        wssrcstr += wstring(prefix) + to_wstring(rand());
    }

    wchar_t* srcstr = const_cast<wchar_t*>(wssrcstr.c_str());

    wcout << "prefix:" << prefix << endl;
    wcout << "srcstr:" << srcstr << endl;

    wfilter wf;
    wf.add(prefix);
    for (size_t i = 0; i < FilterCount; i++)
    {
        wstring ws(prefix);
        ws += to_wstring(i);
        wf.add(ws.c_str());
    }

    {
        wstr p(srcstr);
        wchar_t* str = p.ptr;
        processtime pt1 = processtime();
        for (size_t i = 0;i < LoopCount; i++)
            wf.filter(str);
    }

    //wcout << str <<endl;
    wcout << "c:" << wf.counter() << endl;
    wcout << L"===================" << endl;

    Pattern<wchar_t> pat(0);

    pat.add(prefix);
    for (size_t i = 0; i < FilterCount; i++)
    {
        wstring ws(prefix);
        ws += to_wstring(i);
        pat.add(ws.c_str());
    }

    pat.prepare();
    pat.dump();
    
    {
        wstr p2(srcstr);
        wchar_t* str = p2.ptr;
        processtime pt1 = processtime();
        for (size_t i = 0; i < LoopCount; i++)
            wf.filter(str, &pat);
    }

    wcout << "c:" << wf.counter() << endl;
    //wcout << str << endl;
    wcout << L"===================" << endl;

    return 0;
}