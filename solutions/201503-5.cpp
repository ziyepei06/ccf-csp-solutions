// 201503-5 最小花费
//
// 模型：走 s->t 的唯一路径，每走 1 单位长度吃 1 单位食物，食物只能在城市买，
// 因此经过第 i 条边时的花费 = 边长 * min{s..当前城市} 的粮价（前缀最小值）。
//
// 做法：把路径按 lca 分成 上行段(s->lca) 与 下行段(lca->t)。
// 用树链剖分把路径拆成 O(log n) 条重链上的连续区间；每条重链内部按“位置从上到下”
// 预处理：
//   nxt[i] = i 之后最近的 w 严格更小的位置（下行“前缀最小”记录链）
//   prv[i] = i 之前最近的 w 严格更小的位置（上行“从下往上”记录链）
//   Dsum / Usum 为该记录链上的“代价前缀和”，配合倍增即可 O(log n) 求出
//   "给定进入时的前缀最小值 M，遍历区间 [l,r] 的代价，以及遍历后的新 M"。
// 区间之间只需传递 M，跨界（轻边）单独计入，总复杂度 O((n+m) log^2 n)。
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static const int MAXN = 100005;
static const int LOG = 18;               // 2^17 > 1e5
static const ll INF = (ll)1e18;

int n, m;
int w[MAXN];
vector<pair<int, int>> adj[MAXN];        // (to, len)

int par_[MAXN], dep_[MAXN], sz_[MAXN], heavy_[MAXN];
ll dist_[MAXN];

int chainId[MAXN], posInChain[MAXN];
vector<vector<int>> chainNodes;

struct Chain {
    int len = 0;
    vector<int> wv;                      // 粮价
    vector<ll> dv;                       // 到根的带权深度
    vector<int> nxt, prv;                // 记录链的后继/前驱位置
    vector<ll> Dsum, Usum;               // 记录链上的代价前缀和
    vector<vector<int>> upN, upP;        // 倍增

    void build(const vector<int> &nodes) {
        len = (int)nodes.size();
        wv.resize(len);
        dv.resize(len);
        for (int i = 0; i < len; ++i) {
            wv[i] = w[nodes[i]];
            dv[i] = dist_[nodes[i]];
        }
        // 下一个更小
        nxt.assign(len, -1);
        {
            vector<int> st;
            st.reserve(len);
            for (int i = len - 1; i >= 0; --i) {
                while (!st.empty() && wv[st.back()] >= wv[i]) st.pop_back();
                nxt[i] = st.empty() ? -1 : st.back();
                st.push_back(i);
            }
        }
        // 上一个更小
        prv.assign(len, -1);
        {
            vector<int> st;
            st.reserve(len);
            for (int i = 0; i < len; ++i) {
                while (!st.empty() && wv[st.back()] >= wv[i]) st.pop_back();
                prv[i] = st.empty() ? -1 : st.back();
                st.push_back(i);
            }
        }
        // 下行记录链代价前缀和：w[i]*(dv[nxt]-dv[i]) + Dsum[nxt]
        Dsum.assign(len, 0);
        for (int i = len - 1; i >= 0; --i)
            Dsum[i] = (nxt[i] == -1) ? 0
                                     : (ll)wv[i] * (dv[nxt[i]] - dv[i]) + Dsum[nxt[i]];
        // 上行记录链代价前缀和：w[i]*(dv[i]-dv[prv]) + Usum[prv]
        Usum.assign(len, 0);
        for (int i = 0; i < len; ++i)
            Usum[i] = (prv[i] == -1) ? 0
                                     : (ll)wv[i] * (dv[i] - dv[prv[i]]) + Usum[prv[i]];

        upN.assign(LOG, vector<int>(len, -1));
        upP.assign(LOG, vector<int>(len, -1));
        for (int i = 0; i < len; ++i) {
            upN[0][i] = nxt[i];
            upP[0][i] = prv[i];
        }
        for (int k = 1; k < LOG; ++k)
            for (int i = 0; i < len; ++i) {
                int j = upN[k - 1][i];
                upN[k][i] = (j == -1) ? -1 : upN[k - 1][j];
                j = upP[k - 1][i];
                upP[k][i] = (j == -1) ? -1 : upP[k - 1][j];
            }
    }
};
vector<Chain> chains;

