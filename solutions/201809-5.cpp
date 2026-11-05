/*
 * CCF CSP 201809-5 线性递推式
 *
 * 题意：a_0 = 1，a_n = sum_{i=1}^{min(n,m)} k_i * a_{n-i}  (n >= 1)，
 *       所有运算在 mod Q 意义下（Q = 998244353），求 a_l, a_{l+1}, ..., a_r。
 *
 * 记 Q(x) = 1 - k_1 x - k_2 x^2 - ... - k_m x^m，则生成函数
 *      F(x) = sum_{n>=0} a_n x^n = 1 / Q(x)。
 * 记 d 为实际的最高非零项下标（d <= m），T(x) = x^d - k_1 x^{d-1} - ... - k_d。
 *
 * 两条路径：
 *   (A) r 较小时，直接牛顿迭代求 1/Q(x) mod x^{r+1}，输出系数 l..r。
 *   (B) l 很大时（l <= 1e12）：先用二进制快速幂求 c(x) = x^l mod T(x)，
 *       由 Kitamasa 定理 a_{l+j} = sum_{i=0}^{d-1} c_i * a_{i+j}，
 *       把它写成卷积：设 cr 为 c 的反转，A 为数列前 d+len-1 项（= 1/Q 的级数），
 *       则 a_{l+j} = (cr * A)[d-1+j]，取 j = 0..len-1 即可。
 *
 * 复杂度：时间 O(d log d log l + (d+len) log(d+len))，空间 O(d+len)。
 * 覆盖最大子任务：m <= 1e5, l <= 1e12, r-l <= 1e5。
 */
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <string>
using namespace std;

typedef unsigned int u32;
typedef unsigned long long u64;
static const u32 MOD = 998244353u;   // 题面给定的模数 Q
static const u32 G = 3u;

static inline u32 addmod(u32 a, u32 b) { u32 s = a + b; return s >= MOD ? s - MOD : s; }
static inline u32 submod(u32 a, u32 b) { return a >= b ? a - b : a + MOD - b; }
static inline u32 mulmod(u32 a, u32 b) { return (u32)((u64)a * b % MOD); }

static u32 powmod(u64 a, u64 e) {
    u64 r = 1;
    a %= MOD;
    while (e) { if (e & 1) r = r * a % MOD; a = a * a % MOD; e >>= 1; }
    return (u32)r;
}

/* ---------------- NTT (998244353) ---------------- */
struct FFT {
    u32 sum_e[30], sum_ie[30];
    FFT() {
        int cnt2 = 0;
        u32 t = MOD - 1;
        while (!(t & 1u)) { ++cnt2; t >>= 1; }
        u32 e = powmod(G, (u64)((MOD - 1) >> cnt2)), ie = powmod(e, MOD - 2);
        u32 es[30], ies[30];
        for (int i = cnt2; i >= 2; --i) {
            es[i - 2] = e; ies[i - 2] = ie;
            e = mulmod(e, e); ie = mulmod(ie, ie);
        }
        u32 now = 1, inow = 1;
        for (int i = 0; i <= cnt2 - 2; ++i) {
            sum_e[i] = mulmod(es[i], now);  now  = mulmod(now,  ies[i]);
            sum_ie[i] = mulmod(ies[i], inow); inow = mulmod(inow, es[i]);
        }
    }

    void butterfly(vector<u32>& a) const {
        int n = (int)a.size();
        int h = 0;
        while ((1 << h) < n) ++h;
        for (int ph = 1; ph <= h; ++ph) {
            int w = 1 << (ph - 1), p = 1 << (h - ph);
            u32 now = 1;
            for (int s = 0; s < w; ++s) {
                int offset = s << (h - ph + 1);
                for (int i = 0; i < p; ++i) {
                    u32 l = a[offset + i];
                    u32 r = mulmod(a[offset + i + p], now);
                    a[offset + i]     = addmod(l, r);
                    a[offset + i + p] = submod(l, r);
                }
                now = mulmod(now, sum_e[__builtin_ctz(~(unsigned)s)]);
            }
        }
    }

    void butterfly_inv(vector<u32>& a) const {   // 未除以 n
        int n = (int)a.size();
        int h = 0;
        while ((1 << h) < n) ++h;
        for (int ph = h; ph >= 1; --ph) {
            int w = 1 << (ph - 1), p = 1 << (h - ph);
            u32 inow = 1;
            for (int s = 0; s < w; ++s) {
                int offset = s << (h - ph + 1);
                for (int i = 0; i < p; ++i) {
                    u32 l = a[offset + i];
                    u32 r = a[offset + i + p];
                    a[offset + i]     = addmod(l, r);
                    a[offset + i + p] = mulmod(submod(l, r), inow);
                }
                inow = mulmod(inow, sum_ie[__builtin_ctz(~(unsigned)s)]);
            }
        }
    }
};

static FFT fft;

