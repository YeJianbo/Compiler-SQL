#include <bits/stdc++.h>

#define GRAMMAR_PATH "E:\\Code\\JetBrain\\CLion\\LAXER\\GRAMMAR.txt"


using namespace std;

//缓冲区
char str[1024];
string line;

string rtrim(string str) {
    size_t pos = str.find_last_not_of(' ');
    if (pos != string::npos) {
        str.erase(pos + 1);
    }
    return str;
}

string ltrim(string str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) {
        return "";
    }
    return str.substr(first);
}

string trim(string str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

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

struct Node{ //节点
    int id;
    string name;
    bool operator < (const Node &o) const
    {
        return id < o.id;
    }
};

class FA{   //自动机M = (K, ∑, f, S, Z)
public:
    vector<Node> startState;                   //初态集S
    vector<Node> endState;                     //终态集Z
    vector<Node> States;                       //状态集K
    set<char> charSet;                      //字母表
    map<Node,map<char,set<Node>>> transNFA; //NFA状态转移fN(Node:当前状态 map：不同输入对应的状态转移)
    map<Node,map<char,Node>> transDFA;      //DFA状态转移fD
//    string stringMod;                       //正规式
    void modifyToDFA();                     //函数，用于NFA->DFA
    void minimizeDFA();                     //函数，用于最小化DFA
    int count = 0;                          //节点计数

//    FA(string stringMod){       //输入正规式->创建NFA->确定化为DFA->DFA最小化
//        this->stringMod = stringMod;
//    }
    void GrammarToNFA(string path);
    //输出FA
    void printEdge(){
        for(const auto& from : transNFA){
            for (const auto& e : from.second) {
                cout<<"from "<<from.first.id<<"["<<from.first.name<<"] through ["<<e.first<<"] to ";
                int n = 0;
                for (const auto& to : e.second) {
                    if(n > 0){
                        cout << " And ";
                    }
                    cout<<to.id<<"["<<to.name<<"]";
                    n++;
                }
                cout<<endl;
            }
        }
    }

    void printDFA(){
        for(const auto& from : transDFA){
            for (const auto& e : from.second) {
                cout<<"from "<<from.first.id<<"["<<from.first.name<<"] through ["<<e.first<<"] to "<<e.second.id<<"["<<e.second.name<<"]";

                cout<<endl;
            }
        }
    }

    pair<Node,Node> createNFA();

    bool check(string in);
    
    pair<Node,Node> createNFA(int& l);

    void deal(string line);

    Node insertIntoStartState(string name){
        int flag = 0;
        Node node;
        // 使用迭代器遍历 startState
        for (auto it = startState.begin(); it != startState.end(); ++it) {
            //已有该状态，则不插入
            if (it->name == name) {
                flag = 1;
                node.name = it->name;
                node.id = it->id;
                break;
            }
        }
        //找不到状态,新增该状态
        if (!flag){
            node = {count++,name};
            startState.push_back(node);
            States.push_back(node);
        }
        return node;
    }

    Node insertIntoEndState(string name){
        int flag = 0;
        Node node;
        // 使用迭代器遍历 startState
        for (auto it = endState.begin(); it != endState.end(); ++it) {
            //已有该状态，则不插入
            if (it->name == name) {
                flag = 1;
                node.name = it->name;
                node.id = it->id;
                break;
            }
        }
        //找不到状态,新增该状态
        if (!flag){
            node = {count++,name};
            endState.push_back(node);
            States.push_back(node);
        }
        return node;
    }

    Node insertIntoState(string name){
        int flag = 0;
        Node node;
        // 使用迭代器遍历 startState
        for (auto it = States.begin(); it != States.end(); ++it) {
            //已有该状态，则不插入
            if (it->name == name) {
                flag = 1;
                node.name = it->name;
                node.id = it->id;
                break;
            }
        }
        //找不到状态,新增该状态
        if (!flag){
            node = {count++,name};
            States.push_back(node);
        }
        return node;
    }
};
//构造输入内容为NFA
void FA::GrammarToNFA(string path){
    ifstream file(path);

    if(!file){
        cout<<"找不到文法文件！"<<endl;
        return ;
    }
    //读取第一行存在line中
    getline(file,line);
    int i = 0;
    while (i < line.length() && line[i] != ' ' && line[i] != '-')
        i++;
    //初态
    string start = line.substr(0,i);
    //加入初态
    insertIntoStartState(start);
    do{
        //去除前面的空格
        line = trim(line);
        //跳过注释
        if(line[0] == '/' && line[1] == '/'){
            line.clear();
            getline(cin,line);
            if(line.empty()) break;
            continue;
        }

        //没有'|'，说明是单条产生式，直接处理
        if (line.find('|') == string::npos){
            size_t p2 = line.find("->");
            string h = trim(line.substr(0,p2));
            line = trim(line.substr(p2+2));
//            cout<<line;
            deal(h +" -> "+ line);
        }else{
            size_t p = line.find("->");
            //没找到，产生式错误
            if (p == string::npos){
                cout<<"输入的语法有误！"<<endl;
                return;
            }
            string h = trim(line.substr(0,p));
            line = trim(line.substr(p+2));
//            cout<<line;
            stringstream ss(line);
            string production;
            while (getline(ss, production, '|')) {
                deal(h+" -> "+trim(production));
            }
        }
        //处理完成，清空字符串
        line.clear();
    }while(getline(file,line)&&!line.empty());


}

//处理单条产生式，已去除空格
//如S -> aA
void FA::deal(string line){
    istringstream iss(line);
//    cout<<iss.str()<<endl;
    string ls, arrow, rs; //ls:左边    arrow：箭头    rs：右边
    char input;
    //根据空格切分
    iss >> ls >> arrow >> rs;
//    cout<<ls<<" "<<arrow<<" "<<rs<<endl;
    Node node = insertIntoState(ls);
    //如果是大写字母开头，输入为?（空）
    if (isupper(rs[0])){
        input = '$';
    }else if (rs[0] == '\\'){
        //'\'开头表示转义字符，单独处理
        switch (rs[1]) {
            case '0':
                input = ' ';
            case 't':
                input = '\t';
            case 'n':
                input = '\n';
            default:
                input = '$';
        }
    }else{
        //正常的输入
        input = rs[0];
        //去掉第一位，剩下的是下一状态
        rs = rs.substr(1);
        if (rs.empty()){
            rs = "EndState_"+to_string(endState.size()+1);
            insertIntoEndState(rs);
        }
    }
//    cout<<input<<endl;
//    cout<<insertIntoState(rs).id<<" "<<rs<<endl;
    transNFA[node][input].insert(insertIntoState(rs));
}

void FA::modifyToDFA()
{
    queue<set<Node>> Q;
    set<Node> startSet;
    for (auto s : startState)
    {
        startSet.insert(s);
    }
    Q.push(startSet);
    map<set<Node>, Node> newState;   // DFA新状态对应的旧状态集合
    while (!Q.empty())
    {
        set<Node> currentSet = Q.front();
        Q.pop();
        Node currentState;
        currentState.id = count++;
        for (auto s : currentSet)
        {
            currentState.name += s.name;
            currentState.name += ',';
        }
        States.push_back(currentState);
        newState[currentSet] = currentState;

        for (auto c : charSet)
        {
            set<Node> newSet;
            for (auto s : currentSet)
            {
                if (transNFA[s].find(c) != transNFA[s].end())
                {
                    newSet.insert(transNFA[s][c].begin(), transNFA[s][c].end());
                }
            }
            if (!newSet.empty())
            {
                if (newState.find(newSet) == newState.end())
                {
                    Q.push(newSet);
                    Node newStateNode;
                    newStateNode.id = count++;
                    for (auto s : newSet)
                    {
                        newStateNode.name += s.name;
                        newStateNode.name += ',';
                    }
                    States.push_back(newStateNode);
                    newState[newSet] = newStateNode;
                }
                transDFA[currentState][c] = newState[newSet];
            }
        }
    }

    for (auto s : States)
    {
        for (auto end : endState)
        {
            if (s.name.find(end.name) != string::npos)
            {
                s.name += '*';
                break;
            }
        }
    }
}

int main() {
    //根据文法获取NFA
    FA fa;
    fa.GrammarToNFA(GRAMMAR_PATH);
    //控制台输出该NFA
    fa.printEdge();
    //将该NFA转为DFA,然后最小化,输出最小化的DFA
    fa.modifyToDFA();
    cout<<"DFA:"<<endl;
//    fa.minimizeDFA();
    fa.printDFA();
    //进行词法分析
    return 0;
}
