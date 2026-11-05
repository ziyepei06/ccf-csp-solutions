#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string s;
    if (!getline(cin, s)) return 0;
    // 去掉首尾空白（含可能的 \r）
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ' || s.back() == '\n')) s.pop_back();
    size_t b = 0;
    while (b < s.size() && (s[b] == ' ' || s[b] == '\t')) ++b;
    s = s.substr(b);
    if (s.empty()) return 0;

    // 前 9 个数字依次乘 1..9 求和
    int sum = 0, weight = 1;
    for (size_t i = 0; i + 1 < s.size(); ++i) {
        if (s[i] == '-') continue;
        sum += (s[i] - '0') * weight;
        ++weight;
    }
    int r = sum % 11;
    char code = (r == 10) ? 'X' : char('0' + r);

    if (s.back() == code) {
        cout << "Right\n";
    } else {
        string t = s;
        t.back() = code;
        cout << t << "\n";
    }
    return 0;
}
