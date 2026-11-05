// 201409-5 拼图
// 轮廓线(逐行)状态压缩 DP + 矩阵快速幂
// 状态: 一行 m 列的二进制 mask, 表示该行中已被"上一行开始摆放的 L 形积木"占用的格子。
// 单行转移 T[a][b] = 由入状态 a 摆放完一行后, 恰好压在下一行的 mask 为 b 的方案数。
// 答案 = (T^n)[0][0]  (第 0 行上方无占用, 第 n 行下方不允许越界)。
// m <= 7 => 状态数 2^m <= 128, n <= 1e15 => 矩阵快速幂。
#include <bits/stdc++.h>
using namespace std;

static const unsigned int MOD = 1000000007u;
static int M, S;                     // 列数, 状态数
static vector<unsigned int> *Trow = nullptr;   // 当前正在填的 T 的一行

// 逐格摆放: 处理第 c 列; curMask 为当前行占用(含刚被本行积木占用的格),
// nxtMask 为下一行占用。 (r,c) 必须属于所放积木, 且包围盒左上角只能是 (r,c) 或 (r,c-1),
// 共 4 种朝向 (第 4 种的包围盒缺左上角, 故会同时压住下一行的 c-1、c 两格)。
static void dfs(int c, int curMask, int nxtMask) {
    if (c == M) { (*Trow)[nxtMask]++; if ((*Trow)[nxtMask] >= MOD) (*Trow)[nxtMask] -= MOD; return; }
    if ((curMask >> c) & 1) { dfs(c + 1, curMask, nxtMask); return; }
    if (c + 1 < M) {
        bool fr = !((curMask >> (c + 1)) & 1);   // (r,c+1) 空
        bool fc = !((nxtMask >> c) & 1);         // (r+1,c) 空
        bool fn = !((nxtMask >> (c + 1)) & 1);   // (r+1,c+1) 空
        if (fr && fc) dfs(c + 1, curMask | (1 << (c + 1)), nxtMask | (1 << c));                    // 缺 (r+1,c+1)
        if (fr && fn) dfs(c + 1, curMask | (1 << (c + 1)), nxtMask | (1 << (c + 1)));              // 缺 (r+1,c)
        if (fc && fn) dfs(c + 1, curMask, nxtMask | (1 << c) | (1 << (c + 1)));                    // 缺 (r,c+1)
    }
    if (c > 0) {                                  // 缺 (r,c-1) (包围盒左上角为已处理的格子)
        bool fc = !((nxtMask >> c) & 1);          // (r+1,c) 空
        bool fp = !((nxtMask >> (c - 1)) & 1);    // (r+1,c-1) 空
        if (fc && fp) dfs(c + 1, curMask, nxtMask | (1 << c) | (1 << (c - 1)));
    }
}

typedef vector<unsigned int> Row;

// C = A*B, 用 64 位累加并每 16 个 k 取一次模, 避免内层取模
static vector<Row> matMul(const vector<Row> &A, const vector<Row> &B) {
    vector<Row> C(S, Row(S, 0));
    unsigned long long tmp[130];
    for (int i = 0; i < S; i++) {
        for (int j = 0; j < S; j++) tmp[j] = 0;
        const Row &Ai = A[i];
        for (int k = 0; k < S; k++) {
            unsigned long long v = Ai[k];
            if (!v) continue;
            const Row &Bk = B[k];
            for (int j = 0; j < S; j++) tmp[j] += v * (unsigned long long)Bk[j];
            if ((k & 15) == 15)
                for (int j = 0; j < S; j++) tmp[j] %= MOD;
        }
        for (int j = 0; j < S; j++) C[i][j] = (unsigned int)(tmp[j] % MOD);
    }
    return C;
}

// vector * matrix
static Row vecMul(const Row &v, const vector<Row> &B) {
    Row res(S, 0);
    unsigned long long tmp[130];
    for (int j = 0; j < S; j++) tmp[j] = 0;
    for (int k = 0; k < S; k++) {
        unsigned long long x = v[k];
        if (!x) continue;
        const Row &Bk = B[k];
        for (int j = 0; j < S; j++) tmp[j] += x * (unsigned long long)Bk[j];
        if ((k & 15) == 15)
            for (int j = 0; j < S; j++) tmp[j] %= MOD;
    }
    for (int j = 0; j < S; j++) res[j] = (unsigned int)(tmp[j] % MOD);
    return res;
}

int main() {
    long long n, m;
    if (scanf("%lld %lld", &n, &m) != 2) return 0;
    if (m > n) { long long t = n; n = m; m = t; }   // 旋转棋盘, 保证列数 m 较小 (m <= 7)
    if (m > 16) return 0;                            // 依题意 m <= 7; 旋转后必然 m <= 7, 兜底
    M = (int)m;
    S = 1 << M;

    vector<Row> T(S, Row(S, 0));
    for (int a = 0; a < S; a++) {
        Row row(S, 0);
        Trow = &row;
        dfs(0, a, 0);
        T[a] = row;
    }

    // 答案 = e0 * T^n 的第 0 分量
    Row vec(S, 0);
    vec[0] = 1;
    vector<Row> base = T;
    long long e = n;
    while (e > 0) {
        if (e & 1) vec = vecMul(vec, base);
        e >>= 1;
        if (e > 0) base = matMul(base, base);
    }
    printf("%u\n", vec[0] % MOD);
    return 0;
}
