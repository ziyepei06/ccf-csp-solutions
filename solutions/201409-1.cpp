#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    vector<long long> a(n);
    for (int i = 0; i < n; ++i) scanf("%lld", &a[i]);

    sort(a.begin(), a.end());
    long long ans = 0;
    for (int i = 1; i < n; ++i) {
        if (a[i] - a[i - 1] == 1) ++ans;
    }
    printf("%lld\n", ans);
    return 0;
}
