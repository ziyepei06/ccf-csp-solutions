#include <bits/stdc++.h>
using namespace std;

int main() {
    int n, L, r, t;
    if (scanf("%d %d %d %d", &n, &L, &r, &t) != 4) return 0;

    // 二维前缀和：pre[i][j] = 前 i 行、前 j 列的元素和（1-indexed）
    vector<vector<long long>> pre(n + 1, vector<long long>(n + 1, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            long long x;
            scanf("%lld", &x);
            pre[i + 1][j + 1] = pre[i][j + 1] + pre[i + 1][j] - pre[i][j] + x;
        }
    }

    long long ans = 0;
    for (int i = 0; i < n; ++i) {
        // 邻域在行方向的裁剪区间 [r1, r2]
        int r1 = max(0, i - r), r2 = min(n - 1, i + r);
        for (int j = 0; j < n; ++j) {
            int c1 = max(0, j - r), c2 = min(n - 1, j + r);
            // 前缀和下标为 1-indexed，区间 [r1,r2]×[c1,c2] 平移一位
            long long sum = pre[r2 + 1][c2 + 1] - pre[r1][c2 + 1] - pre[r2 + 1][c1] + pre[r1][c1];
            long long cnt = (long long)(r2 - r1 + 1) * (c2 - c1 + 1);
            // 平均值 <= t  <=>  sum <= t * cnt，避免浮点误差
            if (sum <= (long long)t * cnt) ++ans;
        }
    }

    printf("%lld\n", ans);
    return 0;
}
