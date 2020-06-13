#include <iostream>
#include <regex>

using namespace std;

void test_searchall()
{
    string pattern("[^c]ei");
    pattern = "[[:alpha:]]*" + pattern + "[[:alpha:]]*";

    regex r(pattern);
    smatch results;
    string test = "what noei cei what ur cei neei n!!eei";
    if (regex_search(test, results, r))
    {
        //regex_constants::icase;
        cout << r.flags() << endl;
        cout << results.str() << endl;
    }

    for (sregex_iterator it(test.begin(), test.end(), r), end_it; it != end_it; ++it)
        cout << it->str() << endl;
}

int test_main() {

    test_searchall();

    string text = "2019-02-59";
    regex pattern("[0-9]{4}");

    smatch results;
    if (regex_search(text, results, pattern)) {
        smatch::iterator it = results.begin();
        int i = 0;
        for (; it != results.end(); ++it, ++i)
            cout << i << ": " << *it << endl;
    }
    else {
        cout << "match failed: " << text << endl;
    }
    return 0;
}