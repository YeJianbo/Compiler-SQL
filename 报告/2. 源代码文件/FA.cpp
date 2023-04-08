#include "FA.h"


//输入一组节点，给定一个输入input（来自charset）,求经过该input到达的下一状态
//将该状态求闭包，返回该状态
set<Node> FA::move(char input,const set<Node>& node,FA nfa) {
    set<Node> n;
    //遍历状态中的所有元素，输入给定的字符，将结果添加进新状态
    for (const auto & it : node) {
        addAllElements(n,transNFA[it][input]);
    }
    for (const auto & _it : n) {
        addAllElements(n,nfa.closure(_it));
    }
    return n;
}

//构造输入内容为NFA
void FA::GrammarToNFA(const string& path){
    string line;
    ifstream file(path);
    if(!file){
        cout<<"找不到文法文件！"<<endl;
        return ;
    }
    //读取第一行存在line中
    do {
        getline(file,line);
        line = trim(line);
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
        size_t p = line.find("->");
        //没找到，产生式错误
        if (p == string::npos){
            cout<<"输入的语法有误！"<<endl;
            return;
        }
        string h = trim(line.substr(0,p));
        line = trim(line.substr(p+2));
        //没有'|'，说明是单条产生式，直接处理
        if (line.find('|') == string::npos){
//            deal(h +" -> "+ line);
            deal(h,line);
        }else{
            stringstream ss(line);
            string production;
            while (getline(ss, production, '|')) {
//                deal(h+" -> "+trim(production));
                deal(h, trim(production));
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
void FA::deal(const string& l, const string& r){
    char input;
//    string ls = l;
    string rs = r;
    Node node = insertIntoState(l);
    //如果是大写字母开头，输入为$（空）
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
            string ss;
            //标识符和关键字
            if (startsWith("I",l)){
                ss = "_I";
            }else if (startsWith("O",l)){
                //运算符
                ss = "_O";
            }else if (startsWith("Separator",l)){
                //Separator
                ss = "_S";
            }else if (startsWith("A",l) || startsWith("B",l) || startsWith("C",l) || startsWith("D",l)){
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
            string ss;
            //标识符和关键字
            if (startsWith("I",l)){
                ss = "_I";
            }else if (startsWith("O",l)){
                //运算符
                ss = "_O";
            }else if (startsWith("Separator",l)){
                //Separator
                ss = "_S";
            }else if (startsWith("A",l) || startsWith("B",l) || startsWith("C",l) || startsWith("D",l)){
                //常量
                ss = "_C";
            }
            rs = "EndState_"+to_string(endState.size()+1)+ss;
            insertIntoEndState(rs);
        }
    }
    charSet.insert(input);
    transNFA[node][input].insert(insertIntoState(rs));
}

//将NFA转换为DFA
void FA::TransToDFA(FA nfa){
    set<Node> n = nfa.closure(nfa.startState);
    Node start = insertIntoStartState("Start");
    stateCorr["Start"] = n;
    charSet = nfa.getCharSet();
    charSet.erase('$');
    getDFA(nfa, start, n);
}

//处理数据，处理完得到DFA
void FA::getDFA(FA nfa, const Node& start, const set<Node>& n) {
    //遍历charSet
    for (char it2 : charSet) {
        int flag = 0, flag2 = 0,flag3 = 1;
        // 取得下一状态的集合（已做闭包）
        set<Node> n2 = nfa.move(it2,n,nfa);
        string buf;
        string ss;
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
        for (auto & it4 : stateCorr) {
//            //如果包含的节点中有终态，标记
//            if (nodeStartsWith(it4->second,"EndState")){
//                flag2 = 1;
//            }
            //如果新状态已经存在，跳过
            if (setsAreEqual(it4.second,n2)) {
                buf = it4.first;
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
                getDFA(nfa, newN, n2);
            } else {
                //状态存在，加入对应关系
                newN = insertIntoState(buf);
            }
            transDFA[start][it2] = newN;
        }
        //处理完成，程序结束
    }
}

void FA::printNFA() {
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
    for (char it : charSet) {
        cout << it << " "; // 打印set中的值
    }
}

Node FA::insertIntoStartState(const string& name) {
    // 使用迭代器遍历 startState
    if (startState.name.empty()) {
        startState.name = name;
        startState.id = 1;
        count++;
        States.insert(startState);
    }
    return startState;
}

Node FA::insertIntoEndState(const string& name) {
    int flag = 0;
    Node node;
    // 使用迭代器遍历 startState
    for (const auto & it : endState) {
        //已有该状态，则不插入
        if (it.name == name) {
            flag = 1;
            node.name = it.name;
            node.id = it.id;
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

Node FA::insertIntoState(const string& name) {
    int flag = 0;
    Node node;
    // 使用迭代器遍历 startState
    for (const auto & State : States) {
        //已有该状态，则不插入
        if (State.name == name) {
            flag = 1;
            node.name = State.name;
            node.id = State.id;
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
    //定义一个栈，用于深度优先搜索
    stack<Node> stack;
    //将初始节点压入栈中
    stack.push(node);
    result.insert(node);
    //遍历transNFA表，如果存在ε-转移，加入集合，压栈，反复操作
    while (!stack.empty()) {
        Node current = stack.top();
        stack.pop();
        if (transNFA.count(current) > 0 && transNFA[current].count('$') > 0) {
            // 遍历当前节点的所有ε转移
            for (const Node& next : transNFA[current]['$']) {
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

const map<Node, map<char, Node>> &FA::getTransDfa() const {
    return transDFA;
}

const set<Node> &FA::getEndState() const {
    return endState;
}

const Node &FA::getStartState() const {
    return startState;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

bool nodeStartsWith(const set<Node>& nodes, const string& prefix) {
    // 用std::any_of()替换循环
    bool contains_value = std::any_of(nodes.begin(), nodes.end(),
                                      [prefix](const Node& node){ return node.name.compare(0, prefix.size(), prefix) ==
                                                                         0; });
//    for (const auto& node : nodes) {
//        if (node.name.compare(0, prefix.size(), prefix) == 0) {
//            return true;
//        }
//    }
    return contains_value;
}

bool setsAreEqual(const set<Node>& s1, set<Node> s2) {
    if (s1.size() != s2.size()) {
        return false;
    }
    for (const Node& x : s1) {
        if (s2.find(x) == s2.end()) {
            return false;
        }
    }
    return true;
}

template<typename T>
void addAllElements(set<T> &destSet, const set<T> &sourceSet) {
    for (const auto& e: sourceSet){
        destSet.insert(e);
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

bool hasNode(set<Node>& nodes, const string& string1, const string& string2) {
    // 用std::any_of()替换循环
    bool b = std::any_of(nodes.begin(), nodes.end(), [&](const auto &node) {
        return (node.name.substr(0, string1.length()) == string1) && (node.name.substr(node.name.length() - string2.length()) == string2);
    });
//    for (const auto & node : nodes) {
//        if (node.name.substr(0, string1.length()) == string1 && node.name.substr(node.name.length() - string2.length()) == string2) {
//            return true;
//        }
//    }
    return b;
}

bool isNodeNameEndsWith(const Node& node, const string& string2) {
    return (node.name.size() >= string2.size() &&
            node.name.compare(node.name.size() - string2.size(), string2.size(), string2) == 0);
}

set<string> readWordsFromFile(const string& path) {
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

void printTokens(vector<Token> tokens) {
    ofstream  out(TOKEN_PATH);
    if (!out.is_open()){
        cout<<"打开Token文件失败！"<<endl;
        return;
    }
    cout << left << setw(20) << "line";
    cout << left << setw(20) << "Type";
    cout << left << setw(20) << "value" << endl;
    for (auto & token : tokens) {
        cout << left << setw(20) << token.line << right;
        out << token.line <<"\t";
        switch (token.type) {
            case 0:
                cout << left << setw(20) << "KEYWORD";
                out << "KEYWORD\t";
                break;
            case 1:
                cout << left << setw(20) << "IDENTIFIER";
                out << "IDENTIFIER\t";
                break;
            case 2:
                cout << left << setw(20) << "CONSTANT";
                out << "CONSTANT\t";
                break;
            case 3:
                cout << left << setw(20) << "DELIMITER";
                out << "DELIMITER\t";
                break;
            case 4:
                cout << left << setw(20) << "OPERATOR";
                out << "OPERATOR\t";
                break;
            default:
                cout << left << setw(20) << "unknown";
                out << "unknown\t";
        }
        if (token.value == " "){
            token.value = "\\0";
        }
        if (token.value == "  "){
            token.value = "\\t";
        }
        if (token.value == "\n"){
            token.value = "\\n";
        }
        cout << left << setw(20) << token.value << endl;
        out << token.value << "\n";
    }
    out.close();
}

//单行词法分析
vector<Token> LAbyLine(const FA& dfa, const string& line, int n) {
    vector<Token> t;
    string l = trim(line);
    //缓冲区，保存输入的字符
    string buf;
    //当前状态为初态
    Node state = dfa.getStartState();
    map<Node,map<char,Node>> transDFA = dfa.getTransDfa();
    set<Node> e = dfa.getEndState();
    int i = 0;
    Token ttt;
    //遍历行
    while ( i < l.length() ){
        state = transDFA[state][l[i]];
//        if (l=="float b = 1.145e+14.1;"){
//            int iii = 0;
//        }
        //找不到下一状态
        if (state.id == 0){
            vector<Token> v;
            string s;
            s+=l[i];
            //是常量，有e，说明是科学计数法，返回信息提示错误
            if (ttt.type == CONSTANT && !(ttt.value.find('e') == string::npos && ttt.value.find('E') == string::npos)){
                //1为错误码
                v.push_back({ERROR,s,1});
                v.push_back(ttt);
            } else if (ttt.type == CONSTANT && isdigit(ttt.value[0])){
                v.push_back({ERROR,s,2});
                v.push_back(ttt);
            } else if (buf[buf.size()-1] == 'e' || buf[buf.size()-1] == 'E'){
                v.push_back({ERROR,s,2});
                ttt.value += buf;
                v.push_back(ttt);
            }else {
                v.push_back({ERROR,s,9});
                v.push_back(ttt);
            }

            return v;
        }
        buf += l[i];
        //如果终态集中找不到state，说明不是终态
        //此时必须继续读取下一个字符
        if(e.find(state) == e.end()){
            i++;
            continue;
        }else{
            if (isdigit(ttt.value[0]) && isalpha(l[i])){
//                if (!(l[i] == 'i')){
                    vector<Token> v;
                    string s;
                    s += l[i];
                    v.push_back({ERROR,s,2});
                    v.push_back(ttt);
                    return v;
//                }
            }
            //包含终态，看下一个字符能否被接受，如果不能被接收，创建Token，读取下一个词
            if(transDFA[state][l[i+1]].id == 0){
                Token token;
                if (isNodeNameEndsWith(state, "I")) {
                    //标识符和关键字，遍历关键字集合，判断是否是关键字
                    set<string> keyword = readWordsFromFile(KEYWORD_PATH);
                    //是关键字
                    if (keyword.find(buf) != keyword.end()) {
                        token.type = KEYWORD;
                    } else{
                        token.type = IDENTIFIER;
                        if (isdigit(buf[0])){
                            cout << "程序错误！在第 " << n << "行发现了不合法的标识符：" << buf << endl;
                        }
                    }
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
                ttt = token;
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
int LexicalAnalyze(const FA& dfa, const string& path) {
    vector<Token> tokens;
    string line;
    int n = 0;
    ifstream file(path);
    if (!file){
        cout << "找不到源代码文件！" << endl;
        return 0;
    }
    //从file中获取一行，保存在line中
    while(getline(file,line)){
        n++;
        line = trim(line);
        //开头就是注释，跳过
        if ((line[0] == '/' && line[1] == '/') || line.empty()){
            continue;
        }
        //结尾有注释，去除后面的注释
        if (line.find("//") != string::npos){
            line = line.substr(0, line.find("//"));
        }
        vector<Token> to = LAbyLine(dfa, line, n);
        //报错
        if (to[0].type == ERROR){
            cout<<"错误！在第 "<< n <<" 行"<<endl;
            cout<<to[1].value<<to[0].value<<endl;
            for (int i = 0; i < to[1].value.length(); ++i) {
                cout<<" ";
            }
            cout<<"^"<<endl;
            switch (to[0].line) {
                case 1:
                    cout<<"科学计数法错误"<<endl;
                    break;
                case 2:
                    cout<<"标识符错误！"<<endl;
                    break;
                default:
                    cout<<"输入字符 '"<<to[0].value<<"' 在此处不合法！"<<endl;
            }
            return 0;
        }
        tokens.insert(tokens.end(),to.begin(),to.end());
    }
    file.close();
    cout<<"词法分析结果："<<endl;
    printTokens(tokens);
    return 1;
}
