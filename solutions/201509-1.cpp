#include <cstdio>

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;

    int segments = 0;
    long long prev = -1;  // ai >= 0，用 -1 作为哨兵
    for (int i = 0; i < n; ++i) {
        long long x;
        if (scanf("%lld", &x) != 1) return 0;
        if (i == 0 || x != prev) ++segments;
        prev = x;
    }

    printf("%d\n", segments);
    return 0;
}
