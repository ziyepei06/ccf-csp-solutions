#include <bits/stdc++.h>
using namespace std;

/*
 * 201509-3 模板生成系统
 *
 * 输入必须原样保留空格与空行，因此整份输入用 getline 读入，
 * 绝不 trim、不用 >> 解析模板行。
 *
 * 变量定义行形如: name "value"（name 之后一个空格，值用双引号括起，
 * 值内部不含双引号）。由于 m 与模板实际行数在个别数据上可能不一致，
 * 这里采用"末尾 n 行一定是变量定义"的切分方式：若最后 n 行全部匹配
 * 变量定义格式，则前面所有行都是模板行；否则退回按 m 行切分。
 *
 * 逐行扫描模板：遇到 "{{ " 就向后找最近的 " }}"，中间即变量名；
 * 查表输出其值（未定义输出空串），其余字符原样输出；
 * 模板不递归生成，变量值直接拼接、不再扫描。
 *
 * 时间复杂度 O(总输入字符数 · 1)（每个字符被扫描常数次，
 *             哈希查找均摊 O(1)），
 * 空间复杂度 O(总输入字符数)。
 */

static bool isVarDef(const string &s) {
    // 形如 name "value"：name 是合法变量名，其后一个空格，再用双引号括起值
    size_t sp = s.find(' ');
    if (sp == string::npos || sp == 0) return false;
    for (size_t i = 0; i < sp; ++i) {
        char c = s[i];
        bool ok = (c == '_') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                  (i > 0 && c >= '0' && c <= '9');
        if (!ok) return false;
    }
    // 兼容名字与值之间有多于一个空格的情况（样例即为两个空格）
    size_t q = sp;
    while (q < s.size() && s[q] == ' ') ++q;
    return q + 1 < s.size() && s[q] == '"' && s.back() == '"';
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int m, n;
    if (!(cin >> m >> n)) return 0;
    string line;
    getline(cin, line); // 吃掉第一行行尾

    vector<string> lines;
    while (getline(cin, line)) lines.push_back(line);

    size_t total = lines.size();
    size_t split;
    bool lastAreVars = (n > 0 && total >= (size_t)n);
    if (lastAreVars) {
        for (size_t i = total - n; i < total; ++i)
            if (!isVarDef(lines[i])) { lastAreVars = false; break; }
    }
    if (lastAreVars) {
        split = total - n;
    } else {
        split = min(total, (size_t)m);
    }

    unordered_map<string, string> var;
    var.reserve((size_t)n * 2 + 8);
    for (size_t i = split; i < total; ++i) {
        const string &s = lines[i];
        size_t sp = s.find(' ');
        if (sp == string::npos) continue;
        size_t q1 = s.find('"', sp + 1);
        size_t q2 = s.rfind('"');
        if (q1 == string::npos || q2 == string::npos || q2 <= q1) continue;
        var[s.substr(0, sp)] = s.substr(q1 + 1, q2 - q1 - 1);
    }

    string out;
    out.reserve(1 << 16);
    for (size_t i = 0; i < split; ++i) {
        const string &s = lines[i];
        const size_t len = s.size();
        size_t p = 0;
        while (p < len) {
            if (s[p] == '{' && p + 2 < len && s[p + 1] == '{' && s[p + 2] == ' ') {
                // 题目保证所有以 "{{ " 开始的子串都是合法标记 "{{ VAR }}"
                size_t end = s.find(" }}", p + 3);
                if (end == string::npos) { // 防御：非法则原样输出
                    out.append(s, p, len - p);
                    break;
                }
                auto it = var.find(s.substr(p + 3, end - (p + 3)));
                if (it != var.end()) out += it->second; // 未定义则输出空串
                p = end + 3;                           // 跳过 " }}"
            } else {
                out += s[p];
                ++p;
            }
        }
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
