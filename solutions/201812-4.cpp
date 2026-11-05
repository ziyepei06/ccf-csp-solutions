#include <cstdio>
#include <cstdlib>
#include <algorithm>

// 201812-4 数据中心
// 题意：给无向带权图，选一棵生成树使根的"最大深度层内最大边权"最小。
// 等价于最小化生成树中的最大边权 => 最小瓶颈生成树 = 最小生成树。
// 故答案为 Kruskal 求 MST 时最后一条被选入的边的权值（MST 的最大边）。

struct Edge {
    int u, v, w;
};

static bool cmpEdge(const Edge &a, const Edge &b) { return a.w < b.w; }

static int *par;
static int *sz;

static int findRoot(int x) {
    int r = x;
    while (par[r] != r) r = par[r];
    while (par[x] != r) { int nxt = par[x]; par[x] = r; x = nxt; }
    return r;
}

static bool unite(int a, int b) {
    int ra = findRoot(a), rb = findRoot(b);
    if (ra == rb) return false;
    if (sz[ra] < sz[rb]) { int t = ra; ra = rb; rb = t; }
    par[rb] = ra;
    sz[ra] += sz[rb];
    return true;
}

// 快速输入
static const int BUFSZ = 1 << 20;
static char ibuf[BUFSZ];
static int ipos = 0, ilen = 0;

static inline int readChar() {
    if (ipos == ilen) {
        ilen = (int)fread(ibuf, 1, BUFSZ, stdin);
        ipos = 0;
        if (ilen <= 0) return -1;
    }
    return (unsigned char)ibuf[ipos++];
}

static bool readInt(int &out) {
    int c = readChar();
    while (c != -1 && (c < '0' || c > '9')) c = readChar();
    if (c == -1) return false;
    int x = 0;
    while (c >= '0' && c <= '9') {
        x = x * 10 + (c - '0');
        c = readChar();
    }
    out = x;
    return true;
}

int main() {
    int n, m, root;
    if (!readInt(n)) return 0;
    if (!readInt(m)) return 0;
    if (!readInt(root)) return 0;

    Edge *edges = (Edge *)malloc(sizeof(Edge) * (size_t)(m > 0 ? m : 1));
    int maxV = n;
    for (int i = 0; i < m; i++) {
        int u, v, w;
        readInt(u); readInt(v); readInt(w);
        edges[i].u = u; edges[i].v = v; edges[i].w = w;
        if (u > maxV) maxV = u;
        if (v > maxV) maxV = v;
    }

    std::sort(edges, edges + m, cmpEdge);

    int N = maxV + 1;
    par = (int *)malloc(sizeof(int) * (size_t)(N + 1));
    sz = (int *)malloc(sizeof(int) * (size_t)(N + 1));
    for (int i = 0; i <= N; i++) { par[i] = i; sz[i] = 1; }

    int ans = 0, used = 0;
    for (int i = 0; i < m && used < n - 1; i++) {
        if (unite(edges[i].u, edges[i].v)) {
            ans = edges[i].w;   // 排序后最后选入的边权即 MST 最大边
            used++;
        }
    }

    printf("%d\n", ans);
    free(edges); free(par); free(sz);
    return 0;
}
