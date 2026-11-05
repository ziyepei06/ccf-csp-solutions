#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    if (!(cin >> n)) return 0;
    // 读者编号是不超过 n 的正整数，直接开计数桶
    vector<int> cnt(n + 2, 0);
    for (int i = 0; i < n; i++) {
        int x;
        cin >> x;
        cnt[x]++;
        if (i) cout << ' ';
        cout << cnt[x];
    }
    cout << '\n';
    return 0;
}
