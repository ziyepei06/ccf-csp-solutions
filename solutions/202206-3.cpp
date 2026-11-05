// 202206-3 角色授权
// 思路：把所有字符串（角色名/操作/资源种类/资源名称/授权对象名）统一 intern 成整数 id（n<=500）。
//   1) 操作、资源种类、资源名称各建一组"token id -> 角色位图"的稠密索引：
//      位图第 i 位表示角色 i，W=(n+63)/64 个字。另维护三个全局位图：
//        starOp  —— 操作清单含 '*' 的角色
//        starTyp —— 资源种类清单含 '*' 的角色
//        emptyNm —— 资源名称清单为空（即允许任意名称）的角色
//   2) 授权对象用稠密数组区分：subjU[id] / subjG[id] 分别是"用户 id""用户组 id"
//      被关联到的角色位图块号。同一角色的多个角色关联自然取并集。
//   3) 每次询问：把 用户 与 ng 个用户组 的位图 OR 成 acc（该用户可用的角色集合），再算
//        allowOp  = starOp  | 含该操作的角色
//        allowTyp = starTyp | 含该资源种类的角色
//        allowNm  = emptyNm | 含该资源名称的角色（清单为空则不限名称）
//      若 acc & allowOp & allowTyp & allowNm 非空则输出 1，否则 0。
//   复杂度：时间 O(输入规模 + q*W)，空间 O(不同字符串数 * W)。
//   覆盖 100% 数据：n,m<=500，q<=5000，nv,no,ns,ng<=400，nn<=400。
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <vector>
#include <algorithm>

using namespace std;

static const int BUF = 1 << 22;
static char ibuf[BUF];
static int ipos = 0, ilen = 0;
static inline int gc() {
    if (ipos == ilen) {
        ilen = (int)fread(ibuf, 1, BUF, stdin);
        ipos = 0;
        if (ilen <= 0) return -1;
    }
    return (unsigned char)ibuf[ipos++];
}
static inline int rdInt() {
    int c = gc();
    while (c != -1 && (c < '0' || c > '9') && c != '-') c = gc();
    int sgn = 1;
    if (c == '-') { sgn = -1; c = gc(); }
    long long v = 0;
    while (c >= '0' && c <= '9') { v = v * 10 + (c - '0'); c = gc(); }
    return (int)(v * sgn);
}
static char tbuf[64];
static inline int rdTok() {  // 返回长度，内容写入 tbuf
    int c = gc();
    while (c != -1 && (c == ' ' || c == '\n' || c == '\r' || c == '\t')) c = gc();
    int len = 0;
    while (c != -1 && c != ' ' && c != '\n' && c != '\r' && c != '\t') {
        if (len < 60) tbuf[len++] = (char)c;
        c = gc();
    }
    return len;
}

// 字符串 intern：开放寻址哈希表 + 连续 arena，避免每个 token 一次堆分配
struct Interner {
    vector<int> tbl;
    unsigned msk;
    vector<char> arena;
    vector<uint32_t> off;
    vector<uint8_t> len;
    void init() { tbl.assign(1u << 17, -1); msk = (1u << 17) - 1; }
    static inline unsigned hs(const char* s, int l) {
        unsigned h = 2166136261u;
        for (int i = 0; i < l; ++i) { h ^= (unsigned char)s[i]; h *= 16777619u; }
        h ^= h >> 15; h *= 2246822519u; h ^= h >> 13;
        return h;
    }
    int findId(const char* s, int l) const {
        unsigned i = hs(s, l) & msk;
        for (;;) {
            int v = tbl[i];
            if (v < 0) return -1;
            if ((int)len[v] == l && memcmp(&arena[off[v]], s, l) == 0) return v;
            i = (i + 1) & msk;
        }
    }
    int intern(const char* s, int l) {
        unsigned i = hs(s, l) & msk;
        for (;;) {
            int v = tbl[i];
            if (v < 0) break;
            if ((int)len[v] == l && memcmp(&arena[off[v]], s, l) == 0) return v;
            i = (i + 1) & msk;
        }
        int id = (int)off.size();
        off.push_back((uint32_t)arena.size());
        len.push_back((uint8_t)l);
        arena.insert(arena.end(), s, s + l);
        tbl[i] = id;
        if ((size_t)(id + 2) * 10 > tbl.size() * 7) rebuild();
        return id;
    }
    void rebuild() {
        unsigned nsz = (unsigned)tbl.size() * 2;
        vector<int> nt(nsz, -1);
        unsigned nm = nsz - 1;
        for (int id = 0; id < (int)off.size(); ++id) {
            unsigned i = hs(&arena[off[id]], len[id]) & nm;
            while (nt[i] >= 0) i = (i + 1) & nm;
            nt[i] = id;
        }
        tbl.swap(nt);
        msk = nm;
    }
};

// 稠密 int 数组按需增长（-1 表示未设置）
static inline void ensure(vector<int>& v, int idx) {
    if ((int)v.size() <= idx) {
        int ns = (int)v.size() * 2;
        if (ns <= idx) ns = idx + 1;
        v.resize(ns, -1);
    }
}

