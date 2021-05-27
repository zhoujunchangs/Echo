#include "../interpreter.h"
#include "testutil.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

string Test(string code) {

    string compressedcode = "{" + std::move(code) + "}";
    lexToken(compressedcode);
    if (!Result::empty()) {
        while (!tokenbuf.empty()) tokenbuf.pop();
        cerr << "Syntax Lexer Error: \n" + Result::getResult();
    }
    auto expr = AST::ParseBlock();
    if (expr == nullptr) {
        cerr << "Syntax Parser Error: \n" + Result::getResult();
    }

    if (expr->Analyse(Analyzer::instance()) == -1) {
        cerr << "Syntax Analyzer Error: \n" + Result::getResult();
    }

    Analyzer::instance().opset.push_back(OpCodeType::OpEnd);

    TestUtil::outputOpCode(&(*Analyzer::instance().opset.begin()), Analyzer::instance().opset.size());
    TestUtil::showStringSet();

    return Result::getResult();
}

int main(int argc, char** argv) {

    Analyzer::init();
    Executor::init();

    if (argc == 2) {
        ifstream t(argv[1]);
        stringstream buffer;
        buffer << t.rdbuf();
        string contents(buffer.str());
        cout << Test(contents);
    }
    else if (argc == 4) {
        if (string(argv[2]) != string("-o"))
        {
            cerr << "wronginput: only option '-o' allowed.\n";
        }
        ifstream t(argv[1]);
        stringstream buffer;
        buffer << t.rdbuf();
        string contents(buffer.str());
        fstream outfile(argv[3], ios::out);
        outfile << Test(contents);
    }
    else {
        cerr << "Error: Require a code file!";
    }

	return 0;
}