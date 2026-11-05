#include <bits/stdc++.h>
using namespace std;

// 「有趣的数」：只用 0,1,2,3 且四个都出现；所有 0 在所有 1 之前，
// 所有 2 在所有 3 之前；最高位不为 0。
//
// 计数思路（组合数闭式，O(n) 求和）：
//   把位置按「来自 {0,1}」/「来自 {2,3}」分成两组，大小 a 与 b = n-a。
//   一组内给定顺序要求后，只需选一个「分界点」：
//     {0,1} 组：p 个 0 在前、a-p 个 1 在后，p = 1..a-1（0、1 都出现）共 a-1 种；
//     {2,3} 组：q 个 2 在前、b-q 个 3 在后，q = 1..b-1（2、3 都出现）共 b-1 种。
//   于是「四个数字都出现且两组内部有序」的串数（允许首位为 0）为
//       T = Σ_{a=2}^{n-2} C(n,a) * (a-1) * (b-1),   b = n-a (≥2)
//
//   再减去首位为 0 的串（这些不合法）：第 1 位固定 0，剩下 n-1 位中
//       {0,1} 组取 a 个位置（a ≥ 1，因为 1 必须出现），{2,3} 组取 b = n-1-a 个（b ≥ 2）
//       {0,1} 组内 p 个 0 在前、a-p 个 1 在后，p = 0..a-1（0 已由首位提供，可不再出现）
//         共 a 种；{2,3} 组内 q = 1..b-1 共 b-1 种；
//       且组内 0 都在组内 1 之前、首位 0 又最靠前，顺序约束自动满足。
//       F = Σ_{a=1}^{n-3} C(n-1,a) * a * (b-1),   b = n-1-a
//
//   答案 = (T - F) mod 1e9+7。
//   已用暴力枚举（n=4..9 -> 3,20,85,294,903,2568）与独立状态机 DP（n=4..300）交叉验证。
static const long long MOD = 1000000007LL;

static long long power_mod(long long b, long long e) {
    long long r = 1;
    b %= MOD;
    while (e > 0) {
        if (e & 1) r = r * b % MOD;
        b = b * b % MOD;
        e >>= 1;
    }
    return r;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    int N = n;
    vector<long long> fact(N + 1, 1), inv_fact(N + 1, 1);
    for (int i = 1; i <= N; ++i) fact[i] = fact[i - 1] * i % MOD;
    inv_fact[N] = power_mod(fact[N], MOD - 2);
    for (int i = N; i > 0; --i) inv_fact[i - 1] = inv_fact[i] * i % MOD;
    auto C = [&](int nn, int kk) -> long long {
        if (kk < 0 || kk > nn) return 0;
        return fact[nn] * inv_fact[kk] % MOD * inv_fact[nn - kk] % MOD;
    };

    // T：四个数字都出现、两组内部各自有序的串数（允许首位为 0）
    long long T = 0;
    for (int a = 2; a <= n - 2; ++a) {
        long long b = n - a;                              // b >= 2
        T = (T + C(n, a) * (a - 1) % MOD * ((b - 1) % MOD)) % MOD;
    }

    // F：首位固定为 0 的串数（不合法，需要扣除）
    long long F = 0;
    for (int a = 1; a <= n - 3; ++a) {
        long long b = (n - 1LL) - a;                      // b >= 2
        F = (F + C(n - 1, a) * (a % MOD) % MOD * ((b - 1) % MOD)) % MOD;
    }

    long long ans = (T - F) % MOD;
    if (ans < 0) ans += MOD;
    cout << ans << '\n';
    return 0;
}
