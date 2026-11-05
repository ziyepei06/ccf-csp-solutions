// 202309-5 阻击
// 树上最大路径和，边权 c_e = b_e - w_e，单点（边权）修改，输出每次修改后的最大路径和。
// 动态 DP + 重链剖分 + 每条重链一棵（幂次大小的）线段树维护 max-plus 矩阵复合。
//
// 记边权挂在深度较大的端点上：c[u] = b(edge u-par[u]) - w(edge u-par[u])。
// g[u]  = max(0, max_{v 是 u 的孩子} (c[v] + g[v]))          （u 向下的最大权和路径，允许空）
// hs[u] = max(0, L1) 其中 L1 是轻儿子中 c[v]+g[v] 的最大值
// best[u] = max(0, best[轻儿子], best[重儿子], c[hc]+g[hc], L1, c[hc]+g[hc]+L1, L1+L2)
//
// 把 (x,y) = (g[hc], best[hc]) 送进 u 的矩阵，得 (g[u], best[u])：
//   g    = max(p + x, q)
//   best = max(r + x, s + y, t)
// 对应 5 元组 (p,q,r,s,t)，可复合：
//   p = pB + pA
//   q = max(pB + qA, qB)
//   r = max(rB + pA, sB + rA)
//   s = sB + sA      （恒为 0）
//   t = max(rB + qA, sB + tA, tB)
// 链底节点把输入视为空路径 (0,0)，其重边权取 -BIG，于是重边相关项自然被淘汰。

#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static const ll BIG = 100000000000000000LL; // 1e17，作为"负无穷"

struct Mat {
    ll p, q, r, s, t;
};

// 先作用 A，再作用 B
static inline Mat comp(const Mat &A, const Mat &B) {
    Mat C;
    C.p = B.p + A.p;
    C.q = max(B.p + A.q, B.q);
    C.r = max(B.r + A.p, B.s + A.r);
    C.s = B.s + A.s;
    C.t = max(B.r + A.q, max(B.s + A.t, B.t));
    return C;
}

static const Mat IDMAT = {0, -BIG, -BIG, 0, -BIG};

// ---------- 快速输入 ----------
static char inbuf[1 << 23];
static int inlen = 0, inpos = 0;
static inline int gc() {
    if (inpos == inlen) {
        inlen = (int)fread(inbuf, 1, sizeof(inbuf), stdin);
        inpos = 0;
        if (inlen <= 0) return -1;
    }
    return inbuf[inpos++];
}
static inline ll rd() {
    int c = gc();
    while (c != -1 && (c < '0' || c > '9')) c = gc();
    ll x = 0;
    while (c >= '0' && c <= '9') {
        x = x * 10 + (c - '0');
        c = gc();
    }
    return x;
}

