#include <cstdio>
#include <vector>
using namespace std;

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    vector<long long> a(n);
    for (int i = 0; i < n; ++i) scanf("%lld", &a[i]);

    // 数据保证有序（升/降序皆可），故最值取首尾即可
    long long mx = a[0] > a[n - 1] ? a[0] : a[n - 1];
    long long mn = a[0] < a[n - 1] ? a[0] : a[n - 1];

    if (n % 2 == 1) {
        printf("%lld %lld %lld\n", mx, a[n / 2], mn);
    } else {
        long long s = a[n / 2 - 1] + a[n / 2];
        if (s % 2 == 0) {
            printf("%lld %lld %lld\n", mx, s / 2, mn);
        } else {
            // 和的一半带 .5，按四舍五入保留 1 位小数输出（负数同样正确）
            printf("%lld %.1f %lld\n", mx, s / 2.0, mn);
        }
    }
    return 0;
}
