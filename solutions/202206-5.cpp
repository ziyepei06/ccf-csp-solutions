// CCF CSP 202206-5 PS无限版
// 所有操作 1~5 都是平面上的仿射变换 x' = A x + t (2x2 矩阵 A + 平移 t)。
// 用线段树维护区间内 cnt, sum x, sum y, sum x^2, sum xy, sum y^2，
// 仿射变换对这几个矩的作用是线性的，因此可以直接作为区间懒标记下传。
// 操作 6: 重心 = (sum x / cnt, sum y / cnt)
// 操作 7: sum (x-a)^2+(y-b)^2 = Sxx+Syy - 2a*Sx - 2b*Sy + cnt*(a^2+b^2)
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

typedef long double ld;

static ld *SX, *SY, *SXX, *SXY, *SYY;
static ld *A00, *A01, *A10, *A11, *TB0, *TB1;
static int *CNT;
static bool *LZ;

static int sz_, lg_;

struct Tf { ld a00, a01, a10, a11, b0, b1; };

static inline void applyNode(int k, const Tf &t) {
    int c = CNT[k];
    if (c == 0) return;
    ld a00 = t.a00, a01 = t.a01, a10 = t.a10, a11 = t.a11;
    ld b0 = t.b0, b1 = t.b1;
    ld sx = SX[k], sy = SY[k], sxx = SXX[k], sxy = SXY[k], syy = SYY[k];
    ld nc = (ld)c;
    SX[k]  = a00 * sx + a01 * sy + b0 * nc;
    SY[k]  = a10 * sx + a11 * sy + b1 * nc;
    SXX[k] = a00 * a00 * sxx + 2.0L * a00 * a01 * sxy + a01 * a01 * syy
           + 2.0L * b0 * (a00 * sx + a01 * sy) + b0 * b0 * nc;
    SXY[k] = a00 * a10 * sxx + (a00 * a11 + a01 * a10) * sxy + a01 * a11 * syy
           + b0 * (a10 * sx + a11 * sy) + b1 * (a00 * sx + a01 * sy) + b0 * b1 * nc;
    SYY[k] = a10 * a10 * sxx + 2.0L * a10 * a11 * sxy + a11 * a11 * syy
           + 2.0L * b1 * (a10 * sx + a11 * sy) + b1 * b1 * nc;
    // 复合懒标记: 先做原来的 (l00..lb1)，再做 t
    ld l00 = A00[k], l01 = A01[k], l10 = A10[k], l11 = A11[k];
    ld lb0 = TB0[k], lb1 = TB1[k];
    A00[k] = a00 * l00 + a01 * l10;
    A01[k] = a00 * l01 + a01 * l11;
    A10[k] = a10 * l00 + a11 * l10;
    A11[k] = a10 * l01 + a11 * l11;
    TB0[k] = a00 * lb0 + a01 * lb1 + b0;
    TB1[k] = a10 * lb0 + a11 * lb1 + b1;
    LZ[k] = true;
}

static inline void pull(int k) {
    int l = k << 1, r = l | 1;
    SX[k] = SX[l] + SX[r];
    SY[k] = SY[l] + SY[r];
    SXX[k] = SXX[l] + SXX[r];
    SXY[k] = SXY[l] + SXY[r];
    SYY[k] = SYY[l] + SYY[r];
    CNT[k] = CNT[l] + CNT[r];
}

static inline void push(int k) {
    if (!LZ[k]) return;
    Tf t{A00[k], A01[k], A10[k], A11[k], TB0[k], TB1[k]};
    applyNode(k << 1, t);
    applyNode(k << 1 | 1, t);
    LZ[k] = false;
    A00[k] = 1.0L; A01[k] = 0.0L; A10[k] = 0.0L; A11[k] = 1.0L;
    TB0[k] = 0.0L; TB1[k] = 0.0L;
}