// 上行：从 hi 往上走到 lo（同一重链）。处理“边 i -> i-1”，i ∈ [lo+1, hi]。
// M 为进入本段时已知的前缀最小值（本段下方所有城市的最小价格，可能 INF）。
// 返回本段代价，并把 M 更新为 min(M, 本段 [lo,hi] 的最小粮价)。
static inline ll upSeg(const Chain &c, int lo, int hi, ll &M) {
    if (lo >= hi) {
        if (lo == hi && (ll)c.wv[lo] < M) M = c.wv[lo];
        return 0;
    }
    ll cost = 0;
    int r;
    if ((ll)c.wv[hi] < M) {
        r = hi;
    } else {
        int i = hi;
        for (int k = LOG - 1; k >= 0; --k) {
            int j = c.upP[k][i];
            if (j != -1 && (ll)c.wv[j] >= M) i = j;
        }
        r = c.prv[i];
    }
    if (r == -1 || r <= lo) {                     // 段内(不含 lo)没有更小的记录点
        cost += M * (c.dv[hi] - c.dv[lo]);
        if ((ll)c.wv[lo] < M) M = c.wv[lo];
        return cost;
    }
    cost += M * (c.dv[hi] - c.dv[r]);             // i ∈ (r, hi]，系数 M
    int i = r;
    for (int k = LOG - 1; k >= 0; --k) {
        int j = c.upP[k][i];
        if (j != -1 && j > lo) i = j;
    }
    int rp = i;                                    // 记录链上最后一个位置 > lo 的点
    cost += c.Usum[r] - c.Usum[rp] + (ll)c.wv[rp] * (c.dv[rp] - c.dv[lo]);
    if ((ll)c.wv[lo] < M) M = c.wv[lo];
    if ((ll)c.wv[rp] < M) M = c.wv[rp];
    return cost;
}

// 下行：从 lo 往下走到 hi（同一重链）。处理“边 i -> i+1”，i ∈ [lo, hi-1]。
// 返回本段代价，并把 M 更新为 min(M, 本段 [lo,hi] 的最小粮价)。
static inline ll downSeg(const Chain &c, int lo, int hi, ll &M) {
    if (lo >= hi) {
        if (lo == hi && (ll)c.wv[lo] < M) M = c.wv[lo];
        return 0;
    }
    ll cost = 0;
    int r;
    if ((ll)c.wv[lo] < M) {
        r = lo;
    } else {
        int i = lo;
        for (int k = LOG - 1; k >= 0; --k) {
            int j = c.upN[k][i];
            if (j != -1 && (ll)c.wv[j] >= M) i = j;
        }
        r = c.nxt[i];
    }
    if (r == -1 || r > hi) {                      // 段内没有更小的记录点
        cost += M * (c.dv[hi] - c.dv[lo]);
        return cost;
    }
    cost += M * (c.dv[r] - c.dv[lo]);             // i ∈ [lo, r)，系数 M
    int i = r;
    for (int k = LOG - 1; k >= 0; --k) {
        int j = c.upN[k][i];
        if (j != -1 && j <= hi) i = j;
    }
    int rp = i;                                    // 记录链上最后一个位置 <= hi 的点
    cost += c.Dsum[r] - c.Dsum[rp] + (ll)c.wv[rp] * (c.dv[hi] - c.dv[rp]);
    if ((ll)c.wv[rp] < M) M = c.wv[rp];
    return cost;
}

static inline int headOf(int u) { return chainNodes[chainId[u]][0]; }

static inline int lcaOf(int a, int b) {
    while (chainId[a] != chainId[b]) {
        int ha = headOf(a), hb = headOf(b);
        if (dep_[ha] > dep_[hb]) a = par_[ha];
        else b = par_[hb];
    }
    return dep_[a] < dep_[b] ? a : b;
}

// 下行分段的缓存（每条查询的段数 O(log n)）
static int segCid[64], segLo[64], segHi[64];

