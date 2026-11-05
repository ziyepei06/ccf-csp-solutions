#include <bits/stdc++.h>
using namespace std;

int main() {
    int N, K;
    if (scanf("%d %d", &N, &K) != 2) return 0;

    // hook[i] = key id hanging on hook i, 0 means empty
    vector<int> hook(N + 1);
    for (int i = 1; i <= N; ++i) hook[i] = i;

    vector<int> w(K), s(K), c(K);
    int maxT = 0;
    for (int i = 0; i < K; ++i) {
        scanf("%d %d %d", &w[i], &s[i], &c[i]);
        maxT = max(maxT, s[i] + c[i]);
    }

    for (int t = 1; t <= maxT; ++t) {
        // 1) returns first, in increasing key order (leftmost empty hook)
        vector<int> ret;
        for (int i = 0; i < K; ++i)
            if (s[i] + c[i] == t) ret.push_back(w[i]);
        sort(ret.begin(), ret.end());
        for (int key : ret) {
            for (int p = 1; p <= N; ++p) {
                if (hook[p] == 0) { hook[p] = key; break; }
            }
        }
        // 2) then borrows: take the key, leaving an empty hook
        for (int i = 0; i < K; ++i) {
            if (s[i] == t) {
                for (int p = 1; p <= N; ++p) {
                    if (hook[p] == w[i]) { hook[p] = 0; break; }
                }
            }
        }
    }

    for (int i = 1; i <= N; ++i) {
        if (i > 1) putchar(' ');
        printf("%d", hook[i]);
    }
    putchar('\n');
    return 0;
}
