#include <bits/stdc++.h>
using namespace std;

// 关键推导：设 g(k) = sum_{j=1..k} (a_j - b_j)，g(0)=0。
// 正常巡查所需初始能量 w = a0 + max_{0<=k<=n} g(k)。
// 若第 i 个区域发生意外（b_i 变为 0），则所有 k >= i 的 g(k) 都增加 b_i，
// 其余不变，于是
//   w(i) = a0 + max( max_{0<=k<i} g(k),  b_i + max_{i<=k<=n} g(k) )。
// 用前缀最大值与后缀最大值即可 O(n) 得到全部 w(i)。

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    vector<long long> a(n + 1), b(n + 1);
    for (int i = 0; i <= n; i++) scanf("%lld", &a[i]);
    for (int i = 1; i <= n; i++) scanf("%lld", &b[i]);

    // g[k]: k = 0..n
    vector<long long> g(n + 1);
    g[0] = 0;
    for (int k = 1; k <= n; k++) g[k] = g[k - 1] + a[k] - b[k];

    // pre[k] = max_{0<=j<=k} g[j]
    vector<long long> pre(n + 1);
    pre[0] = g[0];
    for (int k = 1; k <= n; k++) pre[k] = max(pre[k - 1], g[k]);

    // suf[k] = max_{k<=j<=n} g[j]
    vector<long long> suf(n + 1);
    suf[n] = g[n];
    for (int k = n - 1; k >= 0; k--) suf[k] = max(suf[k + 1], g[k]);

    for (int i = 1; i <= n; i++) {
        long long best = pre[i - 1];          // max_{0<=k<i} g(k)，至少含 g(0)=0
        best = max(best, b[i] + suf[i]);      // max_{i<=k<=n} (g(k)+b_i)
        long long w = a[0] + best;
        if (i > 1) putchar(' ');
        printf("%lld", w);
    }
    putchar('\n');
    return 0;
}
