#include <bits/stdc++.h>
using namespace std;

int main() {
    int q;
    if (!(cin >> q)) return 0;
    while (q--) {
        long long n;
        long long k;
        cin >> n >> k;
        long long rest = n;
        long long ans = 1;
        for (long long p = 2; p * p <= rest; ++p) {
            if (rest % p != 0) continue;
            long long e = 0;
            while (rest % p == 0) {
                rest /= p;
                ++e;
            }
            if (e >= k) {
                for (long long i = 0; i < e; ++i) ans *= p;
            }
        }
        // 剩余部分为 1 或一个素因子（指数为 1）
        if (rest > 1 && k <= 1) ans *= rest;
        cout << ans << '\n';
    }
    return 0;
}
