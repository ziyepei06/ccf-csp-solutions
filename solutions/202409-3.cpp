// 202409-3 补丁应用
// 1) 读入 n 行原文件；2) 补丁去掉 # 注释行，按 @ 开头的行切块；
// 3) 逐块严格校验头部格式与内容行数；4) 逐块在当前文件上用 KMP 找 |delta| 最小
//    (并列取更小 delta) 的匹配位置并替换，同时把 delta 累加进后续块的 NN。
#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef string_view sv;

static const ll CAP = 4000000000000000LL; // 数值饱和上限，防止恶意大数运算溢出

static string g_in;              // 全部输入（生命周期覆盖全程）
static vector<sv> g_lines;       // 按行切分后的视图

static void damaged() {
    fputs("Patch is damaged.\n", stdout);
    exit(0);
}

// 解析一个正整数：首位必须是 1..9，之后若干 0..9；过大则饱和到 CAP
static bool parseNum(sv s, size_t &i, ll &out) {
    if (i >= s.size()) return false;
    char c = s[i];
    if (c < '1' || c > '9') return false;
    __int128 v = c - '0';
    ++i;
    while (i < s.size() && s[i] >= '0' && s[i] <= '9') {
        v = v * 10 + (s[i] - '0');
        if (v > (__int128)CAP) v = CAP;
        ++i;
    }
    out = (ll)v;
    return true;
}

// 严格匹配 "@@ -NN,MM +nn,mm @@"
static bool parseHeader(sv s, ll &NN, ll &MM, ll &nn, ll &mm) {
    size_t i = 0;
    auto lit = [&](char c) -> bool {
        if (i < s.size() && s[i] == c) { ++i; return true; }
        return false;
    };
    if (!lit('@')) return false;
    if (!lit('@')) return false;
    if (!lit(' ')) return false;
    if (!lit('-')) return false;
    if (!parseNum(s, i, NN)) return false;
    if (!lit(',')) return false;
    if (!parseNum(s, i, MM)) return false;
    if (!lit(' ')) return false;
    if (!lit('+')) return false;
    if (!parseNum(s, i, nn)) return false;
    if (!lit(',')) return false;
    if (!parseNum(s, i, mm)) return false;
    if (!lit(' ')) return false;
    if (!lit('@')) return false;
    if (!lit('@')) return false;
    return i == s.size();
}

struct Blk {
    ll NN = 0, MM = 0, nn = 0, mm = 0;
    vector<int> orig, neu; // 行内容 -> id
};

struct SvHash {
    size_t operator()(sv s) const { return std::hash<sv>()(s); }
};

