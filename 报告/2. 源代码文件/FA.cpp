#include "FA.h"


//����һ��ڵ㣬����һ������input������charset��,�󾭹���input�������һ״̬
//����״̬��հ������ظ�״̬
set<Node> FA::move(char input,const set<Node>& node,FA nfa) {
    set<Node> n;
    //����״̬�е�����Ԫ�أ�����������ַ����������ӽ���״̬
    for (const auto & it : node) {
        addAllElements(n,transNFA[it][input]);
    }
    for (const auto & _it : n) {
        addAllElements(n,nfa.closure(_it));
    }
    return n;
}

//������������ΪNFA
void FA::GrammarToNFA(const string& path){
    string line;
    ifstream file(path);
    if(!file){
        cout<<"�Ҳ����ķ��ļ���"<<endl;
        return ;
    }
    //��ȡ��һ�д���line��
    do {
        getline(file,line);
        line = trim(line);
    }while(line[0] == '/' && line[1] == '/');
    int i = 0;
    while (i < line.length() && line[i] != ' ' && line[i] != '-')
        i++;
    //��̬
    string start = line.substr(0,i);
    //�����̬
    insertIntoStartState(start);
    do{
        //ȥ��ǰ��Ŀո�
        line = trim(line);
        //����ע��(��һ�в�����)
        if(line[0] == '/' && line[1] == '/'){
            line.clear();
            getline(file,line);
            if(line.empty()) break;
            continue;
        }
        size_t p = line.find("->");
        //û�ҵ�������ʽ����
        if (p == string::npos){
            cout<<"������﷨����"<<endl;
            return;
        }
        string h = trim(line.substr(0,p));
        line = trim(line.substr(p+2));
        //û��'|'��˵���ǵ�������ʽ��ֱ�Ӵ���
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
        //������ɣ�����ַ���
        line.clear();
        getline(file,line);
    }while(!line.empty());
    file.close();
}

//����������ʽ����ȥ���ո�
//��S -> aA
void FA::deal(const string& l, const string& r){
    char input;
//    string ls = l;
    string rs = r;
    Node node = insertIntoState(l);
    //����Ǵ�д��ĸ��ͷ������Ϊ$���գ�
    if (isupper(rs[0])){
        input = '$';
    }else if (rs[0] == '\\'){
        //'\'��ͷ��ʾת���ַ�����������
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
        //����̬����¼���
        if (rs.empty()){
            string ss;
            //��ʶ���͹ؼ���
            if (startsWith("I",l)){
                ss = "_I";
            }else if (startsWith("O",l)){
                //�����
                ss = "_O";
            }else if (startsWith("Separator",l)){
                //Separator
                ss = "_S";
            }else if (startsWith("A",l) || startsWith("B",l) || startsWith("C",l) || startsWith("D",l)){
                //����
                ss = "_C";
            }
            rs = "EndState_"+to_string(endState.size()+1) + ss;
            insertIntoEndState(rs);
        }
    }else{
        //����������
        input = rs[0];
        //ȥ����һλ��ʣ�µ�����һ״̬
        rs = rs.substr(1);
        if (rs.empty()){
            string ss;
            //��ʶ���͹ؼ���
            if (startsWith("I",l)){
                ss = "_I";
            }else if (startsWith("O",l)){
                //�����
                ss = "_O";
            }else if (startsWith("Separator",l)){
                //Separator
                ss = "_S";
            }else if (startsWith("A",l) || startsWith("B",l) || startsWith("C",l) || startsWith("D",l)){
                //����
                ss = "_C";
            }
            rs = "EndState_"+to_string(endState.size()+1)+ss;
            insertIntoEndState(rs);
        }
    }
    charSet.insert(input);
    transNFA[node][input].insert(insertIntoState(rs));
}

//��NFAת��ΪDFA
void FA::TransToDFA(FA nfa){
    set<Node> n = nfa.closure(nfa.startState);
    Node start = insertIntoStartState("Start");
    stateCorr["Start"] = n;
    charSet = nfa.getCharSet();
    charSet.erase('$');
    getDFA(nfa, start, n);
}

//�������ݣ�������õ�DFA
void FA::getDFA(FA nfa, const Node& start, const set<Node>& n) {
    //����charSet
    for (char it2 : charSet) {
        int flag = 0, flag2 = 0,flag3 = 1;
        // ȡ����һ״̬�ļ��ϣ������հ���
        set<Node> n2 = nfa.move(it2,n,nfa);
        string buf;
        string ss;
        //flag3 = 0,˵��û����һ״̬
        if(n2.empty())
            flag3 = 0;
        //��������Ľڵ�������̬�����
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
        //����״̬�Ƿ����
        for (auto & it4 : stateCorr) {
//            //��������Ľڵ�������̬�����
//            if (nodeStartsWith(it4->second,"EndState")){
//                flag2 = 1;
//            }
            //�����״̬�Ѿ����ڣ�����
            if (setsAreEqual(it4.second,n2)) {
                buf = it4.first;
                flag = 1;
                break;
            }
        }
        if(flag3) {
            //flag == 0,˵��״̬�����ڣ���ʱ�����½ڵ㣬�����½ڵ���д���
            Node newN;
            string name = "State" + to_string(States.size() + 1);
            if (!flag) {
                //flag2 == 1,˵����״̬Ϊ��̬����Ҫ��ӽ���̬��
                if (flag2) {
                    name = "EndState" + to_string(endState.size() + 1) + ss;
                    newN = insertIntoEndState(name);
                }
                newN = insertIntoState(name);
                stateCorr[name] = n2;
                getDFA(nfa, newN, n2);
            } else {
                //״̬���ڣ������Ӧ��ϵ
                newN = insertIntoState(buf);
            }
            transDFA[start][it2] = newN;
        }
        //������ɣ��������
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
        cout << it << " "; // ��ӡset�е�ֵ
    }
}

