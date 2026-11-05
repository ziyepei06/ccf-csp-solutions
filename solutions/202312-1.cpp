#include <bits/stdc++.h>
using namespace std;

int main() {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 0;
    vector<vector<int>> a(n, vector<int>(m));
    for (int i = 0; i < n; ++i)
        for (int d = 0; d < m; ++d) scanf("%d", &a[i][d]);

    for (int i = 0; i < n; ++i) {
        int ans = 0;
        for (int j = 0; j < n; ++j) {          // 从小编号开始扫，第一个满足的就是答案
            bool ok = true;
            for (int d = 0; d < m && ok; ++d)
                if (a[j][d] <= a[i][d]) ok = false;
            if (ok) { ans = j + 1; break; }
        }
        printf("%d\n", ans);
    }
    return 0;
}
