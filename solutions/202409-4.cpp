// CCF CSP 第35次（2024-09）第4题 通讯延迟
// 建模：节点 1..n + 基站 1..m 的二部图 Dijkstra
//   节点 u -> 基站 j：若 u 落在 j 的正方形内，代价 t_j
//   基站 j -> 节点 v：若 v 落在 j 的正方形内，代价 0
// 关键性质：每一条边的代价只与起点类型有关（t_j 或 0），且按 dist 升序出堆，
// 因此一个顶点第一次被“触及”时得到的就是它的最终最短路，可一次性定型（settle-once），
// 无需重复松弛。每个顶点只会被扫描一次，总代价 O(n*m)。
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
const ll INF = (ll)4e18;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<ll> nx(n), ny(n);
    for (int i = 0; i < n; i++) cin >> nx[i] >> ny[i];

    vector<ll> sx(m), sy(m), sr(m), st(m);
    for (int j = 0; j < m; j++) cin >> sx[j] >> sy[j] >> sr[j] >> st[j];

    // 顶点编号：节点 0..n-1，基站 n..n+m-1
    const int N = n + m;
    vector<ll> dist(N, INF);
    vector<char> done(N, 0);     // 已出堆（最短路已确定）
    vector<char> touched(N, 0);  // 已被首次触及（dist 已定型）

    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;

    dist[0] = 0;
    touched[0] = 1;
    pq.push({0, 0});

    ll ans = -1;

    while (!pq.empty()) {
        auto pr = pq.top();
        pq.pop();
        ll d = pr.first;
        int u = pr.second;
        if (done[u]) continue;
        done[u] = 1;

        if (u == n - 1) {  // 到达节点 n
            ans = d;
            break;
        }

        if (u < n) {
            // 节点 u -> 所有尚未触及且覆盖 u 的基站
            ll x = nx[u], y = ny[u];
            for (int j = 0; j < m; j++) {
                int w = n + j;
                if (touched[w]) continue;
                ll dx = x - sx[j];
                if (dx < 0) dx = -dx;
                if (dx > sr[j]) continue;
                ll dy = y - sy[j];
                if (dy < 0) dy = -dy;
                if (dy > sr[j]) continue;
                touched[w] = 1;       // 首次触及即为最终最短路
                dist[w] = d + st[j];
                pq.push({dist[w], w});
            }
        } else {
            // 基站 j -> 所有尚未触及且被 j 覆盖的节点，代价 0
            int j = u - n;
            ll x = sx[j], y = sy[j], r = sr[j];
            for (int v = 0; v < n; v++) {
                if (touched[v]) continue;
                ll dx = x - nx[v];
                if (dx < 0) dx = -dx;
                if (dx > r) continue;
                ll dy = y - ny[v];
                if (dy < 0) dy = -dy;
                if (dy > r) continue;
                touched[v] = 1;
                dist[v] = d;
                pq.push({d, v});
            }
        }
    }

    if (ans < 0)
        cout << "Nan\n";
    else
        cout << ans << "\n";

    return 0;
}
