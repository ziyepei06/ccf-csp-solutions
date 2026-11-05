// CCF CSP 202206-4 光线追踪
//
// 关键变换：令 u = x+y, v = x-y。
//   水平/竖直光线 -> (u,v) 平面中沿对角线方向 (du,dv), 且 |du|=|dv|=1；
//   斜率 -1 的反射面 (x+y=c) -> 竖直线 u=c, 记录 v 区间；
//   斜率 +1 的反射面 (y-x=c) -> 水平线 v=x-y=-c, 记录 u 区间。
//   反射 = 翻转 du 或 dv 之一。
// 光线当前所在对角线为 v = s*u + alpha (s = du*dv = ±1)。
// 对反射面 u=U (v∈(vlo,vhi))，被该对角线打到的充要条件是
//   vlo < alpha + s*U < vhi   <=>   alpha ∈ [vlo-s*U+1, vhi-s*U-1]。
// 对反射面 v=V (u∈(ulo,uhi)) 同理 alpha ∈ 区间。
// 由于端点坐标是整数，命中点 (u,v) 均为整数，故可直接枚举整数 alpha，
// 把 "每个反射面对每个会打到它的 alpha 贡献一个 (alpha, key) 条目"，
// 条目总数 = O(Σ|x1-x2|)。同一个 (s,alpha,种类) 组内 key 互不相同。
// 每组：有序 key 数组 + 树状数组(存活标记)，查询方向最近存活条目 O(log)。
// 强度每反射一次乘 a ≤ 0.8，I ≤ 1e9，故一次询问最多约 93 次反射。
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;
typedef long long ll;
typedef unsigned long long ull;

static const ll INFLL = (ll)4e18;
static const ll ALPHA_OFF = 10000000000LL;

static inline ull makeGk(ll alpha, int sbit, int kind) {
    return (((ull)(alpha + ALPHA_OFF)) << 2) | ((ull)sbit << 1) | (ull)kind;
}

/* ------------------------------- fast io ------------------------------- */
namespace io {
static const int BUFSZ = 1 << 16;
static char buf[BUFSZ];
static int len = 0, pos = 0;
inline int gc() {
    if (pos >= len) {
        len = (int)fread(buf, 1, BUFSZ, stdin);
        pos = 0;
        if (len <= 0) return -1;
    }
    return buf[pos++];
}
inline ll readInt() {
    int c = gc();
    while (c != -1 && c != '-' && (c < '0' || c > '9')) c = gc();
    if (c == -1) return 0;
    bool neg = false;
    if (c == '-') { neg = true; c = gc(); }
    ll v = 0;
    while (c >= '0' && c <= '9') { v = v * 10 + (c - '0'); c = gc(); }
    return neg ? -v : v;
}
static const double POW10[7] = {1.0, 10.0, 100.0, 1000.0, 10000.0, 100000.0, 1000000.0};
inline double readDouble() {
    int c = gc();
    while (c != -1 && c != '-' && c != '.' && (c < '0' || c > '9')) c = gc();
    if (c == -1) return 0.0;
    bool neg = false;
    if (c == '-') { neg = true; c = gc(); }
    ll ip = 0;
    while (c >= '0' && c <= '9') { ip = ip * 10 + (c - '0'); c = gc(); }
    ll fp = 0;
    int k = 0;
    if (c == '.') {
        c = gc();
        while (c >= '0' && c <= '9') { fp = fp * 10 + (c - '0'); k++; c = gc(); }
    }
    double v = (double)ip;
    if (k > 0) v += (double)fp / POW10[k <= 6 ? k : 6];
    return neg ? -v : v;
}
} // namespace io

/* ----------------------------- global tables ---------------------------- */
static vector<int> gStart;   // 每组在条目数组中的起点
static vector<int> gSize;    // 每组元素个数
static vector<int> gAlive;   // 每组当前存活条目数
static vector<int> entKey;   // 条目 key（A 面为 u，B 面为 v）
static vector<int> entMir;   // 条目所属反射面
static vector<int> entGid;   // 条目所属组
static vector<int> fen;      // 每组内嵌树状数组，第 i 个元素在 fen[gs+i-1]
static vector<ull> hKey;     // 哈希表
static vector<int> hVal;
static int hashShift = 60, hashMask = 0;

