#include "interpreter.h"
#include <string>
#include <fstream>
#include <sstream>
using namespace std;


void test() {
    char a[800];
    while (cin.getline(a, 800))
    {
        cout << ExecuteCode(a);
    }
}

int main(int argc, char** argv) {

    Analyzer::init();
    Executor::init();

    string a;
    if (argc <= 1)
    {
        test();
        return 0;
    }
    if (argc == 2)
    {
        ifstream t(argv[1]);
        stringstream buffer;
        buffer << t.rdbuf();
        string contents(buffer.str());
        cout << ExecuteCode(contents);
    }
    if (argc == 3)
    {
        cout << "wronginput:option'-o' requires a parameter.\n";
        return 0;
    }
    if (argc == 4)
    {
        if (string(argv[2]) != string("-o"))
        {
            cout << "wronginput: only option '-o' allowed.\n";
        }
        ifstream t(argv[1]);
        stringstream buffer;
        buffer << t.rdbuf();
        string contents(buffer.str());
        fstream outfile(argv[3], ios::out);
        outfile << ExecuteCode(contents);
    }
    if (argc >= 5) {
        cout << "wronginput: too many parameters.\n";
    }
    return 0;
}