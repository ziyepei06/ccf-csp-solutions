// 201709-5 除法
//
// 关键观察：v >= 2 时，一次「有效」除法（a_i % v == 0）会让 a_i 至少缩小为原来的 1/2，
// 而 a_i <= 1e6，所以每个位置总共最多被有效除约 log2(1e6) ≈ 20 次，全场有效更新次数
// O(N log A)。难点只是「如何在 [l,r] 中快速找到 v 的倍数」，按 v 的大小分治：
//
//   * v <= B（小 v）：对每个小 v 维护一个「哪些位置当前值被 v 整除」的二级位图
//     （n 位 + 摘要层），findNext 近似 O(1)。一次操作只会访问真正会被除的位置，
//     命中一次就是一次有效更新，总代价 O(命中数 · (τ + log n))。
//   * v >  B（大 v）：v 的倍数不超过 1e6/v < 1e6/B 个。用「值是否出现」的位图枚举
//     v, 2v, 3v, ... 直到当前最大值 maxA，再对出现的值取「值 -> 位置有序集合」，
//     把 [l,r] 内的位置收进候选列表（先收完再统一做一次除法，保证每个位置一次操作
//     只被除一次）。
//
// 区间和用树状数组（单点修改 + 前缀和），O(log n)。
// 值为 0 的位置除以任何 v 仍是 0，永远不参与「有效更新」，从一开始就被排除在所有结构外。
// v == 1 时除法不改变任何数，直接跳过。
//
// 复杂度：时间 O(MAXV log log MAXV + N·d_small + (有效更新次数)·(τ + log n) + M·(1e6/B + log n))，
// 其中 d_small = 值的 <=B 的因子个数（≤ 64），有效更新次数 ≤ N·20；空间 O(B·N/8 + MAXV)。
#include <bits/stdc++.h>
using namespace std;

static const int MAXN = 100005;
static const int MAXV = 1000000;
static const int B = 1000;                       // 小 v / 大 v 分界

// ---------- 快速输入 ----------
static char ibuf[1 << 23];
static size_t ipos = 0, ilen = 0;

static int readInt() {
    while (ipos < ilen) {
        char c = ibuf[ipos];
        if (c > ' ') break;
        ++ipos;
    }
    int sign = 1;
    if (ipos < ilen && ibuf[ipos] == '-') { sign = -1; ++ipos; }
    int x = 0;
    while (ipos < ilen) {
        char c = ibuf[ipos];
        if (c < '0' || c > '9') break;
        x = x * 10 + (c - '0');
        ++ipos;
    }
    return x * sign;
}

// ---------- 全局状态 ----------
static int n, m;
static int a[MAXN];
static long long fw[MAXN];                        // 树状数组
static int cntVal[MAXV + 1];                      // 当前各值的出现次数
static int maxA = 0;                              // 当前最大出现值

// 小 v 位图：位 i 表示位置 i 的当前值被 v 整除（v ∈ [2, B]）
static const int WMAX = 1600;                     // ceil((MAXN+1)/64)
static const int SWMAX = 25;                      // ceil(WMAX/64)
static uint64_t bitsTab[B + 1][WMAX];
static uint64_t smryTab[B + 1][SWMAX];
static int nWords = 0;

// 值 -> 位置集合（只为出现过的值分配）
static set<int>* posOf[MAXV + 1];
// 值是否出现（位图，用于大 v 枚举时的快速判断）
static uint64_t presentTab[(MAXV >> 6) + 2];

static int spf[MAXV + 1];

// ---------- 位图基本操作 ----------
static inline void setBit(int v, int i) {
    int w = i >> 6;
    uint64_t b = 1ULL << (i & 63);
    if (!(bitsTab[v][w] & b)) {
        bitsTab[v][w] |= b;
        smryTab[v][w >> 6] |= 1ULL << (w & 63);
    }
}

static inline void clearBit(int v, int i) {
    int w = i >> 6;
    bitsTab[v][w] &= ~(1ULL << (i & 63));
    if (bitsTab[v][w] == 0) smryTab[v][w >> 6] &= ~(1ULL << (w & 63));
}

// 最小的非空字号 >= w，找不到返回 -1
static inline int nextWord(int v, int w) {
    if (w >= nWords) return -1;
    int sj = w >> 6;
    uint64_t s = smryTab[v][sj];
    if (w & 63) s &= (~0ULL << (w & 63));
    for (;;) {
        if (s) {
            int ww = (sj << 6) + __builtin_ctzll(s);
            return ww < nWords ? ww : -1;
        }
        ++sj;
        if (sj >= SWMAX) return -1;
        s = smryTab[v][sj];
    }
}

// 最小的被置位位置 >= start（1..n），无则 -1
static inline int nextSet(int v, int start) {
    if (start < 1) start = 1;
    if (start > n) return -1;
    int w = start >> 6;
    uint64_t cur = bitsTab[v][w];
    if (start & 63) cur &= (~0ULL << (start & 63));
    for (;;) {
        if (cur) {
            int p = (w << 6) + __builtin_ctzll(cur);
            return p <= n ? p : -1;
        }
        w = nextWord(v, w + 1);
        if (w < 0) return -1;
        cur = bitsTab[v][w];
    }
}

