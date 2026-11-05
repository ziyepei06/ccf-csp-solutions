#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string s;
    if (!(cin >> s)) return 0;
    int sensitive;
    cin >> sensitive;
    int n;
    cin >> n;

    // 大小写不敏感时统一转小写，再直接子串匹配
    if (sensitive == 0) {
        for (char &c : s)
            if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a');
    }

    for (int i = 0; i < n; ++i) {
        string line;
        cin >> line;
        string hay = line;
        if (sensitive == 0) {
            for (char &c : hay)
                if (c >= 'A' && c <= 'Z') c = char(c - 'A' + 'a');
        }
        if (hay.find(s) != string::npos) cout << line << '\n';
    }
    return 0;
}
