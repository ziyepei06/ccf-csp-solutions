// 202312-4 宝藏
// 区间询问：依次执行第 l..r 条指令，求最终双端队列中矩阵从头到尾的乘积。
// 观察：删除的是"最晚插入"的元素 => 与括号匹配同构，第 i 条指令里的'('在遇到右侧最早的')'时被消掉。
// 最终队列 = (头部插入的幸存矩阵，按插入序逆序) * (尾部插入的幸存矩阵，按插入序正序)。
// 做法：对指令分块(sqrt 分解)。每块的效果固定为：先向传入栈弹出 c 个元素，再压入该块的幸存序列。
// 询问时左侧/右侧残块逐条模拟，中间整块用"块效果"作用在块栈上，块栈保存 (P1,P2,cnt) 前缀乘积。
// 时间 O((n+m) sqrt n)，空间 O(n)。
#include <bits/stdc++.h>
using namespace std;

static const long long MOD = 998244353;

struct Mat { long long a[2][2]; };

static inline Mat ident() {
    Mat r;
    r.a[0][0] = 1; r.a[0][1] = 0;
    r.a[1][0] = 0; r.a[1][1] = 1;
    return r;
}

static inline Mat mmul(const Mat &x, const Mat &y) {
    Mat r;
    r.a[0][0] = (x.a[0][0] * y.a[0][0] + x.a[0][1] * y.a[1][0]) % MOD;
    r.a[0][1] = (x.a[0][0] * y.a[0][1] + x.a[0][1] * y.a[1][1]) % MOD;
    r.a[1][0] = (x.a[1][0] * y.a[0][0] + x.a[1][1] * y.a[1][0]) % MOD;
    r.a[1][1] = (x.a[1][0] * y.a[0][1] + x.a[1][1] * y.a[1][1]) % MOD;
    return r;
}

/* ---------- 快速读入 ---------- */
static char *ibuf;
static int ilen, ipos;
static inline int rdint() {
    while (ipos < ilen && (ibuf[ipos] < '0' || ibuf[ipos] > '9')) ipos++;
    long long x = 0;
    while (ipos < ilen && ibuf[ipos] >= '0' && ibuf[ipos] <= '9') { x = x * 10 + (ibuf[ipos] - '0'); ipos++; }
    return (int)x;
}

int n, m, S, nb;
vector<int> v;          // 每条指令的类型 1/2/3
vector<Mat> M;          // v=1/2 时的矩阵

struct Block {
    int c;              // 该块对传入栈的弹出次数
    int o;              // 该块幸存序列长度
    vector<Mat> P1, P2; // P1[t]: 幸存序列前 t 个中 type1 按插入序逆序的乘积
                        // P2[t]: 幸存序列前 t 个中 type2 按插入序正序的乘积
};
vector<Block> blocks;

static vector<Mat> bsm; static vector<char> bst;   // 重建块时的局部栈

static void buildBlock(int b) {
    int lo = b * S, hi = min(n, lo + S) - 1;
    bsm.clear(); bst.clear();
    int c = 0;
    for (int i = lo; i <= hi; i++) {
        int t = v[i];
        if (t == 3) {
            if (bsm.empty()) c++;
            else { bsm.pop_back(); bst.pop_back(); }
        } else {
            bsm.push_back(M[i]); bst.push_back((char)t);
        }
    }
    Block &B = blocks[b];
    B.c = c;
    B.o = (int)bsm.size();
    if ((int)B.P1.size() != B.o + 1) { B.P1.assign(B.o + 1, ident()); B.P2.assign(B.o + 1, ident()); }
    B.P1[0] = ident(); B.P2[0] = ident();
    for (int t = 1; t <= B.o; t++) {
        B.P1[t] = (bst[t - 1] == 1) ? mmul(bsm[t - 1], B.P1[t - 1]) : B.P1[t - 1];
        B.P2[t] = (bst[t - 1] == 2) ? mmul(B.P2[t - 1], bsm[t - 1]) : B.P2[t - 1];
    }
}

/* ---------- 询问用缓冲区 ---------- */
static vector<Mat> Lst; static vector<char> Ltp;   // 左侧残块的局部栈（幸存序列按插入序）
static vector<Mat> LP1, LP2;
static vector<Mat> Rb; static vector<char> Rtp;    // 右侧残块压缩后的压入序列
static vector<Mat> RP1, RP2;

struct Chunk { const Mat *p1; const Mat *p2; int cnt; }; // cnt 为当前仍存活的"前缀长度"
static vector<Chunk> cs;

static inline void outMat(const Mat &x) {
    printf("%lld %lld %lld %lld\n", x.a[0][0], x.a[0][1], x.a[1][0], x.a[1][1]);
}

