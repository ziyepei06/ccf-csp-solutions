// 202012-5 星际旅行
// 查询答案 = (Σx)^2 + (Σy)^2 + (Σz)^2，即区间内所有装置向量之和的模平方。
// 加 / 乘 / 循环移位都是作用在向量上的线性变换，因此线段树每个节点只需维护
// 区间和向量 s，并带上懒标记 v -> k*R_r v + a。用离线坐标压缩把 n(≤1e9) 压到 O(m) 个基本区间。
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
static const ll MOD = 1000000007LL;

// ---------------- 快读 ----------------
static char ibuf[1 << 20];
static int ilen = 0, ipos = 0;
static inline int gc() {
    if (ipos == ilen) {
        ilen = (int)fread(ibuf, 1, sizeof(ibuf), stdin);
        ipos = 0;
        if (ilen <= 0) return -1;
    }
    return (unsigned char)ibuf[ipos++];
}
static inline ll ri() {
    int c = gc();
    while (c != -1 && (c < '0' || c > '9') && c != '-') c = gc();
    int sg = 1;
    if (c == '-') { sg = -1; c = gc(); }
    ll x = 0;
    while (c >= '0' && c <= '9') { x = x * 10 + (c - '0'); c = gc(); }
    return x * sg;
}

// ---------------- 线段树 ----------------
struct Node {
    ll s[3];    // 区间内所有装置的坐标和
    ll lk;      // 懒标记：缩放 k
    int lr;     // 懒标记：循环移位 v -> (y,z,x) 的次数 (0/1/2)
    ll la[3];   // 懒标记：平移 a
    ll cnt;     // 该节点对应的动力装置个数
};

static vector<Node> tr;
static vector<ll> pts;   // 压缩坐标点
static int K;            // 基本区间个数

// 把变换 v -> k * R_r v + a 作用到节点 o 的聚合量与懒标记上
static inline void applyTag(int o, ll k, int r, const ll *a) {
    Node &nd = tr[o];
    ll ss[3];
    for (int i = 0; i < 3; i++) ss[i] = nd.s[(i + r) % 3] * k % MOD;
    ll L = nd.cnt % MOD;
    // 标记复合：P 作用在旧标记 C 之后 => (k*Ck, r+Cr, k*R_r(Ca)+a)
    ll na[3];
    for (int i = 0; i < 3; i++) na[i] = (k * nd.la[(i + r) % 3] + a[i]) % MOD;
    nd.lk = nd.lk * k % MOD;
    nd.lr = (nd.lr + r) % 3;
    for (int i = 0; i < 3; i++) nd.la[i] = na[i];
    // 聚合量更新
    for (int i = 0; i < 3; i++) nd.s[i] = (ss[i] + L * a[i]) % MOD;
}

static void build(int o, int l, int r) {
    Node &nd = tr[o];
    nd.cnt = pts[r + 1] - pts[l];
    nd.s[0] = nd.s[1] = nd.s[2] = 0;
    nd.lk = 1; nd.lr = 0;
    nd.la[0] = nd.la[1] = nd.la[2] = 0;
    if (l == r) return;
    int mid = (l + r) >> 1;
    build(o << 1, l, mid);
    build(o << 1 | 1, mid + 1, r);
}

static inline void push(int o) {
    Node &nd = tr[o];
    if (nd.lk == 1 && nd.lr == 0 && nd.la[0] == 0 && nd.la[1] == 0 && nd.la[2] == 0)
        return;
    applyTag(o << 1, nd.lk, nd.lr, nd.la);
    applyTag(o << 1 | 1, nd.lk, nd.lr, nd.la);
    nd.lk = 1; nd.lr = 0;
    nd.la[0] = nd.la[1] = nd.la[2] = 0;
}

static inline void pull(int o) {
    Node &nd = tr[o];
    const Node &a = tr[o << 1], &b = tr[o << 1 | 1];
    for (int i = 0; i < 3; i++) nd.s[i] = (a.s[i] + b.s[i]) % MOD;
}

static void update(int o, int l, int r, int ql, int qr, ll k, int r_, const ll *a) {
    if (ql <= l && r <= qr) { applyTag(o, k, r_, a); return; }
    push(o);
    int mid = (l + r) >> 1;
    if (ql <= mid) update(o << 1, l, mid, ql, qr, k, r_, a);
    if (qr > mid) update(o << 1 | 1, mid + 1, r, ql, qr, k, r_, a);
    pull(o);
}

static void query(int o, int l, int r, int ql, int qr, ll *out) {
    if (ql <= l && r <= qr) {
        const Node &nd = tr[o];
        for (int i = 0; i < 3; i++) out[i] = (out[i] + nd.s[i]) % MOD;
        return;
    }
    push(o);
    int mid = (l + r) >> 1;
    if (ql <= mid) query(o << 1, l, mid, ql, qr, out);
    if (qr > mid) query(o << 1 | 1, mid + 1, r, ql, qr, out);
}

int main() {
    ll n = ri();
    int m = (int)ri();

    vector<int> typ(m);
    vector<ll> L(m), R(m), A(m), B(m), C(m);
    pts.reserve((size_t)2 * m + 4);
    pts.push_back(1);
    pts.push_back(n + 1);
    for (int i = 0; i < m; i++) {
        typ[i] = (int)ri();
        L[i] = ri();
        R[i] = ri();
        if (typ[i] == 1) { A[i] = ri(); B[i] = ri(); C[i] = ri(); }
        else if (typ[i] == 2) { A[i] = ri(); }
        pts.push_back(L[i]);
        pts.push_back(R[i] + 1);
    }
    sort(pts.begin(), pts.end());
    pts.erase(unique(pts.begin(), pts.end()), pts.end());
    K = (int)pts.size() - 1;
    if (K <= 0) { return 0; }

    tr.assign((size_t)4 * K + 10, Node());
    build(1, 0, K - 1);

    string out;
    out.reserve((size_t)m * 12);
    for (int i = 0; i < m; i++) {
        int gl = (int)(lower_bound(pts.begin(), pts.end(), L[i]) - pts.begin());
        int gr = (int)(lower_bound(pts.begin(), pts.end(), R[i] + 1) - pts.begin()) - 1;
        if (gl > gr) continue;
        ll a[3] = {0, 0, 0};
        if (typ[i] == 1) {
            a[0] = A[i] % MOD; a[1] = B[i] % MOD; a[2] = C[i] % MOD;
            update(1, 0, K - 1, gl, gr, 1, 0, a);
        } else if (typ[i] == 2) {
            update(1, 0, K - 1, gl, gr, A[i] % MOD, 0, a);
        } else if (typ[i] == 3) {
            update(1, 0, K - 1, gl, gr, 1, 1, a);
        } else {
            ll s[3] = {0, 0, 0};
            query(1, 0, K - 1, gl, gr, s);
            ll ans = (s[0] * s[0] % MOD + s[1] * s[1] % MOD + s[2] * s[2] % MOD) % MOD;
            out += to_string(ans);
            out += '\n';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
