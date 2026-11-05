// 202009-5 密信与计数
// 思路：
//   1. 解密密码本中同一页 26 个 out 两两不同 => 每页是密文字母到明文字母的双射，
//      故给定“当前页 p + 想要的明文字母 o”，密文字母 c = inv[p][o] 唯一确定，
//      下一站 next[p][c] 也随之确定。于是：给定词典中某个单词 w 和起始页 p，
//      整段密文以及结束页都被唯一确定（明文 = 单词拼接，分段因“无前缀”而唯一）。
//   2. 对词典建 AC 自动机，用于判定密文中是否出现词典词作为子串。
//   3. DP：状态 = (当前页 p, AC 结点 v)，表示已拼出若干完整单词、密文长度累计为 k
//      且密文尚无词典子串。转移 = 再拼一个单词（预先把 (p,v,word) 的落点与合法性算好）。
//   4. 对每个 k 输出所有状态计数之和。
#include <bits/stdc++.h>
using namespace std;

static const int MOD = 998244353;
static const int MAXNODE = 64;  // 词典总长 <= 50，结点数 <= 51

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    static int outP[50][26], nxtP[50][26];
    for (int i = 0; i < 26; i++) {
        for (int j = 0; j < n; j++) {
            string s;
            cin >> s;
            outP[j][i] = s[0] - 'a';
            nxtP[j][i] = stoi(s.substr(1)) - 1;  // 页号转 0 基
        }
    }

    // 每页的逆置换：inv[p][o] = 使 out 为 o 的密文字母
    static int inv[50][26];
    for (int p = 0; p < n; p++)
        for (int r = 0; r < 26; r++) inv[p][outP[p][r]] = r;

    vector<string> words;
    {
        string s;
        while (cin >> s) words.push_back(s);
    }

    // ---------- AC 自动机（模式 = 词典单词，跑在密文上） ----------
    static int ch[MAXNODE][26];
    static int fail[MAXNODE];
    static char term[MAXNODE];
    for (int i = 0; i < MAXNODE; i++)
        for (int k = 0; k < 26; k++) ch[i][k] = -1;

    int nodes = 1;
    for (const string &w : words) {
        int u = 0;
        for (char cc : w) {
            int k = cc - 'a';
            if (ch[u][k] == -1) ch[u][k] = nodes++;
            u = ch[u][k];
        }
        term[u] = 1;
    }
    {
        queue<int> q;
        fail[0] = 0;
        for (int k = 0; k < 26; k++) {
            int v = ch[0][k];
            if (v != -1) {
                fail[v] = 0;
                q.push(v);
            } else {
                ch[0][k] = 0;  // 缺失边直接补成 0，之后 ch 即自动机的 go 表
            }
        }
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            if (term[fail[u]]) term[u] = 1;
            for (int k = 0; k < 26; k++) {
                int v = ch[u][k];
                if (v != -1) {
                    fail[v] = ch[fail[u]][k];
                    q.push(v);
                } else {
                    ch[u][k] = ch[fail[u]][k];
                }
            }
        }
    }

    const int B = nodes;                    // AC 结点数
    const int W = (int)words.size();
    const long long nB = (long long)n * B;  // 状态总数（页 * AC 结点）

    // ---------- 预处理：状态 idx=(p*B+v) 再拼一个单词 wi 的落点/合法性 ----------
    vector<int> tgt(nB * W);
    vector<char> ok(nB * W, 0);
    vector<int> wlen(W);
    for (int i = 0; i < W; i++) wlen[i] = (int)words[i].size();

    for (int p = 0; p < n; p++) {
        for (int v = 0; v < B; v++) {
            if (term[v]) continue;  // 已匹配到词典词的状态视为非法，不会被用到
            long long idx = (long long)p * B + v;
            for (int wi = 0; wi < W; wi++) {
                int cp = p, cv = v;
                bool good = true;
                for (char cc : words[wi]) {
                    int c = inv[cp][cc - 'a'];   // 该明文字母对应的密文字母
                    cv = ch[cv][c];              // AC 上走一步
                    if (term[cv]) {              // 密文中出现了词典词
                        good = false;
                        break;
                    }
                    cp = nxtP[cp][c];            // 翻页
                }
                tgt[idx * W + wi] = cp * B + cv;
                ok[idx * W + wi] = good ? 1 : 0;
            }
        }
    }

    // ---------- DP ----------
    vector<int> dp((long long)(m + 1) * nB, 0);
    dp[0] = 1;  // k=0，第 1 页（0 基 0），AC 根

    for (int k = 0; k < m; k++) {
        long long base = (long long)k * nB;
        for (long long idx = 0; idx < nB; idx++) {
            int val = dp[base + idx];
            if (!val) continue;
            const int *tg = &tgt[idx * W];
            const char *okp = &ok[idx * W];
            for (int wi = 0; wi < W; wi++) {
                if (!okp[wi]) continue;
                int nk = k + wlen[wi];
                if (nk > m) continue;
                int &dst = dp[(long long)nk * nB + tg[wi]];
                int s = dst + val;
                if (s >= MOD) s -= MOD;
                dst = s;
            }
        }
    }

    string outbuf;
    outbuf.reserve((size_t)m * 12);
    for (int k = 1; k <= m; k++) {
        long long base = (long long)k * nB;
        int ans = 0;
        for (long long idx = 0; idx < nB; idx++) {
            ans += dp[base + idx];
            if (ans >= MOD) ans -= MOD;
        }
        outbuf += to_string(ans);
        outbuf += '\n';
    }
    fputs(outbuf.c_str(), stdout);
    return 0;
}
