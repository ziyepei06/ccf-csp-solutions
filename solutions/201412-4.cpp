#include <bits/stdc++.h>
using namespace std;

// ---- 快速输入（缓冲读入，不读文件） ----
static const int BUFSZ = 1 << 16;
static char ibuf[BUFSZ];
static int ipos = 0, ilen = 0;

static inline int gc() {
    if (ipos == ilen) {
        ilen = (int)fread(ibuf, 1, BUFSZ, stdin);
        ipos = 0;
        if (ilen <= 0) return -1;
    }
    return (unsigned char)ibuf[ipos++];
}

static long long readInt() {
    int c = gc();
    while (c != -1 && (c < '0' || c > '9') && c != '-') c = gc();
    int sign = 1;
    if (c == '-') {
        sign = -1;
        c = gc();
    }
    long long v = 0;
    while (c >= '0' && c <= '9') {
        v = v * 10 + (c - '0');
        c = gc();
    }
    return v * sign;
}

struct DSU {
    vector<int> p, r;
    explicit DSU(int n) : p(n + 1), r(n + 1, 0) {
        for (int i = 0; i <= n; ++i) p[i] = i;
    }
    int find(int x) {
        while (p[x] != x) {
            p[x] = p[p[x]];
            x = p[x];
        }
        return x;
    }
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (r[a] < r[b]) swap(a, b);
        p[b] = a;
        if (r[a] == r[b]) ++r[a];
        return true;
    }
};

struct Edge {
    int u, v, w;
};

int main() {
    int n = (int)readInt();
    int m = (int)readInt();
    if (n <= 0) {
        printf("0\n");
        return 0;
    }

    vector<Edge> edges;
    edges.reserve(m);
    for (int i = 0; i < m; ++i) {
        int a = (int)readInt();
        int b = (int)readInt();
        int c = (int)readInt();
        edges.push_back({a, b, c});
    }

    sort(edges.begin(), edges.end(),
         [](const Edge &x, const Edge &y) { return x.w < y.w; });

    // 水井在 1 号麦田，所有麦田都要（直接或经中转）连到 1 号，
    // 因此最小费用就是整张图的最小生成树权值。Kruskal 求解。
    DSU dsu(n);
    long long total = 0;
    int used = 0;
    for (const Edge &e : edges) {
        if (dsu.unite(e.u, e.v)) {
            total += e.w;
            if (++used == n - 1) break;
        }
    }

    printf("%lld\n", total);
    return 0;
}
