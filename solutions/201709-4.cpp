// 201709-4 通信网络
// 部门 a 知道 b  <=>  a 能到达 b  或  b 能到达 a   (单向可达即互相知道)。
// 对每个点 i 做一次正向 BFS、一次反向 BFS，把两类可达点合并记到同一行，
// 若该行（含 i 自己）覆盖了全部 N 个部门，则答案 +1。
#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<vector<int>> g(n), rg(n);      // g: 原图, rg: 反图
    for (int i = 0; i < m; ++i) {
        int a, b;
        cin >> a >> b;
        --a; --b;
        g[a].push_back(b);
        rg[b].push_back(a);
    }

    vector<unsigned char> know((size_t)n * n, 0);  // know[i*n+j]: i 知道 j
    vector<int> vis(n, 0);
    int stamp = 0;

    // 从 s 出发在 adj 上 BFS，把访问到的点写入 know 的第 s 行
    auto bfs = [&](int s, const vector<vector<int>>& adj) {
        ++stamp;
        unsigned char* row = &know[(size_t)s * n];
        queue<int> q;
        q.push(s);
        vis[s] = stamp;
        row[s] = 1;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int v : adj[u]) {
                if (vis[v] != stamp) {
                    vis[v] = stamp;
                    row[v] = 1;
                    q.push(v);
                }
            }
        }
    };

    long long ans = 0;
    for (int i = 0; i < n; ++i) {
        bfs(i, g);       // i 能通知到的
        bfs(i, rg);      // 能通知到 i 的
        int cnt = 0;
        const unsigned char* row = &know[(size_t)i * n];
        for (int j = 0; j < n; ++j) cnt += row[j];
        if (cnt == n) ++ans;
    }

    cout << ans << '\n';
    return 0;
}
