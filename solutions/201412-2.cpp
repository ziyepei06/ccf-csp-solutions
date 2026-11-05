#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    vector<vector<int>> a(n, vector<int>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) scanf("%d", &a[i][j]);

    vector<int> out;
    out.reserve((size_t)n * n);
    // 按反对角线 i+j=s 输出；偶数 s 沿右上方向，奇数 s 沿左下方向
    for (int s = 0; s <= 2 * n - 2; ++s) {
        int lo = max(0, s - n + 1), hi = min(n - 1, s);
        if (s % 2 == 0) {
            for (int i = hi; i >= lo; --i) out.push_back(a[i][s - i]);
        } else {
            for (int i = lo; i <= hi; ++i) out.push_back(a[i][s - i]);
        }
    }

    for (size_t k = 0; k < out.size(); ++k) {
        if (k) putchar(' ');
        printf("%d", out[k]);
    }
    putchar('\n');
    return 0;
}
