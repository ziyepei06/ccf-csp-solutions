#include <bits/stdc++.h>
using namespace std;

int main() {
    int n, k;
    if (!(scanf("%d %d", &n, &k) == 2)) return 0;
    if (n < 0) n = 0;
    vector<char> inited(n + 1, 0);   // inited[v] = 变量 a_v 是否已作为左值出现过
    long long ans = 0;
    for (int i = 0; i < k; ++i) {
        int x, y;
        if (scanf("%d %d", &x, &y) != 2) break;
        // 右值为常量 a0 时无需初始化检查
        if (y != 0 && y <= n && !inited[y]) ++ans;
        // 本条语句给左值 a_x 完成初始化，供后续语句使用
        if (x >= 0 && x <= n) inited[x] = 1;
    }
    printf("%lld\n", ans);
    return 0;
}
