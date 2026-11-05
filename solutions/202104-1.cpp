#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m, L;
    if (!(cin >> n >> m >> L)) return 0;

    // n,m <= 500, L <= 256; n*m <= 250000 fits in int
    vector<int> h(L, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int x;
            cin >> x;
            ++h[x];  // 0 <= x < L
        }
    }

    for (int x = 0; x < L; ++x) {
        if (x) cout << ' ';
        cout << h[x];
    }
    cout << '\n';
    return 0;
}
