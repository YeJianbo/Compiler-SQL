#include "FA.h"

// Token类型枚举
enum TokenType {
    KEYWORD,
    IDENTIFIER,
    CONSTANT,
    DELIMITER,
    OPERATOR
};

// Token结构体
struct Token {
    TokenType type; // Token类型
    string value;   // Token值
    int line;       // 所在行数
};

//单行词法分析
set<Token> LAbyLine(FA dfa, string line){

}

//词法分析
//传入dfa与文件路径，对其进行词法分析，切分得到Token集
set<Token> LexicalAnalyze(FA dfa,string path){
    set<Token> tokens;
    string line;
    ifstream file(path);
    if (!file){
        cout << "找不到源代码文件！" << endl;
        return tokens;
    }
    //从file中获取一行，保存在line中
    while(getline(file,line)){
        addAllElements(tokens,LAbyLine(dfa, line));
    }
    return tokens;
}




int main() {
    //根据文法获取NFA
    FA fa;
    fa.GrammarToNFA(GRAMMAR_PATH);
    //控制台输出该NFA
    cout<<"NFA:"<<endl;
    fa.printEdge();
//    cout<<"charSet:"<<"[ ";
//    fa.printCharSet();
//    cout<<"]"<<endl;
/*    set<Node> node = {{1,"111"},{2,"222"},{3,"啵啊"}};
    set<Node> n2 = {{1,"111"},{4,"钵钵鸡啊啊啊"}};
    set<Node> n3;
    addAllElements(n3,node);
    for (set<Node>::iterator it = n3.begin(); it != n3.end(); ++it) {
        cout << it->id << ": " << it->name << endl; // 打印set中的值
    }*/
/*    //ε-闭包测试
    set<Node> n = fa.closure(fa.getStartState().front());
    for (set<Node>::iterator it = n.begin(); it != n.end(); ++it) {
        cout << it->id << ": " << it->name << endl; // 打印set中的值
    }*/
    FA dfa;
    dfa.TransToDFA(fa);
    //将该NFA转为DFA,然后最小化,输出最小化的DFA
//    fa.TransToDFA();
    cout<<"DFA:"<<endl;
//    fa.minimizeDFA();
    dfa.printDFA();
    //进行词法分析
    set<Token> tokens = LexicalAnalyze(dfa,SOURCE_PATH);
    for (set<Token>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        cout << it->line << ": " << it->type <<" "<< it->value << endl; // 打印set中的值
    }
    return 0;
}


