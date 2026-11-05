// 201712-5 商路
//
// 记 D[u] 为 u 到根的距离，best[u] 为从城市 u 出发的商路的最大总价值
// （允许 k=1，价值 0）。由于“u 出发的一段走到 w 之后剩下的部分”可以独立
// 从 w 重新决策，故
//   best[u] = max(0, max_{w 是 u 的真后代} v_u - (f_u - d(u,w))^2 + best[w])
// 答案即 sum_u best[u]。
//
// 展开这一段：d = D[w] - D[u]，
//   v_u - (f_u-d)^2 + best[w]
// = K_u + [ 2*D[w] * x_u + (best[w] - D[w]^2) ],  x_u = f_u + D[u],
//   K_u = v_u - f_u^2 - 2*f_u*D[u] - D[u]^2.
// 把每个 w 看成直线 y = 2*D[w]*x + (best[w]-D[w]^2)，则 u 的 dp 值等价于
// “在子树（真后代）内所有直线上、于 x_u 处取最大值”——李超树 + 子树合并。
// 因保证 par[i] < i，按 i = n..1 倒序处理天然满足子先父后；把子树的李超树
// 合并起来（各子树的结点直接复用），再插入自己的直线即可。
//
// 复杂度：时间 O(n log n log n)（合并摊还），空间 O(n log n)。
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef __int128 lll;

static const int MAXN = 100005;
static const lll NEG = -(((lll)1) << 120);
static const lll MOD = 1000000000000000000LL;  // 10^18

int n;
ll par[MAXN], slen[MAXN], vval[MAXN], fval[MAXN], dep[MAXN];
int head[MAXN], nxt[MAXN];       // 子结点邻接表（父编号 < 子编号）
lll bestv[MAXN];
int rootOf[MAXN];

// ---- 直线：编号 0 为 -inf，1..n 对应各城市 ----
vector<ll> lsl;   // 斜率 2*D[w]
vector<lll> lic;  // 截距 best[w] - D[w]^2

static inline lll val(int id, ll x) { return (lll)lsl[id] * x + lic[id]; }

// ---- 离散化后的李超树（下标 0..m-1，xs 为对应 x 值）----
int m;
ll xs[MAXN];
vector<int> lch, rch, nl;

static inline int newNode(int line) {
    lch.push_back(0);
    rch.push_back(0);
    nl.push_back(line);
    return (int)nl.size() - 1;
}

// 把直线 id 插入以 p 为根、区间 [l,r] 的李超树（p 可为 0）
static void add(int &p, int l, int r, int id) {
    if (!p) { p = newNode(id); return; }
    int mid = (l + r) >> 1;
    if (val(id, xs[mid]) > val(nl[p], xs[mid])) swap(nl[p], id);
    if (l == r) return;
    if (val(id, xs[l]) > val(nl[p], xs[l])) {
        if (!lch[p]) { lch[p] = newNode(id); return; }
        add(lch[p], l, mid, id);
    } else if (val(id, xs[r]) > val(nl[p], xs[r])) {
        if (!rch[p]) { rch[p] = newNode(id); return; }
        add(rch[p], mid + 1, r, id);
    }
}

// 合并李超树 b 到 a，返回合并后的根
static int mergeTree(int a, int b, int l, int r) {
    if (!a) return b;
    if (!b) return a;
    add(a, l, r, nl[b]);
    if (l < r) {
        int mid = (l + r) >> 1;
        lch[a] = mergeTree(lch[a], lch[b], l, mid);
        rch[a] = mergeTree(rch[a], rch[b], mid + 1, r);
    }
    return a;
}

// 查询下标 xi 处的最大值
static lll query(int p, int l, int r, int xi) {
    if (!p) return NEG;
    lll res = val(nl[p], xs[xi]);
    while (l < r) {
        int mid = (l + r) >> 1;
        if (xi <= mid) { p = lch[p]; r = mid; }
        else { p = rch[p]; l = mid + 1; }
        if (!p) break;
        lll t = val(nl[p], xs[xi]);
        if (t > res) res = t;
    }
    return res;
}

static void printMod18(lll x) {
    lll r = x % MOD;
    if (r < 0) r += MOD;
    if (r == 0) { printf("0\n"); return; }
    char buf[40];
    int len = 0;
    while (r > 0) { buf[len++] = char('0' + (int)(r % 10)); r /= 10; }
    while (len--) putchar(buf[len]);
    putchar('\n');
}

int main() {
    int T;
    if (scanf("%d", &T) != 1) return 0;
    lsl.reserve(MAXN);
    lic.reserve(MAXN);
    lsl.push_back(0); lic.push_back(NEG);  // 直线 0 = 负无穷
    lch.reserve(1 << 20);
    rch.reserve(1 << 20);
    nl.reserve(1 << 20);

    while (T--) {
        if (scanf("%d", &n) != 1) return 0;
        lsl.resize(n + 1);
        lic.resize(n + 1);
        for (int i = 1; i <= n; i++) {
            ll u, s, v, f;
            scanf("%lld %lld %lld %lld", &u, &s, &v, &f);
            par[i] = u; slen[i] = s; vval[i] = v; fval[i] = f;
            head[i] = 0;
        }
        for (int i = 2; i <= n; i++) { nxt[i] = head[par[i]]; head[par[i]] = i; }

        dep[1] = 0;
        for (int i = 2; i <= n; i++) dep[i] = dep[par[i]] + slen[i];

        // 离散化所有查询点 x_u = f_u + D[u]
        for (int i = 1; i <= n; i++) xs[i - 1] = fval[i] + dep[i];
        sort(xs, xs + n);
        m = int(unique(xs, xs + n) - xs);

        // 重置李超树（结点 0 为哨兵）
        lch.clear(); rch.clear(); nl.clear();
        lch.push_back(0); rch.push_back(0); nl.push_back(0);

        for (int i = 1; i <= n; i++) rootOf[i] = 0;
        lll ans = 0;

        for (int i = n; i >= 1; i--) {
            int rt = 0;
            for (int c = head[i]; c; c = nxt[c]) rt = mergeTree(rt, rootOf[c], 0, m - 1);

            int xi = int(lower_bound(xs, xs + m, fval[i] + dep[i]) - xs);
            lll q = query(rt, 0, m - 1, xi);
            lll K = (lll)vval[i] - (lll)fval[i] * fval[i]
                    - 2 * (lll)fval[i] * dep[i] - (lll)dep[i] * dep[i];
            lll cur = K + q;
            if (cur < 0) cur = 0;
            bestv[i] = cur;
            ans += cur;

            lsl[i] = 2 * dep[i];
            lic[i] = bestv[i] - (lll)dep[i] * dep[i];
            add(rt, 0, m - 1, i);
            rootOf[i] = rt;
        }

        printMod18(ans);
    }
    return 0;
}