// ---------- 因子枚举（只输出 <= lim 的因子，含 1） ----------
static int genDivisors(int d, int lim, int* out) {
    int ps[8], es[8], np = 0, tmp = d;
    while (tmp > 1) {
        int p = spf[tmp], e = 0;
        while (tmp % p == 0) { tmp /= p; ++e; }
        ps[np] = p; es[np] = e; ++np;
    }
    int sz = 1;
    out[0] = 1;
    for (int i = 0; i < np; ++i) {
        int p = ps[i], e = es[i], base = sz;
        long long pk = 1;
        for (int k = 1; k <= e; ++k) {
            pk *= p;
            if (pk > lim) break;
            for (int j = 0; j < base; ++j) {
                long long v2 = (long long)out[j] * pk;
                if (v2 <= lim) out[sz++] = (int)v2;
            }
        }
    }
    return sz;  // out[0] == 1，out[1..sz-1] 为 2..lim 内的因子
}

static int divBuf[256];

static inline void addDivBits(int i, int d) {
    int c = genDivisors(d, B, divBuf);
    for (int k = 1; k < c; ++k) setBit(divBuf[k], i);
}

static inline void delDivBits(int i, int d) {
    int c = genDivisors(d, B, divBuf);
    for (int k = 1; k < c; ++k) clearBit(divBuf[k], i);
}

// ---------- 树状数组 ----------
static inline void fwAdd(int i, long long d) {
    for (; i <= n; i += i & -i) fw[i] += d;
}
static inline long long fwPrefix(int i) {
    long long s = 0;
    for (; i > 0; i -= i & -i) s += fw[i];
    return s;
}

// ---------- 把位置 i 的当前值除以 v（调用前保证 a[i] % v == 0, v >= 2） ----------
static inline void applyDiv(int i, int v) {
    int old = a[i];
    int nw = old / v;
    delDivBits(i, old);
    if (nw > 1) addDivBits(i, nw);
    // 值 -> 位置 集合
    set<int>*& po = posOf[old];
    if (po) po->erase(i);
    set<int>*& pn = posOf[nw];
    if (!pn) pn = new set<int>();
    pn->insert(i);
    // 出现值位图
    if (--cntVal[old] == 0) {
        presentTab[old >> 6] &= ~(1ULL << (old & 63));
        if (old == maxA) {
            while (maxA > 0 && cntVal[maxA] == 0) --maxA;
        }
    }
    if (cntVal[nw]++ == 0) presentTab[nw >> 6] |= 1ULL << (nw & 63);

    a[i] = nw;
    fwAdd(i, (long long)nw - old);
}

int main() {
    ilen = fread(ibuf, 1, sizeof(ibuf), stdin);
    n = readInt();
    m = readInt();
    if (n <= 0) { return 0; }
    for (int i = 1; i <= n; ++i) a[i] = readInt();

    // 线性筛/最小素因子
    for (int i = 2; i <= MAXV; ++i) {
        if (!spf[i]) {
            spf[i] = i;
            if ((long long)i * i <= MAXV)
                for (long long j = (long long)i * i; j <= MAXV; j += i)
                    if (!spf[j]) spf[j] = (int)i;
        }
    }

    nWords = (n + 64) / 64;
    for (int i = 1; i <= n; ++i) {
        fwAdd(i, a[i]);
        if (a[i] > 0) {
            if (cntVal[a[i]]++ == 0) presentTab[a[i] >> 6] |= 1ULL << (a[i] & 63);
            if (a[i] > maxA) maxA = a[i];
            int c = genDivisors(a[i], B, divBuf);
            for (int k = 1; k < c; ++k) setBit(divBuf[k], i);
            set<int>*& po = posOf[a[i]];
            if (!po) po = new set<int>();
            po->insert(i);
        }
    }

    string out;
    out.reserve(1 << 21);
    char tmp[32];
    vector<int> cand;

    for (int q = 0; q < m; ++q) {
        int opt = readInt();
        if (opt == 1) {
            int l = readInt(), r = readInt(), v = readInt();
            if (v <= 1) continue;                    // 除以 1 无任何变化
            if (l > r) swap(l, r);
            if (v <= B) {
                int p = nextSet(v, l);
                while (p >= 0 && p <= r) {
                    applyDiv(p, v);
                    p = nextSet(v, p + 1);           // 每个位置本次操作只除一次
                }
            } else {
                cand.clear();
                for (int d = v; d <= maxA; d += v) {
                    if (!((presentTab[d >> 6] >> (d & 63)) & 1ULL)) continue;
                    set<int>* ps = posOf[d];
                    if (!ps) continue;
                    set<int>::iterator it = ps->lower_bound(l);
                    while (it != ps->end() && *it <= r) {
                        cand.push_back(*it);
                        ++it;
                    }
                }
                for (size_t k = 0; k < cand.size(); ++k) applyDiv(cand[k], v);
            }
        } else {
            int l = readInt(), r = readInt();
            if (l > r) swap(l, r);
            long long s = fwPrefix(r) - fwPrefix(l - 1);
            int len = snprintf(tmp, sizeof(tmp), "%lld\n", s);
            out.append(tmp, len);
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
