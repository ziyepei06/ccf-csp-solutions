// 202312-5 彩色路径
// 求 0 到 N-1 的点数 <= L、且点上颜色两两不同的最长路径（边权和最大）。
// 关键性质：路径上颜色两两不同 => 点也两两不同（重复点必重复色），故只需对"已用颜色集合"做状态。
// L <= 9 且 K <= 30：整条路径的颜色集合最大 9 种，2^30 无法状压（那只能做 K<=15 的部分分）。
// 做法：折半 + 状态压缩。
//   前半：从 0 出发、点数 a <= 5 的所有彩色路径，记 f[v][mask] = 到 v 的最大长度（mask 为已用颜色集合）。
//   后半：从 N-1 逆着走、点数 b <= 5 的所有彩色路径，记 g[v][mask] = 从 v 到 N-1 的最大长度。
//   任一条 q <= 9 的答案路径总能在第 i=min(4,q-1) 个点 v 处拆成两段（a=i+1<=5, b=q-i<=5），
//   故在 v 处合并：要求 (maskF & maskB) == {C[v]}（只公共 v 这一种颜色，于是颜色全不同）且
//   popcount(maskF)+popcount(maskB)-1 <= L。合并答案取 f+g。
//   每侧状态数上界 ~ sum_{t<=5} C(K-2,t-2) <= 1+28+378+3276 = 3683（含端色 0 / K-1 与 C[v]），
//   合并时按权值降序扫、遇到第一个合法即最优并提前 break。
// 时间 O(L * M * 状态/点 + 合并扫描)，空间 O(N * C(K-2,3))。
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
static const int HALF = 5; // 每侧最多 5 个点（覆盖 L<=9：5+5-1=9）

static int N, M, L, K;
static vector<int> col;
static vector<vector<pair<int, int>>> adj, radj; // (终点/起点, 边长)

