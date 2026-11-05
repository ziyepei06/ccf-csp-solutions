// 201403-5 任务调度
//
// 关键结构：四类执行方式中，方式 2/4 独占两个 CPU，所以它们必须两两串行，
// 构成时轴上的一段段“整机块”；其余任务（方式 1/3，只用 1 个 CPU）只能落在
// 这些整机块之间的空隙里。把整机块全部右移到末尾、所有空隙合并成一个长度 G
// 的连续区间不会变差（每个 CPU 上的单 CPU 任务总时长本来就不超过空隙总和）。
//
// 于是总时间 T = B + G，其中
//   B = 整机块总时长，
//   G = 单 CPU 区间的时长，需同时 ≥ L1, L2（两个 CPU 各自的单区负载）与 gl
//       （单区内 GPU 负载，因为 GPU 一次只能被一个 c 方式任务占用）。
// 单区（2 CPU + 1 GPU）可排布当且仅当 L1,L2,gl <= G（每个任务时长为正且已计入
// 对应负载，故不会被 G 截断）。
//
// 令 u = B + L1, v = B + L2, w = B + gl，则 T = max(u, v, w)。
// 每个任务的迁移（两种 CPU 对称）：
//   方式 1（单 CPU）      : u += a          （或 v += a）
//   方式 3（单 CPU + GPU）: u += c, w += c  （或 v += c, w += c）
//   方式 2 / 4（整机块）  : u,v,w 同时 += min(b, d)
// 方式 4 的 GPU 只在整机块内、块与块串行，故 GPU 在整机块中永不冲突，
// 只需取 min(b, d)；若 c >= a 则方式 3 被方式 1 支配，无需考虑。
//
// DP: dp[u][v] = 达到 (u,v) 所需的最小 w。答案 = min max(u, v, dp[u][v])。
// u,v ≤ sum(a) ≤ 40*10 = 400，状态 401^2，转移 5 种。
#include <bits/stdc++.h>
using namespace std;

static const int MX = 405;
static const int INF = 1e9;
static int cur[MX][MX], nxt[MX][MX];

int main() {
    int n;
    if (!(cin >> n)) return 0;

    for (int i = 0; i < MX; ++i)
        for (int j = 0; j < MX; ++j) cur[i][j] = INF;
    cur[0][0] = 0;
    int hi = 0;  // 当前可能的最大坐标，剪掉无用的扫描

    for (int t = 0; t < n; ++t) {
        int a, b, c, d;
        cin >> a >> b >> c >> d;
        int m = min(b, d);              // 整机块（方式 2/4 取快者）
        bool useC = (c < a);            // 只有 c < a 时方式 3 才不被方式 1 支配

        for (int i = 0; i <= hi + 10; ++i)
            for (int j = 0; j <= hi + 10; ++j) nxt[i][j] = INF;
        int nhi = hi + max({a, useC ? c : 0, m});

        for (int i = 0; i <= hi; ++i) {
            for (int j = 0; j <= hi; ++j) {
                int w = cur[i][j];
                if (w >= INF) continue;
                // 方式 1：单 CPU
                if (w < nxt[i + a][j]) nxt[i + a][j] = w;
                if (w < nxt[i][j + a]) nxt[i][j + a] = w;
                // 方式 3：单 CPU + GPU
                if (useC) {
                    if (w + c < nxt[i + c][j]) nxt[i + c][j] = w + c;
                    if (w + c < nxt[i][j + c]) nxt[i][j + c] = w + c;
                }
                // 方式 2 / 4：整机块
                if (w + m < nxt[i + m][j + m]) nxt[i + m][j + m] = w + m;
            }
        }
        hi = nhi;
        for (int i = 0; i <= hi; ++i)
            for (int j = 0; j <= hi; ++j) cur[i][j] = nxt[i][j];
    }

    int ans = INF;
    for (int i = 0; i <= hi; ++i)
        for (int j = 0; j <= hi; ++j) {
            int w = cur[i][j];
            if (w >= INF) continue;
            ans = min(ans, max(max(i, j), w));
        }
    cout << ans << '\n';
    return 0;
}
