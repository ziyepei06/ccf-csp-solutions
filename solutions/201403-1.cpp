#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    if (!(cin >> n)) return 0;
    unordered_set<int> s;
    vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        cin >> a[i];
        s.insert(a[i]);
    }
    int cnt = 0;
    for (int i = 0; i < n; ++i) {
        if (a[i] < 0 && s.count(-a[i])) ++cnt;
    }
    // a[i] > 0 的一侧不重复计数：每对只在其负数出现时统计一次
    cout << cnt << '\n';
    return 0;
}