int main() {
    if (scanf("%d %d %d %d", &N, &M, &L, &K) != 4) return 0;
    col.resize(N);
    for (int i = 0; i < N; i++) scanf("%d", &col[i]);
    vector<int> U(M), V(M), D(M);
    for (int i = 0; i < M; i++) scanf("%d", &U[i]);
    for (int i = 0; i < M; i++) scanf("%d", &V[i]);
    for (int i = 0; i < M; i++) scanf("%d", &D[i]);

    adj.assign(N, {});
    radj.assign(N, {});
    adj[0].reserve(8);
    for (int j = 0; j < M; j++) {
        adj[U[j]].push_back(make_pair(V[j], D[j]));
        radj[V[j]].push_back(make_pair(U[j], D[j]));
    }

    // bestF[v]: 0 -> v 的彩色路径（点数 <= 5、颜色集合 mask）的最大边权和
    // bestB[v]: v -> N-1 的彩色路径（点数 <= 5、颜色集合 mask）的最大边权和
    vector<unordered_map<int, ll>> bestF(N), bestB(N);
    for (int v = 0; v < N; v++) {
        bestF[v].reserve(64);
        bestB[v].reserve(64);
    }

    {
        vector<unordered_map<int, ll>> cur(N), nxt(N);
        for (int v = 0; v < N; v++) { cur[v].reserve(64); nxt[v].reserve(64); }
        // 前半
        cur[0][1 << col[0]] = 0;
        bestF[0][1 << col[0]] = 0;
        for (int t = 1; t < HALF; t++) {
            for (int i = 0; i < N; i++) nxt[i].clear();
            bool any = false;
            for (int v = 0; v < N; v++) {
                if (cur[v].empty()) continue;
                for (unordered_map<int, ll>::iterator it = cur[v].begin(); it != cur[v].end(); ++it) {
                    int mask = it->first;
                    ll w = it->second;
                    const vector<pair<int, int>> &es = adj[v];
                    for (size_t k = 0; k < es.size(); k++) {
                        int u = es[k].first;
                        if ((mask >> col[u]) & 1) continue;
                        int nm = mask | (1 << col[u]);
                        ll nw = w + es[k].second;
                        unordered_map<int, ll>::iterator jt = nxt[u].find(nm);
                        if (jt == nxt[u].end()) nxt[u].emplace(nm, nw);
                        else if (jt->second < nw) jt->second = nw;
                        any = true;
                    }
                }
            }
            if (!any) break;
            swap(cur, nxt);
            for (int v = 0; v < N; v++) {
                for (unordered_map<int, ll>::iterator it = cur[v].begin(); it != cur[v].end(); ++it) {
                    unordered_map<int, ll>::iterator jt = bestF[v].find(it->first);
                    if (jt == bestF[v].end()) bestF[v].emplace(it->first, it->second);
                    else if (jt->second < it->second) jt->second = it->second;
                }
            }
        }
        // 后半（沿反向边从 N-1 走）
        for (int i = 0; i < N; i++) cur[i].clear();
        cur[N - 1][1 << col[N - 1]] = 0;
        bestB[N - 1][1 << col[N - 1]] = 0;
        for (int t = 1; t < HALF; t++) {
            for (int i = 0; i < N; i++) nxt[i].clear();
            bool any = false;
            for (int v = 0; v < N; v++) {
                if (cur[v].empty()) continue;
                for (unordered_map<int, ll>::iterator it = cur[v].begin(); it != cur[v].end(); ++it) {
                    int mask = it->first;
                    ll w = it->second;
                    const vector<pair<int, int>> &es = radj[v];
                    for (size_t k = 0; k < es.size(); k++) {
                        int u = es[k].first;
                        if ((mask >> col[u]) & 1) continue;
                        int nm = mask | (1 << col[u]);
                        ll nw = w + es[k].second;
                        unordered_map<int, ll>::iterator jt = nxt[u].find(nm);
                        if (jt == nxt[u].end()) nxt[u].emplace(nm, nw);
                        else if (jt->second < nw) jt->second = nw;
                        any = true;
                    }
                }
            }
            if (!any) break;
            swap(cur, nxt);
            for (int v = 0; v < N; v++) {
                for (unordered_map<int, ll>::iterator it = cur[v].begin(); it != cur[v].end(); ++it) {
                    unordered_map<int, ll>::iterator jt = bestB[v].find(it->first);
                    if (jt == bestB[v].end()) bestB[v].emplace(it->first, it->second);
                    else if (jt->second < it->second) jt->second = it->second;
                }
            }
        }
    }

    // 在共同点 v 处合并两段
    vector<vector<pair<int, ll>>> B(N);
    for (int v = 0; v < N; v++) {
        B[v].assign(bestB[v].begin(), bestB[v].end());
        sort(B[v].begin(), B[v].end(),
             [](const pair<int, ll> &x, const pair<int, ll> &y) { return x.second > y.second; });
    }

    const ll NEG = -(1LL << 62);
    ll ans = NEG;
    for (int v = 0; v < N; v++) {
        if (bestF[v].empty() || B[v].empty()) continue;
        int cbit = 1 << col[v];
        vector<pair<int, ll>> F(bestF[v].begin(), bestF[v].end());
        sort(F.begin(), F.end(),
             [](const pair<int, ll> &x, const pair<int, ll> &y) { return x.second > y.second; });
        ll maxB = B[v][0].second;
        for (size_t fi = 0; fi < F.size(); fi++) {
            int mf = F[fi].first;
            ll wf = F[fi].second;
            if (wf + maxB <= ans) break; // 后面的 wf 更小，组合不出更优
            int a = __builtin_popcount((unsigned)mf);
            for (size_t bi = 0; bi < B[v].size(); bi++) {
                int mb = B[v][bi].first;
                ll wb = B[v][bi].second;
                if (wf + wb <= ans) break; // B 按权值降序，后面更小
                if (a + __builtin_popcount((unsigned)mb) - 1 > L) continue;
                if ((mf & mb) != cbit) continue; // 除 v 外颜色必须全不同
                ans = wf + wb;
                break;
            }
        }
    }

    if (ans == NEG) ans = 0;
    printf("%lld\n", ans);
    return 0;
}
