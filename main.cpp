#include "LR.h"


int main() {
    //根据文法获取NFA
    FA fa;
    fa.GrammarToNFA(GRAMMAR_PATH);
    //控制台输出该NFA
    cout<<"NFA:"<<endl;
    fa.printNFA();
    FA dfa;
    dfa.TransToDFA(fa);
    //将该NFA转为DFA,然后最小化,输出最小化的DFA
    cout<<"DFA:"<<endl;
    dfa.printDFA();
    //进行词法分析
    int i = LexicalAnalyze(dfa,SOURCE_PATH);
    if (!i){
        cout<<"程序错误，现已终止！"<<endl;
        cout << endl << "按Enter键退出...";
        getchar();
        return -1;
    }
    LR lr;
    //语法分析
    lr.readGrammar(GRAMMAR_2NF_PATH);
//    lr.printProduction();
    lr.construct_LR1_itemSets();
    lr.printItemSet();
    lr.parse(TOKEN_PATH);
    cout << endl << "按Enter键退出...";
    getchar();
    return 0;
}