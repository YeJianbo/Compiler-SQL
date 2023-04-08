//
// Created by YeJianbo on 2023/3/14.
//
#include <bits/stdc++.h>

#define GRAMMAR_PATH "GRAMMAR.txt"
#define SOURCE_PATH "SOURCE_CODE.txt"
#define KEYWORD_PATH "KEYWORD.txt"
#define TOKEN_PATH "TOKEN.txt"


using namespace std;


struct Node{ //节点
    int id;
    string name;
    bool operator < (const Node &o) const;
    bool operator == (const Node &o) const;
};


// Token类型枚举
enum TokenType {
    KEYWORD,            //关键词，比如if、for、while、int
    IDENTIFIER,         //标识符，一般的单词，字母或下划线开头
    CONSTANT,           //常量，数字，比如29+52i（虚数）、1.14（小数）、514（整数）
    DELIMITER,          //界符，比如{}、（）、""、''、[]、<>
    OPERATOR,           //运算符，比如”+“、”-“、”*“、”/“、”+=“
    ERROR               //单独的错误信息标识
};


// Token结构体
struct Token {
    TokenType type; // Token类型
    string value;   // Token值
    int line;       // 所在行数
};


class FA{                                   //自动机M = (K, ∑, f, S, Z)
private:
    Node startState;                        //初态S
    set<Node> endState;                     //终态集Z
    set<Node> States;                       //状态集K
    set<char> charSet;                      //字母表
    map<Node,map<char,set<Node>>> transNFA; //NFA状态转移fN(Node:当前状态 map：不同输入对应的状态转移)
    map<Node,map<char,Node>> transDFA;      //DFA状态转移fD
    int count = 0;                          //节点计数
    map<string,set<Node>> stateCorr;        //状态对应关系
public:
    //获取非终结符集合
    const set<char> &getCharSet() const;
    //获取DFA状态转移
    const map<Node, map<char, Node>> &getTransDfa() const;
    //分析输入的语法，将其转换为NFA
    void GrammarToNFA(const string& path);
    //输出NFA的状态转移关系
    void printNFA();
    //输出DFA的状态转移关系
    void printDFA();
    //输出字母表中所有的字母
    void printCharSet();
    //分析处理一行数据
    void deal(const string& l, const string& r);
    //将一个状态加入初态集（同时加入状态集），如果存在，返回该节点
    Node insertIntoStartState(const string& name);
    //将一个状态加入终态集
    Node insertIntoEndState(const string& name);
    //将一个状态加入状态集
    Node insertIntoState(const string& name);
    //求输入节点的ε-闭包
    set<Node> closure(const Node& node);
    //NFA转DFA
    void TransToDFA(FA nfa);
    //NFA转DFA的处理
    void getDFA(FA nfa, const Node& start, const set<Node>& n);
    //获取该自动机的初态
    const Node &getStartState() const;
    //获取该自动机的终态集
    const set<Node> &getEndState() const;
    //输入字符，求在传入的NFA经过该字符到达的下一状态的集合，将集合返回
    set<Node> move(char input,const set<Node>& node,FA nfa);
};

//去除字符串前后的空格
string trim(const string& str);

//检查节点集合中是否已指定字符串开头的Node
bool nodeStartsWith(const set<Node>& nodes, const string& prefix);

//检查两个set是否相等
bool setsAreEqual(const set<Node>& s1, set<Node> s2);

//将sourceSet中的所有元素加入到destSet中
template <typename T>
void addAllElements(std::set<T>& destSet, const std::set<T>& sourceSet);

//检查字符串是否以指定子串开头
bool startsWith(const std::string& prefix, const std::string& str);

//检查状态集中是否有以指定字符串开头并且以指定字符串结尾的状态
bool hasNode(set<Node>& nodes, const string& string1, const string& string2);

//检查状态集中是否有以指定名称结尾的状态
bool isNodeNameEndsWith(const Node& node, const string& string2);

//从文件中读取关键字文件
set<string> readWordsFromFile(const string& path);

//打印并输出Token表
void printTokens(vector<Token> tokens);

//语法分析
int LexicalAnalyze(const FA& dfa,const string& path);

//单行的语法分析
vector<Token> LAbyLine(const FA& dfa, const string& line, int n);