//
// Created by YeJianbo on 2023/3/16.
//

#include "LR.h"


//����ջ
template<class T>
void reverseStack(stack<T> &s) {
    if (s.empty()) {
        return;
    }
    auto temp = s.top();
    s.pop();
    reverseStack(s);
    stack<T> tempStack;
    while (!s.empty()) {
        tempStack.push(s.top());
        s.pop();
    }
    s.push(temp);
    while (!tempStack.empty()) {
        s.push(tempStack.top());
        tempStack.pop();
    }
}

void addAllElement(set<char> &destSet, const set<char> &sourceSet) {
    for (const auto& element : sourceSet)
    {
        destSet.insert(element);
    }
}

bool Production::operator==(const Production &o) const {
    return l == o.l && r == o.r;
}

bool Production::operator<(const Production &o) const {
    if (l != o.l)
        return l<o.l;
    else
        return r < o.r;
}

bool Item::operator==(const Item &o) const {
    return rule == o.rule && dot == o.dot && lookahead == o.lookahead;
}

bool Item::operator<(const Item &o) const {
    if (rule == o.rule){
        if (dot == o.dot){
            //������λ����ͬ����Ҫ����lookahead��ͬ������
            //ע�⣺�˴�������������⣡�������ܣ��Ժ���
            return lookahead < o.lookahead;
        }
        return dot < o.dot;
    }
    return rule < o.rule;
}

bool ItemSet::operator==(const ItemSet &o) const {
    return items == o.items;
}

bool ItemSet::operator<(const ItemSet &o) const {
    return stoi(name.substr(1, name.size())) < stoi(o.name.substr(1, o.name.size()));
}

void printTree(TreeNode* node, int depth) {
    if (node == nullptr) {
        return;
    }

    for (int i = 0; i < depth; i++) {
        cout << "|--"; // ÿ����һ����ȣ�����2���ո�
    }
    cout << node->symbol << endl;

    for (auto child : node->children) {
        printTree(child, depth + 1); // �ݹ�����ӽڵ�
    }
}


