#include <bits/stdc++.h>
using namespace std;

int main() {
    int N;
    if (scanf("%d", &N) != 1) return 0;
    int n = N / 10;                      // 能买的瓶数
    // 每买5瓶送2瓶（7瓶/5瓶，单位花费收益 1.4），比每买3瓶送1瓶（4/3≈1.333）更划算，
    // 所以先把钱尽量花在 5 瓶一组上，剩下的再凑 3 瓶一组，最后零头按原价。
    int ans = n / 5 * 7 + (n % 5) / 3 * 4 + (n % 5) % 3;
    printf("%d\n", ans);
    return 0;
}
