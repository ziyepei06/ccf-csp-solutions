#include <bits/stdc++.h>
using namespace std;

// 共有 63 种字符：A-Z, a-z, 0-9, 空格
static int idOf(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';        // 0..25
    if (c >= 'a' && c <= 'z') return 26 + (c - 'a'); // 26..51
    if (c >= '0' && c <= '9') return 52 + (c - '0'); // 52..61
    if (c == ' ') return 62;
    return -1;                                       // 不会出现
}
static char chOf(int i) {
    if (i < 26) return char('A' + i);
    if (i < 52) return char('a' + (i - 26));
    if (i < 62) return char('0' + (i - 52));
    return ' ';
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string line;
    // 第一行：#s#
    getline(cin, line);
    while (!line.empty() && line.back() == '\r') line.pop_back();
    string s;
    if (line.size() >= 2) s = line.substr(1, line.size() - 2);

    // 第二行：n
    int n = 0;
    getline(cin, line);
    { stringstream ss(line); ss >> n; }

    // f 是 63 个字符上的置换（单射 + 未定义映射到自身）
    int perm[63];
    for (int i = 0; i < 63; ++i) perm[i] = i;
    for (int t = 0; t < n; ++t) {
        getline(cin, line);
        while (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.size() < 3) { --t; continue; }     // 容错：跳过空行
        int a = idOf(line[1]), b = idOf(line[2]);
        if (a >= 0 && b >= 0) perm[a] = b;
    }

    // 对每个字符求其所在循环节，f^k(ch) = 循环节[(k mod 长度)]
    vector<vector<int>> cyc(63);
    for (int i = 0; i < 63; ++i) {
        int cur = i;
        for (int step = 0; step <= 63; ++step) {   // 至多 63 步，必然回到起点
            cyc[i].push_back(cur);
            cur = perm[cur];
            if (cur == i) break;
        }
    }

    // 第 n+3 行：m
    int m = 0;
    getline(cin, line);
    { stringstream ss(line); ss >> m; }

    // 下一行：m 个正整数 k
    vector<long long> ks;
    ks.reserve(m);
    while ((int)ks.size() < m) {
        if (!getline(cin, line)) break;
        stringstream ss(line);
        long long k;
        while (ss >> k) {
            ks.push_back(k);
            if ((int)ks.size() == m) break;
        }
    }

    string out;
    out.reserve(s.size() * (size_t)max(1, (int)ks.size()) * 2 + 16);
    for (long long k : ks) {
        out.push_back('#');
        for (char c : s) {
            int i = idOf(c);
            if (i < 0) { out.push_back(c); continue; }
            const vector<int>& v = cyc[i];
            size_t L = v.size();
            out.push_back(chOf(v[(size_t)(k % (long long)L)]));
        }
        out.push_back('#');
        out.push_back('\n');
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
