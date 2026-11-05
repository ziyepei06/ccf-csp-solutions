#include <bits/stdc++.h>
using namespace std;

/*
 * 202406-4 货物调度
 *
 * 对单个仓库 j：若调用 k 件货物，花费固定为 bj + k*cj，因此同仓库内一定优先选
 * 价值最大的那 k 件。把各仓库货物按价值降序排序求前缀和 pre[k]。
 *
 * 于是问题变成分组背包：第 j 组有 cnt_j+1 个选项（k = 0..cnt_j），
 * 代价 cost = (k==0 ? 0 : bj + k*cj)，收益 gain = (k==0 ? 0 : pre[k] - cost)，
 * 要求总收益 >= v，最小化总代价。
 *
 * 总代价上界：每个被调用的仓库至多贡献 20 的基本费用，计件费用总和至多 20*m，
 * 故总代价 <= 20*n + 20*m <= 40000。用 dp[c] = 总代价恰为 c 时的最大净收益，
 * 最后取最小的 c 使 dp[c] >= v。
 */

int main() {
    int n, m;
    long long v;
    if (scanf("%d %d %lld", &n, &m, &v) != 3) return 0;

    vector<int> b(n), c(n);
    for (int j = 0; j < n; ++j) {
        if (scanf("%d %d", &b[j], &c[j]) != 2) return 0;
    }

    vector<vector<int>> goods(n);
    for (int i = 0; i < m; ++i) {
        int a, t;
        if (scanf("%d %d", &a, &t) != 2) return 0;
        if (t >= 0 && t < n) goods[t].push_back(a);
    }

    const int LIM = 20 * n + 20 * m + 1;      // 总代价上界
    const int NEG = INT_MIN / 2;

    vector<int> dp(LIM + 1, NEG), ndp(LIM + 1, NEG);
    dp[0] = 0;
    int curMax = 0;                            // 当前可达的最大代价

    for (int j = 0; j < n; ++j) {
        if (goods[j].empty()) continue;

        vector<int>& g = goods[j];
        sort(g.begin(), g.end(), greater<int>());
        int cnt = (int)g.size();
        vector<int> pre(cnt + 1, 0);
        for (int k = 1; k <= cnt; ++k) pre[k] = pre[k - 1] + g[k - 1];

        const int bj = b[j], cj = c[j];

        // 清空 ndp，并保留 k = 0（不调用该仓库）的转移
        fill(ndp.begin(), ndp.end(), NEG);
        for (int cc = 0; cc <= curMax; ++cc) ndp[cc] = dp[cc];

        int newMax = curMax;
        for (int k = 1; k <= cnt; ++k) {
            int addCost = bj + k * cj;
            if (addCost > LIM) break;
            int gain = pre[k] - addCost;
            int hi = min(curMax, LIM - addCost);
            for (int cc = 0; cc <= hi; ++cc) {
                int d = dp[cc];
                if (d == NEG) continue;
                int nc = cc + addCost;
                int val = d + gain;
                if (val > ndp[nc]) ndp[nc] = val;
            }
            newMax = max(newMax, min(LIM, curMax + addCost));
        }

        curMax = newMax;
        dp.swap(ndp);
    }

    for (int cost = 0; cost <= curMax; ++cost) {
        if (dp[cost] >= v) {
            printf("%d\n", cost);
            return 0;
        }
    }
    return 0;
}
