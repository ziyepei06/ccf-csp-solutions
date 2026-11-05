#include <bits/stdc++.h>
using namespace std;

// 202012-4 食材运输
// 思路：对答案 T 二分。固定 T 后，对每种食材 i 与每个酒店 v 计算
// cost[i][v] = 从 v 出发送完所有需要食材 i 的酒店所需的最短行驶时间。
// 树上从 v 出发遍历必访点集 S 的最优路线为走遍 v∪S 的最小连通子树（总长 W），
// 最后停在离 v 最远的必访点，故 cost = 2W - maxDepth(v)。
// 于是 v 在 T 限制下能覆盖的食材集合是一个 K 位掩码，问题变为
// 用不超过 M 个酒店覆盖全部 K 种食材 —— 状压 DP 求最少检查点数，与 M 比较。
// 复杂度 O(K*N^2 + log(SumW) * 2^K * N)。

int N, M, K;
vector<vector<pair<int, long long>>> g;  // (to, w)
vector<vector<char>> need;               // need[v][i]
vector<vector<long long>> cost;          // cost[i][v]

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (!(cin >> N >> M >> K)) return 0;

    need.assign(N + 1, vector<char>(K, 0));
    for (int v = 1; v <= N; ++v)
        for (int i = 0; i < K; ++i) {
            int x;
            cin >> x;
            need[v][i] = (char)(x != 0);
        }

    g.assign(N + 1, {});
    long long sumW = 0;
    for (int e = 0; e + 1 < N; ++e) {
        int u, v;
        long long w;
        cin >> u >> v >> w;
        g[u].push_back({v, w});
        g[v].push_back({u, w});
        sumW += w;
    }

    cost.assign(K, vector<long long>(N + 1, 0));

    vector<int> par(N + 1, 0), ord;
    vector<long long> wpar(N + 1, 0);
    vector<char> has(N + 1, 0);
    vector<long long> ret(N + 1, 0), dep(N + 1, 0);

    for (int i = 0; i < K; ++i) {
        bool any = false;
        for (int v = 1; v <= N; ++v)
            if (need[v][i]) any = true;
        if (!any) {  // 该食材无人需要，任意检查点出发都不需要行驶
            for (int v = 1; v <= N; ++v) cost[i][v] = 0;
            continue;
        }
        for (int root = 1; root <= N; ++root) {
            // 以 root 为根建 BFS 序
            ord.clear();
            ord.reserve(N);
            par[root] = 0;
            wpar[root] = 0;
            ord.push_back(root);
            for (size_t idx = 0; idx < ord.size(); ++idx) {
                int u = ord[idx];
                for (auto &pr : g[u]) {
                    int c = pr.first;
                    long long w = pr.second;
                    if (c == par[u] && u != root) continue;
                    if (c == root) continue;
                    par[c] = u;
                    wpar[c] = w;
                    ord.push_back(c);
                }
            }
            // 自底向上合并
            for (int idx = N - 1; idx >= 0; --idx) {
                int u = ord[idx];
                has[u] = need[u][i];
                ret[u] = 0;
                dep[u] = 0;
            }
            for (int idx = N - 1; idx >= 0; --idx) {
                int u = ord[idx];
                if (u == root) continue;
                int p = par[u];
                if (has[u]) {
                    has[p] = 1;
                    ret[p] += ret[u] + 2 * wpar[u];
                    dep[p] = max(dep[p], dep[u] + wpar[u]);
                }
            }
            cost[i][root] = ret[root] - dep[root];
        }
    }

    const int full = (1 << K) - 1;
    const int INF = 1e9;

    auto feasible = [&](long long T) -> bool {
        vector<int> dp(1 << K, INF);
        dp[0] = 0;
        for (int v = 1; v <= N; ++v) {
            int vm = 0;
            for (int i = 0; i < K; ++i)
                if (cost[i][v] <= T) vm |= (1 << i);
            if (vm == 0) continue;
            for (int mask = full; mask >= 0; --mask) {
                if (dp[mask] >= INF) continue;
                int nm = mask | vm;
                if (dp[mask] + 1 < dp[nm]) dp[nm] = dp[mask] + 1;
            }
        }
        return dp[full] <= M;
    };

    long long lo = 0, hi = 2 * sumW + 1;  // hi 一定可行
    while (lo < hi) {
        long long mid = lo + (hi - lo) / 2;
        if (feasible(mid)) hi = mid;
        else lo = mid + 1;
    }
    cout << lo << '\n';
    return 0;
}
