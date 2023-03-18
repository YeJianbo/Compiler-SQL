//
// Created by YeJianbo on 2023/3/16.
//
#include "FA.h"
#define GRAMMAR_2NF_PATH "E:\\Code\\JetBrain\\CLion\\Compiler\\GRAMMAR_2NF.txt"

//struct Symbol {
////    int id;
//    string value;
//    bool is_terminal;
//    bool operator<(const Symbol &o) const;
//    bool operator==(const Symbol &o) const;
//    bool operator!=(const Symbol &o) const;
//};

//产生式
struct Production{
    char l;           //产生式左部
    string r;           //产生式右部
    bool operator==(const Production& o) const;
    bool operator<(const Production& o) const;
};

//计算符号s的FIRST集合
set<char> calc_first(const char s, map<char, set<char>>& first_set, set<char>& visited, vector<Production>& prods);

//计算输入串s的FIRST集合
set<char> calc_first_s(string& s, map<char, set<char>>& first_set, vector<Production>& prods);

//项
struct Item{
    Production rule;            //产生式
    int dot;                    //产生式中点的位置
    set<char> lookahead;        //展望符
    bool operator==(const Item& o) const;
    bool operator<(const Item& o) const;
};

//项集
struct ItemSet{
    string name;
    set<Item> items;
    bool operator==(const ItemSet& o) const;
    bool operator<(const ItemSet& o) const;
};

//struct Node_LR{
//    string name;
//    int id;
//};

/*语法分析 步骤
 * 读取上下文无关文法（文件给出），保存产生式，增广产生式，计算LR(1)项集族，
 * 通过项集族推导出LR(1)自动机，根据自动机，得到ACTION表及GOTO表，
 * 根据得到的状态对输入的Token表进行语法分析，得到分析结果*/

class LR {
private:
    set<char> terminals;
    set<char> nonTerminals;
    //初始状态（起始符S）
    char start;
    //产生式集合
    vector<Production> Productions;
    //DFA对应关系，一个Node，输入一个Symbol1，展望符为Symbol2到达新状态
    map<Node,map<char,map<char,Node>>> transDFA;
    //ACTION表，Node输入一个Vn，到达一个新的Node，此时需要归约，bool取true，移进取false
    map<Node,map<char,pair<Node,bool>>> Action;
    //GOTO表，Node输入Vt，到达新Node，此时为待约项
    map<Node,map<char,Node>> Goto;

    map<char, set<char>> first_set;



public:
    set<ItemSet> is;
    //从path中读取上下文无关文法，保存到产生式集合中，并将该集合返回
    vector<Production> readGrammar(string path);

    //对文法进行增广（添加一个新的符号，以便起始状态仅在产生式左边出现一次）
    void augmentGrammar(vector<Production>& grammar);

    //计算LR(1)项集族
    void computeItemSets(vector<Production>& grammar);

    //计算项集族闭包
    ItemSet closure(ItemSet &productions);

    //生成LR(1)自动机
    void automaton();

    set<ItemSet> construct_LR1_itemsets();

    void printItemSet();

    void printProduction();

    void calculateFirstSet();
};


