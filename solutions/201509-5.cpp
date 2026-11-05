// 201509-5 最佳文章
// AC 自动机 + (max,+) 矩阵快速幂
// 思路:
//   1) 把 n 个重要单词建成 AC 自动机, 节点数 K <= s+1 <= 101 (s 为所有单词字母总数)。
//      对每个节点 v, gain[v] = 该节点 fail 链上所有终止节点数之和,
//      即 "当前已匹配的最长后缀落在 v 时, 新出现的单词个数"。
//   2) 转移矩阵 M[u][v] = 若存在字母 c 使 go(u,c)=v, 则为 gain[v], 否则为 -inf。
//      答案 = 从根 0 出发走恰好 m 步的最大收益 = max_v (e_0 * M^m)[v]。
//   3) m <= 1e15, 用二进制快速幂: 只需 O(log m) 次矩阵*矩阵 (K^3) 与 O(log m) 次向量*矩阵 (K^2)。
// 复杂度: 时间 O(K^3 log m)(K=s+1<=101), 空间 O(K^2)。
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

static const ll NEG = -(1LL << 60);   // 不可达; 答案上界 1e15*100=1e17, 不会溢出

int K;                                 // 状态数 = AC 自动机节点数

// (max,+) 矩阵乘法: C[i][j] = max_k A[i][k] + B[k][j]
// B 先转置, 使内层循环连续访存; 结果小于 NEG/2 的视为不可达(两个 -inf 相加),
// 夹回 NEG, 避免快速幂过程中越加越小导致溢出。
static vector<ll> matmul(const vector<ll> &A, const vector<ll> &B) {
    const ll HALF = NEG / 2;
    vector<ll> Bt((size_t)K * K), C((size_t)K * K, NEG);
    for (int k = 0; k < K; k++) {
        const ll *brow = &B[(size_t)k * K];
        for (int j = 0; j < K; j++) Bt[(size_t)j * K + k] = brow[j];
    }
    for (int i = 0; i < K; i++) {
        const ll *arow = &A[(size_t)i * K];
        ll *crow = &C[(size_t)i * K];
        for (int j = 0; j < K; j++) {
            const ll *brow = &Bt[(size_t)j * K];
            ll best = NEG;
            for (int k = 0; k < K; k++) {
                ll val = arow[k] + brow[k];
                if (val > best) best = val;
            }
            crow[j] = (best > HALF) ? best : NEG;
        }
    }
    return C;
}

// 行向量 * (max,+) 矩阵
static void vecmul(vector<ll> &v, const vector<ll> &M) {
    vector<ll> nv((size_t)K, NEG);
    for (int k = 0; k < K; k++) {
        ll a = v[k];
        if (a == NEG) continue;
        const ll *row = &M[(size_t)k * K];
        for (int j = 0; j < K; j++) {
            ll b = row[j];
            if (b == NEG) continue;
            ll val = a + b;
            if (val > nv[j]) nv[j] = val;
        }
    }
    v.swap(nv);
}

int main() {
    int n;
    long long m;
    if (scanf("%d %lld", &n, &m) != 2) return 0;

    vector<string> words(n);
    for (int i = 0; i < n; i++) {
        char buf[4096];
        scanf("%s", buf);
        words[i] = buf;
    }

    // ---------- 建 trie ----------
    vector<array<int, 26>> nxt(1);
    nxt[0].fill(-1);
    vector<int> fail(1, 0), cnt(1, 0);
    for (const string &w : words) {
        int u = 0;
        for (char ch : w) {
            int c = ch - 'a';
            if (nxt[u][c] == -1) {
                nxt[u][c] = (int)nxt.size();
                array<int, 26> a;
                a.fill(-1);
                nxt.push_back(a);
                fail.push_back(0);
                cnt.push_back(0);
            }
            u = nxt[u][c];
        }
        cnt[u]++;                      // 该单词在此节点终止
    }
    K = (int)nxt.size();

    // ---------- 补全 AC 自动机的 go 函数并累加 fail 链 ----------
    vector<int> q;
    q.reserve(K);
    for (int c = 0; c < 26; c++) {
        if (nxt[0][c] == -1) nxt[0][c] = 0;
        else { fail[nxt[0][c]] = 0; q.push_back(nxt[0][c]); }
    }
    for (size_t head = 0; head < q.size(); head++) {
        int u = q[head];
        // fail[u] 深度更小, 已出队, 其 cnt 已累加完毕
        cnt[u] += cnt[fail[u]];
        for (int c = 0; c < 26; c++) {
            int v = nxt[u][c];
            if (v == -1) {
                nxt[u][c] = nxt[fail[u]][c];
            } else {
                fail[v] = nxt[fail[u]][c];
                q.push_back(v);
            }
        }
    }

    // ---------- 转移矩阵 ----------
    vector<ll> M((size_t)K * K, NEG);
    for (int u = 0; u < K; u++) {
        for (int c = 0; c < 26; c++) {
            int v = nxt[u][c];
            ll g = cnt[v];             // 走到 v 获得的收益
            ll &cell = M[(size_t)u * K + v];
            if (g > cell) cell = g;
        }
    }

    // ---------- 快速幂求 e_0 * M^m ----------
    vector<ll> v((size_t)K, NEG);
    v[0] = 0;
    while (m > 0) {
        if (m & 1LL) vecmul(v, M);
        m >>= 1;
        if (m) M = matmul(M, M);
    }

    ll ans = 0;
    for (int j = 0; j < K; j++) if (v[j] > ans) ans = v[j];
    printf("%lld\n", ans);
    return 0;
}
