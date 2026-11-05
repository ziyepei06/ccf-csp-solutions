#include <bits/stdc++.h>
using namespace std;

int R, C;
vector<string> g;

int nid(int r, int c) { return r * C + c; }

// 返回格子 (r,c) 可以移动到的格子编号列表
static const int dr[4] = {-1, 1, 0, 0};
static const int dc[4] = {0, 0, -1, 1};
// 上 下 左 右
vector<int> dests(int r, int c) {
    vector<int> v;
    char ch = g[r][c];
    auto add = [&](int dir) {
        int nr = r + dr[dir], nc = c + dc[dir];
        if (nr < 0 || nr >= R || nc < 0 || nc >= C) return;
        if (g[nr][nc] == '#') return;
        v.push_back(nid(nr, nc));
    };
    if (ch == '+') { add(0); add(1); add(2); add(3); }
    else if (ch == '-') { add(2); add(3); }
    else if (ch == '|') { add(0); add(1); }
    else if (ch == '.') { add(1); }
    else if (ch == 'S' || ch == 'T') { add(0); add(1); add(2); add(3); }
    return v;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if (!(cin >> R >> C)) return 0;
    g.assign(R, string());
    int sr = -1, sc = -1, tr = -1, tc = -1;
    for (int i = 0; i < R; i++) {
        cin >> g[i];
        for (int j = 0; j < C; j++) {
            if (g[i][j] == 'S') { sr = i; sc = j; }
            if (g[i][j] == 'T') { tr = i; tc = j; }
        }
    }
    if (sr < 0 || tr < 0) { cout << "I'm stuck!\n"; return 0; }
    int N = R * C;
    // 正向图与反向图
    vector<vector<int>> adj(N), radj(N);
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++) {
            if (g[i][j] == '#') continue;
            int u = nid(i, j);
            for (int v : dests(i, j)) {
                adj[u].push_back(v);
                radj[v].push_back(u);
            }
        }

    // 从 S 可达
    vector<char> fromS(N, 0);
    {
        queue<int> q;
        int s = nid(sr, sc);
        fromS[s] = 1; q.push(s);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) if (!fromS[v]) { fromS[v] = 1; q.push(v); }
        }
    }
    int t = nid(tr, tc);
    if (!fromS[t]) { cout << "I'm stuck!\n"; return 0; }

    // 能到达 T 的格子：在反图上从 T 做 BFS
    vector<char> toT(N, 0);
    {
        queue<int> q;
        toT[t] = 1; q.push(t);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : radj[u]) if (!toT[v]) { toT[v] = 1; q.push(v); }
        }
    }

    int ans = 0;
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++) {
            if (g[i][j] == '#') continue;
            int u = nid(i, j);
            if (fromS[u] && !toT[u]) ans++;
        }
    cout << ans << "\n";
    return 0;
}
