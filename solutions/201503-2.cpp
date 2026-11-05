#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    vector<int> cnt(1001, 0);
    for (int i = 0; i < n; ++i) {
        int x;
        scanf("%d", &x);
        ++cnt[x];
    }
    vector<int> nums;
    for (int v = 0; v <= 1000; ++v)
        if (cnt[v] > 0) nums.push_back(v);
    sort(nums.begin(), nums.end(), [&](int a, int b) {
        if (cnt[a] != cnt[b]) return cnt[a] > cnt[b];
        return a < b;
    });
    for (int v : nums) printf("%d %d\n", v, cnt[v]);
    return 0;
}
