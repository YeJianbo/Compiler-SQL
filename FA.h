//
// Created by YeJianbo on 2023/3/14.
//
#include <bits/stdc++.h>

#define GRAMMAR_PATH "E:\\Code\\JetBrain\\CLion\\LAXER\\GRAMMAR.txt"
#define SOURCE_PATH "E:\\Code\\JetBrain\\CLion\\LAXER\\SOURCE_CODE.txt"
#define KEYWORD_PATH "E:\\Code\\JetBrain\\CLion\\LAXER\\KEYWORD.txt"


using namespace std;

//缓冲区
//char str[1024];
//string line;


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
    OPERATOR            //运算符，比如”+“、”-“、”*“、”/“、”+=“
};

// Token结构体
struct Token {
    TokenType type; // Token类型
    string value;   // Token值
    int line;       // 所在行数

    bool operator<(const Token &o) const;
};

string trim(string str);

bool nodeStartsWith(set<Node> nodes, string prefix);

bool setsAreEqual(set<Node> s1, set<Node> s2);

template <typename T>
void addAllElements(std::set<T>& destSet, const std::set<T>& sourceSet);

bool startsWith(const std::string& prefix, const std::string& str);

bool hasNode(set<Node>& nodes, string string1, string string2);

bool isNodeNameEndsWith(const Node& node, const string& string2);

set<string> readWordsFromFile(string path);

template <typename T>
void addVector(vector<T>& v1, vector<T>& v2);


class FA{   //自动机M = (K, ∑, f, S, Z)
    Node startState;                   //初态集S
    set<Node> endState;                     //终态集Z
    set<Node> States;                       //状态集K
    set<char> charSet;                      //字母表
    map<Node,map<char,set<Node>>> transNFA; //NFA状态转移fN(Node:当前状态 map：不同输入对应的状态转移)
    map<Node,map<char,Node>> transDFA;      //DFA状态转移fD
    int count = 0;                          //节点计数
    map<string,set<Node>> stateCorr;        //状态对应关系

public:

    const set<char> &getCharSet() const;

    const map<Node, map<char, set<Node>>> &getTransNfa() const;

    const map<Node, map<char, Node>> &getTransDfa() const;

    void minimizeDFA();                     //函数，用于最小化DFA

    void GrammarToNFA(string path);         //函数，分析输入的语法，将其转换为NFA

    void printEdge();                       //函数，输出NFA的状态转移关系

    void printDFA();                        //函数，输出DFA的状态转移关系

    void printCharSet();                    //函数，输出字母表中所有的字母

    void deal(string line);                 //函数，分析处理一行数据

    Node insertIntoStartState(string name); //函数，将一个状态加入初态集（同时加入状态集），如果存在，返回该节点

    Node insertIntoEndState(string name);   //函数，将一个状态加入终态集

    Node insertIntoState(string name);      //将一个状态加入状态集

    set<Node> closure(const Node& node);    //求输入节点的ε-闭包

    void TransToDFA(FA nfa);

    void deal2(FA nfa,Node start, set<Node> n);

    const Node &getStartState() const;

    const set<Node> &getEndState() const;

    set<Node> move(char input,set<Node> node,FA nfa);

};