static vector<u32> convolution(const vector<u32>& a, const vector<u32>& b) {
    int n = (int)a.size(), m = (int)b.size();
    if (!n || !m) return vector<u32>();
    if (min(n, m) <= 48) {
        vector<u32> c(n + m - 1, 0);
        for (int i = 0; i < n; ++i) {
            if (!a[i]) continue;
            u64 ai = a[i];
            for (int j = 0; j < m; ++j) {
                if (!b[j]) continue;
                c[i + j] = (u32)((c[i + j] + ai * b[j]) % MOD);
            }
        }
        return c;
    }
    int z = 1;
    while (z < n + m - 1) z <<= 1;
    vector<u32> fa(z, 0), fb(z, 0);
    for (int i = 0; i < n; ++i) fa[i] = a[i];
    for (int i = 0; i < m; ++i) fb[i] = b[i];
    fft.butterfly(fa); fft.butterfly(fb);
    for (int i = 0; i < z; ++i) fa[i] = mulmod(fa[i], fb[i]);
    fft.butterfly_inv(fa);
    u32 iz = powmod(z, MOD - 2);
    vector<u32> c(n + m - 1);
    for (int i = 0; i < n + m - 1; ++i) c[i] = mulmod(fa[i], iz);
    return c;
}

/* 1/f(x) mod x^n ，要求 f[0] != 0 */
static vector<u32> poly_inv(const vector<u32>& f, int n) {
    vector<u32> g(1, powmod(f[0], MOD - 2));
    while ((int)g.size() < n) {
        int m = min(2 * (int)g.size(), n);
        vector<u32> ft(min((int)f.size(), m), 0);
        for (int i = 0; i < (int)ft.size(); ++i) ft[i] = f[i];
        vector<u32> t = convolution(ft, g);     // f*g mod x^m
        t.resize(m, 0);
        for (int i = 0; i < m; ++i) t[i] = t[i] ? MOD - t[i] : 0;   // 2 - f*g
        t[0] = addmod(t[0], 2);
        g = convolution(g, t);
        g.resize(m, 0);
    }
    return g;
}

/* ---------------- Kitamasa：求 x^l mod T(x) ---------------- */
/*
 * T(x) = x^d - k_1 x^{d-1} - ... - k_d  (monic, 次数 d)
 * 因此 x^d == K(x) := sum_{t=0}^{d-1} K[t] x^t ，K[t] = k_{d-t}
 * 多项式乘法后用「反转除法」约化：
 *   h 次数 <= 2d-2，H = 2d-2，k = H-d+1 = d-1，
 *   q_rev = (rev_H(h) mod x^k) * (1/rev(T)) mod x^k ，rev(T) = Q
 *   余式 rem = h - rev(q_rev) * T 的低 d 项
 */
struct KitamasaNTT {
    int d = 0, N = 0;
    u32 invN = 1;
    vector<u32> Fhat, That;      // 预先变换好的 1/Q 与 T

    void init(int d_, const vector<u32>& F, const vector<u32>& K) {
        d = d_;
        N = 1;
        while (N < 2 * d - 1) N <<= 1;
        invN = powmod(N, MOD - 2);
        Fhat.assign(N, 0);
        for (int i = 0; i < d && i < (int)F.size(); ++i) Fhat[i] = F[i];
        fft.butterfly(Fhat);
        That.assign(N, 0);
        for (int i = 0; i < d; ++i) That[i] = K[i] ? MOD - K[i] : 0;   // T[i] = -K[i]
        That[d] = 1;                                                   // 首项 x^d
        fft.butterfly(That);
    }

    // h = a*b (次数 < 2d-1) 后约化，返回次数 < d 的结果
    void reduce_and_mul(const vector<u32>& a, const vector<u32>& b, bool same,
                        vector<u32>& out) {
        vector<u32> ha(N, 0);
        for (int i = 0; i < d; ++i) ha[i] = a[i];
        fft.butterfly(ha);
        if (same) {
            for (int i = 0; i < N; ++i) ha[i] = mulmod(ha[i], ha[i]);
        } else {
            vector<u32> hb(N, 0);
            for (int i = 0; i < d; ++i) hb[i] = b[i];
            fft.butterfly(hb);
            for (int i = 0; i < N; ++i) ha[i] = mulmod(ha[i], hb[i]);
        }
        fft.butterfly_inv(ha);
        // h[i] = ha[i] * invN ，i = 0..2d-2
        // 商的反转：u[j] = h[2d-2-j]，j = 0..d-2
        vector<u32> u(N, 0);
        for (int j = 0; j < d - 1; ++j) u[j] = mulmod(ha[2 * d - 2 - j], invN);
        fft.butterfly(u);
        for (int i = 0; i < N; ++i) u[i] = mulmod(u[i], Fhat[i]);
        fft.butterfly_inv(u);
        // q_rev = u * invN，q[i] = q_rev[d-2-i]
        vector<u32> q(N, 0);
        for (int i = 0; i < d - 1; ++i) q[i] = mulmod(u[d - 2 - i], invN);
        fft.butterfly(q);
        for (int i = 0; i < N; ++i) q[i] = mulmod(q[i], That[i]);
        fft.butterfly_inv(q);
        out.assign(d, 0);
        for (int i = 0; i < d; ++i)
            out[i] = submod(mulmod(ha[i], invN), mulmod(q[i], invN));
    }
};

