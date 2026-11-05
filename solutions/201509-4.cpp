#include <bits/stdc++.h>
using namespace std;

// 201509-4 高速公路
// 求强连通分量（SCC），每个大小为 s 的 SCC 内任意两点互相可达，
// 贡献 C(s,2) = s*(s-1)/2 个便利城市对。
// 用迭代式 Tarjan，避免 n 达 10000 时的深递归爆栈。

int main() {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 0;

    vector<int> head(n + 1, -1), nxt(m), to(m);
    for (int i = 0; i < m; ++i) {
        int a, b;
        scanf("%d %d", &a, &b);
        to[i] = b;
        nxt[i] = head[a];
        head[a] = i;
    }

    vector<int> dfn(n + 1, 0), low(n + 1, 0), stk;
    vector<char> inStk(n + 1, 0);
    vector<int> callNode, callEdge;  // 模拟递归栈：当前节点、下一条待处理边
    stk.reserve(n);
    int timer = 0;
    long long ans = 0;

    for (int s = 1; s <= n; ++s) {
        if (dfn[s]) continue;
        dfn[s] = low[s] = ++timer;
        stk.push_back(s);
        inStk[s] = 1;
        callNode.push_back(s);
        callEdge.push_back(head[s]);

        while (!callNode.empty()) {
            int u = callNode.back();
            int e = callEdge.back();
            if (e != -1) {
                callEdge.back() = nxt[e];
                int v = to[e];
                if (!dfn[v]) {
                    dfn[v] = low[v] = ++timer;
                    stk.push_back(v);
                    inStk[v] = 1;
                    callNode.push_back(v);
                    callEdge.push_back(head[v]);
                } else if (inStk[v]) {
                    if (dfn[v] < low[u]) low[u] = dfn[v];
                }
            } else {
                // u 的所有出边处理完，回溯
                callNode.pop_back();
                callEdge.pop_back();
                if (!callNode.empty()) {
                    int p = callNode.back();
                    if (low[u] < low[p]) low[p] = low[u];
                }
                if (low[u] == dfn[u]) {
                    long long cnt = 0;
                    while (true) {
                        int w = stk.back();
                        stk.pop_back();
                        inStk[w] = 0;
                        ++cnt;
                        if (w == u) break;
                    }
                    ans += cnt * (cnt - 1) / 2;
                }
            }
        }
    }

    printf("%lld\n", ans);
    return 0;
}
