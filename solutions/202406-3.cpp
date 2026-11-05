#include <bits/stdc++.h>
using namespace std;

/*
 * 202406-3 文本分词
 *
 * 核心：词汇表每次迭代，选取「相邻词汇对」出现频率（按单词频率加权）最高的一组，
 * 并列时依次比较：拼接串长度最短 -> 前一个词汇长度最短 -> 拼接串字典序最小；
 * 然后把所有单词的词汇序列从左到右按该规则合并。
 *
 * 关键性质：一次合并产生的新相邻对必然包含「刚生成的新词汇 c」，也就是 (L,c)/(c,R)。
 * 因此任意既有词汇对 (a,b)（a、b 都是旧词汇）的计数只可能减少，不会增加，
 * 也就不可能被重复选中；每次选中的词汇对只会被处理一遍。
 *
 * 数据结构：
 *  - 每个单词用双向链表（数组 prev/next/tok）表示词汇序列；合并时保留左侧结点、
 *    删除右侧结点，因此总合并次数 <= 初始总词数 (<= 250000)。
 *  - 用 unordered_map<pairKey, Info{cnt, occ}> 维护所有词汇对的加权计数与出现位置
 *    （左结点下标）。由于结点按单词顺序连续编号，按下标排序即得到「从前到后」的
 *    合并顺序，保证 <a,a> 这类重叠情形得到正确结果。
 *  - 最大堆按 (cnt, 拼接长度, 前词长度, 拼接串字典序) 排序；计数变化时重新压入新条目，
 *    取堆顶时用 map 里的当前计数做惰性校验（当前计数必然在堆中存在，且各对计数单调不增）。
 *  - 计数变化次数 = O(合并次数)，合并次数 <= 初始总词数，故整体 O(S log S)。
 */

static vector<string> tstr;      // 词汇 id -> 字符串
static vector<int> tlen;         // 词汇 id -> 字符串长度

static inline uint64_t pkey(int a, int b) {
    return ((uint64_t)(uint32_t)a << 32) | (uint32_t)b;
}

// 合并串前 8 个字节（不足补 0）：只在「拼接长度相等」的两个词汇对之间比较字典序，
// 因此补 0 不改变比较结果。
static uint64_t ck8(int a, int b) {
    const string &sa = tstr[a], &sb = tstr[b];
    uint64_t r = 0;
    int la = (int)sa.size();
    for (int i = 0; i < 8; ++i) {
        r <<= 8;
        int ch = 0;
        if (i < la) ch = (unsigned char)sa[i];
        else {
            int j = i - la;
            if (j < (int)sb.size()) ch = (unsigned char)sb[j];
        }
        r |= (uint64_t)ch;
    }
    return r;
}

struct Ent {
    int c;      // 压入时的计数
    int clen;   // 拼接串长度
    int alen;   // 前一个词汇长度
    int a, b;   // 词汇对
    uint64_t ck; // 拼接串前 8 字节
};

static bool cmbGreater(const Ent &x, const Ent &y) {
    const string &xa = tstr[x.a], &xb = tstr[x.b];
    const string &ya = tstr[y.a], &yb = tstr[y.b];
    int lx = (int)xa.size(), ly = (int)ya.size();
    int L = x.clen;
    for (int k = 0; k < L; ++k) {
        int u = (k < lx) ? (unsigned char)xa[k] : (unsigned char)xb[k - lx];
        int v = (k < ly) ? (unsigned char)ya[k] : (unsigned char)yb[k - ly];
        if (u != v) return u > v;
    }
    return false;
}

struct Cmp {
    bool operator()(const Ent &x, const Ent &y) const {
        if (x.c != y.c) return x.c < y.c;              // 计数大的优先
        if (x.clen != y.clen) return x.clen > y.clen;  // 拼接串短的优先
        if (x.alen != y.alen) return x.alen > y.alen;  // 前词短的优先
        if (x.ck != y.ck) return x.ck > y.ck;          // 拼接串字典序小的优先
        return cmbGreater(x, y);
    }
};

struct Info {
    int c = 0;
    vector<int> occ;
};

static unordered_map<uint64_t, Info> M;
static priority_queue<Ent, vector<Ent>, Cmp> pq;

static int totalMerges = 0;

// 计数变化 +delta，并（可选）登记一个出现位置；计数>0 时压入新的堆条目。
static inline void addPair(int a, int b, int delta, int node) {
    uint64_t k = pkey(a, b);
    Info &in = M[k];
    in.c += delta;
    if (node >= 0) in.occ.push_back(node);
    int c = in.c;
    if (c > 0) {
        pq.push(Ent{c, tlen[a] + tlen[b], tlen[a], a, b, ck8(a, b)});
    } else {
        if (!in.occ.empty()) in.occ.clear();
    }
}

