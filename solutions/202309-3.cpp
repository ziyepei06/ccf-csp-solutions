// 202309-3 梯度求解
// 思路：把逆波兰式建成表达式树。建树时先压子节点再建父节点，因此节点下标即后缀序：
// 子节点下标恒小于父节点。于是
//   1) 正向：按 0..N-1 顺序对给定点求每个节点取值（模 1e9+7）；
//   2) 反向（reverse-mode AD）：按 N-1..0 倒序把 du/dnode 累加到子节点：
//      + : 两边都加；  - : 左加右减；  * : 左 += v_r，右 += v_l。
// 根 adj=1，最后把所有 x_i 叶子节点的 adj 求和即为 du/dx_i。
// 每次询问只需一次正向、一次反向遍历，复杂度 O(m * N)。
// 预处理变量叶子链表：若某个自变量根本不出现，直接输出 0，省去整趟遍历。
#include <bits/stdc++.h>
using namespace std;

static const long long MOD = 1000000007LL;

int main() {
    // 一次性读入全部输入，之后按空白切分 token
    string in;
    {
        char buf[1 << 20];
        size_t r;
        while ((r = fread(buf, 1, sizeof buf, stdin)) > 0) in.append(buf, r);
    }
    size_t pos = 0, len = in.size();
    auto nextTok = [&](size_t &a, size_t &b) -> bool {
        while (pos < len && isspace((unsigned char)in[pos])) ++pos;
        if (pos >= len) return false;
        a = pos;
        while (pos < len && !isspace((unsigned char)in[pos])) ++pos;
        b = pos;
        return true;
    };
    auto readInt = [&]() -> long long {
        size_t a, b;
        if (!nextTok(a, b)) return 0;
        bool neg = false;
        size_t i = a;
        if (in[i] == '-') { neg = true; ++i; }
        else if (in[i] == '+') ++i;
        long long v = 0;
        for (; i < b; ++i) v = v * 10 + (in[i] - '0');
        return neg ? -v : v;
    };

    int n = (int)readInt();
    int m = (int)readInt();

    // 表达式独占一行（单元素表达式也只有 1 个 token），故按行取范围再切 token
    while (pos < len && in[pos] != '\n') ++pos;   // 跳过第一行剩余部分
    if (pos < len) ++pos;
    size_t eB = pos, eE;
    for (;;) {                                    // 跳过可能多余的空行
        eE = eB;
        while (eE < len && in[eE] != '\n') ++eE;
        bool blank = true;
        for (size_t i = eB; i < eE; ++i)
            if (!isspace((unsigned char)in[i])) { blank = false; break; }
        if (!blank || eE >= len) break;
        eB = eE + 1;
    }
    size_t ep = eB;
    auto nextExprTok = [&](size_t &a, size_t &b) -> bool {
        while (ep < eE && isspace((unsigned char)in[ep])) ++ep;
        if (ep >= eE) return false;
        a = ep;
        while (ep < eE && !isspace((unsigned char)in[ep])) ++ep;
        b = ep;
        return true;
    };

    enum { T_CONST, T_VAR, T_ADD, T_SUB, T_MUL };
    vector<int> typ, lc, rc, vidx;
    vector<long long> cval;
    vector<int> stk;

    // 解析逆波兰式
    for (;;) {
        size_t a, b;
        if (!nextExprTok(a, b)) break;
        int tl = (int)(b - a);
        int id;
        if (tl == 1 && in[a] == '+') {
            int r = stk.back(); stk.pop_back();
            int l = stk.back(); stk.pop_back();
            id = (int)typ.size();
            typ.push_back(T_ADD); lc.push_back(l); rc.push_back(r);
            vidx.push_back(0); cval.push_back(0);
            stk.push_back(id);
        } else if (tl == 1 && in[a] == '-') {
            int r = stk.back(); stk.pop_back();
            int l = stk.back(); stk.pop_back();
            id = (int)typ.size();
            typ.push_back(T_SUB); lc.push_back(l); rc.push_back(r);
            vidx.push_back(0); cval.push_back(0);
            stk.push_back(id);
        } else if (tl == 1 && in[a] == '*') {
            int r = stk.back(); stk.pop_back();
            int l = stk.back(); stk.pop_back();
            id = (int)typ.size();
            typ.push_back(T_MUL); lc.push_back(l); rc.push_back(r);
            vidx.push_back(0); cval.push_back(0);
            stk.push_back(id);
        } else if (in[a] == 'x') {
            int vi = 0;
            for (size_t i = a + 1; i < b; ++i) vi = vi * 10 + (in[i] - '0');
            id = (int)typ.size();
            typ.push_back(T_VAR); lc.push_back(-1); rc.push_back(-1);
            vidx.push_back(vi); cval.push_back(0);
            stk.push_back(id);
        } else {
            bool neg = false;
            size_t i = a;
            if (in[i] == '-') { neg = true; ++i; }
            else if (in[i] == '+') ++i;
            long long v = 0;
            for (; i < b; ++i) v = v * 10 + (in[i] - '0');
            if (neg) v = -v;
            v %= MOD;
            if (v < 0) v += MOD;
            id = (int)typ.size();
            typ.push_back(T_CONST); lc.push_back(-1); rc.push_back(-1);
            vidx.push_back(0); cval.push_back(v);
            stk.push_back(id);
        }
    }
    pos = (eE < len) ? eE + 1 : len;   // 定位到表达式行之后的查询数据

    int N = (int)typ.size();
    int root = N - 1;

    // 每个自变量对应的变量叶子节点链表（下标即节点号）
    vector<int> head(n + 1, -1), nxt(N, -1);
    for (int u = 0; u < N; ++u) {
        if (typ[u] == T_VAR && vidx[u] >= 1 && vidx[u] <= n) {
            nxt[u] = head[vidx[u]];
            head[vidx[u]] = u;
        }
    }

    vector<long long> val(N), adj(N), coord(n + 1);
    string out;
    out.reserve((size_t)m * 12 + 16);

    for (int q = 0; q < m; ++q) {
        int qi = (int)readInt();          // 求偏导的自变量编号
        for (int j = 1; j <= n; ++j) {
            long long x = readInt() % MOD;
            if (x < 0) x += MOD;
            coord[j] = x;
        }
        if (head[qi] == -1) {             // f 不含 x_qi，偏导恒为 0
            out += '0';
            out += '\n';
            continue;
        }
        // 正向：按后缀序求各节点在给定点的值
        for (int u = 0; u < N; ++u) {
            int t = typ[u];
            if (t == T_CONST) val[u] = cval[u];
            else if (t == T_VAR) val[u] = coord[vidx[u]];
            else if (t == T_ADD) val[u] = (val[lc[u]] + val[rc[u]]) % MOD;
            else if (t == T_SUB) val[u] = (val[lc[u]] - val[rc[u]] + MOD) % MOD;
            else val[u] = val[lc[u]] * val[rc[u]] % MOD;
        }
        // 反向：把 du/dnode 从根往叶子推
        fill(adj.begin(), adj.end(), 0);
        adj[root] = 1;
        for (int u = N - 1; u >= 0; --u) {
            long long g = adj[u];
            if (g == 0) continue;
            int t = typ[u];
            if (t == T_CONST || t == T_VAR) continue;
            int l = lc[u], r = rc[u];
            if (t == T_ADD) {
                adj[l] = (adj[l] + g) % MOD;
                adj[r] = (adj[r] + g) % MOD;
            } else if (t == T_SUB) {
                adj[l] = (adj[l] + g) % MOD;
                adj[r] = (adj[r] - g + MOD) % MOD;
            } else {
                adj[l] = (adj[l] + g * val[r]) % MOD;
                adj[r] = (adj[r] + g * val[l]) % MOD;
            }
        }
        long long ans = 0;
        for (int u = head[qi]; u != -1; u = nxt[u]) ans = (ans + adj[u]) % MOD;
        out += to_string(ans);
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
