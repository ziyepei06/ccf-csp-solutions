// 202109-3 脉冲神经网络 (SNN)
// Izhikevich 神经元模型 + 脉冲源伪随机发放 + 突触传播延迟。
// 每个时间步 O(N + outdeg(firing))；脉冲按到达时刻入桶(仅保留 <=T 的到达)。
#include <bits/stdc++.h>
using namespace std;

static unsigned long long rnd_state = 1;
static inline int myrand_() {
    rnd_state = rnd_state * 1103515245ULL + 12345ULL;
    return (int)((rnd_state / 65536ULL) % 32768ULL);
}

int main() {
    int N, S, P, T;
    if (scanf("%d %d %d %d", &N, &S, &P, &T) != 4) return 0;
    double dt;
    scanf("%lf", &dt);

    vector<double> v(N), u(N), a(N), b(N), c(N), d(N);
    {
        int idx = 0;
        for (int line = 0; line < N; ) {
            int rn; double vv, uu, aa, bb, cc, dd;
            if (scanf("%d %lf %lf %lf %lf %lf %lf", &rn, &vv, &uu, &aa, &bb, &cc, &dd) != 7) break;
            for (int j = 0; j < rn && idx < N; ++j, ++idx) {
                v[idx] = vv; u[idx] = uu;
                a[idx] = aa; b[idx] = bb; c[idx] = cc; d[idx] = dd;
            }
            line += rn;
        }
    }

    vector<int> srcR(P);
    for (int i = 0; i < P; ++i) scanf("%d", &srcR[i]);

    // 突触按入结点建 CSR：结点 0..N-1 为神经元，N..N+P-1 为脉冲源
    int V = N + P;
    vector<int> head(V + 1, 0);
    vector<int> synS(S), synT(S), synD(S);
    vector<double> synW(S);
    for (int i = 0; i < S; ++i) {
        int s, t, D; double w;
        scanf("%d %d %lf %d", &s, &t, &w, &D);
        synS[i] = s; synT[i] = t; synW[i] = w; synD[i] = D;
        ++head[s + 1];
    }
    for (int i = 0; i < V; ++i) head[i + 1] += head[i];
    // 依 head 排布：outIdx 为按入结点聚合后的顺序
    vector<int> pos(head.begin(), head.end()), outS(S), outT(S), outD(S);
    vector<double> outW(S);
    for (int i = 0; i < S; ++i) {
        int p = pos[synS[i]]++;
        outS[p] = synS[i]; outT[p] = synT[i]; outW[p] = synW[i]; outD[p] = synD[i];
    }

    // 到达桶：arr[k] 保存时刻 k 到达神经元的 (目标, 强度)
    vector<vector<pair<int, double> > > arr(T + 1);

    vector<long long> cnt(N, 0);
    vector<double> I(N, 0.0);

    for (int k = 1; k <= T; ++k) {
        // 1. 接收脉冲
        if (!arr[k].empty()) {
            for (size_t i = 0; i < arr[k].size(); ++i) {
                I[arr[k][i].first] += arr[k][i].second;
            }
            vector<pair<int, double> >().swap(arr[k]);
        }
        // 2. 神经元状态更新（前向欧拉，均用 k-1 时刻的取值）
        for (int i = 0; i < N; ++i) {
            double vi = v[i], ui = u[i];
            double vn = vi + dt * (0.04 * vi * vi + 5.0 * vi + 140.0 - ui) + I[i];
            double un = ui + dt * (a[i] * (b[i] * vi - ui));
            I[i] = 0.0;
            v[i] = vn; u[i] = un;
        }
        // 3. 发放判定
        for (int i = 0; i < N; ++i) {
            if (v[i] >= 30.0) {
                v[i] = c[i];
                u[i] += d[i];
                ++cnt[i];
                for (int e = head[i]; e < head[i + 1]; ++e) {
                    int at = k + outD[e];
                    if (at <= T) arr[at].push_back(make_pair(outT[e], outW[e]));
                }
            }
        }
        // 4. 脉冲源按编号顺序发放
        for (int p = 0; p < P; ++p) {
            int r = myrand_();
            if (srcR[p] > r) {
                int node = N + p;
                for (int e = head[node]; e < head[node + 1]; ++e) {
                    int at = k + outD[e];
                    if (at <= T) arr[at].push_back(make_pair(outT[e], outW[e]));
                }
            }
        }
    }

    double minv = 1e300, maxv = -1e300;
    long long minc = LLONG_MAX, maxc = LLONG_MIN;
    for (int i = 0; i < N; ++i) {
        if (v[i] < minv) minv = v[i];
        if (v[i] > maxv) maxv = v[i];
        if (cnt[i] < minc) minc = cnt[i];
        if (cnt[i] > maxc) maxc = cnt[i];
    }
    printf("%.3f %.3f\n%lld %lld\n", minv, maxv, minc, maxc);
    return 0;
}
