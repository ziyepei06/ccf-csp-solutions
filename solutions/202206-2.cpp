#include <bits/stdc++.h>
using namespace std;

int main() {
    int n, S;
    long long L;
    if (scanf("%d %lld %d", &n, &L, &S) != 3) return 0;

    vector<pair<long long, long long>> trees(n);
    for (int k = 0; k < n; ++k) {
        long long x, y;
        scanf("%lld %lld", &x, &y);
        trees[k] = {x, y};
    }

    // 藏宝图 B[i][j]，i、j 为 0..S；输入顺序是先 B[S][*]，最后 B[0][*]
    vector<vector<int>> B(S + 1, vector<int>(S + 1, 0));
    for (int i = S; i >= 0; --i)
        for (int j = 0; j <= S; ++j) {
            int v;
            scanf("%d", &v);
            B[i][j] = v;
        }

    // 把树的坐标压成 64 位整数放进哈希表，O(1) 查询某个格子是否种树
    auto key = [](long long x, long long y) -> unsigned long long {
        return (unsigned long long)x * 2000000003ULL + (unsigned long long)y;
    };
    unordered_set<unsigned long long> pos;
    pos.reserve((size_t)n * 2 + 10);
    for (auto &t : trees) pos.insert(key(t.first, t.second));

    long long ans = 0;
    // 枚举宝藏位置（必然是某棵树的位置），逐格与 B 比对
    for (auto &t : trees) {
        long long x = t.first, y = t.second;
        if (x + S > L || y + S > L) continue;  // 右上角越界，不可能
        bool ok = true;
        for (int i = 0; i <= S && ok; ++i) {
            for (int j = 0; j <= S; ++j) {
                bool has = pos.count(key(x + i, y + j)) != 0;
                if (has != (B[i][j] != 0)) { ok = false; break; }
            }
        }
        if (ok) ++ans;
    }

    printf("%lld\n", ans);
    return 0;
}
