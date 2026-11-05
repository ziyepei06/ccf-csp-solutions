#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    if (scanf("%d", &n) != 1) return 0;
    const int MAXV = 10000;
    vector<int> cnt(MAXV + 1, 0);
    int best = -1, bestCnt = -1;
    for (int i = 0; i < n; ++i) {
        int x;
        scanf("%d", &x);
        ++cnt[x];
    }
    for (int v = 1; v <= MAXV; ++v) {
        if (cnt[v] > bestCnt) {
            bestCnt = cnt[v];
            best = v;
        }
    }
    printf("%d\n", best);
    return 0;
}
