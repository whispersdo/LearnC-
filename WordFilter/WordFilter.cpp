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

int main()
{
    // �������
    wcout.imbue(locale("chs"));

    wfilter wf;
    wf.addWord(L"ţ��");
    wf.addWord(L"ţ��");
    wf.addWord(L"bg");

    wchar_t* str = new wchar_t[100];
    for (size_t i = 0; i < 100; ++i)
        str[i] = L"do ţ�ᬔ���� you b��ag huh? ����"[i];

    wf.filter(str);

    wcout << str <<endl;
    wcout << L"===================" << endl;

    Pattern<wchar_t> pat(0);
    
    pat.add(L"word");
    pat.dump();
    pat.add(L"wore");
    pat.dump();
    pat.add(L"worwww");
    pat.dump();
    pat.add(L"aorwww");
    pat.dump();

    for (size_t i = 0; i < 100; ++i)
        str[i] = L"word wored worwww, a, ao , aorww, aorwww, w"[i];

    wf.filter(str, &pat);
    wcout << str << endl;
    wcout << L"===================" << endl;

    return 0;
}