/* 小 d 用朴素平方 + 朴素约化，常数更小 */
static vector<u32> kitamasa_naive(u64 l, int d, const vector<u32>& K) {
    auto mulmod_poly = [&](const vector<u32>& a, const vector<u32>& b) {
        vector<u32> h(2 * d - 1, 0);
        for (int i = 0; i < d; ++i) {
            if (!a[i]) continue;
            u64 ai = a[i];
            for (int j = 0; j < d; ++j) {
                if (!b[j]) continue;
                h[i + j] = (u32)((h[i + j] + ai * b[j]) % MOD);
            }
        }
        for (int i = 2 * d - 2; i >= d; --i) {
            u32 v = h[i];
            if (!v) continue;
            for (int t = 0; t < d; ++t) {
                if (!K[t]) continue;
                h[i - d + t] = addmod(h[i - d + t], mulmod(v, K[t]));
            }
        }
        h.resize(d);
        return h;
    };
    vector<u32> res(d, 0), base(d, 0);
    res[0] = 1;
    if (d == 1) base[0] = K[0];
    else base[1] = 1;                  // base = x mod T
    u64 e = l;
    while (e) {
        if (e & 1) res = mulmod_poly(res, base);
        e >>= 1;
        if (e) base = mulmod_poly(base, base);
    }
    return res;
}

static vector<u32> kitamasa(u64 l, int d, const vector<u32>& K,
                            const vector<u32>& F /* = 1/Q mod x^d */) {
    if (d <= 400) return kitamasa_naive(l, d, K);
    KitamasaNTT kn;
    kn.init(d, F, K);
    vector<u32> res(d, 0), base(d, 0);
    res[0] = 1;
    base[1] = 1;                       // x mod T （d >= 2）
    u64 e = l;
    while (e) {
        if (e & 1) kn.reduce_and_mul(res, base, false, res);
        e >>= 1;
        if (e) kn.reduce_and_mul(base, base, true, base);
    }
    return res;
}

#ifndef FORCE_PATH_B
#define FORCE_PATH_B 0
#endif

int main() {
    long long m_, l_, r_;
    if (scanf("%lld %lld %lld", &m_, &l_, &r_) != 3) return 0;
    int m = (int)m_;
    vector<u32> k(m + 1, 0);
    for (int i = 1; i <= m; ++i) {
        long long v;
        if (scanf("%lld", &v) != 1) v = 0;
        k[i] = (u32)(((v % (long long)MOD) + MOD) % (long long)MOD);
    }

    // 输出
    string out;
    auto put = [&](u32 v) { char b[16]; int n = sprintf(b, "%u\n", v); out.append(b, n); };

    int d = m;
    while (d > 0 && k[d] == 0) --d;
    if (l_ > r_) { return 0; }
    long long L = r_ - l_ + 1;

    if (d == 0) {                      // Q(x) = 1，故 a_0 = 1，其余为 0
        for (long long i = l_; i <= r_; ++i) put(i == 0 ? 1u : 0u);
        fwrite(out.data(), 1, out.size(), stdout);
        return 0;
    }

    // Q(x) = 1 - k_1 x - ... - k_d x^d ，K[t] = x^d mod T 的系数 = k_{d-t}
    vector<u32> Q(d + 1, 0), K(d, 0);
    Q[0] = 1;
    for (int i = 1; i <= d; ++i) Q[i] = k[i] ? MOD - k[i] : 0;
    for (int t = 0; t < d; ++t) K[t] = k[d - t];

    bool useDirect;
    {
        long long costDirect = r_ + 1;                 // 直接求到 r+1 项
        long long boundB = (long long)d * 8 + 200000;  // 粗略估计 (B) 的规模
        useDirect = (costDirect <= 4000000 && costDirect <= boundB) || l_ <= 300000;
        if (l_ <= 300000 && r_ + 1 > 8000000) useDirect = false; // 防御：l 小但 r 畸形大
        if (FORCE_PATH_B) useDirect = false;
    }

    if (useDirect) {                                  // (A) 直接做级数
        int need = (int)(r_ + 1);
        vector<u32> A = poly_inv(Q, need);
        for (long long i = l_; i <= r_; ++i) put(A[i]);
    } else {                                          // (B) Kitamasa + 卷积
        int needA = (int)((long long)d + L - 1);      // a_0 .. a_{d+L-2}
        vector<u32> A = poly_inv(Q, needA);
        vector<u32> F(d);
        for (int i = 0; i < d; ++i) F[i] = A[i];
        vector<u32> c = kitamasa((u64)l_, d, K, F);   // x^l mod T
        vector<u32> cr(d);
        for (int i = 0; i < d; ++i) cr[i] = c[d - 1 - i];
        vector<u32> conv = convolution(cr, A);
        for (long long j = 0; j < L; ++j) put(conv[d - 1 + j]);
    }

    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