int main() {
    int n = (int)rd();
    int m = (int)rd();

    vector<int> eu(n), ev(n);
    vector<ll> ww(n), bb(n);
    vector<vector<int>> adj(n + 1);
    for (int i = 1; i < n; i++) {
        eu[i] = (int)rd();
        ev[i] = (int)rd();
        ww[i] = rd();
        bb[i] = rd();
        adj[eu[i]].push_back(i);
        adj[ev[i]].push_back(i);
    }

    // ---------- 建根、深度、子树大小、重儿子 ----------
    vector<int> par(n + 1, 0), dep(n + 1, 0), order;
    order.reserve(n);
    {
        vector<int> stk;
        stk.reserve(n);
        stk.push_back(1);
        par[1] = 0;
        while (!stk.empty()) {
            int u = stk.back();
            stk.pop_back();
            order.push_back(u);
            for (int e : adj[u]) {
                int v = eu[e] ^ ev[e] ^ u;
                if (v == par[u]) continue;
                par[v] = u;
                dep[v] = dep[u] + 1;
                stk.push_back(v);
            }
        }
    }

    vector<int> sz(n + 1, 1), hvy(n + 1, 0);
    for (int i = n - 1; i >= 0; i--) {
        int u = order[i];
        int bsz = 0;
        for (int e : adj[u]) {
            int v = eu[e] ^ ev[e] ^ u;
            if (par[v] == u) {
                sz[u] += sz[v];
                if (sz[v] > bsz) {
                    bsz = sz[v];
                    hvy[u] = v;
                }
            }
        }
    }

    // ---------- 重链 ----------
    vector<int> chd(n + 1, 0), cpos(n + 1, 0);
    vector<int> heads;
    heads.reserve(n);
    for (int u = 1; u <= n; u++)
        if (par[u] == 0 || hvy[par[u]] != u) heads.push_back(u);

    vector<vector<int>> chainNodes;
    chainNodes.reserve(heads.size());
    for (size_t i = 0; i < heads.size(); i++) {
        vector<int> nodes;
        int u = heads[i];
        while (u) {
            nodes.push_back(u);
            chd[u] = heads[i];
            cpos[u] = (int)nodes.size() - 1;
            u = hvy[u];
        }
        chainNodes.push_back(std::move(nodes));
    }

    vector<int> cbase(n + 1, 0), cpow(n + 1, 0);
    int totalSlots = 0;
    for (size_t i = 0; i < chainNodes.size(); i++) {
        int h = heads[i];
        int ln = (int)chainNodes[i].size();
        int P = 1;
        while (P < ln) P <<= 1;
        cpow[h] = P;
        cbase[h] = totalSlots;
        totalSlots += 2 * P;
    }
    vector<Mat> T(totalSlots + 2, IDMAT);

    // 边权（挂在子端点）
    vector<ll> cw(n + 1, 0);
    for (int i = 1; i < n; i++) {
        int u = (par[eu[i]] == ev[i]) ? eu[i] : ev[i];
        cw[u] = bb[i] - ww[i];
    }

    vector<ll> hw(n + 1, -BIG);     // 重边权
    vector<ll> L1(n + 1, -BIG), L2(n + 1, -BIG), Lb(n + 1, -BIG);
    vector<ll> gv(n + 1, 0), bv(n + 1, 0), hval(n + 1, 0);
    vector<multiset<pair<ll, int>>> MH(n + 1); // 轻儿子的 c+g
    vector<multiset<ll>> MB(n + 1);            // 轻儿子的 best

    auto refreshAgg = [&](int u) {
        if (MH[u].empty()) {
            L1[u] = L2[u] = -BIG;
        } else {
            auto it = MH[u].rbegin();
            L1[u] = it->first;
            ++it;
            L2[u] = (it != MH[u].rend()) ? it->first : -BIG;
        }
        Lb[u] = MB[u].empty() ? -BIG : *MB[u].rbegin();
    };

    auto leafOf = [&](int u) -> Mat {
        ll m = max(L1[u], 0LL);
        Mat A;
        A.p = hw[u];
        A.q = m;
        A.r = hw[u] + m;
        A.s = 0;
        ll t = max(max(Lb[u], L1[u]), max(L1[u] + L2[u], 0LL));
        A.t = t;
        return A;
    };

    auto segPut = [&](int h, int pos, const Mat &val) {
        int base = cbase[h], P = cpow[h];
        int k = P + pos;
        T[base + k] = val;
        for (k >>= 1; k >= 1; k >>= 1) T[base + k] = comp(T[base + 2 * k + 1], T[base + 2 * k]);
    };

    // ---------- 按链头深度从深到浅初始化 ----------
    {
        vector<int> hidx(heads.size());
        for (size_t i = 0; i < heads.size(); i++) hidx[i] = (int)i;
        sort(hidx.begin(), hidx.end(), [&](int a, int b) { return dep[heads[a]] > dep[heads[b]]; });

        for (size_t oi = 0; oi < hidx.size(); oi++) {
            int h = heads[hidx[oi]];
            vector<int> &nodes = chainNodes[hidx[oi]];
            int ln = (int)nodes.size();
            int base = cbase[h], P = cpow[h];
            for (int pi = 0; pi < ln; pi++) {
                int u = nodes[pi];
                for (int e : adj[u]) {
                    int v = eu[e] ^ ev[e] ^ u;
                    if (par[v] == u && v != hvy[u]) {
                        ll hh = cw[v] + gv[v];
                        hval[v] = hh;
                        MH[u].insert(make_pair(hh, v));
                        MB[u].insert(bv[v]);
                    }
                }
                refreshAgg(u);
                hw[u] = hvy[u] ? cw[hvy[u]] : -BIG;
                T[base + P + pi] = leafOf(u);
            }
            for (int pi = ln; pi < P; pi++) T[base + P + pi] = IDMAT;
            for (int k = P - 1; k >= 1; k--) T[base + k] = comp(T[base + 2 * k + 1], T[base + 2 * k]);
            const Mat &R = T[base + 1];
            gv[h] = max(R.p, R.q);
            bv[h] = max(R.r, max(R.t, 0LL));
            hval[h] = cw[h] + gv[h];
        }
    }

    string out;
    out.reserve((size_t)(m + 1) * 20);
    out += to_string(bv[1]);
    out += '\n';

    for (int qi = 0; qi < m; qi++) {
        int x = (int)rd();
        ll y = rd();
        int u = (par[eu[x]] == ev[x]) ? eu[x] : ev[x];
        cw[u] = bb[x] - y;
        int p = par[u];
        if (hvy[p] != u) {
            // 轻儿子：h(u) = c[u] + g[u] 变化，best[u] 不变
            ll oldh = hval[u];
            ll newh = cw[u] + gv[u];
            MH[p].erase(MH[p].find(make_pair(oldh, u)));
            MH[p].insert(make_pair(newh, u));
            hval[u] = newh;
            refreshAgg(p);
        } else {
            hw[p] = cw[u];
        }
        int cur = p;
        while (true) {
            int h = chd[cur];
            segPut(h, cpos[cur], leafOf(cur));
            const Mat &R = T[cbase[h] + 1];
            ll ng = max(R.p, R.q);
            ll nb = max(R.r, max(R.t, 0LL));
            if (ng == gv[h] && nb == bv[h]) break;
            ll oldh = hval[h], oldb = bv[h];
            gv[h] = ng;
            bv[h] = nb;
            hval[h] = cw[h] + ng;
            int P = par[h];
            if (!P) break;
            MH[P].erase(MH[P].find(make_pair(oldh, h)));
            MH[P].insert(make_pair(hval[h], h));
            if (oldb != nb) {
                MB[P].erase(MB[P].find(oldb));
                MB[P].insert(nb);
            }
            refreshAgg(P);
            cur = P;
        }
        out += to_string(bv[1]);
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