static inline int fenSum(int gs, int i) { // 组内前 i 个 (local 1..i)
    int s = 0;
    for (; i > 0; i -= i & (-i)) s += fen[gs + i - 1];
    return s;
}
static inline void fenAdd(int gs, int g, int i, int delta) {
    for (; i <= g; i += i & (-i)) fen[gs + i - 1] += delta;
}
// 组内第 k 个存活元素的 local 下标 (1-based)
static inline int fenKth(int gs, int g, int k) {
    int idx = 0;
    int step = 1 << (31 - __builtin_clz((unsigned)g));
    for (; step; step >>= 1) {
        int nxt = idx + step;
        if (nxt <= g && fen[gs + nxt - 1] < k) { idx = nxt; k -= fen[gs + nxt - 1]; }
    }
    return idx + 1;
}
static inline int hashLookup(ull gk) {
    ull h = gk * 0x9E3779B97F4A7C15ULL;
    int idx = (int)(h >> hashShift);
    while (true) {
        int g = hVal[idx];
        if (g < 0) return -1;
        if (hKey[idx] == gk) return g;
        idx = (idx + 1) & hashMask;
    }
}
// 在组 gid 中沿 dir 方向找最近的存活条目，返回条目位置；没有则 -1
static inline int searchDir(int gid, ll key0, int dir) {
    int gs = gStart[gid], g = gSize[gid];
    int p;
    if (dir > 0) {
        p = (int)(upper_bound(entKey.begin() + gs, entKey.begin() + gs + g, key0,
                              [](ll vv, int e) { return vv < (ll)e; }) - entKey.begin());
        if (p >= gs + g) return -1;
        int cnt = fenSum(gs, p - gs);
        if (cnt >= gAlive[gid]) return -1;
        int kth = fenKth(gs, g, cnt + 1);
        return gs + kth - 1;
    } else {
        p = (int)(lower_bound(entKey.begin() + gs, entKey.begin() + gs + g, key0,
                              [](int e, ll vv) { return (ll)e < vv; }) - entKey.begin());
        int hi = p - 1;
        if (hi < gs) return -1;
        int cnt = fenSum(gs, hi - gs + 1);
        if (cnt == 0) return -1;
        int kth = fenKth(gs, g, cnt);
        return gs + kth - 1;
    }
}

/* --------------------------------- main -------------------------------- */
struct Op {
    int type;
    int mi;
    ll x, y, t;
    int d;
    double I;
};
struct Raw {
    ull gk;
    int key;
    int mir;
};

