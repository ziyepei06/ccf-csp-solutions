// 201703-5 引水入城
//
// 建模为最大流 = 最小割。
// 超级源点连第 1 行（容量 INF），第 n 行连超级汇点（容量 INF）。
// 纵向管道：向下容量 c，向上容量 INF（"放水"水量任意大）。
// 横向管道（仅存在于第 2..n-1 行）：左右两个方向各容量 c。
//
// 最小割里，向上的 INF 边迫使同一列割的位置单调：
// 每列存在阈值 h_t ∈ [1, n-1]，使第 1..h_t 行在 S 侧、第 h_t+1..n 行在 T 侧
// （第 1 行在 S 侧、第 n 行在 T 侧均被 INF 强制）。
//   纵向贡献  = V(h_t, t)
//   相邻两列 t、t+1 的横向贡献 = Σ_{s=min+1}^{max} H(s,t) = |P_t(h_t) − P_t(h_{t+1})|,
//   其中 P_t(h) = Σ_{s=2..h} H(s,t) 关于 h 单调不减。
// 所以按列做 DP：
//   dp'[v] = V(v, t+1) + min_u { dp[u] + |P_t(v) − P_t(u)| },
// 由 P_t 的单调性用前缀/后缀最小值把转移压到 O(n)，总计 O(n·m)。
#include <bits/stdc++.h>

typedef unsigned long long u64;
typedef long long ll;

int main() {
    int n, m;
    u64 A, B, Q, X0;
    if (scanf("%d %d %llu %llu %llu %llu", &n, &m, &A, &B, &Q, &X0) != 6) return 0;

    const ll vcnt = (ll)(n - 1) * m;                                  // 纵向管道数
    const ll hcnt = (n > 2) ? (ll)(n - 2) * (m - 1) : 0;              // 横向管道数
    const ll total = vcnt + hcnt;

    // Q == 1 时序列恒为 0，所有管道容量为 0。
    if (Q == 1) { printf("0\n"); return 0; }

    // 伪随机序列：X[0] 对应题面的 X_1，即由 X0 迭代一次得到。
    // vert(h,t)  (h∈[1,n-1], t∈[1,m])   = X[(h-1)*m + (t-1)]
    // horiz(h,t) (h∈[2,n-1], t∈[1,m-1]) = X[(n-1)*m + (h-2)*(m-1) + (t-1)]
    unsigned int* X = new unsigned int[(size_t)total];
    {
        const u64 mu = (u64)(((__uint128_t)1 << 64) / Q);  // Barrett: floor(2^64/Q)
        u64 x = X0;
        for (ll i = 0; i < total; ++i) {
            u64 y = A * x + B;                       // ≤ 1e9*(1e9-1)+1e9 < 2^60
            u64 q = (u64)(((__uint128_t)y * mu) >> 64);
            u64 r = y - q * Q;
            while (r >= Q) r -= Q;                   // 至多一两次修正
            x = r;
            X[(size_t)i] = (unsigned int)x;
        }
    }

    const int H = n - 1;                 // 阈值取值 1..H（下标 0..H-1 对应 h=1..H）
    const ll hbase = vcnt;               // 横向管道区起点
    const ll hw = (ll)(m - 1);

    std::vector<ll> dp(H), ndp(H), P(H), best(H);

    // 第 1 列：dp[h] = vert(h+1, 1) = X[h*m]
    for (int h = 0; h < H; ++h) dp[h] = (ll)X[(size_t)h * m];

    for (int t = 2; t <= m; ++t) {
        // 第 t-1 列与第 t 列之间横向管道的 P(h)=Σ_{s=2..h}H(s,t-1)，
        // 同时做前缀最小值：min_u{ dp[u] − P(u) } + P(v)  (P(u) ≤ P(v))
        P[0] = 0;
        {
            ll mn = dp[0] - P[0];
            best[0] = mn + P[0];
            ll acc = 0;
            for (int h = 2; h <= H; ++h) {
                acc += (ll)X[(size_t)(hbase + (ll)(h - 2) * hw + (t - 2))];
                P[h - 1] = acc;
                ll v = dp[h - 1] - acc;
                if (v < mn) mn = v;
                best[h - 1] = mn + acc;
            }
        }
        // 后缀最小值：min_u{ dp[u] + P(u) } − P(v)  (P(u) ≥ P(v))，并直接算 ndp
        {
            ll mn = LLONG_MAX;
            for (int h = H - 1; h >= 0; --h) {
                ll v = dp[h] + P[h];
                if (v < mn) mn = v;
                ll cand = mn - P[h];
                if (cand < best[h]) best[h] = cand;
                ndp[h] = best[h] + (ll)X[(size_t)h * m + (t - 1)];  // + vert(h+1,t)
            }
        }
        dp.swap(ndp);
    }

    ll ans = LLONG_MAX;
    for (int h = 0; h < H; ++h) if (dp[h] < ans) ans = dp[h];
    printf("%lld\n", ans);
    delete[] X;
    return 0;
}
