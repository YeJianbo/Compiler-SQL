//#include "FA.h"
#include "LR.h"


int main() {
    //根据文法获取NFA
    FA fa;
    fa.GrammarToNFA(GRAMMAR_PATH);
    //控制台输出该NFA
    cout<<"NFA:"<<endl;
    fa.printEdge();
    FA dfa;
    dfa.TransToDFA(fa);
    //将该NFA转为DFA,然后最小化,输出最小化的DFA
    cout<<"DFA:"<<endl;
    dfa.printDFA();
    //进行词法分析
    vector<Token> tokens = LexicalAnalyze(dfa,SOURCE_PATH);
    cout<<"词法分析结果："<<endl;
    printTokens(tokens);
    LR lr;
    //读语法
    lr.readGrammar(GRAMMAR_2NF_PATH);
    lr.printProduction();

//    lr.is = lr.closure();
    lr.construct_LR1_itemsets();
    lr.printItemSet();
    lr.parse(TOKEN_PATH);
//    lr.printToken();
    return 0;
}