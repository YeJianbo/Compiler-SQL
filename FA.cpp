#include "FA.h"


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
    string line;
    ifstream file(path);
    if(!file){
        cout<<"找不到文法文件！"<<endl;
        return ;
    }
    //读取第一行存在line中
    do {
        getline(file,line);
        trim(line);
    }while(line[0] == '/' && line[1] == '/');
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
        //跳过注释(第一行不能用)
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
        getline(file,line);
    }while(!line.empty());
    file.close();
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
        rs = rs.substr(2);
        //是终态，记录左边
        if (rs.empty()){
            string ss = "";
            //标识符和关键字
            if (startsWith("I",ls)){
                ss = "_I";
            }else if (startsWith("O",ls)){
                //运算符
                ss = "_O";
            }else if (startsWith("Separator",ls)){
                //Separator
                ss = "_S";
            }else if (startsWith("A",ls) || startsWith("B",ls) || startsWith("C",ls) || startsWith("D",ls)){
                //常量
                ss = "_C";
            }
            rs = "EndState_"+to_string(endState.size()+1) + ss;
            insertIntoEndState(rs);
        }
    }else{
        //正常的输入
        input = rs[0];
        //去掉第一位，剩下的是下一状态
        rs = rs.substr(1);
        if (rs.empty()){
            string ss = "";
            //标识符和关键字
            if (startsWith("I",ls)){
                ss = "_I";
            }else if (startsWith("O",ls)){
                //运算符
                ss = "_O";
            }else if (startsWith("Separator",ls)){
                //Separator
                ss = "_S";
            }else if (startsWith("A",ls) || startsWith("B",ls) || startsWith("C",ls) || startsWith("D",ls)){
                //常量
                ss = "_C";
            }
            rs = "EndState_"+to_string(endState.size()+1)+ss;
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
    set<Node> n = nfa.closure(nfa.startState);
//    for (set<Node>::iterator it2 = n.begin(); it2 != n.end(); ++it2) {
//        cout<<it2->id<<" "<<it2->name<<endl;
//    }
    Node start = insertIntoStartState("Start");
    stateCorr["Start"] = n;
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
        string ss = "";
        //flag3 = 0,说明没有下一状态
        if(n2.empty())
            flag3 = 0;
        //如果包含的节点中有终态，标记
        if (nodeStartsWith(n2,"EndState")){
            flag2 = 1;
            if(hasNode(n2, "EndState", "I")){
                ss = "_I";
            }else if (hasNode(n2,"EndState","O")){
                ss = "_O";
            }else if (hasNode(n2,"EndState","S")){
                ss = "_S";
            }else if(hasNode(n2,"EndState","C")){
                ss = "_C";
            }
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
                    name = "EndState" + to_string(endState.size() + 1) + ss;
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
    // 使用迭代器遍历 startState
    if (startState.name.empty()) {
        startState.name = name;
        startState.id = 1;
        count++;
        States.insert(startState);
    }
    return startState;
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
        node = {++count,name};
        endState.insert(node);
        States.insert(node);
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
        node = {++count,name};
        States.insert(node);
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


const set<char> &FA::getCharSet() const {
    return charSet;
}

const map<Node, map<char, set<Node>>> &FA::getTransNfa() const {
    return transNFA;
}

const map<Node, map<char, Node>> &FA::getTransDfa() const {
    return transDFA;
}

const set<Node> &FA::getEndState() const {
    return endState;
}

const Node &FA::getStartState() const {
    return startState;
}


string trim(string str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

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

template<typename T>
void addAllElements(set<T> &destSet, const set<T> &sourceSet) {
    for (const auto& element : sourceSet)
    {
        destSet.insert(element);
    }
}

bool Node::operator<(const Node &o) const {
    return id < o.id;
}

bool Node::operator==(const Node &o) const {
    return id == o.id;
}


bool startsWith(const string &prefix, const string &str) {
    if (str.size() < prefix.size()) {
        return false;
    }
    return std::equal(prefix.begin(), prefix.end(), str.begin());
}

bool hasNode(set<Node>& nodes, string string1, string string2) {
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        if (it->name.substr(0, string1.length()) == string1 && it->name.substr(it->name.length() - string2.length()) == string2) {
            return true;
        }
    }
    return false;
}

bool isNodeNameEndsWith(const Node& node, const string& string2) {
    return (node.name.size() >= string2.size() &&
            node.name.compare(node.name.size() - string2.size(), string2.size(), string2) == 0);
}

set<string> readWordsFromFile(string path) {
    set<string> words;
    ifstream file(path);
    string word;
    if (!file){
        cout<<"找不到关键字文件！"<<endl;
        return words;
    }
    while (getline(file,word)) {
        words.insert(trim(word));
    }
    file.close();
    return words;
}

template <typename T>
void addVector(vector<T>& v1, vector<T>& v2) {
    v2.insert(v2.end(), v1.begin(), v1.end());
}

void printTokens(vector<Token> tokens) {
    cout << left << setw(20) << "line";
    cout << left << setw(20) << "Type";
    cout << left << setw(20) << "value" << endl;
    for (vector<Token>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        cout << left << setw(20) << it->line << right;
        switch (it->type) {
            case 0:
                cout << left << setw(20) << "KEYWORD";
                break;
            case 1:
                cout << left << setw(20) << "IDENTIFIER";
                break;
            case 2:
                cout << left << setw(20) << "CONSTANT";
                break;
            case 3:
                cout << left << setw(20) << "DELIMITER";
                break;
            case 4:
                cout << left << setw(20) << "OPERATOR";
                break;
            default:
                cout << left << setw(20) << "unknown";
        }
        if (it->value == " "){
            it->value = "'\\0'";
        }
        if (it->value == "  "){
            it->value = "'\\t'";
        }
        if (it->value == "\n"){
            it->value = "'\\n'";
        }
        cout << left << setw(20) << it->value << endl;
    }
}

bool Token::operator<(const Token &o) const {
    return value.compare(o.value);
}
