// 202409-5 木板切割
//
// 关键观察：
//   每次操作的产物（新木板 i+1）就是“被切下的那一块”，而在该时刻 i+1 号木板
//   尚未被任何后续操作切割。于是用「时光倒流」：把操作 k..1 依次撤销，撤销操作 i
//   等价于把木板 i+1 的集合并入木板 x_i 的集合 —— 只有合并，没有分裂。
//   每次询问就是在合并前读当前连通块（= 恰为被切下的那一块）的整体信息。
//
// 阶段一（正向）：用一棵以“段编号”为键的 Treap 表示每块木板的下标集合，
//   一次切割 = 按 l、r+1 两次 split + merge，O(log n)。结束后得到每段最终归属。
// 阶段二（逆向）：并查集维护连通块；块内用 std::set<int> 维护有序下标，
//   合并时把较小块按顺序插入较大块（启发式合并），同时维护：
//     - csp[root]  : 相邻同色点对数（插入一个点只影响 u-p、p-v、u-v 三对）
//     - cdist[root]: 不同颜色数（用哈希表记录 (root,color) 是否出现）
//   颜色段数 = 块大小 - csp。
#include <cstdio>
#include <cstring>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>
using namespace std;

static const int MAXN = 100005;
static const int MAXB = 100005;

static int n, m, k;
static int col[MAXN];
static int qx[MAXB], ql[MAXB], qr[MAXB];

/* ---------------- 快速读入 ---------------- */
static char ibuf[1 << 22];
static int ipos = 0, ilen = 0;
static inline int ReadInt() {
    while (ipos >= ilen) { ilen = (int)fread(ibuf, 1, sizeof(ibuf), stdin); ipos = 0; if (ilen <= 0) return -1; }
    while (ibuf[ipos] <= ' ') { ipos++; if (ipos >= ilen) { ilen = (int)fread(ibuf, 1, sizeof(ibuf), stdin); ipos = 0; if (ilen <= 0) return -1; } }
    int x = 0;
    while (ipos < ilen && ibuf[ipos] > ' ') { x = x * 10 + (ibuf[ipos] - '0'); ipos++; }
    return x;
}