Node FA::insertIntoStartState(const string& name) {
    // ʹ�õ��������� startState
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
    // ʹ�õ��������� startState
    for (const auto & it : endState) {
        //���и�״̬���򲻲���
        if (it.name == name) {
            flag = 1;
            node.name = it.name;
            node.id = it.id;
            break;
        }
    }
    //�Ҳ���״̬,������״̬
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
    // ʹ�õ��������� startState
    for (const auto & State : States) {
        //���и�״̬���򲻲���
        if (State.name == name) {
            flag = 1;
            node.name = State.name;
            node.id = State.id;
            break;
        }
    }
    //�Ҳ���״̬,������״̬
    if (!flag){
        node = {++count,name};
        States.insert(node);
    }
    return node;
}

//������ڵ�Ħ�-�հ�
set<Node> FA::closure(const Node& node) {
    set<Node> result;
    //����һ��ջ�����������������
    stack<Node> stack;
    //����ʼ�ڵ�ѹ��ջ��
    stack.push(node);
    result.insert(node);
    //����transNFA��������ڦ�-ת�ƣ����뼯�ϣ�ѹջ����������
    while (!stack.empty()) {
        Node current = stack.top();
        stack.pop();
        if (transNFA.count(current) > 0 && transNFA[current].count('$') > 0) {
            // ������ǰ�ڵ�����Ц�ת��
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
    // ��std::any_of()�滻ѭ��
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
    // ��std::any_of()�滻ѭ��
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
        cout<<"�Ҳ����ؼ����ļ���"<<endl;
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
        cout<<"��Token�ļ�ʧ�ܣ�"<<endl;
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

//���дʷ�����
vector<Token> LAbyLine(const FA& dfa, const string& line, int n) {
    vector<Token> t;
    string l = trim(line);
    //������������������ַ�
    string buf;
    //��ǰ״̬Ϊ��̬
    Node state = dfa.getStartState();
    map<Node,map<char,Node>> transDFA = dfa.getTransDfa();
    set<Node> e = dfa.getEndState();
    int i = 0;
    Token ttt;
    //������
    while ( i < l.length() ){
        state = transDFA[state][l[i]];
//        if (l=="float b = 1.145e+14.1;"){
//            int iii = 0;
//        }
        //�Ҳ�����һ״̬
        if (state.id == 0){
            vector<Token> v;
            string s;
            s+=l[i];
            //�ǳ�������e��˵���ǿ�ѧ��������������Ϣ��ʾ����
            if (ttt.type == CONSTANT && !(ttt.value.find('e') == string::npos && ttt.value.find('E') == string::npos)){
                //1Ϊ������
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
        //�����̬�����Ҳ���state��˵��������̬
        //��ʱ���������ȡ��һ���ַ�
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
            //������̬������һ���ַ��ܷ񱻽��ܣ�������ܱ����գ�����Token����ȡ��һ����
            if(transDFA[state][l[i+1]].id == 0){
                Token token;
                if (isNodeNameEndsWith(state, "I")) {
                    //��ʶ���͹ؼ��֣������ؼ��ּ��ϣ��ж��Ƿ��ǹؼ���
                    set<string> keyword = readWordsFromFile(KEYWORD_PATH);
                    //�ǹؼ���
                    if (keyword.find(buf) != keyword.end()) {
                        token.type = KEYWORD;
                    } else{
                        token.type = IDENTIFIER;
                        if (isdigit(buf[0])){
                            cout << "��������ڵ� " << n << "�з����˲��Ϸ��ı�ʶ����" << buf << endl;
                        }
                    }
                } else if (isNodeNameEndsWith(state, "O")) {
                    token.type = OPERATOR;
                } else if (isNodeNameEndsWith(state, "S")) {
                    token.type = DELIMITER;
                } else if (isNodeNameEndsWith(state, "C")) {
                    token.type = CONSTANT;
                }
                //�������̬,���ﱻ���ܣ�����Token
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
                //��һ��input�ܱ�����
                i++;
                continue;
            }
        }
    }
    return t;
}

//�ʷ�����
//����dfa���ļ�·����������дʷ��������зֵõ�Token��
int LexicalAnalyze(const FA& dfa, const string& path) {
    vector<Token> tokens;
    string line;
    int n = 0;
    ifstream file(path);
    if (!file){
        cout << "�Ҳ���Դ�����ļ���" << endl;
        return 0;
    }
    //��file�л�ȡһ�У�������line��
    while(getline(file,line)){
        n++;
        line = trim(line);
        //��ͷ����ע�ͣ�����
        if ((line[0] == '/' && line[1] == '/') || line.empty()){
            continue;
        }
        //��β��ע�ͣ�ȥ�������ע��
        if (line.find("//") != string::npos){
            line = line.substr(0, line.find("//"));
        }
        vector<Token> to = LAbyLine(dfa, line, n);
        //����
        if (to[0].type == ERROR){
            cout<<"�����ڵ� "<< n <<" ��"<<endl;
            cout<<to[1].value<<to[0].value<<endl;
            for (int i = 0; i < to[1].value.length(); ++i) {
                cout<<" ";
            }
            cout<<"^"<<endl;
            switch (to[0].line) {
                case 1:
                    cout<<"��ѧ����������"<<endl;
                    break;
                case 2:
                    cout<<"��ʶ������"<<endl;
                    break;
                default:
                    cout<<"�����ַ� '"<<to[0].value<<"' �ڴ˴����Ϸ���"<<endl;
            }
            return 0;
        }
        tokens.insert(tokens.end(),to.begin(),to.end());
    }
    file.close();
    cout<<"�ʷ����������"<<endl;
    printTokens(tokens);
    return 1;
}
