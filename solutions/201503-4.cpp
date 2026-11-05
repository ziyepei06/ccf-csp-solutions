#include <bits/stdc++.h>
using namespace std;

// 网络延时：整棵树（交换机 + 终端电脑）的直径。
// 树的直径 = 任取一点出发最远的点 u，再从 u 出发最远的点 v，dist(u,v) 即直径。

int main() {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 0;

    int N = n + m;  // 节点编号：1..n 交换机，n+1..n+m 电脑
    vector<vector<int>> g(N + 1);

    auto addEdge = [&](int a, int b) {
        g[a].push_back(b);
        g[b].push_back(a);
    };

    // 第 2..n 台交换机的上层交换机（父节点）
    for (int i = 2; i <= n; ++i) {
        int p;
        scanf("%d", &p);
        addEdge(p, i);
    }
    // 第 1..m 台电脑所连的交换机
    for (int j = 1; j <= m; ++j) {
        int p;
        scanf("%d", &p);
        addEdge(p, n + j);
    }

    vector<int> dist(N + 1, -1);
    vector<int> q(N);

    // 从 start 出发 BFS，返回 {最远点, 最远距离}
    auto bfs = [&](int start) -> pair<int, int> {
        fill(dist.begin(), dist.end(), -1);
        int head = 0, tail = 0;
        q[tail++] = start;
        dist[start] = 0;
        int farNode = start, farDist = 0;
        while (head < tail) {
            int u = q[head++];
            if (dist[u] > farDist) {
                farDist = dist[u];
                farNode = u;
            }
            for (int v : g[u]) {
                if (dist[v] == -1) {
                    dist[v] = dist[u] + 1;
                    q[tail++] = v;
                }
            }
        }
        return {farNode, farDist};
    };

    // 只连了 1 个节点时距离为 0；否则两次 BFS 求直径
    int u = bfs(1).first;
    int ans = bfs(u).second;
    printf("%d\n", ans);
    return 0;
}