int main() {
    int n, m;
    if (scanf("%d %d", &n, &m) != 2) return 0;

    vector<string> words(n);
    vector<int> wf(n);
    bool hasChar[26] = {false};
    for (int i = 0; i < n; ++i) {
        char buf[64];
        int f;
        if (scanf("%63s %d", buf, &f) != 2) return 0;
        words[i] = buf;
        wf[i] = f;
        for (char ch : words[i]) hasChar[ch - 'a'] = true;
    }

    // 初始词汇表：出现过的字母按字典序
    vector<int> vocab;
    int chId[26];
    for (int c = 0; c < 26; ++c) {
        if (hasChar[c]) {
            chId[c] = (int)tstr.size();
            tstr.push_back(string(1, (char)('a' + c)));
            tlen.push_back(1);
            vocab.push_back(chId[c]);
        } else {
            chId[c] = -1;
        }
    }

    vector<int> tok, prv, nxt, fnode;
    vector<char> alive;
    long long total = 0;
    for (int i = 0; i < n; ++i) total += (long long)words[i].size();
    tok.reserve(total);
    prv.reserve(total);
    nxt.reserve(total);
    fnode.reserve(total);
    alive.reserve(total);

    vector<int> wstart(n, -1), wend(n, -1);
    for (int i = 0; i < n; ++i) {
        int prevNode = -1;
        for (char ch : words[i]) {
            int id = chId[ch - 'a'];
            int node = (int)tok.size();
            tok.push_back(id);
            fnode.push_back(wf[i]);
            prv.push_back(prevNode);
            nxt.push_back(-1);
            alive.push_back(1);
            if (prevNode >= 0) nxt[prevNode] = node;
            else wstart[i] = node;
            prevNode = node;
        }
        wend[i] = prevNode;
    }

    long long tot = (long long)tok.size();

    M.reserve(1 << 20);
    {
        vector<Ent> tmp;
        tmp.reserve(1 << 21);
        pq = priority_queue<Ent, vector<Ent>, Cmp>(Cmp(), std::move(tmp));
    }

    // 初始相邻对统计
    for (int i = 0; i < n; ++i) {
        for (int u = wstart[i]; u >= 0 && nxt[u] >= 0; u = nxt[u]) {
            addPair(tok[u], tok[nxt[u]], fnode[u], u);
        }
    }

    vector<int> list;
    while ((int)vocab.size() < m && tot > (long long)n) {
        // 取出当前最优词汇对
        int a = -1, b = -1;
        while (!pq.empty()) {
            Ent e = pq.top();
            auto it = M.find(pkey(e.a, e.b));
            int cur = (it == M.end()) ? 0 : it->second.c;
            if (cur == e.c && cur > 0) {
                a = e.a;
                b = e.b;
                pq.pop();
                break;
            }
            pq.pop();
        }
        if (a < 0) break;  // 所有单词都已是单个词汇

        int c = (int)tstr.size();
        tstr.push_back(tstr[a] + tstr[b]);
        tlen.push_back(tlen[a] + tlen[b]);
        vocab.push_back(c);

        // 取出该词汇对的全部出现位置，按从前到后（结点下标升序）合并
        list.clear();
        {
            auto it = M.find(pkey(a, b));
            if (it != M.end()) list.swap(it->second.occ);
        }
        sort(list.begin(), list.end());

        for (size_t idx = 0; idx < list.size(); ++idx) {
            int i = list[idx];
            if (!alive[i] || tok[i] != a) continue;
            int j = nxt[i];
            if (j < 0 || !alive[j] || tok[j] != b) continue;

            int f = fnode[i];
            // 删除 (a,b)
            addPair(a, b, -f, -1);
            int L = prv[i];
            if (L >= 0) {
                int l = tok[L];
                addPair(l, a, -f, -1);
                addPair(l, c, f, L);
            }
            int R = nxt[j];
            if (R >= 0) {
                int r = tok[R];
                addPair(b, r, -f, -1);
                addPair(c, r, f, i);
            }
            // 保留 i，删除 j
            tok[i] = c;
            nxt[i] = R;
            if (R >= 0) prv[R] = i;
            alive[j] = 0;
            --tot;
            ++totalMerges;
        }
    }

    string out;
    out.reserve(64 * vocab.size());
    for (int id : vocab) {
        out += tstr[id];
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
