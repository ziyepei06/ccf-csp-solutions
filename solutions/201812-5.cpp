// CCF CSP 201812-5 管道清洁
// 思路：所求是一条从 1 出发、回到 1 的闭合行走（Euler 回路），其中
//   A 类边至少走 1 次（可多次）；B 类边恰好走 1 次；C 类边可走任意次（含 0）；
//   D 类边至多走 1 次。
// 先把 A、B 各取 1 份作为"基础"重数，得到每个点的出入度差
//   b(v) = in_base(v) - out_base(v)，
// 再补若干次额外经过，使 b(v) 被抹平且总次数最少。
// 补边可看作从 b>0 的点（供给 b 单位）沿原图有向边流向 b<0 的点（需求 -b 单位），
// 每用一条边计 1 次花费 —— 于是化为容量上限为：A/C 无限、D 为 1、B 为 0 的
// 最小费用最大流；流不满则无解。最小费用即额外经过次数。
// 由于题面保证 A∪B 及其端点（含 1 号点）弱连通，且最优补边可分解为端点都在该
// 连通块内的路径（回路只会增加费用、可删去），故度数配平即等价于存在 Euler 回路。
#include <bits/stdc++.h>
using namespace std;

struct MCMF {
    struct Edge { int to, cap, cost, rev; };
    int n;
    vector<vector<Edge>> g;
    vector<int> h, dist, pv, pe;

    explicit MCMF(int n_) : n(n_), g(n_), h(n_, 0), dist(n_), pv(n_), pe(n_) {}

    void addEdge(int u, int v, int cap, int cost) {
        g[u].push_back({v, cap, cost, (int)g[v].size()});
        g[v].push_back({u, 0, -cost, (int)g[u].size() - 1});
    }

    // 返回 {flow, cost}；边权非负，用 Dijkstra + 势能
    pair<int, int> run(int s, int t) {
        const int INF = INT_MAX / 2;
        int flow = 0, cost = 0;
        vector<char> done(n);
        while (true) {
            fill(dist.begin(), dist.end(), INF);
            fill(done.begin(), done.end(), 0);
            dist[s] = 0;
            priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
            pq.push({0, s});
            while (!pq.empty()) {
                pair<int, int> top = pq.top(); pq.pop();
                int u = top.second;
                if (done[u]) continue;
                done[u] = 1;
                int du = top.first;
                for (int i = 0; i < (int)g[u].size(); ++i) {
                    Edge &e = g[u][i];
                    if (e.cap <= 0) continue;
                    int nd = du + e.cost + h[u] - h[e.to];
                    if (nd < dist[e.to]) {
                        dist[e.to] = nd;
                        pv[e.to] = u;
                        pe[e.to] = i;
                        pq.push({nd, e.to});
                    }
                }
            }
            if (dist[t] >= INF) break;
            for (int v = 0; v < n; ++v)
                if (dist[v] < INF) h[v] += dist[v];
            int f = INF;
            for (int v = t; v != s; v = pv[v]) f = min(f, g[pv[v]][pe[v]].cap);
            for (int v = t; v != s; v = pv[v]) {
                Edge &e = g[pv[v]][pe[v]];
                e.cap -= f;
                g[v][e.rev].cap += f;
            }
            flow += f;
            cost += f * h[t];
        }
        return {flow, cost};
    }
};

int main() {
    int T, S, E;
    if (!(cin >> T >> S >> E)) return 0;
    while (T--) {
        int n, m;
        cin >> n >> m;
        vector<int> eu(m), ev(m);
        vector<char> et(m);
        vector<int> inBase(n + 1, 0), outBase(n + 1, 0);
        int base = 0;
        for (int i = 0; i < m; ++i) {
            char c;
            cin >> eu[i] >> ev[i] >> c;
            et[i] = c;
            if (c == 'A' || c == 'B') {
                ++base;
                ++outBase[eu[i]];
                ++inBase[ev[i]];
            }
        }
        if (base == 0) {           // 没有需要清理的管道
            cout << 0 << '\n';
            continue;
        }
        // b(v) = in_base - out_base：>0 表示还缺出边，<0 表示还缺入边
        vector<int> b(n + 1);
        int need = 0;
        for (int v = 1; v <= n; ++v) {
            b[v] = inBase[v] - outBase[v];
            if (b[v] > 0) need += b[v];
        }
        int src = 0, snk = n + 1;
        MCMF mf(n + 2);
        for (int v = 1; v <= n; ++v) {
            if (b[v] > 0) mf.addEdge(src, v, b[v], 0);
            else if (b[v] < 0) mf.addEdge(v, snk, -b[v], 0);
        }
        const int BIG = m + 5;     // 额外经过次数不需要超过 m 次
        for (int i = 0; i < m; ++i) {
            int cap;
            if (et[i] == 'A' || et[i] == 'C') cap = BIG;
            else if (et[i] == 'D') cap = 1;
            else cap = 0;          // B 类只能走一次
            if (cap > 0) mf.addEdge(eu[i], ev[i], cap, 1);
        }
        pair<int, int> res = mf.run(src, snk);
        if (res.first < need) {
            cout << -1 << '\n';
        } else {
            long long total = (long long)base + res.second;
            cout << total * E << '\n';
        }
    }
    return 0;
}