int main() {
    // ---- 快速读入 ----
    {
        char buf[1 << 16];
        size_t r;
        while ((r = fread(buf, 1, sizeof buf, stdin)) > 0) g_in.append(buf, r);
    }
    {
        size_t p = 0;
        while (p < g_in.size()) {
            size_t q = g_in.find('\n', p);
            if (q == string::npos) {
                g_lines.emplace_back(g_in.data() + p, g_in.size() - p);
                break;
            }
            g_lines.emplace_back(g_in.data() + p, q - p);
            p = q + 1;
        }
    }
    if (g_lines.empty()) damaged();

    // ---- 读 n 与原文件 ----
    ll n = 0;
    {
        sv s = g_lines[0];
        size_t p = 0;
        __int128 v = 0;
        while (p < s.size() && (s[p] == ' ' || s[p] == '\t')) ++p;
        if (p >= s.size()) damaged();
        while (p < s.size() && s[p] >= '0' && s[p] <= '9') {
            v = v * 10 + (s[p] - '0');
            if (v > (__int128)CAP) v = CAP;
            ++p;
        }
        n = (ll)v;
    }

    // ---- 行内容池（复用去重） ----
    unordered_map<sv, int, SvHash> idOf;
    vector<sv> strOf;
    auto internStr = [&](sv s) -> int {
        auto it = idOf.find(s);
        if (it != idOf.end()) return it->second;
        int x = (int)strOf.size();
        strOf.push_back(s);
        idOf.emplace(s, x);
        return x;
    };

    vector<int> file;
    file.reserve((size_t)(n < 1000000 ? n : 1000000) + 1);
    for (ll i = 0; i < n; ++i) {
        size_t idx = (size_t)i + 1;
        sv s = (idx < g_lines.size()) ? g_lines[idx] : sv();
        file.push_back(internStr(s));
    }

    // ---- 补丁：去掉 # 开头的注释行 ----
    vector<sv> patch;
    for (size_t i = (size_t)n + 1; i < g_lines.size(); ++i) {
        sv s = g_lines[i];
        if (!s.empty() && s[0] == '#') continue;
        patch.push_back(s);
    }

    // ---- 按 @ 开头的行切块并校验 ----
    vector<size_t> at;
    for (size_t i = 0; i < patch.size(); ++i)
        if (!patch[i].empty() && patch[i][0] == '@') at.push_back(i);
    if (at.empty()) damaged();

    vector<Blk> blocks;
    blocks.reserve(at.size());
    for (size_t k = 0; k < at.size(); ++k) {
        size_t b = at[k];
        size_t e = (k + 1 < at.size()) ? at[k + 1] : patch.size();
        Blk blk;
        if (!parseHeader(patch[b], blk.NN, blk.MM, blk.nn, blk.mm)) damaged();
        for (size_t j = b + 1; j < e; ++j) {
            sv L = patch[j];
            char c = L.empty() ? '\0' : L[0];
            if (c == '-') {
                blk.orig.push_back(internStr(L.substr(1)));
            } else if (c == '+') {
                blk.neu.push_back(internStr(L.substr(1)));
            } else if (c == ' ') {
                int x = internStr(L.substr(1));
                blk.orig.push_back(x);
                blk.neu.push_back(x);
            } else {
                damaged();
            }
        }
        if ((ll)blk.orig.size() != blk.MM) damaged();
        if ((ll)blk.neu.size() != blk.mm) damaged();
        if (k > 0) {
            const Blk &pr = blocks[k - 1];
            if (blk.NN < pr.NN || blk.NN - pr.NN < pr.MM) damaged();
        }
        blocks.push_back(std::move(blk));
    }

    // ---- 逐块应用 ----
    ll off = 0;       // 累计 delta：块的实际起点 = NN + off
    ll prevStart = 0; // 前一块的实际起点（1 起）
    ll prevMM = 0;
    for (size_t k = 0; k < blocks.size(); ++k) {
        const Blk &B = blocks[k];
        const ll m = B.MM;
        const ll NNcur = B.NN + off;
        const ll lower = (k == 0) ? 1 : prevStart + prevMM;
        const size_t msz = B.orig.size();

        // KMP：求 orig 在 file 中所有出现位置
        vector<int> pi(msz, 0);
        for (size_t i = 1; i < msz; ++i) {
            size_t j = (size_t)pi[i - 1];
            while (j > 0 && B.orig[i] != B.orig[j]) j = (size_t)pi[j - 1];
            if (B.orig[i] == B.orig[j]) ++j;
            pi[i] = (int)j;
        }

        ll bestPos = -1, bestDelta = 0, bestAbs = LLONG_MAX;
        {
            size_t j = 0;
            for (size_t t = 0; t < file.size(); ++t) {
                while (j > 0 && file[t] != B.orig[j]) j = (size_t)pi[j - 1];
                if (file[t] == B.orig[j]) ++j;
                if (j == msz) {
                    ll pos = (ll)t + 2 - m; // 匹配窗口的 1 起起点
                    if (pos >= lower) {
                        ll d = pos - NNcur;
                        ll ad = d < 0 ? -d : d;
                        // 位置递增 => d 递增，用严格小于即可实现 (|d|, d) 字典序最小
                        if (ad < m && ad < bestAbs) {
                            bestAbs = ad;
                            bestDelta = d;
                            bestPos = pos;
                        }
                    }
                    j = (size_t)pi[j - 1];
                }
            }
        }
        if (bestPos < 0) damaged();

        // 用新内容片段替换 [bestPos, bestPos+MM-1]
        file.erase(file.begin() + (bestPos - 1), file.begin() + (bestPos - 1 + m));
        file.insert(file.begin() + (bestPos - 1), B.neu.begin(), B.neu.end());
        off += bestDelta;
        prevStart = bestPos;
        prevMM = m;
    }

    // ---- 输出 ----
    {
        string out;
        size_t tot = 0;
        for (int x : file) tot += strOf[x].size() + 1;
        out.reserve(tot + 1);
        for (int x : file) {
            out.append(strOf[x].data(), strOf[x].size());
            out.push_back('\n');
        }
        fwrite(out.data(), 1, out.size(), stdout);
    }
    return 0;
}
