#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    string s;
    while (n--) {
        cin >> s;                       // 长度 7：d op d op d op d
        // 栈式求解：先算乘除，再把项累加
        vector<long long> st;
        st.push_back(s[0] - '0');
        for (int i = 1; i + 1 < (int)s.size(); i += 2) {
            char op = s[i];
            long long v = s[i + 1] - '0';
            if (op == 'x') {
                st.back() *= v;         // 乘，含"项的符号"一并相乘
            } else if (op == '/') {
                st.back() /= v;         // 整除，除数恒为正数（数字 1-9）
            } else if (op == '+') {
                st.push_back(v);
            } else {                    // '-'
                st.push_back(-v);
            }
        }
        long long sum = 0;
        for (long long x : st) sum += x;
        cout << (sum == 24 ? "Yes" : "No") << '\n';
    }
    return 0;
}