/* ---------------- 随机数 ---------------- */
static unsigned long long rngState = 0x243F6A8885A308D3ULL;
static inline unsigned long long NextRand() {
    rngState += 0x9E3779B97F4A7C15ULL;
    unsigned long long z = rngState;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

/* ---------------- Treap：结点编号 == 段编号 == 键 ---------------- */
static int tlc[MAXN], trc[MAXN];
static unsigned tpri[MAXN];

// 把 t 按 key 分裂：a 中键 < key，b 中键 >= key
static void Split(int t, int key, int &a, int &b) {
    if (!t) { a = b = 0; return; }
    if (t < key) { Split(trc[t], key, trc[t], b); a = t; }
    else { Split(tlc[t], key, a, tlc[t]); b = t; }
}
// 合并，要求 a 的所有键 < b 的所有键
static int Merge(int a, int b) {
    if (!a) return b;
    if (!b) return a;
    if (tpri[a] < tpri[b]) { trc[a] = Merge(trc[a], b); return a; }
    else { tlc[b] = Merge(a, tlc[b]); return b; }
}

static int rootOf[MAXB];
static int owner[MAXN];

static void Dfs(int t, int b) {
    if (!t) return;
    Dfs(tlc[t], b);
    owner[t] = b;
    Dfs(trc[t], b);
}

/* ---------------- 并查集 ---------------- */
static int par[MAXB];
static int Find(int x) {
    while (par[x] != x) { par[x] = par[par[x]]; x = par[x]; }
    return x;
}

/* ---------------- 哈希 ---------------- */
struct Hash64 {
    size_t operator()(unsigned long long x) const {
        x += 0x9E3779B97F4A7C15ULL;
        x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9ULL;
        x = (x ^ (x >> 27)) * 0x94D049BB133111EBULL;
        return (size_t)(x ^ (x >> 31));
    }
};

static set<int> *S[MAXB];
static int csz[MAXB], csp[MAXB], cdist[MAXB], lastp[MAXB];
static int ansD[MAXB], ansS[MAXB];

int main() {
    n = ReadInt(); m = ReadInt(); k = ReadInt();
    if (n < 0) return 0;
    for (int i = 1; i <= n; i++) col[i] = ReadInt();
    for (int i = 1; i <= k; i++) {
        qx[i] = ReadInt(); ql[i] = ReadInt(); qr[i] = ReadInt();
    }

    /* ---------- 阶段一：正向确定每段的最终归属 ---------- */
    for (int p = 1; p <= n; p++) {
        tlc[p] = trc[p] = 0;
        tpri[p] = (unsigned)NextRand();
    }
    int root = 0;
    for (int p = 1; p <= n; p++) root = Merge(root, p);
    rootOf[1] = root;

    for (int i = 1; i <= k; i++) {
        int x = qx[i], l = ql[i], r = qr[i];
        int a, t, s, b;
        Split(rootOf[x], l, a, t);        // a: <l
        Split(t, r + 1, s, b);            // s: [l,r]
        rootOf[x] = Merge(a, b);          // 母木板剩下的部分重新连起来
        rootOf[i + 1] = s;                // 新木板
    }

    memset(owner, 0, sizeof(owner));
    for (int j = 1; j <= k + 1; j++)
        if (rootOf[j]) Dfs(rootOf[j], j);

    /* ---------- 阶段二：逆向合并 ---------- */
    for (int j = 1; j <= k + 1; j++) par[j] = j;
    unordered_set<unsigned long long, Hash64> seen;
    seen.reserve((size_t)n * 4 + 1024);
    seen.max_load_factor(0.7f);

    for (int p = 1; p <= n; p++) {
        int j = owner[p];
        if (!S[j]) S[j] = new set<int>();
        S[j]->insert(S[j]->end(), p);
        csz[j]++;
        if (lastp[j] && col[lastp[j]] == col[p]) csp[j]++;
        lastp[j] = p;
        unsigned long long key = (unsigned long long)j * (unsigned long long)(m + 1) + (unsigned long long)col[p];
        if (seen.insert(key).second) cdist[j]++;
    }

    for (int i = k; i >= 1; i--) {
        int a = Find(i + 1);                 // 当前 a 号连通块恰为第 i 次切下来的那块
        ansD[i] = cdist[a];
        ansS[i] = csz[a] ? csz[a] - csp[a] : 0;

        int b = Find(qx[i]);
        if (a == b) continue;
        if (csz[a] > csz[b]) swap(a, b);     // a 为较小块（提供方），b 为较大块（保留方）
        if (csz[a] == 0) { par[a] = b; continue; }

        set<int> &sa = *S[a], &sb = *S[b];
        for (int p : sa) {
            set<int>::iterator it = sb.upper_bound(p);
            int v = (it == sb.end() ? 0 : *it);
            int u = (it == sb.begin() ? 0 : *prev(it));
            if (u && v) csp[b] -= (col[u] == col[v]);
            if (u) csp[b] += (col[u] == col[p]);
            if (v) csp[b] += (col[p] == col[v]);
            sb.insert(it, p);
            unsigned long long key = (unsigned long long)b * (unsigned long long)(m + 1) + (unsigned long long)col[p];
            if (seen.insert(key).second) cdist[b]++;
        }
        csz[b] += csz[a];
        csz[a] = 0; csp[a] = 0; cdist[a] = 0;
        delete S[a]; S[a] = 0;
        par[a] = b;
    }

    /* ---------- 输出 ---------- */
    {
        static char obuf[1 << 22];
        int opos = 0;
        for (int i = 1; i <= k; i++) {
            int d = ansD[i], s = ansS[i];
            char tmp[24]; int tn = 0;
            if (d == 0) tmp[tn++] = '0';
            while (d) { tmp[tn++] = (char)('0' + d % 10); d /= 10; }
            while (tn) obuf[opos++] = tmp[--tn];
            obuf[opos++] = ' ';
            if (s == 0) tmp[tn++] = '0';
            while (s) { tmp[tn++] = (char)('0' + s % 10); s /= 10; }
            while (tn) obuf[opos++] = tmp[--tn];
            obuf[opos++] = '\n';
            if (opos > (1 << 21)) { fwrite(obuf, 1, opos, stdout); opos = 0; }
        }
        if (opos) fwrite(obuf, 1, opos, stdout);
    }
    return 0;
}
