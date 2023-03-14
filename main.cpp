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

    bool operator == (const Node &o) const{
        return id == o.id;
    }
};

bool nodeStartsWith(set<Node> nodes, string prefix) {
    for (const auto& node : nodes) {
        if (node.name.compare(0, prefix.size(), prefix) == 0) {
            return true;
        }
    }
    return false;
}

bool setsAreEqual(set<Node> s1, set<Node> s2) {
    if (s1.size() != s2.size()) {
        return false;
    }
    for (Node x : s1) {
        if (s2.find(x) == s2.end()) {
            return false;
        }
    }
    return true;
}


template <typename T>
void addAllElements(std::set<T>& destSet, const std::set<T>& sourceSet)
{
    for (const auto& element : sourceSet)
    {
        destSet.insert(element);
    }
}

class FA{   //自动机M = (K, ∑, f, S, Z)
    vector<Node> startState;                   //初态集S
    vector<Node> endState;                     //终态集Z
    vector<Node> States;                       //状态集K
public:
    const vector<Node> &getStartState() const;

    void setStartState(const vector<Node> &startState);

    const vector<Node> &getEndState() const;

    void setEndState(const vector<Node> &endState);

    const vector<Node> &getStates() const;

    void setStates(const vector<Node> &states);

    const set<char> &getCharSet() const;

    void setCharSet(const set<char> &charSet);

    const map<Node, map<char, set<Node>>> &getTransNfa() const;

    void setTransNfa(const map<Node, map<char, set<Node>>> &transNfa);

    const map<Node, map<char, Node>> &getTransDfa() const;

    void setTransDfa(const map<Node, map<char, Node>> &transDfa);

    int getCount() const;

    void setCount(int count);

private:
    set<char> charSet;                      //字母表
    map<Node,map<char,set<Node>>> transNFA; //NFA状态转移fN(Node:当前状态 map：不同输入对应的状态转移)
    map<Node,map<char,Node>> transDFA;      //DFA状态转移fD
    int count = 0;                          //节点计数
    map<string,set<Node>> stateCorr;        //状态对应关系

public:
    FA TransToDFA();                     //函数，用于NFA->DFA

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

    set<Node> move(char input,set<Node> node,FA nfa);
};
//输入一组节点，给定一个输入input（来自charset）,求经过该input到达的下一状态
//将该状态求闭包，返回该状态
set<Node> FA::move(char input,set<Node> node,FA nfa) {
//    cout<<input<<endl;
    set<Node> n;
//    cout<<"[ ";
    //遍历状态中的所有元素，输入给定的字符，将结果添加进新状态
    for (auto it = node.begin(); it != node.end(); ++it) {
        addAllElements(n,transNFA[*it][input]);
    }
    for (auto it__ = n.begin(); it__ != n.end(); ++it__) {
        addAllElements(n,nfa.closure(*it__));
    }
    for (auto it2 = n.begin(); it2 != n.end(); ++it2) {
//        cout<<it2->id<<" "<<it2->name<<" ";
    }
//    cout<<"]"<<endl;
    return n;
}

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
        //跳过注释(暂时不能用)
        if(line[0] == '/' && line[1] == '/'){
            line.clear();
            getline(file,line);
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
                break;
            case 't':
                input = '\t';
                break;
            case 'n':
                input = '\n';
                break;
            default:
                input = '$';
        }
        rs = rs.substr(1);
        if (rs.empty()){
            rs = "EndState_"+to_string(endState.size()+1);
            insertIntoEndState(rs);
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
    charSet.insert(input);
//    cout<<insertIntoState(rs).id<<" "<<rs<<endl;
    transNFA[node][input].insert(insertIntoState(rs));
}

//将NFA转换为DFA
void FA::TransToDFA(FA nfa){
    set<Node> n = nfa.closure(nfa.startState.front());
//    for (set<Node>::iterator it2 = n.begin(); it2 != n.end(); ++it2) {
//        cout<<it2->id<<" "<<it2->name<<endl;
//    }
    Node start = insertIntoStartState("Start"+to_string((startState.size()+1)));
    stateCorr["Start_"+to_string(startState.size())] = n;
    charSet = nfa.getCharSet();
    charSet.erase('$');
    deal2(nfa,start,n);
}

