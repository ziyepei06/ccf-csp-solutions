#include <bits/stdc++.h>
using namespace std;

static const int MOD = 1000000007;

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    vector<int> a(n);
    for (int i = 0; i < n; ++i) scanf("%d", &a[i]);

    int maxd = a[n - 1] - a[0];
    if (maxd <= 0) maxd = 1;

    // divs[m] = 所有正因子 d（1 <= d <= m）
    vector<vector<int>> divs(maxd + 1);
    for (int d = 1; d <= maxd; ++d)
        for (int m = d; m <= maxd; m += d)
            divs[m].push_back(d);

    // hit[d] = 最小的 k > j 使得 (a[k]-a[j]) % d == 0（用 stamp 做懒清空）
    vector<int> stamp(maxd + 1, -1), hit(maxd + 1, 0);
    vector<long long> dp(n, 0);
    dp[0] = 1;

    for (int j = 0; j + 1 < n; ++j) {
        // 预处理：对每个公差 d，找第一个落在等差格点上的障碍物下标
        for (int k = j + 1; k < n; ++k) {
            int diff = a[k] - a[j];
            for (int d : divs[diff]) {
                if (stamp[d] != j) { stamp[d] = j; hit[d] = k; }
            }
        }
        if (dp[j] == 0) continue;
        // dp[i] += dp[j] * #{公差 d : d|(a[i]-a[j]), d < L, 内部无障碍物}
        for (int i = j + 1; i < n; ++i) {
            int L = a[i] - a[j];
            long long cnt = 0;
            for (int d : divs[L]) {
                if (d == L) continue;              // d == L 时区间内部没有树，不合法
                if (stamp[d] == j && hit[d] >= i) ++cnt; // 内部 (j,i) 无格点障碍物
            }
            if (cnt)
                dp[i] = (dp[i] + dp[j] % MOD * (cnt % MOD)) % MOD;
        }
    }

    printf("%lld\n", dp[n - 1] % MOD);
    return 0;
}
