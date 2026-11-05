// 201703-4 地铁修建
// 题意：选 1 到 n 的一条路径，使路径上用天数最大的那段隧道尽可能小
//       （所有公司同时开工，故总工期 = 路径上最大边权）。
// 即「最小瓶颈路」：答案 = 1 与 n 在最小生成森林中首次连通时的边权。
// 做法：把 m 条候选隧道按天数升序做 Kruskal（并查集合并），
//       第一次让 1 与 n 落在同一集合时的边权就是答案。
// 复杂度 O(m log m) 时间、O(n + m) 空间。
#include <bits/stdc++.h>
using namespace std;

namespace io {
const int SZ = 1 << 16;
char buf[SZ];
int len = 0, pos = 0;
inline int gc() {
    if (pos >= len) {
        len = (int)fread(buf, 1, SZ, stdin);
        pos = 0;
        if (len <= 0) return -1;
    }
    return buf[pos++];
}
inline int readInt() {
    int c = gc();
    while (c != -1 && (c < '0' || c > '9')) c = gc();
    int x = 0;
    while (c >= '0' && c <= '9') { x = x * 10 + (c - '0'); c = gc(); }
    return x;
}
}  // namespace io

const int MAXN = 100005;
const int MAXM = 200005;

static int par[MAXN], rnk[MAXN];

int findRoot(int x) {
    // 带路径压缩（路径减半），迭代实现，无递归
    while (par[x] != x) {
        par[x] = par[par[x]];
        x = par[x];
    }
    return x;
}

struct Edge {
    int a, b, c;
} e[MAXM];

int main() {
    int n = io::readInt();
    int m = io::readInt();

    if (n <= 1) {  // 1 号与 n 号重合，无需施工
        printf("0\n");
        return 0;
    }

    for (int i = 0; i < m; ++i) {
        e[i].a = io::readInt();
        e[i].b = io::readInt();
        e[i].c = io::readInt();
    }

    for (int i = 0; i <= n; ++i) { par[i] = i; rnk[i] = 0; }

    sort(e, e + m, [](const Edge& x, const Edge& y) { return x.c < y.c; });

    int ans = 0;
    for (int i = 0; i < m; ++i) {
        int a = findRoot(e[i].a), b = findRoot(e[i].b);
        if (a == b) continue;
        if (rnk[a] < rnk[b]) swap(a, b);
        par[b] = a;
        if (rnk[a] == rnk[b]) ++rnk[a];
        if (findRoot(1) == findRoot(n)) {  // 首个使两端连通的边权即最小瓶颈
            ans = e[i].c;
            break;
        }
    }

    printf("%d\n", ans);
    return 0;
}