static void query(int l, int r) {
    int bl = l / S, br = r / S;
    cs.clear();
    if (bl == br) {
        Lst.clear(); Ltp.clear();
        for (int i = l; i <= r; i++) {
            if (v[i] == 3) { if (!Lst.empty()) { Lst.pop_back(); Ltp.pop_back(); } }
            else { Lst.push_back(M[i]); Ltp.push_back((char)v[i]); }
        }
        Mat A = ident(), B = ident();
        for (int k = (int)Lst.size() - 1; k >= 0; k--) if (Ltp[k] == 1) A = mmul(A, Lst[k]);
        for (int k = 0; k < (int)Lst.size(); k++) if (Ltp[k] == 2) B = mmul(B, Lst[k]);
        outMat(mmul(A, B));
        return;
    }
    // 左侧残块: [l, end of block bl]
    Lst.clear(); Ltp.clear();
    int hi = min(n - 1, (bl + 1) * S - 1);
    for (int i = l; i <= hi; i++) {
        if (v[i] == 3) { if (!Lst.empty()) { Lst.pop_back(); Ltp.pop_back(); } }
        else { Lst.push_back(M[i]); Ltp.push_back((char)v[i]); }
    }
    int sz = (int)Lst.size();
    LP1[0] = ident(); LP2[0] = ident();
    for (int t = 1; t <= sz; t++) {
        LP1[t] = (Ltp[t - 1] == 1) ? mmul(Lst[t - 1], LP1[t - 1]) : LP1[t - 1];
        LP2[t] = (Ltp[t - 1] == 2) ? mmul(LP2[t - 1], Lst[t - 1]) : LP2[t - 1];
    }
    if (sz > 0) cs.push_back({LP1.data(), LP2.data(), sz});

    // 中间整块
    for (int b = bl + 1; b <= br - 1; b++) {
        int c = blocks[b].c;
        while (c > 0 && !cs.empty()) {
            int t = min(c, cs.back().cnt);
            cs.back().cnt -= t; c -= t;
            if (cs.back().cnt == 0) cs.pop_back();
        }
        if (blocks[b].o > 0) cs.push_back({blocks[b].P1.data(), blocks[b].P2.data(), blocks[b].o});
    }

    // 右侧残块: [start of block br, r]
    int own = 0;
    for (int i = br * S; i <= r; i++) {
        if (v[i] == 3) {
            if (own > 0) own--;
            else if (!cs.empty()) { if (--cs.back().cnt == 0) cs.pop_back(); }
        } else {
            Rb[own] = M[i]; Rtp[own] = (char)v[i]; own++;
        }
    }
    RP1[0] = ident(); RP2[0] = ident();
    for (int t = 1; t <= own; t++) {
        RP1[t] = (Rtp[t - 1] == 1) ? mmul(Rb[t - 1], RP1[t - 1]) : RP1[t - 1];
        RP2[t] = (Rtp[t - 1] == 2) ? mmul(RP2[t - 1], Rb[t - 1]) : RP2[t - 1];
    }
    if (own > 0) cs.push_back({RP1.data(), RP2.data(), own});

    // 合并：type1 按插入序逆序在所有幸存者上做乘积，type2 按插入序正序
    Mat A = ident(), B = ident();
    for (int k = (int)cs.size() - 1; k >= 0; k--) A = mmul(A, cs[k].p1[cs[k].cnt]);
    for (int k = 0; k < (int)cs.size(); k++) B = mmul(B, cs[k].p2[cs[k].cnt]);
    outMat(mmul(A, B));
}

int main() {
    // 读入全部输入
    {
        size_t cap = 1 << 20, len = 0;
        ibuf = (char *)malloc(cap);
        for (;;) {
            if (len + (1 << 16) > cap) { cap *= 2; ibuf = (char *)realloc(ibuf, cap); }
            size_t got = fread(ibuf + len, 1, (1 << 16), stdin);
            len += got;
            if (got == 0) break;
        }
        ilen = (int)len; ipos = 0;
    }

    n = rdint(); m = rdint();
    v.assign(n, 3);
    M.assign(n, ident());
    for (int i = 0; i < n; i++) {
        int t = rdint();
        v[i] = t;
        if (t == 1 || t == 2) {
            for (int r = 0; r < 2; r++) for (int c = 0; c < 2; c++) M[i].a[r][c] = rdint();
        }
    }

    S = (int)sqrt((double)n) + 1;
    nb = (n + S - 1) / S;
    blocks.assign(nb, Block());
    for (int b = 0; b < nb; b++) buildBlock(b);

    Lst.reserve(S + 2); Ltp.reserve(S + 2);
    Rb.resize(S + 2); Rtp.resize(S + 2);
    LP1.assign(S + 2, ident()); LP2.assign(S + 2, ident());
    RP1.assign(S + 2, ident()); RP2.assign(S + 2, ident());
    cs.reserve(2 * (n / S + 4) + S + 4);

    for (int q = 0; q < m; q++) {
        int t = rdint();
        if (t == 1) {
            int i = rdint() - 1;
            int vt = rdint();
            v[i] = vt;
            if (vt == 1 || vt == 2) {
                for (int r = 0; r < 2; r++) for (int c = 0; c < 2; c++) M[i].a[r][c] = rdint();
            }
            buildBlock(i / S);
        } else {
            int l = rdint() - 1, r = rdint() - 1;
            query(l, r);
        }
    }
    fflush(stdout);
    return 0;
}
