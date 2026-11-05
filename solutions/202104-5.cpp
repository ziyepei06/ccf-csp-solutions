// 202104-5 疫苗运输
// 做法：状态 = (线路 i, 该线路上的第 j 个站点)，dist 表示"疫苗在 i 号线车上、
// 车正停在该线路第 j 个站点的最早时刻"。由于车是周期 T_i 的定点循环，
// 所有可达时刻必然 ≡ offset[i][j] (mod T_i)，故同一状态只需保留最早时刻（最小值支配）。
// 两类转移：
//   1) 沿线路往前开一格：dist += travel[i][j]；
//   2) 在同名站点换乘到另一条线路 i'：需解同余 d + k*T_i ≡ s' (mod T_i')，
//      取最小 k>=0，用扩展欧几里得（等价于 exCRT / 线性同余）。
// 图上做 Dijkstra（转移权重关于 d 单调不减，故正确）。答案 = 各站点上所有状态的最小时间。
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef __int128 lll;

static const ll INF = (ll)4e18;

/* ---------------- 快速输入 ---------------- */
static char *ibuf = nullptr;
static size_t ipos = 0, ilen = 0;
static inline void initInput() {
    ilen = (size_t)1 << 23;              // 8MB 缓冲，足够 500*500 条记录
    ibuf = (char *)malloc(ilen);
    ilen = fread(ibuf, 1, ilen, stdin);
    ipos = 0;
}
static inline int readInt() {
    while (ipos < ilen && (ibuf[ipos] < '0' || ibuf[ipos] > '9') && ibuf[ipos] != '-') ++ipos;
    int sgn = 1;
    if (ipos < ilen && ibuf[ipos] == '-') { sgn = -1; ++ipos; }
    int x = 0;
    while (ipos < ilen && ibuf[ipos] >= '0' && ibuf[ipos] <= '9') { x = x * 10 + (ibuf[ipos] - '0'); ++ipos; }
    return sgn * x;
}

/* ---------------- 扩展欧几里得 / 模逆 ---------------- */
static ll exgcd(ll a, ll b, ll &x, ll &y) {
    if (!b) { x = 1; y = 0; return a; }
    ll g = exgcd(b, a % b, y, x);
    y -= (a / b) * x;
    return g;
}
// 返回 a 在模 mod 下的逆元（要求 gcd(a,mod)=1, mod>1）
static ll invMod(ll a, ll mod) {
    ll x, y;
    exgcd(a, mod, x, y);
    x %= mod;
    if (x < 0) x += mod;
    return x;
}

static ll mygcd(ll a, ll b) { while (b) { ll t = a % b; a = b; b = t; } return a < 0 ? -a : a; }

/* ---------------- 站点上的线路出现记录 ---------------- */
struct Occ { int line, pos; ll off; };

int main() {
    initInput();
    int n = readInt(), m = readInt();
    if (n < 1) return 0;
    if (m <= 0) { for (int v = 2; v <= n; ++v) printf("inf\n"); return 0; }

    vector<int>  len(m), T(m);
    vector<vector<int>> st(m), trav(m);
    vector<vector<ll>>  off(m);
    vector<vector<Occ>> occ(n + 1);

    for (int i = 0; i < m; ++i) {
        int l = readInt();
        if (l < 1) l = 1;
        len[i] = l;
        st[i].resize(l); trav[i].resize(l); off[i].resize(l);
        ll s = 0;
        for (int j = 0; j < l; ++j) {
            int a = readInt(), t = readInt();
            st[i][j] = a; trav[i][j] = t; off[i][j] = s;
            if (a >= 1 && a <= n) occ[a].push_back(Occ{i, j, s});
            s += t;
        }
        T[i] = (int)s;
    }

    // 节点编号：线路 i 的第 j 个站点 -> base[i] + j
    vector<int> base(m);
    int K = 0;
    for (int i = 0; i < m; ++i) { base[i] = K; K += len[i]; }
    vector<int> nodeLine(K), nodePos(K);
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < len[i]; ++j) { nodeLine[base[i] + j] = i; nodePos[base[i] + j] = j; }

    // 预处理有序线路对 (i,i') 的同余参数：g = gcd(T_i,T_i')，M = T_i'/g，
    // inv = (T_i/g)^{-1} mod M。这样每条换乘边的解只需一次乘一次取模。
    size_t sz = (size_t)m * (size_t)m;
    vector<int> pg(sz, 0);
    vector<ll>  pM(sz, 1), pInv(sz, 0);
    for (int i = 0; i < m; ++i) {
        for (int i2 = 0; i2 < m; ++i2) {
            if (i == i2) continue;
            ll g = mygcd((ll)T[i], (ll)T[i2]);
            ll M = (ll)T[i2] / g;
            size_t id = (size_t)i * m + i2;
            pg[id] = (int)g;
            pM[id] = M;
            if (M > 1) pInv[id] = invMod((ll)T[i] / g, M);
        }
    }

    // Dijkstra
    vector<ll> dist(K, INF);
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < len[i]; ++j) {
            if (st[i][j] == 1) {                 // 1 号站有冷库，可在 0 时刻起任意等待
                ll d = off[i][j];                // 车首次到达 1 号站的时刻
                int u = base[i] + j;
                if (d < dist[u]) { dist[u] = d; pq.push({d, u}); }
            }
        }
    }

    while (!pq.empty()) {
        pair<ll, int> pr = pq.top(); pq.pop();
        ll d = pr.first; int u = pr.second;
        if (d != dist[u]) continue;
        int i = nodeLine[u], j = nodePos[u];
        int l = len[i];
        ll Ti = (ll)T[i], s1 = off[i][j];

        // 1) 沿线路继续往前开
        {
            int j2 = (j + 1 == l) ? 0 : j + 1;
            ll nd = d + (ll)trav[i][j];
            int u2 = base[i] + j2;
            if (nd < dist[u2]) { dist[u2] = nd; pq.push({nd, u2}); }
        }

        // 2) 在站点 st[i][j] 换乘
        int v = st[i][j];
        for (const Occ &o : occ[v]) {
            if (o.line == i) continue;
            size_t id = (size_t)i * m + o.line;
            int g = pg[id];
            if (g == 0) continue;
            ll delta = o.off - s1;
            if (delta % (ll)g != 0) continue;        // 两车永不同时到该站
            ll M = pM[id];
            ll c = 0;
            if (M > 1) {
                ll q = delta / g;
                q %= M; if (q < 0) q += M;
                c = (ll)((lll)q * pInv[id] % M);
            }
            ll kp = (d - s1) / Ti;                   // d ≡ s1 (mod Ti)，已转过 kp 圈
            ll kk = (c - kp) % M;                    // 还需再转的圈数 (< M)
            if (kk < 0) kk += M;
            lll Bb = (lll)d + (lll)kk * Ti;
            ll B = (Bb >= (lll)INF) ? INF : (ll)Bb;
            int u2 = base[o.line] + o.pos;
            if (B < dist[u2]) { dist[u2] = B; pq.push({B, u2}); }
        }
    }

    vector<ll> ans(n + 1, INF);
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < len[i]; ++j) {
            ll d = dist[base[i] + j];
            int v = st[i][j];
            if (v >= 1 && v <= n && d < ans[v]) ans[v] = d;
        }

    for (int v = 2; v <= n; ++v) {
        if (ans[v] >= INF) printf("inf\n");
        else printf("%lld\n", ans[v]);
    }
    return 0;
}