void LR::printToken() {
    cout << left << setw(20) << "line";
    cout << left << setw(20) << "Type";
    cout << left << setw(20) << "value" << endl;
    for (auto & token : tokens) {
        cout << left << setw(20) << token.line << right;
        switch (token.type) {
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
    }
}

//�����ַ�����first��
set<char> calc_first_s(string &s, map<char, set<char>> &first_set, vector<Production> &prods) {
    //first����õĽ�� visited����¼�ѷ��ʵķ���
    set<char> first;
    set<char> visited;
    //���s�ĵ�һ���������ս��(Сд��ĸ)��ֱ�ӽ������FIRST������
    if (!isupper(s[0]) || s[0] == '$') {
        if (s[0] != '$') {
            first.insert(s[0]);
        }
        else {
            first.insert('$');
        }
    }
    //���򣬼����һ�����ŵ�FIRST����
    else {
        //����calc_first���㵥���ַ���first���������صļ��ϼ������У���������մ����ݹ���ã�ֱ��û�пմ�����
        auto tmp_set = calc_first(s[0], first_set, visited, prods);
        first.insert(tmp_set.begin(), tmp_set.end());
        if (tmp_set.find('$') != tmp_set.end()) {
            int i = 1;
            while (i < s.size()) {
                tmp_set = calc_first(s[i], first_set, visited, prods);
                first.insert(tmp_set.begin(), tmp_set.end());
                if (tmp_set.find('$') == tmp_set.end()) {
                    break;
                }
                i++;
            }
        }
    }
    return first;
}

//���㵥���ַ���FIRST��
set<char> calc_first(const char s, map<char, set<char>> &first_set, set<char> &visited, vector<Production> &prods) {
    set<char> first;
    //���s�Ѿ������ʹ���˵���÷��ŵ�FIRST�����Ѿ��������ֱ�ӷ���
    if (visited.find(s) != visited.end()) {
        return first_set[s];
    }
    //���s���ս����ֱ�ӽ������FIRST������
    if (!isupper(s) || s == '$') {
        first.insert(s);
    }
    //���򣬱�������ʽ���ϣ��ҵ�������sΪ�󲿵Ĳ���ʽ�����������Ҳ����ŵ�FIRST����
    else {
        for (auto& p : prods) {
            if (p.l == s) {
                //����Ҳ���һ������Ϊs������Ҫ����FIRST�����г��˿մ��ķ��ż��뵱ǰ���ŵ�FIRST������
                if (!isupper(p.r[0]) || p.r[0] == '$') {
                    if (p.r[0] != '$') {
                        first.insert(p.r[0]);
                    }
                    else {
                        first.insert('$');
                    }
                }
                else {
                    // �ݹ����calc_first�����������Ҳ���һ�����ŵ�FIRST���������صļ��ϼ��������У�
                    // ������صļ����а����մ�������Ҫ���������Ҳ�������ŵ�FIRST���ϣ���������뵱ǰ
                    // ���ŵ�FIRST�����У�ֱ���������մ����߱������Ҳ����з���
                    auto tmp_set = calc_first(p.r[0], first_set, visited, prods);
                    first.insert(tmp_set.begin(), tmp_set.end());
                    if (tmp_set.find('$') != tmp_set.end()) {
                        int i = 1;
                        while (i < p.r.size()) {
                            tmp_set = calc_first(p.r[i], first_set, visited, prods);
                            first.insert(tmp_set.begin(), tmp_set.end());
                            if (tmp_set.find('$') == tmp_set.end()) {
                                break;
                            }
                            i++;
                        }
                    }
                }
            }
        }
    }
    visited.insert(s);
    first_set[s] = first;
    return first;
}

//��ȡ�����ķ����涨�ö����ķ���VN��VT��Ϊ�����ַ���������productions��
vector<Production> LR::readGrammar(const string& path) {
    ifstream file(path);
    vector<Production> productions;
    string line;
    //���ļ�
    if (!file){
        cout<<"�Ҳ����ؼ����ļ���"<<endl;
        return productions;
    }
    // ˼·����ȡһ�У��и����ߣ�����ұ���"|"�����и��ұߣ�
    // �������productions��
    getline(file,line);
    while(!line.empty()){
        //��ȡ��һ��
        line = trim(line);
        //����ע��
        if (line[0] == '/' && line[1] == '/'){
            line.clear();
            getline(file,line);
            if (line.empty()) break;
            continue;
        }
        size_t p = line.find("->");
        if (p == string::npos){
            cout << "������﷨����"<<endl;
            return {};
        }
        string h = trim(line.substr(0,p));
        line = trim(line.substr(p + 2));
        //�õ���������ʽ
        if (line.find('|') == string::npos){
            Production production;
//            vector<Symbol> sy;
            //�õ������h���ұ�line
                production.r += line;
            if (!isupper(h[0])){
                cout << "��������﷨����"<<endl;
                return {};
            }
            for (auto& sss:production.r) {
                if (isupper(sss)){
                    terminals.insert(sss);
                }else
                    nonTerminals.insert(sss);
            }
            production.l = h[0];
            terminals.insert(h[0]);
            productions.push_back(production);
        } else{
            stringstream ss(line);
            string s;
            while(getline(ss,s,'|')){
                //�õ�h����ߣ���trim(s)���ұߣ�
                Production production;
                production.r = trim(s);
                if (!isupper(h[0])){
                    cout << "��������﷨����"<<endl;
                    return {};
                }
                for (auto& sss:production.r) {
                    if (isupper(sss)){
                        terminals.insert(sss);
                    }else
                        nonTerminals.insert(sss);
                }
                production.l = h[0];
                terminals.insert(h[0]);
                productions.push_back(production);
            }
        }
        line.clear();
        getline(file,line);
    }
    //�Եõ����ķ���������
    augmentGrammar(productions);
    Productions = productions;
    return productions;
}

//���ķ��������㴦��
void LR::augmentGrammar(vector<Production> &grammar) {
//    vector<Production> productions;
    char _start = 'Z';
    Production production = {_start, {grammar[0].l}};
    grammar.insert(grammar.begin(), production);
    grammar[0].l = _start;
}

// ���ڹ���LR(1)���������
// ���췽�������Ȼ�ȡ��ʼ״̬������Ը�������������е����ַ���ת�Ƶ���״̬��
// �Ը���״̬�����հ����հ����Ϊ��״̬����������δ���ֱ��û����������õ����
set<ItemSet> LR::construct_LR1_itemSets() {
    // ��ʼ�ΪZ -> .S, $
    ItemSet initial_itemSet;
    initial_itemSet.items.insert(Item{Production{'Z', "S" }, 0, {'$' } });
    // ���
    set<ItemSet> itemSets;
    initial_itemSet.name = "I" + to_string(itemSets.size());
    itemSets.insert(closure(initial_itemSet));
    // �����������
    queue<ItemSet> itemSet_queue;
    // �����ʼ״̬
    itemSet_queue.push(initial_itemSet);
    // �������д�������I������ÿ�����е�ÿ���ķ����ţ�GOTO(I, X)�ǿ��Ҳ��ڱհ��У���GOTO����հ�
    while (!itemSet_queue.empty()) {
        //ȡ�������ÿ���ȡ�����������ţ�������״̬����״̬�����հ����õ�������������
        //ֱ��û�����������壬������ֹ
        ItemSet itemSet = itemSet_queue.front();
        itemSet_queue.pop();
        // ��ÿ���ķ����Ž�����չ
        map<char, ItemSet> next_itemSets;
        set<char> charSet;
        map<char,ItemSet> itemMap;
        //������е�ÿһ���ȡ���ĵ�һ���ַ�
        for (const auto& item : itemSet.items){
            auto item1 = item;
            //����㲻�����Ϊ�ƽ�/��Լ��Ŀ�������
            if (item.dot < item.rule.r.length()){
                charSet.insert(item.rule.r[item.dot]);
                item1.dot++;
                itemMap[item.rule.r[item.dot]].items.insert(item1);
            }else{
                //������󣬹�Լ������Action���ڶ���ItemSet����һ������ʽ����ʾʹ����������ʽ��Լ��
                for (const auto &l: item.lookahead){
                    ItemSet iss;
                    Item ii;
                    Production p = item.rule;
                    ii.rule = p;
                    iss.items.insert(ii);
                    Action[itemSet][l] = {iss, true};
                    //����Z -> S.��״̬Ϊacc
                    if (p.l == 'Z' && p.r == "S"){
                        iss.name = "ACC";
                        Action[itemSet][l] = {iss, false};
                    }
                }
            }
        }
        //�������п����������ÿ��������������Ѱ����ı��ʽ�����һ�����������հ����հ�����½�״̬
        for (auto& c:charSet) {
            ItemSet iss = itemMap[c];
            //����հ��󣬵õ���״̬����״̬��������ڣ���ô�������õ�״̬�������Ƿ�����������ӹ�ϵת�Ʊ�
            closure(iss);
            //���flag��ֵ����˵��û���ҵ����������ʱ�½�һ���
            int flag = 0;
            for (auto& i:itemSets) {
                //i == iss��˵���ҵ�����ȫһ�µ�itemSet������Ҫ����״̬
                if (i == iss){
                    flag = 1;
                    iss.name = i.name;
                    break;
                }
            }
            //û�ҵ�һ�µ�״̬,����
            if (!flag){
                iss.name = "I" + to_string(itemSets.size());
                itemSet_queue.push(iss);
                itemSets.insert(iss);
            }
            //��״̬ת�ƽ������ACTION/GOTO��
            //���ڴ�д��ĸ�����ս��������״̬ת�Ƽ���GOTO��
            if (isupper(c)){
                Goto[itemSet][c] = iss;
            }else{
                //���ڷ��ս����״̬����Action��Ϊ�ƽ�״̬
                Action[itemSet][c] = {iss, false};
            }
        }
    }
    is = itemSets;
    return itemSets;
}


//�������հ�
//����ÿ������ʽ���������ַ����Ƿ��ս�������÷��ս���Ĳ���ʽ���룬���ڵ�һλ�������¼���Ĳ���ʽ
ItemSet LR::closure(ItemSet& items) {
//    ItemSet item_set;
    queue<Item> item_queue;
    //���������ItemSet����ÿ��Item���������
    for (auto& item : items.items) {
        item_queue.push(item);
    }
    //���ݶ���˳�����δ���
    while (!item_queue.empty()) {
        //ȡһ�������"Z -> .S", $, �� rule.l = '\'', rule.r = S, dot = 0, lookahead = {$}
        auto item = item_queue.front();
        item_queue.pop();
        //��������Ҷˣ�����Ҫ��������
        if (item.dot == item.rule.r.size()) {
            continue;
        }
        //ȡ����һ���ַ�
        char next_symbol = item.rule.r[item.dot];
        //������ս��������Сд��ĸ��������Ҫ��������
        if (!isupper(next_symbol)) {
            continue;
        }
        //��Ų������Ҷˣ�������ս�������Ե���Ƿ��ս������д��ĸ������ʱ��Ҫ����
        //�����ս�����Ƶ�������ڵ�һλ��չ�����Ǵ���һλ��ʼ����β��FIRST��
        //��������ʽ���ϣ��ҵ���Ҫ�������
        for (auto& prod : Productions) {
            //�ҵ���Ӧ��ʽ��
            if (prod.l == next_symbol) {
                //�����һλ��ʼ����β��FIRST���ϣ���Ϊչ����
                string s = item.rule.r.substr(item.dot + 1);
                set<char> lookahead;
                for (auto & l:item.lookahead) {
                    string ss = s + l;
                    set<char> c = calc_first_s(ss, first_set, Productions);
                    addAllElement(lookahead,c);
                }
                //��������
                Item new_item{prod, 0, lookahead};
                //�Ҳ��������뼯��
                if (items.items.find(new_item) == items.items.end()) {
                    items.items.insert(new_item);
                    item_queue.push(new_item);
                }
            }
        }
    }
    return items;
}

//��������״̬ת��
void LR::printItemSet() {
    cout<<"ItemSet:"<<endl;
    for (const auto& i:is) {
        cout<<i.name<<": "<<endl;
        for (const auto& item : i.items){
            auto ii = item;
            ii.rule.r.insert(item.dot,".");
            cout<<item.rule.l<<" -> "<<ii.rule.r<<", lookahead: { ";
            for(const auto& l:item.lookahead){
                cout<<l<<" ";
            }
            cout<<"}"<<endl;
        }
        nonTerminals.insert('$');
        //����ACTION��GOTO������������״̬ת��
        for (const auto &c: nonTerminals){
            auto a = Action[i][c];
            if (!a.first.name.empty()){
                cout<<"--"<< c << "-->" << a.first.name<<"  ";
            } else {
                Production pp = a.first.items.begin()->rule;
                if (pp.l != '\0')
                    cout<<"--"<< c << "--> { " << pp.l << " -> "<<pp.r<<" }  ";
            }
        }
        for (const auto &c: terminals){
            auto g = Goto[i][c];
            if (!g.name.empty()){
                cout<<"--"<< c << "-->" << g.name<<"  ";
            }
        }
        cout<<endl;
    }
    cout<<endl;
}

void LR::printProduction() {
    cout<<"Productions:"<<endl;
    for (auto const& p:Productions) {
        cout<<p.l<<" -> "<<p.r<<endl;
    }
}

//�﷨����
//����Token��·��path����ȡToken���Ը�Token�����﷨������
//���ͨ�����룬���"YES"���������"NO"
void LR::parse(const string& path) {
    //��¼�����������������������
    int n = 0;
    //��ȡToken��Token����, ����Token��ת��Ϊ�ɽ��ܵķ��Ŵ�
    string ts = readToken(path);
    //����ջ
    stack<char> charStack;
    //״̬ջ
    stack<ItemSet> stateStack;
    stack<TreeNode*> treeStack;
//    TreeNode* node1 = new TreeNode();
//    node1->symbol = 'Z';
    treeStack.push(nullptr);
    //���ó�̬
    charStack.push('$');
    ItemSet iss = *is.begin();
    stateStack.push(iss);
    //������DFA�ĳ�̬��Ȼ�����ζ�ȡ�ַ�������ACTION��GOTO�����״̬ת�ƣ�����ACC˵�������ܣ�
    //����������ܸ��������ʾ
    //�ӵ�һ���ַ���ʼ����tokenString����������ַ����ΰ���ACTION��GOTO����
    //������ƽ�/��Լ״̬��������ջ��ջ����
    //����ǹ�Լ״̬�����Ȳ�Ҫ������ջ�����ȹ�Լ��Ŀ��ȷ��û�й�Լ������ƽ��ַ�
    for (int i = 0; i < ts.size(); ++i) {
        //ȡջ��Ԫ��
        auto& top = stateStack.top();
        ItemSet next;
        //��д��ĸ�����ս��������GOTO�������ACTION��
        if (isupper(ts[i])){
            next = Goto[top][ts[i]];
        } else {
            auto p = Action[top][ts[i]];
            //boolΪ�棬��ʱ��Ҫ��Լ
            //��Լ���̣�ȡ��Լ����������������ͬ���Ӧ״̬���γ�ջ��Ȼ�󽫹�Լʽ�������ջ��
            //ȡ״̬ջջ������ѯ����ջԪ�ص�GOTO�����ҵ���״̬����״̬ջ�����Ų鿴��һ��״̬
            while (p.second){
                //ȡ��Լʽ
                Production pp = p.first.items.begin()->rule;
                vector<TreeNode*> children(pp.r.size());
                //���������Լʽ�ұߣ����������γ�ջ
                for (int j = pp.r.size() - 1; j >= 0; --j) {
                    stateStack.pop();
                    charStack.pop();
                    auto a = treeStack.top();
                    children[j] = a;
                    treeStack.pop();
                }
                TreeNode * node = new TreeNode();
                node->symbol = pp.l;
                node->children = children;
                treeStack.push(node);
                //�����ջ
                charStack.push(pp.l);
                //��GOTO��
                auto ppp = Goto[stateStack.top()][pp.l];
                //״̬��ջ
                stateStack.push(ppp);
                p = Action[stateStack.top()][ts[i]];
                cout<<"�� "<<++n<<" �δ���"<<endl;
                cout<<"��ǰ��ȡ�ַ���"<<ts[i]<<endl;
                cout<<"charStack: ";
                auto cc = charStack;
                reverseStack(cc);
                for (auto temp = cc; !temp.empty(); temp.pop()) {
                    cout << temp.top() << " ";
                }
                cout<<endl<<"stateStack:";
                auto ss = stateStack;
                reverseStack(ss);
                for (auto temp = ss; !temp.empty(); temp.pop()) {
                    cout << temp.top().name << " ";
                }
                cout<<endl<<"������ͨ������ʽ��"<<pp.l<<" -> "<<pp.r<<"����Լ"<<endl<<endl;
            }
            //��Լ�����꣬�ƽ�
            next = Action[stateStack.top()][ts[i]].first;
        }
        //������ƿգ�˵����������һ״̬����ʱ������ѣ���ֹ����
        if (next.name.empty()){
            //ȡts[i]����Token��������һ�����
            auto token = tokenLine[i-1];
            cout << "���������ַ����͡�"<<dic2[ts[i]]<<"�����Ҳ�����һ״̬��������"<<token.line<<endl;
            stack<string> tokenStack;
            //�����һ�У��������λ��
            for (int j = i-1; j > 0 ; --j) {
                if (tokenLine[j].line == token.line){
                    tokenStack.push(tokenLine[j].value);
                } else{
                    break;
                }
            }
            string a;
            do {
                a += tokenStack.top();
                a += " ";
                tokenStack.pop();
            } while (!tokenStack.empty());
            cout<<a<<endl;
            for (int j = 1; j < a.length(); ++j) {
                cout<<" ";
            }
            cout<<"^"<<endl;
            //�����һ���ַ���Ӧ��չ����
            //����ACTION/GOTO����������ݵ��ַ�
            cout << "���������룺";
            for (const auto &c: nonTerminals){
                auto _a =Action[stateStack.top()][c];
                if (!_a.first.name.empty()){
                    cout<< dic2[c] <<"  ";
                } else {
                    if (_a.first.items.begin()->rule.l != '\0')
                        cout<< dic2[c] <<"  ";
                }
            }
            cout<<endl;
            return;
        }
        //�ƽ�
        TreeNode* node = new TreeNode();
        node->symbol = ts[i];
        treeStack.push(node);
        //��ջ
        charStack.push(ts[i]);
        stateStack.push(next);
        cout<<"�� "<<++n<<" �δ���"<<endl;
        cout<<"��ǰ��ȡ�ַ���"<<ts[i]<<endl;
        cout<<"charStack: ";
        auto cc = charStack;
        reverseStack(cc);
        for (auto temp = cc; !temp.empty(); temp.pop()) {
            cout << temp.top() << " ";
        }
        cout<<endl<<"stateStack:";
        auto ss = stateStack;
        reverseStack(ss);
        for (auto temp = ss; !temp.empty(); temp.pop()) {
            cout << temp.top().name << " ";
        }
        cout<<endl<<"�������ƽ�"<<endl<<endl;
    }
    cout<<"����״̬��";
    if (stateStack.top().name == "ACC"){
        cout<<"YES"<<endl;
        treeStack.pop();
        printTree(treeStack.top(),0);
    }else
        cout<<"NO"<<endl;
}

string LR::readToken(const string& path) {
    readDic(MATCH_PATH,MATCH2_PATH);
    //���ļ�����Token��ȡ��Token���У�Ȼ���Token���з���
    string line;
    ifstream file(path);
    if (!file){
        cout << "�Ҳ���Դ�����ļ���" << endl;
        return "";
    }
    string s;
    //��file�л�ȡһ�У�������line�У����д�������Token��
    while(getline(file,line)){
        istringstream iss(line);
        string l, type, value;

        //���ݿո��з֣����εõ��кš����͡�ֵ
        iss >> l >> type >> value;
        Token token;
        token.line = stoi(l);
        token.value = value;
        if (type == "KEYWORD"){
            token.type = KEYWORD;
            if (!dic[token.value]){
                dic[token.value] = 'k';
            }
        }else if (type == "IDENTIFIER"){
            token.type = IDENTIFIER;
            dic[token.value] = 't';
        } else if (type == "CONSTANT"){
            token.type = CONSTANT;
            dic[token.value] = 'd';
        } else if (type == "DELIMITER"){
            if (token.value == "\\0" || token.value == "\\t" || token.value == "\\n"){
                //�����ո�
                continue;
            }
            token.type = DELIMITER;
            dic[token.value] = token.value[0];
            dic2[token.value[0]] = token.value;
        } else if (type == "OPERATOR"){
            token.type = OPERATOR;
            if (!dic[token.value]){
                dic[token.value] = token.value[0];
                dic2[token.value[0]] = token.value;
            }
        }
        tokenLine[s.size()] = token;
        s += dic[token.value];
        tokens.push_back(token);
        if (line.empty()){
            break;
        }
    }
    file.close();
    s += '$';
    return s;
}

//��ȡ�ؼ��ֶ�Ӧ���ֵ�
void LR::readDic(const string& path1, const string& path2) {
    //���ļ������ؼ��ֵĶ�Ӧ���룬�����������ֵ���
    string line;
    ifstream file(path1);
    if (!file){
        cout << "�Ҳ���MATCH�ļ���" << endl;
        return ;
    }
    getline(file,line);
    //��file�л�ȡһ�У�������line�У����д�������dic
    while(!line.empty()){
        //����ע��
        line = trim(line);
        if (line[0] == '/' && line[1] == '/'){
            line.clear();
            getline(file,line);
            if (line.empty()) break;
            continue;
        }
        istringstream iss(line);
        string key;
        char value;
        //���ݿո��з֣��õ�key��value
        iss >> key >> value;
        dic[key] = value;
        getline(file,line);
    }
    file.close();
    string line2;
    ifstream file2(path2);
    if (!file2){
        cout << "�Ҳ���MATCH2�ļ���" << endl;
        return ;
    }
    getline(file2,line2);
    //��file�л�ȡһ�У�������line�У����д�������dic
    while(!line2.empty()){
        //����ע��
        line2 = trim(line2);
        if (line2[0] == '/' && line2[1] == '/'){
            line2.clear();
            getline(file2,line2);
            if (line2.empty()) break;
            continue;
        }
        istringstream iss2(line2);
        char key2;
        string value2;
        //���ݿո��з֣��õ�key��value
        iss2 >> key2 >> value2;
        dic2[key2] = value2;
        getline(file2,line2);
    }
    file2.close();
}
