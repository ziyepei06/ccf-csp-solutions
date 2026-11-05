#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    vector<int> a(n + 2, 0);
    int maxv = 0;
    for (int i = 1; i <= n; ++i) {
        scanf("%d", &a[i]);
        if (a[i] > maxv) maxv = a[i];
    }
    // 按数值分桶：bucket[v] 存放所有值为 v 的下标
    vector<vector<int>> bucket(maxv + 1);
    for (int i = 1; i <= n; ++i)
        if (a[i] > 0) bucket[a[i]].push_back(i);

    // 从大到小扫 p：把值 >= p 的位置视为"激活"，统计激活位置形成的连续段数
    vector<char> active(n + 2, 0);
    int seg = 0, ans = 0;
    for (int v = maxv; v >= 1; --v) {
        for (int idx : bucket[v]) {
            active[idx] = 1;
            int l = active[idx - 1], r = active[idx + 1];
            if (!l && !r) ++seg;          // 新开一段
            else if (l && r) --seg;       // 把左右两段连成一段
        }
        if (seg > ans) ans = seg;
    }
    printf("%d\n", ans);
    return 0;
}
