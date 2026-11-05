#include <cstdio>

// 202203-2 出行计划
// 第 i 项计划：t_i 时刻进入场所，需要 c_i 单位内的核酸证明。
// 若在 q 时刻做检测，q+k 时刻出结果，可在 [q+k, q+k+c_i-1] 内入场，
// 故满足条件当且仅当 q+k <= t_i <= q+k+c_i-1，即
//     t_i - c_i + 1 - k <= q <= t_i - k。
// 每个计划对应 q 轴上一段区间，用差分数组统计每个 q 被多少区间覆盖，
// 查询 O(1) 回答。

int main() {
    int n, m, k;
    if (std::scanf("%d %d %d", &n, &m, &k) != 3) return 0;

    const int MAXQ = 200000;
    static int diff[MAXQ + 3];  // diff 需访问 hi+1，故留出余量

    for (int i = 0; i < n; ++i) {
        int t, c;
        std::scanf("%d %d", &t, &c);
        long long lo = (long long)t - c + 1 - k;  // 可用的最早检测时刻
        long long hi = (long long)t - k;          // 可用的最晚检测时刻
        if (hi < 1 || lo > MAXQ) continue;        // 与 q∈[1,MAXQ] 无交集
        if (lo < 1) lo = 1;
        if (hi > MAXQ) hi = MAXQ;
        ++diff[lo];
        --diff[hi + 1];
    }

    for (int i = 1; i <= MAXQ; ++i) diff[i] += diff[i - 1];

    for (int i = 0; i < m; ++i) {
        int q;
        std::scanf("%d", &q);
        std::printf("%d\n", diff[q]);
    }
    return 0;
}