static ll query(int s, int t) {
    if (s == t) return 0;
    int lca = lcaOf(s, t);

    ll cost = 0, M = INF;

    // ---- 上行：s -> lca（边起点 s .. lca 的儿子） ----
    int cur = s;
    while (chainId[cur] != chainId[lca]) {
        int cid = chainId[cur];
        int hi = posInChain[cur];                  // lo = 0（链顶）
        cost += upSeg(chains[cid], 0, hi, M);
        int h = chainNodes[cid][0];                // 链顶，经轻边跳到父节点
        cost += M * (dist_[h] - dist_[par_[h]]);
        cur = par_[h];
    }
    {
        int q = posInChain[lca];
        int hi = posInChain[cur];
        if (q < hi) cost += upSeg(chains[chainId[cur]], q, hi, M);
    }
    if ((ll)w[lca] < M) M = w[lca];                // 现在 M = min{w | 城市∈s..lca}
    ll W = M;

    // ---- 下行：lca -> t ----
    int cnt = 0;
    cur = t;
    while (chainId[cur] != chainId[lca]) {
        int cid = chainId[cur];
        segCid[cnt] = cid; segLo[cnt] = 0; segHi[cnt] = posInChain[cur];
        ++cnt;
        cur = par_[chainNodes[cid][0]];
    }
    segCid[cnt] = chainId[cur]; segLo[cnt] = posInChain[lca]; segHi[cnt] = posInChain[cur];
    ++cnt;
    for (int k = cnt - 1; k >= 0; --k) {
        int cid = segCid[k];
        if (k != cnt - 1) {                        // 跨界的轻边：父 -> 链顶
            int h = chainNodes[cid][segLo[k]];
            cost += W * (dist_[h] - dist_[par_[h]]);
        }
        cost += downSeg(chains[cid], segLo[k], segHi[k], W);
    }
    return cost;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if (!(cin >> n >> m)) return 0;
    for (int i = 1; i <= n; ++i) cin >> w[i];
    for (int i = 0; i < n - 1; ++i) {
        int u, v, e;
        cin >> u >> v >> e;
        adj[u].push_back({v, e});
        adj[v].push_back({u, e});
    }

    // 迭代 DFS：父子关系、深度、带权距离、前序
    vector<int> order;
    order.reserve(n);
    {
        vector<int> stk;
        stk.reserve(n);
        stk.push_back(1);
        par_[1] = 0; dep_[1] = 0; dist_[1] = 0;
        while (!stk.empty()) {
            int u = stk.back(); stk.pop_back();
            order.push_back(u);
            for (auto &e : adj[u]) {
                int v = e.first;
                if (v == par_[u]) continue;
                par_[v] = u;
                dep_[v] = dep_[u] + 1;
                dist_[v] = dist_[u] + e.second;
                stk.push_back(v);
            }
        }
    }
    // 子树大小、重儿子
    for (int i = 1; i <= n; ++i) sz_[i] = 1;
    for (int i = n - 1; i >= 0; --i) {
        int u = order[i], p = par_[u];
        if (p) {
            sz_[p] += sz_[u];
            if (heavy_[p] == 0 || sz_[u] > sz_[heavy_[p]]) heavy_[p] = u;
        }
    }
    // 重链划分
    chainNodes.reserve(n);
    for (int i = 0; i < n; ++i) {
        int u = order[i];
        if (u == 1 || heavy_[par_[u]] != u) {
            vector<int> ch;
            for (int x = u; x; x = heavy_[x]) {
                chainId[x] = (int)chainNodes.size();
                posInChain[x] = (int)ch.size();
                ch.push_back(x);
            }
            chainNodes.push_back(std::move(ch));
        }
    }
    chains.resize(chainNodes.size());
    for (size_t i = 0; i < chainNodes.size(); ++i) chains[i].build(chainNodes[i]);

    string out;
    out.reserve(20 * (size_t)m);
    char buf[24];
    for (int i = 0; i < m; ++i) {
        int s, t;
        cin >> s >> t;
        ll ans = query(s, t);
        int len = 0;
        if (ans == 0) buf[len++] = '0';
        while (ans > 0) { buf[len++] = char('0' + ans % 10); ans /= 10; }
        while (len > 0) out.push_back(buf[--len]);
        out.push_back('\n');
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