// 区间 [l, r) 施加仿射变换 t
static void rangeApply(int l, int r, const Tf &t) {
    if (l >= r) return;
    l += sz_; r += sz_;
    int l0 = l, r0 = r;
    for (int h = lg_; h >= 1; --h) {
        if (((l0 >> h) << h) != l0) push(l0 >> h);
        if (((r0 >> h) << h) != r0) push((r0 - 1) >> h);
    }
    while (l < r) {
        if (l & 1) applyNode(l++, t);
        if (r & 1) applyNode(--r, t);
        l >>= 1; r >>= 1;
    }
    for (int h = 1; h <= lg_; ++h) {
        if (((l0 >> h) << h) != l0) pull(l0 >> h);
        if (((r0 >> h) << h) != r0) pull((r0 - 1) >> h);
    }
}

static int qcnt;
static ld qsx, qsy, qsxx, qsxy, qsyy;

static void rangeQuery(int l, int r) {
    qcnt = 0; qsx = qsy = qsxx = qsxy = qsyy = 0.0L;
    if (l >= r) return;
    l += sz_; r += sz_;
    for (int h = lg_; h >= 1; --h) {
        if (((l >> h) << h) != l) push(l >> h);
        if (((r >> h) << h) != r) push((r - 1) >> h);
    }
    while (l < r) {
        if (l & 1) {
            qcnt += CNT[l]; qsx += SX[l]; qsy += SY[l];
            qsxx += SXX[l]; qsxy += SXY[l]; qsyy += SYY[l];
            ++l;
        }
        if (r & 1) {
            --r;
            qcnt += CNT[r]; qsx += SX[r]; qsy += SY[r];
            qsxx += SXX[r]; qsxy += SXY[r]; qsyy += SYY[r];
        }
        l >>= 1; r >>= 1;
    }
}

// ---------- 快速读入 ----------
static char *pp;
static inline void skipws() {
    while (*pp == ' ' || *pp == '\n' || *pp == '\r' || *pp == '\t') ++pp;
}
static inline long long rdInt() {
    skipws();
    long long s = 1;
    if (*pp == '-') { s = -1; ++pp; } else if (*pp == '+') ++pp;
    long long v = 0;
    while (*pp >= '0' && *pp <= '9') { v = v * 10 + (*pp - '0'); ++pp; }
    return s * v;
}
static ld pow10tab[40];
static inline ld rdReal() {
    skipws();
    int s = 1;
    if (*pp == '-') { s = -1; ++pp; } else if (*pp == '+') ++pp;
    unsigned long long m = 0;
    int frac = 0;
    while (*pp >= '0' && *pp <= '9') { m = m * 10 + (unsigned)(*pp - '0'); ++pp; }
    if (*pp == '.') {
        ++pp;
        while (*pp >= '0' && *pp <= '9') { m = m * 10 + (unsigned)(*pp - '0'); ++pp; ++frac; }
    }
    int ex = 0;
    if (*pp == 'e' || *pp == 'E') {
        ++pp;
        int es = 1;
        if (*pp == '-') { es = -1; ++pp; } else if (*pp == '+') ++pp;
        int e = 0;
        while (*pp >= '0' && *pp <= '9') { e = e * 10 + (*pp - '0'); ++pp; }
        ex = es * e;
    }
    ex -= frac;
    ld v = (ld)m;
    if (ex > 0) {
        if (ex <= 38) v *= pow10tab[ex];
        else v *= powl(10.0L, (ld)ex);
    } else if (ex < 0) {
        int e2 = -ex;
        if (e2 <= 38) v /= pow10tab[e2];
        else v /= powl(10.0L, (ld)e2);
    }
    return s * v;
}