int main() {
    const int MAXW = 16;  // n<=500 只需 8 个字，留足余量
    int n = rdInt(), m = rdInt(), q = rdInt();
    if (n <= 0) return 0;
    int W = (n + 63) >> 6;
    if (W > MAXW) W = MAXW;

    Interner it;
    it.init();
    int L;
    const int STAR = it.intern("*", 1);

    vector<int> opIdx, typIdx, nmIdx;      // token id -> 位图块号(-1 = 无)
    vector<uint64_t> opPool, typPool, nmPool;
    uint64_t starOp[MAXW] = {0}, starTyp[MAXW] = {0}, emptyNm[MAXW] = {0};

    auto setBit = [&](vector<int>& idx, vector<uint64_t>& pool, int tid, int bit) {
        ensure(idx, tid);
        int ci = idx[tid];
        if (ci < 0) {
            ci = (int)(pool.size() / W);
            idx[tid] = ci;
            pool.resize(pool.size() + W, 0);
        }
        pool[(size_t)ci * W + (bit >> 6)] |= (uint64_t)1 << (bit & 63);
    };

    vector<int> roleOfTok;  // 角色名 token -> 角色下标
    for (int i = 0; i < n; ++i) {
        L = rdTok();
        int rn = it.intern(tbuf, L);
        ensure(roleOfTok, rn);
        roleOfTok[rn] = i;
        int nv = rdInt();
        for (int j = 0; j < nv; ++j) {
            L = rdTok();
            int t = it.intern(tbuf, L);
            if (t == STAR) starOp[i >> 6] |= (uint64_t)1 << (i & 63);
            else setBit(opIdx, opPool, t, i);
        }
        int no = rdInt();
        for (int j = 0; j < no; ++j) {
            L = rdTok();
            int t = it.intern(tbuf, L);
            if (t == STAR) starTyp[i >> 6] |= (uint64_t)1 << (i & 63);
            else setBit(typIdx, typPool, t, i);
        }
        int nn = rdInt();
        if (nn == 0) emptyNm[i >> 6] |= (uint64_t)1 << (i & 63);
        for (int j = 0; j < nn; ++j) {
            L = rdTok();
            int t = it.intern(tbuf, L);
            setBit(nmIdx, nmPool, t, i);
        }
    }

    // 授权对象：subjU[id] / subjG[id] -> 该用户 / 用户组被关联到的角色位图块号
    vector<int> subjU, subjG;
    vector<uint64_t> subjPool;
    for (int i = 0; i < m; ++i) {
        L = rdTok();
        int rn = it.intern(tbuf, L);
        ensure(roleOfTok, rn);
        int r = roleOfTok[rn];
        int ns = rdInt();
        for (int j = 0; j < ns; ++j) {
            L = rdTok();
            int isG = (L == 1 && tbuf[0] == 'g') ? 1 : 0;
            L = rdTok();
            int nm = it.intern(tbuf, L);
            if (r < 0) continue;
            vector<int>& tab = isG ? subjG : subjU;
            ensure(tab, nm);
            int ci = tab[nm];
            if (ci < 0) {
                ci = (int)(subjPool.size() / W);
                tab[nm] = ci;
                subjPool.resize(subjPool.size() + W, 0);
            }
            subjPool[(size_t)ci * W + (r >> 6)] |= (uint64_t)1 << (r & 63);
        }
    }

    size_t T = it.off.size();
    opIdx.resize(T, -1);
    typIdx.resize(T, -1);
    nmIdx.resize(T, -1);
    subjU.resize(T, -1);
    subjG.resize(T, -1);
    roleOfTok.resize(T, -1);

    static char out[1 << 16];
    int opos = 0;
    for (int qi = 0; qi < q; ++qi) {
        uint64_t acc[MAXW];
        for (int k = 0; k < W; ++k) acc[k] = 0;
        L = rdTok();
        int ut = it.findId(tbuf, L);
        if (ut >= 0) {
            int ci = subjU[ut];
            if (ci >= 0) {
                const uint64_t* p = &subjPool[(size_t)ci * W];
                for (int k = 0; k < W; ++k) acc[k] |= p[k];
            }
        }
        int ng = rdInt();
        for (int j = 0; j < ng; ++j) {
            L = rdTok();
            int gt = it.findId(tbuf, L);
            if (gt >= 0) {
                int ci = subjG[gt];
                if (ci >= 0) {
                    const uint64_t* p = &subjPool[(size_t)ci * W];
                    for (int k = 0; k < W; ++k) acc[k] |= p[k];
                }
            }
        }
        L = rdTok();
        int ot = it.findId(tbuf, L);
        L = rdTok();
        int tt = it.findId(tbuf, L);
        L = rdTok();
        int nt = it.findId(tbuf, L);

        const uint64_t* pOp = (ot >= 0 && opIdx[ot] >= 0) ? &opPool[(size_t)opIdx[ot] * W] : nullptr;
        const uint64_t* pTy = (tt >= 0 && typIdx[tt] >= 0) ? &typPool[(size_t)typIdx[tt] * W] : nullptr;
        const uint64_t* pNm = (nt >= 0 && nmIdx[nt] >= 0) ? &nmPool[(size_t)nmIdx[nt] * W] : nullptr;

        int ok = 0;
        for (int k = 0; k < W; ++k) {
            uint64_t mo = starOp[k] | (pOp ? pOp[k] : 0);
            uint64_t mt = starTyp[k] | (pTy ? pTy[k] : 0);
            uint64_t mn = emptyNm[k] | (pNm ? pNm[k] : 0);
            if (acc[k] & mo & mt & mn) { ok = 1; break; }
        }
        out[opos++] = ok ? '1' : '0';
        out[opos++] = '\n';
        if (opos > (1 << 16) - 4) { fwrite(out, 1, opos, stdout); opos = 0; }
    }
    if (opos) fwrite(out, 1, opos, stdout);
    return 0;
}
