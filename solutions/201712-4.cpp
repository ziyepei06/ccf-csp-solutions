// 201712-4 行车路线
// 关键观察：一条路线 = 若干「小道连续段」与「大道单边」交替。
// 小道连续段的疲劳只取决于段内总长度 s：疲劳为 s^2。
// 因此对每个起点 u 用 Floyd 求出「仅走小道」的最短距离 d[u][v]，
// 则从 u 出发的一段小道必然以最短距离走才最优，代价 d[u][v]^2。
// 状态：(节点, 是否刚结束一段小道)，后者为 1 时下一步只能走大道。
// 状态数 2n，边数 O(n^2 + m)，跑 Dijkstra 即可。
#include <bits/stdc++.h>
using namespace std;

int main() {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 0;
    if (n == 1) { printf("0\n"); return 0; }

    const int BIG = 1000000000;       // 距离意义上的无穷
    vector<vector<int>> bg(n, vector<int>(n, BIG));   // 大道最短路（单边）
    vector<vector<int>> sm(n, vector<int>(n, BIG));   // 小道图邻接
    for (int i = 0; i < n; i++) { bg[i][i] = 0; sm[i][i] = 0; }

    for (int i = 0; i < m; i++) {
        int t, a, b, c;
        if (scanf("%d %d %d %d", &t, &a, &b, &c) != 4) break;
        --a; --b;
        if (a == b) continue;
        if (t == 0) {
            if (c < bg[a][b]) { bg[a][b] = c; bg[b][a] = c; }
        } else {
            if (c < sm[a][b]) { sm[a][b] = c; sm[b][a] = c; }
        }
    }

    // Floyd：仅用小道时的全源最短路
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            int dik = sm[i][k];
            if (dik == BIG) continue;
            for (int j = 0; j < n; j++) {
                int nv = dik + sm[k][j];
                if (nv < sm[i][j]) sm[i][j] = nv;
            }
        }
    }

    // 状态 2*v: 位于 v，可以开始一段小道（或刚走完大道/起点）
    // 状态 2*v+1: 位于 v，刚结束一段小道
    const long long INF = (long long)4e18;
    vector<long long> dist(2 * n, INF);
    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<pair<long long, int>>> pq;
    dist[0] = 0;
    pq.push({0, 0});

    while (!pq.empty()) {
        auto [du, id] = pq.top(); pq.pop();
        if (du != dist[id]) continue;
        int v = id >> 1;
        bool ended = (id & 1) != 0;
        if (du > dist[id]) continue;

        // 走一条大道 -> 变为可开始小道
        for (int w = 0; w < n; w++) {
            int c = bg[v][w];
            if (w == v || c == BIG) continue;
            long long nd = du + c;
            int nid = 2 * w;
            if (nd < dist[nid]) { dist[nid] = nd; pq.push({nd, nid}); }
        }
        // 刚结束小道则不能再连走小道
        if (ended) continue;

        // 走一段小道 v..w，代价 d^2
        for (int w = 0; w < n; w++) {
            if (w == v) continue;
            int d = sm[v][w];
            if (d >= BIG) continue;
            long long nd = du + (long long)d * d;
            int nid = 2 * w + 1;
            if (nd < dist[nid]) { dist[nid] = nd; pq.push({nd, nid}); }
        }
    }

    long long ans = min(dist[2 * (n - 1)], dist[2 * (n - 1) + 1]);
    printf("%lld\n", ans);
    return 0;
}
