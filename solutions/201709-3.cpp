#include <bits/stdc++.h>
using namespace std;

// 值：字符串 或 对象（键 -> 值）
struct Node {
    bool isStr = false;
    string s;
    map<string, Node> obj;
};

static string src;
static size_t pos = 0;

static void skipWS() {
    while (pos < src.size()) {
        char c = src[pos];
        if (c == ' ' || c == '\n' || c == '\t' || c == '\r') ++pos;
        else break;
    }
}

// 调用时 src[pos] == '"'，返回去掉转义后的字符串内容
static string parseString() {
    ++pos;  // 跳过开头的引号
    string r;
    while (pos < src.size() && src[pos] != '"') {
        if (src[pos] == '\\') {
            ++pos;
            if (pos < src.size()) r += src[pos++];
        } else {
            r += src[pos++];
        }
    }
    ++pos;  // 跳过结尾的引号
    return r;
}

static Node parseValue() {
    skipWS();
    Node n;
    if (pos < src.size() && src[pos] == '{') {
        n.isStr = false;
        ++pos;
        skipWS();
        if (pos < src.size() && src[pos] == '}') { ++pos; return n; }
        while (pos < src.size()) {
            skipWS();
            string k = parseString();
            skipWS();
            ++pos;  // ':'
            Node v = parseValue();
            n.obj.emplace(std::move(k), std::move(v));
            skipWS();
            if (pos < src.size() && src[pos] == ',') { ++pos; continue; }
            ++pos;  // '}'
            break;
        }
    } else {
        n.isStr = true;
        n.s = parseString();
    }
    return n;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;
    string line;
    getline(cin, line);  // 吃掉第一行剩余内容
    src.reserve((size_t)n * 84);
    for (int i = 0; i < n; ++i) {
        getline(cin, line);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        src += line;
        src += '\n';
    }

    pos = 0;
    Node root = parseValue();

    string q;
    for (int i = 0; i < m; ++i) {
        if (!getline(cin, q)) q.clear();
        if (!q.empty() && q.back() == '\r') q.pop_back();

        // 按 '.' 切分查询路径（键中不会出现 '.'，查询给出的是键的真实内容，无需反转义）
        const Node *cur = &root;
        bool ok = true;
        size_t start = 0;
        while (true) {
            size_t dot = q.find('.', start);
            string key = q.substr(start, dot == string::npos ? string::npos : dot - start);
            if (cur->isStr) { ok = false; break; }
            auto it = cur->obj.find(key);
            if (it == cur->obj.end()) { ok = false; break; }
            cur = &it->second;
            if (dot == string::npos) break;
            start = dot + 1;
        }

        if (!ok) {
            cout << "NOTEXIST\n";
        } else if (cur->isStr) {
            cout << "STRING " << cur->s << '\n';
        } else {
            cout << "OBJECT\n";
        }
    }
    return 0;
}
