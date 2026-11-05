// 201412-5 货物调度
//
// 建模：最小费用流（循环流），时间展开成一周 7 天。
//
// 由于「城市之间距离不远、当天即可到达、每天运输量无限制」，同一天内沿道路运输等价于
// 走最短路且各边容量无限；而运输途中不占用仓库，所以无需在全图上做最短路收缩，直接按天
// 复制道路即可（同一天内多点中转是允许的，路径成本自动等于最短路）。
//
// 结点：(天 d, 城市 i)，d∈[0,7)，编号 d*n+i；另加超级源 S、超级汇 T。
//   * 生产：S -> (d,i)，容量 a[i][d]，费用 0
//   * 消耗：(d,i) -> T，容量 b[i][d]，费用 0
//   * 过夜存储：(d,i) -> ((d+1) mod 7, i)，容量 v[i]，费用 w[i]
//     （d=6 回到 d=0，对应每周循环；费用为正，故最优解不会出现无用的绕圈货物）
//   * 道路：对每天 d 与每条道路 (s,t,c)，加 (d,s)->(d,t) 与 (d,t)->(d,s)，容量 INF，费用 c
//
// 每周产量=消耗量，所以需要推送的总流量 = Σa = Σb。最小费用最大流即为答案。
//
// 求解：原始-对偶（Dijkstra + 势能）求最短路，再在「零约化费用」的允许图上跑 Dinic
// 分层阻塞流一次推多条路径，避免按单位流量逐条增广（总量可达 7·100·100=70000）。
#include <bits/stdc++.h>

struct MinCostFlow {
    struct Edge {
        int to, rev;
        long long cap, cost;
    };
    int n;
    std::vector<std::vector<Edge>> g;
    std::vector<long long> pot, dist;

    explicit MinCostFlow(int n_) : n(n_), g(n_), pot(n_, 0) {}

    void addEdge(int u, int v, long long cap, long long cost) {
        g[u].push_back(Edge{v, (int)g[v].size(), cap, cost});
        g[v].push_back(Edge{u, (int)g[u].size() - 1, 0, -cost});
    }

    // 在约化费用为 0 的残量边上求分层图，用 Dinic 阻塞流一次尽量多推
    long long blockingFlow(int s, int t, long long limit) {
        std::vector<int> level(n, -1), it(n, 0), que;
        que.reserve(n);
        level[s] = 0;
        que.push_back(s);
        for (size_t h = 0; h < que.size(); ++h) {
            int u = que[h];
            for (const Edge &e : g[u]) {
                if (e.cap > 0 && level[e.to] < 0 && e.cost + pot[u] - pot[e.to] == 0) {
                    level[e.to] = level[u] + 1;
                    que.push_back(e.to);
                }
            }
        }
        if (level[t] < 0) return 0;
        std::function<long long(int, long long)> dfs = [&](int u, long long f) -> long long {
            if (u == t) return f;
            for (int &i = it[u]; i < (int)g[u].size(); ++i) {
                Edge &e = g[u][i];
                if (e.cap > 0 && level[e.to] == level[u] + 1 &&
                    e.cost + pot[u] - pot[e.to] == 0) {
                    long long d = dfs(e.to, std::min(f, e.cap));
                    if (d > 0) {
                        e.cap -= d;
                        g[e.to][e.rev].cap += d;
                        return d;
                    }
                }
            }
            level[u] = -1;  // 剪枝：该点已无可用出边
            return 0;
        };
        long long total = 0;
        while (total < limit) {
            long long f = dfs(s, limit - total);
            if (f == 0) break;
            total += f;
        }
        return total;
    }

    // 返回 {flow, cost}
    std::pair<long long, long long> solve(int s, int t, long long need) {
        const long long INFLL = std::numeric_limits<long long>::max() / 4;
        long long flow = 0, cost = 0;
        while (flow < need) {
            dist.assign(n, INFLL);
            dist[s] = 0;
            typedef std::pair<long long, int> P;
            std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
            pq.push(P(0, s));
            while (!pq.empty()) {
                P cur = pq.top();
                pq.pop();
                int u = cur.second;
                if (cur.first > dist[u]) continue;
                for (const Edge &e : g[u]) {
                    if (e.cap <= 0) continue;
                    long long rc = e.cost + pot[u] - pot[e.to];  // 约化费用，非负
                    if (dist[u] + rc < dist[e.to]) {
                        dist[e.to] = dist[u] + rc;
                        pq.push(P(dist[e.to], e.to));
                    }
                }
            }
            if (dist[t] >= INFLL) break;  // 无法继续增广
            for (int v = 0; v < n; ++v)
                if (dist[v] < INFLL) pot[v] += dist[v];
            long long f = blockingFlow(s, t, need - flow);
            if (f <= 0) break;
            flow += f;
            cost += f * (pot[t] - pot[s]);  // 允许路径上约化费用全为 0，原费用可伸缩求和
        }
        return std::make_pair(flow, cost);
    }
};

int main() {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 0;

    std::vector<std::vector<long long>> prod(n, std::vector<long long>(7));
    std::vector<std::vector<long long>> cons(n, std::vector<long long>(7));
    std::vector<long long> cap(n), price(n);
    long long total = 0;
    for (int i = 0; i < n; ++i) {
        for (int d = 0; d < 7; ++d) {
            if (scanf("%lld", &prod[i][d]) != 1) return 0;
            total += prod[i][d];
        }
        for (int d = 0; d < 7; ++d) {
            if (scanf("%lld", &cons[i][d]) != 1) return 0;
        }
        if (scanf("%lld %lld", &cap[i], &price[i]) != 2) return 0;
    }

    std::vector<int> rs, rt;
    std::vector<long long> rc;
    rs.reserve(m);
    rt.reserve(m);
    rc.reserve(m);
    for (int k = 0; k < m; ++k) {
        int s, t;
        long long c;
        if (scanf("%d %d %lld", &s, &t, &c) != 3) return 0;
        --s;
        --t;
        rs.push_back(s);
        rt.push_back(t);
        rc.push_back(c);
    }

    if (total == 0) {
        printf("0\n");
        return 0;
    }

    int S = 7 * n, T = 7 * n + 1;
    MinCostFlow mcf(7 * n + 2);
    const long long INFCAP = total;  // 道路/存储边容量上限只需覆盖总流量

    for (int d = 0; d < 7; ++d) {
        for (int i = 0; i < n; ++i) {
            int id = d * n + i;
            if (prod[i][d] > 0) mcf.addEdge(S, id, prod[i][d], 0);
            if (cons[i][d] > 0) mcf.addEdge(id, T, cons[i][d], 0);
            if (cap[i] > 0) mcf.addEdge(id, ((d + 1) % 7) * n + i, cap[i], price[i]);
        }
        for (int k = 0; k < m; ++k) {
            mcf.addEdge(d * n + rs[k], d * n + rt[k], INFCAP, rc[k]);
            mcf.addEdge(d * n + rt[k], d * n + rs[k], INFCAP, rc[k]);
        }
    }

    std::pair<long long, long long> res = mcf.solve(S, T, total);
    printf("%lld\n", res.second);
    return 0;
}
