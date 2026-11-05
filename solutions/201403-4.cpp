// 201403-4 无线网络
// 状态 BFS：dist[点][已增设路由器数]，每条边代价 1（即每经过一个路由器算一次跳数）。
// 答案 = 最少跳数 - 1，即中转路由器个数；只在 已用新增 <= k 的状态里转移。
#include <bits/stdc++.h>
using namespace std;

int main() {
    int n, m, k;
    long long r;
    if (!(cin >> n >> m >> k >> r)) return 0;

    int N = n + m;
    vector<long long> x(N), y(N);
    for (int i = 0; i < N; ++i) cin >> x[i] >> y[i];

    vector<vector<int>> adj(N);
    long long rr = r * r;                       // r <= 1e8 -> rr <= 1e16
    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            long long dx = x[i] - x[j], dy = y[i] - y[j];
            if (dx * dx + dy * dy <= rr) {      // coords <= 1e8 -> 8e16 < 9.2e18
                adj[i].push_back(j);
                adj[j].push_back(i);
            }
        }
    }

    // dist[u][c] : 从路由器 1(下标 0) 出发到达 u、且路上新增了 c 个路由器的最少跳数
    vector<vector<int>> dist(N, vector<int>(k + 1, -1));
    deque<int> q;                                // 编码 u*(k+1)+c
    dist[0][0] = 0;
    q.push_back(0 * (k + 1) + 0);
    while (!q.empty()) {
        int code = q.front(); q.pop_front();
        int u = code / (k + 1), c = code % (k + 1);
        int d = dist[u][c];
        for (int v : adj[u]) {
            int nc = c + (v >= n ? 1 : 0);       // 进入新增路由器才计数
            if (nc > k) continue;
            if (dist[v][nc] != -1) continue;
            dist[v][nc] = d + 1;
            q.push_back(v * (k + 1) + nc);
        }
    }

    int best = INT_MAX;
    for (int c = 0; c <= k; ++c)
        if (dist[1][c] != -1) best = min(best, dist[1][c]);
    cout << best - 1 << '\n';                    // 跳数 - 1 = 中转路由器数
    return 0;
}