int main() {
    ll m = io::readInt();
    if (m <= 0) return 0;

    vector<Op> ops((size_t)m);
    vector<double> mirA;
    mirA.reserve((size_t)m);
    vector<int> mirEntCnt;
    mirEntCnt.reserve((size_t)m);
    vector<int> opToMir((size_t)m + 1, -1);
    vector<Raw> raw;
    raw.reserve(1 << 21);

    for (ll i = 0; i < m; i++) {
        ll tp = io::readInt();
        ops[(size_t)i].type = (int)tp;
        if (tp == 1) {
            ll x1 = io::readInt(), y1 = io::readInt();
            ll x2 = io::readInt(), y2 = io::readInt();
            double a = io::readDouble();
            int mi = (int)mirA.size();
            mirA.push_back(a);
            opToMir[(size_t)i + 1] = mi;
            ops[(size_t)i].mi = mi;
            size_t before = raw.size();
            if (x1 + y1 == x2 + y2) { // 斜率 -1：u = x+y 恒定，竖直线
                ll u = x1 + y1;
                ll v1 = x1 - y1, v2 = x2 - y2;
                ll vlo = min(v1, v2), vhi = max(v1, v2);
                for (int ss = 1; ss >= -1; ss -= 2) {
                    ll lo = vlo - (ll)ss * u + 1;
                    ll hi = vhi - (ll)ss * u - 1;
                    int sbit = (ss == 1) ? 0 : 1;
                    for (ll al = lo; al <= hi; al++)
                        raw.push_back({makeGk(al, sbit, 0), (int)u, mi});
                }
            } else { // 斜率 +1：v = x-y 恒定，水平线
                ll V = x1 - y1;
                ll u1 = x1 + y1, u2 = x2 + y2;
                ll ulo = min(u1, u2), uhi = max(u1, u2);
                {
                    ll lo = V - uhi + 1, hi = V - ulo - 1;
                    for (ll al = lo; al <= hi; al++)
                        raw.push_back({makeGk(al, 0, 1), (int)V, mi});
                }
                {
                    ll lo = ulo + V + 1, hi = uhi + V - 1;
                    for (ll al = lo; al <= hi; al++)
                        raw.push_back({makeGk(al, 1, 1), (int)V, mi});
                }
            }
            mirEntCnt.push_back((int)(raw.size() - before));
        } else if (tp == 2) {
            ll k = io::readInt();
            ops[(size_t)i].mi = opToMir[(size_t)k];
        } else {
            ll x = io::readInt(), y = io::readInt(), d = io::readInt();
            double I = io::readDouble();
            ll t = io::readInt();
            ops[(size_t)i].x = x;
            ops[(size_t)i].y = y;
            ops[(size_t)i].d = (int)d;
            ops[(size_t)i].I = I;
            ops[(size_t)i].t = t;
        }
    }

    sort(raw.begin(), raw.end(), [](const Raw &a, const Raw &b) {
        return a.gk != b.gk ? (a.gk < b.gk) : (a.key < b.key);
    });

    int E = (int)raw.size();
    entKey.resize((size_t)E);
    entMir.resize((size_t)E);
    entGid.resize((size_t)E);
    {
        vector<ull> gGk;
        for (int i = 0; i < E; i++) {
            if (i == 0 || raw[(size_t)i].gk != raw[(size_t)i - 1].gk) {
                gStart.push_back(i);
                gSize.push_back(0);
                gGk.push_back(raw[(size_t)i].gk);
            }
            gSize.back()++;
            entKey[(size_t)i] = raw[(size_t)i].key;
            entMir[(size_t)i] = raw[(size_t)i].mir;
            entGid[(size_t)i] = (int)gStart.size() - 1;
        }
        vector<Raw>().swap(raw);

        int numG = (int)gStart.size();
        gAlive.assign((size_t)numG, 0);
        fen.assign((size_t)E, 0);

        int hashCap = 2;
        while (hashCap < (numG + 1) * 2) hashCap <<= 1;
        hashMask = hashCap - 1;
        hashShift = 64 - __builtin_ctz((unsigned)hashCap);
        hKey.assign((size_t)hashCap, 0);
        hVal.assign((size_t)hashCap, -1);
        for (int g = 0; g < numG; g++) {
            ull gk = gGk[(size_t)g];
            ull h = gk * 0x9E3779B97F4A7C15ULL;
            int idx = (int)(h >> hashShift);
            while (hVal[(size_t)idx] != -1) idx = (idx + 1) & hashMask;
            hKey[(size_t)idx] = gk;
            hVal[(size_t)idx] = g;
        }
    }

    // 每个反射面的条目位置（CSR）
    int nM = (int)mirA.size();
    vector<int> mirPosStart((size_t)nM + 1, 0);
    for (int i = 0; i < nM; i++)
        mirPosStart[(size_t)i + 1] = mirPosStart[(size_t)i] + mirEntCnt[(size_t)i];
    vector<int> mirPos((size_t)mirPosStart[(size_t)nM]);
    {
        vector<int> cur(mirPosStart.begin(), mirPosStart.end() - 1);
        for (int i = 0; i < E; i++) mirPos[(size_t)cur[(size_t)entMir[(size_t)i]]++] = i;
    }
    for (ll i = 0; i < m; i++) {
        Op &op = ops[(size_t)i];
        if (op.type == 1) {
            for (int j = mirPosStart[(size_t)op.mi]; j < mirPosStart[(size_t)op.mi + 1]; j++) {
                int p = mirPos[(size_t)j];
                int gid = entGid[(size_t)p];
                fenAdd(gStart[(size_t)gid], gSize[(size_t)gid], p - gStart[(size_t)gid] + 1, 1);
                gAlive[(size_t)gid]++;
            }
        } else if (op.type == 2) {
            for (int j = mirPosStart[(size_t)op.mi]; j < mirPosStart[(size_t)op.mi + 1]; j++) {
                int p = mirPos[(size_t)j];
                int gid = entGid[(size_t)p];
                fenAdd(gStart[(size_t)gid], gSize[(size_t)gid], p - gStart[(size_t)gid] + 1, -1);
                gAlive[(size_t)gid]--;
            }
        } else {
            ll x = op.x, y = op.y;
            int d = op.d;
            double I = op.I;
            ll t = op.t;
            if (!(I >= 1.0)) { fputs("0 0 0\n", stdout); continue; }
            ll u = x + y, v = x - y;
            int du, dv;
            if (d == 0) { du = 1; dv = 1; }
            else if (d == 1) { du = 1; dv = -1; }
            else if (d == 2) { du = -1; dv = -1; }
            else { du = -1; dv = 1; }
            bool dead = false;
            for (int guard = 0; guard < 400; guard++) {
                int s = du * dv;
                int sbit = (s == 1) ? 0 : 1;
                ll alpha = v - (ll)s * u;
                int posA = -1, posB = -1;
                {
                    int gid = hashLookup(makeGk(alpha, sbit, 0));
                    if (gid >= 0) posA = searchDir(gid, u, du);
                }
                {
                    int gid = hashLookup(makeGk(alpha, sbit, 1));
                    if (gid >= 0) posB = searchDir(gid, v, dv);
                }
                ll tA = (posA >= 0) ? llabs((ll)entKey[(size_t)posA] - u) : INFLL;
                ll tB = (posB >= 0) ? llabs((ll)entKey[(size_t)posB] - v) : INFLL;
                if (tA == INFLL && tB == INFLL) {
                    u += (ll)du * t;
                    v += (ll)dv * t;
                    break;
                }
                if (tA <= tB) {
                    if (tA > t) {
                        u += (ll)du * t;
                        v += (ll)dv * t;
                        break;
                    }
                    ll U = entKey[(size_t)posA];
                    v = alpha + (ll)s * U;
                    u = U;
                    du = -du;
                    t -= tA;
                    I *= mirA[(size_t)entMir[(size_t)posA]];
                    if (I < 1.0) { dead = true; break; }
                } else {
                    if (tB > t) {
                        u += (ll)du * t;
                        v += (ll)dv * t;
                        break;
                    }
                    ll V = entKey[(size_t)posB];
                    u = (ll)s * (V - alpha);
                    v = V;
                    dv = -dv;
                    t -= tB;
                    I *= mirA[(size_t)entMir[(size_t)posB]];
                    if (I < 1.0) { dead = true; break; }
                }
                if (t == 0) break;
            }
            if (dead) fputs("0 0 0\n", stdout);
            else printf("%lld %lld %lld\n", (u + v) / 2, (u - v) / 2, (ll)floor(I));
        }
    }
    return 0;
}
