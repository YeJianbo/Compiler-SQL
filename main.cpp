#include "FA.h"



//单行词法分析
vector<Token> LAbyLine(FA dfa, string line, int n){
    vector<Token> t;
    string l = trim(line);
    //缓冲区，保存输入的字符
    string buf = "";
    //当前状态为初态
    Node state = dfa.getStartState();
    map<Node,map<char,Node>> transDFA = dfa.getTransDfa();
    set<Node> e = dfa.getEndState();
    int i = 0;
    //遍历行
    while ( i < l.length() ){
//        if (l[i] == ' '){
//            if (++i < l.length()){
//                continue;
//            }
//            break;
//        }
        state = transDFA[state][l[i]];
        if (state.id == 0){
            cout<<"程序错误！在第 "<< n <<" 行"<<endl;
        }
        buf += l[i];
        //如果终态集中找不到state，说明不是终态
        //此时必须继续读取下一个字符
        if(e.find(state) == e.end()){
            i++;
            continue;
        }else{
            //包含终态，看下一个字符能否被接受，如果不能被接收，创建Token，读取下一个词
            if(transDFA[state][l[i+1]].id == 0){
                Token token;
                if (isNodeNameEndsWith(state, "I")) {
                    //标识符和关键字，遍历关键字集合，判断是否是关键字
                    set<string> keyword = readWordsFromFile(KEYWORD_PATH);
                    //是关键字
                    if (keyword.find(buf) != keyword.end()) {
                        token.type = KEYWORD;
                    } else
                        token.type = IDENTIFIER;
                } else if (isNodeNameEndsWith(state, "O")) {
                    token.type = OPERATOR;
                } else if (isNodeNameEndsWith(state, "S")) {
                    token.type = DELIMITER;
                } else if (isNodeNameEndsWith(state, "C")) {
                    token.type = CONSTANT;
                }
                //到达接受态,词语被接受，存入Token
//            token.type = ;
                token.line = n;
                token.value = buf;
                buf.clear();
                t.push_back(token);
                i++;
                state = dfa.getStartState();
                if (i < l.length())
                    continue;
                break;
            }
            else{
                //下一个input能被接受
                i++;
                continue;
            }
        }
    }
    return t;
}

//词法分析
//传入dfa与文件路径，对其进行词法分析，切分得到Token集
vector<Token> LexicalAnalyze(FA dfa,string path){
    vector<Token> tokens;
    string line;
    int n = 0;
    ifstream file(path);
    if (!file){
        cout << "找不到源代码文件！" << endl;
        return tokens;
    }
    //从file中获取一行，保存在line中
    while(getline(file,line)){
        n++;
        vector<Token> to = LAbyLine(dfa, line, n);
        tokens.insert(tokens.end(),to.begin(),to.end());
    }
    file.close();
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
    vector<Token> tokens = LexicalAnalyze(dfa,SOURCE_PATH);
    cout<<"词法分析结果："<<endl;
    printTokens(tokens);
    return 0;
}


