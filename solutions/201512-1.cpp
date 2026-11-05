#include <cstdio>

int main() {
    long long n;
    if (scanf("%lld", &n) != 1) return 0;
    if (n < 0) n = -n;
    int sum = 0;
    do {
        sum += static_cast<int>(n % 10);
        n /= 10;
    } while (n > 0);
    printf("%d\n", sum);
    return 0;
}