int main() {
    pow10tab[0] = 1.0L;
    for (int i = 1; i < 40; ++i) pow10tab[i] = pow10tab[i - 1] * 10.0L;

    // 读入全部输入
    size_t bcap = 1 << 22, len = 0;
    char *buf = (char *)malloc(bcap);
    for (;;) {
        if (len + 1 >= bcap) {
            bcap <<= 1;
            buf = (char *)realloc(buf, bcap);
        }
        size_t got = fread(buf + len, 1, bcap - len - 1, stdin);
        if (got == 0) break;
        len += got;
    }
    buf[len] = 0;
    pp = buf;

    int n = (int)rdInt();
    int q = (int)rdInt();

    sz_ = 1; lg_ = 0;
    while (sz_ < n) { sz_ <<= 1; ++lg_; }
    int cap = sz_ << 1;
    SX  = (ld *)calloc(cap, sizeof(ld)); SY  = (ld *)calloc(cap, sizeof(ld));
    SXX = (ld *)calloc(cap, sizeof(ld)); SXY = (ld *)calloc(cap, sizeof(ld));
    SYY = (ld *)calloc(cap, sizeof(ld));
    A00 = (ld *)calloc(cap, sizeof(ld)); A01 = (ld *)calloc(cap, sizeof(ld));
    A10 = (ld *)calloc(cap, sizeof(ld)); A11 = (ld *)calloc(cap, sizeof(ld));
    TB0 = (ld *)calloc(cap, sizeof(ld)); TB1 = (ld *)calloc(cap, sizeof(ld));
    CNT = (int *)calloc(cap, sizeof(int));
    LZ  = (bool *)calloc(cap, sizeof(bool));
    if (!SX || !SY || !SXX || !SXY || !SYY || !A00 || !A01 || !A10 || !A11 ||
        !TB0 || !TB1 || !CNT || !LZ) return 77;
    for (int k = 1; k < cap; ++k) {
        A00[k] = 1.0L; A01[k] = 0.0L; A10[k] = 0.0L; A11[k] = 1.0L;
        TB0[k] = 0.0L; TB1[k] = 0.0L;
        CNT[k] = 0;
    }
    for (int i = 0; i < n; ++i) {
        ld x = rdReal(), y = rdReal();
        int k = sz_ + i;
        SX[k] = x; SY[k] = y;
        SXX[k] = x * x; SXY[k] = x * y; SYY[k] = y * y;
        CNT[k] = 1;
    }
    for (int k = sz_ - 1; k >= 1; --k) pull(k);

    char *out = (char *)malloc((size_t)q * 80 + 1024);
    size_t olen = 0;

    for (int qi = 0; qi < q; ++qi) {
        int op = (int)rdInt();
        int l = (int)rdInt() - 1;
        int r = (int)rdInt();
        if (op == 1) {
            ld a = rdReal(), b = rdReal();
            Tf t{1.0L, 0.0L, 0.0L, 1.0L, a, b};
            rangeApply(l, r, t);
        } else if (op == 2) {
            ld a = rdReal(), b = rdReal(), th = rdReal();
            ld c = cosl(th), s = sinl(th);
            // 绕 (a,b) 逆时针旋转 th
            Tf t{c, -s, s, c,
                 a - (c * a - s * b),
                 b - (s * a + c * b)};
            rangeApply(l, r, t);
        } else if (op == 3) {
            ld a = rdReal(), b = rdReal(), lam = rdReal();
            Tf t{lam, 0.0L, 0.0L, lam, a - lam * a, b - lam * b};
            rangeApply(l, r, t);
        } else if (op == 4) {
            ld th = rdReal(), y0 = rdReal();
            ld c = cosl(th), s = sinl(th);
            ld c2 = c * c - s * s, s2 = 2.0L * s * c;
            // 关于直线 y = tan(th) x + y0 对称, 过点 p = (0, y0)
            // A = [[c2, s2], [s2, -c2]], t = p - A p
            Tf t{c2, s2, s2, -c2, -s2 * y0, y0 + c2 * y0};
            rangeApply(l, r, t);
        } else if (op == 5) {
            ld th = rdReal(), y0 = rdReal();
            ld c = cosl(th), s = sinl(th);
            // 投影到直线, 过点 p = (0, y0), A = d d^T = [[c^2, cs], [cs, s^2]]
            Tf t{c * c, c * s, c * s, s * s, -c * s * y0, y0 - s * s * y0};
            rangeApply(l, r, t);
        } else if (op == 6) {
            rangeQuery(l, r);
            ld inv = 1.0L / (ld)qcnt;
            olen += (size_t)snprintf(out + olen, 80, "%.10Lf %.10Lf\n", qsx * inv, qsy * inv);
        } else {
            ld a = rdReal(), b = rdReal();
            rangeQuery(l, r);
            ld ans = qsxx + qsyy - 2.0L * a * qsx - 2.0L * b * qsy
                   + (ld)qcnt * (a * a + b * b);
            olen += (size_t)snprintf(out + olen, 80, "%.10Lf\n", ans);
        }
    }
    fwrite(out, 1, olen, stdout);
    return 0;
}
