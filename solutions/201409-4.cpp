#include <bits/stdc++.h>
using namespace std;

// 最优配餐：所有分店做多源 BFS，得到每个客户格点的最短距离，
// 成本 = sum(ci * dist)，多源 BFS 保证了每个客户取最近分店，
// 且各分店无配送上限、彼此不冲突，故贪心最短路即为全局最优。

static const int BUFSZ = 1 << 22;
static char ibuf[BUFSZ];
static int ilen = 0, ipos = 0;
static inline int gc() {
    if (ipos == ilen) {
        ilen = (int)fread(ibuf, 1, BUFSZ, stdin);
        ipos = 0;
        if (ilen <= 0) return -1;
    }
    return ibuf[ipos++];
}
static inline long long readInt() {
    int c = gc();
    while (c != -1 && (c < '0' || c > '9')) c = gc();
    long long v = 0;
    while (c >= '0' && c <= '9') { v = v * 10 + (c - '0'); c = gc(); }
    return v;
}

int main() {
    int n = (int)readInt();
    long long m = readInt(), k = readInt(), d = readInt();
    int N = n * n;

    vector<int> shop;
    shop.reserve((size_t)m);
    for (long long i = 0; i < m; i++) {
        int x = (int)readInt() - 1, y = (int)readInt() - 1;
        shop.push_back(x * n + y);
    }
    vector<int> cx((size_t)k), cy((size_t)k), cc((size_t)k);
    for (long long i = 0; i < k; i++) {
        cx[(size_t)i] = (int)readInt() - 1;
        cy[(size_t)i] = (int)readInt() - 1;
        cc[(size_t)i] = (int)readInt();
    }
    vector<char> blocked((size_t)N, 0);
    for (long long i = 0; i < d; i++) {
        int x = (int)readInt() - 1, y = (int)readInt() - 1;
        blocked[(size_t)(x * n + y)] = 1;
    }

    // 多源 BFS
    vector<int> dist((size_t)N, -1);
    vector<int> q;
    q.reserve((size_t)N);
    for (size_t i = 0; i < shop.size(); i++) {
        int s = shop[i];
        if (dist[(size_t)s] == -1) {
            dist[(size_t)s] = 0;
            q.push_back(s);
        }
    }
    for (size_t h = 0; h < q.size(); h++) {
        int u = q[h];
        int nd = dist[(size_t)u] + 1;
        int ux = u / n, uy = u % n;
        int v;
        if (ux > 0) {
            v = u - n;
            if (dist[(size_t)v] == -1 && !blocked[(size_t)v]) { dist[(size_t)v] = nd; q.push_back(v); }
        }
        if (ux < n - 1) {
            v = u + n;
            if (dist[(size_t)v] == -1 && !blocked[(size_t)v]) { dist[(size_t)v] = nd; q.push_back(v); }
        }
        if (uy > 0) {
            v = u - 1;
            if (dist[(size_t)v] == -1 && !blocked[(size_t)v]) { dist[(size_t)v] = nd; q.push_back(v); }
        }
        if (uy < n - 1) {
            v = u + 1;
            if (dist[(size_t)v] == -1 && !blocked[(size_t)v]) { dist[(size_t)v] = nd; q.push_back(v); }
        }
    }

    long long ans = 0;
    for (long long i = 0; i < k; i++) {
        int p = cx[(size_t)i] * n + cy[(size_t)i];
        int dv = dist[(size_t)p];
        if (dv < 0) dv = 0; // 题面保证可送达，此处仅防御
        ans += (long long)cc[(size_t)i] * dv;
    }
    printf("%lld\n", ans);
    return 0;
}
