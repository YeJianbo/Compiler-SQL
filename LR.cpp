//
// Created by YeJianbo on 2023/3/16.
//

#include "LR.h"

//bool Symbol::operator<(const Symbol &o) const {
//    return value.compare(o.value);
//}
//
//bool Symbol::operator==(const Symbol &o) const {
//    return value == o.value && is_terminal == o.is_terminal;
//}
//
//bool Symbol::operator!=(const Symbol &o) const {
//    return !(*this == o);
//}

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
            //此处代码有问题！！！能跑，以后修
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


//计算字符串的first集
set<char> calc_first_s(string &s, map<char, set<char>> &first_set, vector<Production> &prods) {
    set<char> first;
    set<char> visited;
    //如果s的第一个符号是终结符(小写字母)，直接将其加入FIRST集合中
    if (!isupper(s[0]) || s[0] == '$') {
        if (s[0] != '$') {
            first.insert(s[0]);
        }
        else {
            first.insert('$');
        }
    }
    //否则，计算第一个符号的FIRST集合
    else {
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

//计算单个字符的FIRST集
set<char> calc_first(const char s, map<char, set<char>> &first_set, set<char> &visited, vector<Production> &prods) {
    set<char> first;
    //如果s已经被访问过，说明该符号的FIRST集合已经计算过，直接返回
    if (visited.find(s) != visited.end()) {
        return first_set[s];
    }
    //如果s是终结符，直接将其加入FIRST集合中
    if (!isupper(s) || s == '$') {
        first.insert(s);
    }
        //否则，遍历产生式集合，找到所有以s为左部的产生式，并计算其右部符号的FIRST集合
    else {
        for (auto& p : prods) {
            if (p.l == s) {
                //如果右部第一个符号为s，则需要将其FIRST集合中除了空串的符号加入当前符号的FIRST集合中
                if (!isupper(p.r[0]) || p.r[0] == '$') {
                    if (p.r[0] != '$') {
                        first.insert(p.r[0]);
                    }
                    else {
                        first.insert('$');
                    }
                }
                else {
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

//读取二型文法，规定该二型文法的VN与VT均为单个字符，保存在productions中
vector<Production> LR::readGrammar(string path) {
    ifstream file(path);
    vector<Production> productions;
    string line;
    //打开文件
    if (!file){
        cout<<"找不到关键字文件！"<<endl;
        return productions;
    }
    // 思路：获取一行，切割两边，如果右边有"|"，再切割右边，
    // 逐个放入productions中
    getline(file,line);
    while(!line.empty()){
        //获取到一行
        line = trim(line);
        //跳过注释
        if (line[0] == '/' && line[1] == '/'){
            line.clear();
            getline(file,line);
            if (line.empty()) break;
            continue;
        }
        size_t p = line.find("->");
        if (p == string::npos){
            cout << "输入的语法有误！"<<endl;
            return vector<Production>();
        }
        string h = trim(line.substr(0,p));
        line = trim(line.substr(p + 2));
        //得到单条产生式
        if (line.find('|') == string::npos){
            Production production;
//            vector<Symbol> sy;
            //得到了左边h，右边line
                production.r += line;
            if (!isupper(h[0])){
                cout << "您输入的语法有误！"<<endl;
                return vector<Production>();
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
                //得到h（左边）与trim(s)（右边）
                Production production;
                production.r = trim(s);
                if (!isupper(h[0])){
                    cout << "您输入的语法有误！"<<endl;
                    return vector<Production>();
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
    //对得到的文法进行增广
    augmentGrammar(productions);
    Productions = productions;
    return productions;
}

//对文法进行增广处理
void LR::augmentGrammar(vector<Production> &grammar) {
//    vector<Production> productions;
    char start = 'Z';
    Production production = {start, {grammar[0].l}};
    grammar.insert(grammar.begin(), production);
    grammar[0].l = start;
}

/*void LR::computeItemSets(vector<Production> &grammar) {
    //取已增广处理过的文法
    vector<Production> g = grammar;
    //初始化初态
    //项集
    ItemSet is;
    //项
    Item start;
    //项集族
    vector<ItemSet> sets;
    start.rule.l = '\'';
    start.rule.r = "S";
    start.dot = 0;
    start.lookahead = {'$'};
    is.items.insert(start);
//    sets.push_back(is);
    //得到闭包is
    closure(is);
    //遍历闭包中的每个项集I，对于每个项中的每个文法符号，GOTO(I, X)非空且不在闭包中，则将GOTO加入闭包
    for (auto &i:is.items) {

    }
//    int i = 0;
//
//    while (i < sets.size()){
//        for(const Item&item:sets[i].items){
//            //点后面是VN(小写字母)
//            if (item.dot_position < item.rule.r.size() && !item.rule.r[item.dot_position].is_terminal){
//                set<Symbol> lookahead;
//                if (item.dot_position + 1 < item.rule.r.size()){
//                    vector<Symbol> sub(item.rule.r.begin()+item.dot_position+1,item.rule.r.end());
//                    for (auto &s : sub) {
//                        auto s_first = calc_first(s, first_set);
//                        lookahead.insert(s_first.begin(), s_first.end());
//                        if (!s_first.count(Symbol{""})) {
//                            // 如果当前符号的FIRST集合不包含空串，则不需要计算后续符号的FIRST集合
//                            break;
//                        }
//                    }
//                } else{
//                    lookahead = {item.lookahead};
//                }
//                Item new_item;
//                new_item.rule.l = item.rule.l;
//                new_item.rule.r = item.rule.r;
//                swap(new_item.rule.r[item.dot_position],new_item.rule.r[item.dot_position+1]);
//                new_item.lookahead;
//            }
//        }
//    }
}*/


// 用于构造LR(1)分析的项集族
// 构造方法：首先获取初始状态的项集，对该项集依次输入所有点后的字符，转移到新状态，
// 对该新状态求项集族闭包，闭包结果为该状态所有项集，依次处理，直到没有项集新增，得到项集族
set<ItemSet> LR::construct_LR1_itemsets() {
    // 初始项集为Z -> .S, $
    ItemSet initial_itemset;
    initial_itemset.items.insert(Item{ Production{ 'Z', "S" }, 0, { '$' } });
    // 项集族
    set<ItemSet> itemsets;
    initial_itemset.name = "I" + to_string(itemsets.size());
    itemsets.insert(closure(initial_itemset));
    // 待处理项集队列
    queue<ItemSet> itemset_queue;
    // 处理初始状态
    itemset_queue.push(initial_itemset);
    // 遍历所有待处理的项集I，对于每个项中的每个文法符号，GOTO(I, X)非空且不在闭包中，则将GOTO加入闭包
    while (!itemset_queue.empty()) {
        //取项集，遍历每个项，取项点后的输入符号，产生新状态，新状态求项集族闭包，得到项集，加入项集族中
        //直到没有新项集加入项集族，程序终止
        ItemSet itemset = itemset_queue.front();
        itemset_queue.pop();
        // 对每个文法符号进行扩展
        map<char, ItemSet> next_itemsets;
        set<char> charSet;
        map<char,ItemSet> itemMap;
        //遍历项集中的每一个项，取点后的第一个字符
        for (const auto& item : itemset.items){
            charSet.insert(item.rule.r[item.dot]);
            auto item1 = item;
            //如果点不在最后，为移进/待约项目，点后移
            if (item.dot < item.rule.r.length()){
                item1.dot++;
                itemMap[item.rule.r[item.dot]].items.insert(item1);
            }else{
                //点在最后，归约

            }

        }
        //遍历所有可输入项，对于每个可输入项，在项集中寻找项的表达式，组成一个项集，将其求闭包，闭包结果新建状态
        for (auto& c:charSet) {
            cout<<c<<"|";
            ItemSet iss = itemMap[c];
            //求完闭包后，得到新状态，新状态如果不存在，那么新增
            closure(iss);
            int flag = 0;
            //找不到该状态，需要新增状态
            for (auto& i:itemsets) {
                //i == iss，说明找到了完全一致的itemset，不新增状态
                if (i == iss){
                    flag = 1;
                    break;
                }
            }
            //没找到一致的状态,新增
            if (!flag){
                iss.name = "I" + to_string(itemsets.size());
                itemset_queue.push(iss);
                itemsets.insert(iss);
            }
        }
//        for (const auto& item : itemset.items) {
//            //如果点不在最后，进行操作，否则跳过该项
//            if (item.dot < item.rule.r.length()) {
//                //取得需要输入的字符
//                char next_symbol = item.rule.r[item.dot];
//                //计算展望符
//                set<char> lookahead;
//                //下一状态不是归约态
//                if (item.dot < item.rule.r.length() - 1) {
//                    // 不是最后一个符号，取后面一个符号的FIRST集合
//                    string s = item.rule.r.substr(item.dot+1);
//                    lookahead = calc_first_s(s, first_set, Productions);
//                } else {
//                    // 是最后一个符号，直接取当前项的展望符
//                    lookahead = item.lookahead;
//                }
//
//                // 计算下一个项集
//                Item next_item = { item.rule, item.dot + 1, lookahead };
//                ItemSet& next_itemset = next_itemsets[next_symbol];
//                next_itemset.items.insert(next_item);
//            }
//        }

        // 将计算得到的新项集加入项集族中，并将其加入待处理队列中
//        for (auto& [symbol, next_itemset] : next_itemsets) {
//            ItemSet closure_itemset = closure(next_itemset);
//            if (itemsets.count(closure_itemset) == 0) {
//                itemsets.insert(closure_itemset);
//                itemset_queue.push(closure_itemset);
//            }
//        }
    }
    is = itemsets;
    return itemsets;
}


//计算项集族闭包
//遍历每个产生式，如果点后字符串是非终结符，将该非终结符的产生式加入，点在第一位，分析新加入的产生式
ItemSet LR::closure(ItemSet& items) {
//    ItemSet item_set;
    queue<Item> item_queue;
    //遍历输入的ItemSet，把每个Item放入队列中
    for (auto& item : items.items) {
        item_queue.push(item);
    }
    //根据队列顺序，依次处理
    while (!item_queue.empty()) {
        //取一个项，比如"Z -> .S", $, 有 rule.l = '\'', rule.r = S, dot = 0, lookahead = {$}
        auto item = item_queue.front();
        item_queue.pop();
        //点号在最右端，不需要处理，跳过
        if (item.dot == item.rule.r.size()) {
            continue;
        }
        //取点后第一个字符
        char next_symbol = item.rule.r[item.dot];
        //点后是终结符（比如小写字母），不需要处理，跳过
        if (!isupper(next_symbol)) {
            continue;
        }
        //点号不在最右端，点后不是终结符，所以点后是非终结符（大写字母），此时需要处理
        //将非终结符的推导加入点在第一位，展望符是从这一位开始到结尾的FIRST集
        //遍历产生式集合，找到需要加入的项
        for (auto& prod : Productions) {
            //找到对应的式子
            if (prod.l == next_symbol) {
                //求从这一位开始到结尾的FIRST集合，作为展望符
                string s = item.rule.r.substr(item.dot + 1);
                set<char> lookahead;
                for (auto & l:item.lookahead) {
                    string ss = s + l;
                    set<char> c = calc_first_s(ss, first_set, Productions);
                    addAllElement(lookahead,c);
                }
                //加入项中
                Item new_item{prod, 0, lookahead};
                //找不到，加入集合
                if (items.items.find(new_item) == items.items.end()) {
                    items.items.insert(new_item);
                    item_queue.push(new_item);
                }
            }
        }
    }
    return items;
}

void LR::printItemSet() {
    cout<<endl;
    for (const auto& i:is) {
        cout<<i.name<<": "<<endl;
        for (const auto& item : i.items){
            cout<<item.rule.l<<" -> "<<item.rule.r<<" dot: "<<item.dot<<" lookahead: {";
            for(const auto& l:item.lookahead){
                cout<<l<<" ";
            }
            cout<<"}"<<endl;
        }
    }
}

void LR::printProduction() {
    for (auto const& p:Productions) {
        cout<<p.l<<" -> "<<p.r<<endl;
    }
}

//int main(){
//    LR lr;
//    //读语法
//    lr.readGrammar(GRAMMAR_2NF_PATH);
//    lr.construct_LR1_itemsets();
//    lr.printItemSet();
//    return 0;
//}

//void LR::calculateFirstSet() {
//    // 对于每个终结符，FIRST集合就是它自己
//    for (char c : terminals) {
//        first_set[c].insert(c);
//    }
//    // 对于每个非终结符，将FIRST集合初始化为空
//    for (char c : nonTerminals) {
//        first_set[c] = {};
//    }
//    // 计算所有终结符和非终结符的FIRST集合
//    bool changed = true;
//    while (changed) {
//        changed = false;
//        for (const Production& p : Productions) {
//            char lhs = p.l;
//            const string& rhs = p.r;
//            for (int i = 0; i < rhs.size(); i++) {
//                char c = rhs[i];
//                if (terminals.find(c) != terminals.end()) {
//                    if (first_set[lhs].find(c) == first_set[lhs].end()) {
//                        first_set[lhs].insert(c);
//                        changed = true;
//                    }
//                    break;
//                } else {
//                    bool flag = false;
//                    for (char f : first_set[c]) {
//                        if (f != '$' && first_set[lhs].find(f) == first_set[lhs].end()) {
//                            first_set[lhs].insert(f);
//                            changed = true;
//                        }
//                        if (f == '$') {
//                            flag = true;
//                        }
//                    }
//                    if (!flag) {
//                        break;
//                    }
//                }
//            }
//            if (i == rhs.size() && first_set[lhs].find('@') == first_set[lhs].end()) {
//                first_set[lhs].insert('@');
//                changed = true;
//            }
//        }
//    }
//}