//处理数据，处理完得到DFA
void FA::deal2(FA nfa, Node start,set<Node> n) {
    //遍历charSet
    for (set<char>::iterator it2 = charSet.begin(); it2 != charSet.end(); ++it2) {
        int flag = 0, flag2 = 0,flag3 = 1;
        // 取得下一状态的集合（已做闭包）
        set<Node> n2 = nfa.move(*it2,n,nfa);
        string buf;
        //flag3 = 0,说明没有下一状态
        if(n2.empty())
            flag3 = 0;
        //如果包含的节点中有终态，标记
        if (nodeStartsWith(n2,"EndState")){
            flag2 = 1;
        }
        //检查该状态是否存在
        for (auto it4 = stateCorr.begin(); it4 != stateCorr.end(); ++it4) {
//            //如果包含的节点中有终态，标记
//            if (nodeStartsWith(it4->second,"EndState")){
//                flag2 = 1;
//            }
            //如果新状态已经存在，跳过
            if (setsAreEqual(it4->second,n2)) {
                buf = it4->first;
                flag = 1;
                break;
            }
        }
        if(flag3) {
            //flag == 0,说明状态不存在，此时新增新节点，并对新节点进行处理
            Node newN;
            string name = "State" + to_string(States.size() + 1);
            if (!flag) {
                //flag2 == 1,说明该状态为终态，需要添加进终态集
                if (flag2) {
                    name = "EndState" + to_string(endState.size() + 1);
                    newN = insertIntoEndState(name);
                }
                newN = insertIntoState(name);
                stateCorr[name] = n2;
                deal2(nfa, newN, n2);
            } else {
                //状态存在，加入对应关系
                newN = insertIntoState(buf);
            }
            transDFA[start][*it2] = newN;
        }
        //处理完成，程序结束
    }
}

void FA::printEdge() {
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

void FA::printDFA() {
    for(const auto& from : transDFA){
        for (const auto& e : from.second) {
            cout<<"from "<<from.first.id<<"["<<from.first.name<<"] through ["<<e.first<<"] to "<<e.second.id<<"["<<e.second.name<<"]";

            cout<<endl;
        }
    }
}

void FA::printCharSet() {
    for (set<char>::iterator it = charSet.begin(); it != charSet.end(); ++it) {
        cout << *it << " "; // 打印set中的值
    }

}

Node FA::insertIntoStartState(string name) {
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

Node FA::insertIntoEndState(string name) {
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

Node FA::insertIntoState(string name) {
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

//求输入节点的ε-闭包
set<Node> FA::closure(const Node& node) {
    set<Node> result;
    stack<Node> stack;
    stack.push(node);
    result.insert(node);
    while (!stack.empty()) {
        Node current = stack.top();
        stack.pop();

        if (transNFA.count(current) > 0 && transNFA[current].count('$') > 0) {
            // 遍历当前节点的所有ε转移
            for (Node next : transNFA[current]['$']) {
                if (result.count(next) == 0) {
                    result.insert(next);
                    stack.push(next);
                }
            }
        }
    }
    return result;
}

const vector<Node> &FA::getStartState() const {
    return startState;
}

void FA::setStartState(const vector<Node> &startState) {
    FA::startState = startState;
}

const vector<Node> &FA::getEndState() const {
    return endState;
}

void FA::setEndState(const vector<Node> &endState) {
    FA::endState = endState;
}

const vector<Node> &FA::getStates() const {
    return States;
}

void FA::setStates(const vector<Node> &states) {
    States = states;
}

const set<char> &FA::getCharSet() const {
    return charSet;
}

void FA::setCharSet(const set<char> &charSet) {
    FA::charSet = charSet;
}

const map<Node, map<char, set<Node>>> &FA::getTransNfa() const {
    return transNFA;
}

void FA::setTransNfa(const map<Node, map<char, set<Node>>> &transNfa) {
    transNFA = transNfa;
}

const map<Node, map<char, Node>> &FA::getTransDfa() const {
    return transDFA;
}

void FA::setTransDfa(const map<Node, map<char, Node>> &transDfa) {
    transDFA = transDfa;
}

int FA::getCount() const {
    return count;
}

void FA::setCount(int count) {
    FA::count = count;
}




int main() {
    //根据文法获取NFA
    FA fa;
    fa.GrammarToNFA(GRAMMAR_PATH);
    //控制台输出该NFA
    cout<<"NFA:"<<endl;
    fa.printEdge();
    cout<<"charSet:"<<"[ ";
    fa.printCharSet();
    cout<<"]"<<endl;
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
    return 0;
}
