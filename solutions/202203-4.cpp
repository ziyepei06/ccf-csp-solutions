// CCF CSP 202203-4 通信系统管理
// 做法：
//   每天先"过期"（end 天结束的申请在 end+1 天开始时减去），再加入当天新申请。
//   任意一对 (u,v) 的当日额度 = 所有覆盖今天的申请的 x 之和（用 unordered_map 增量维护）。
//   对每个点维护一个懒删除大根堆：元素为 (-额度, 对方编号)，堆顶就是"额度最大、编号最小"的
//   主要通信对象；弹堆时用"该元素额度 != 当前实际额度"判定失效。
//   一条边的额度只有起点/终点两端会变，所以任何一次申请的开始/结束最多让 2 个点变脏，
//   每天只需对脏点重算主要通信对象，并增量维护"孤岛数 / 通信对数"。
//   复杂度 O((A+B) log A)，A、B 为申请/查询总数（均 <= 2e5）。

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;

static const int IBUF = 1 << 20;
static char ibuf[IBUF];
static int ilen = 0, ipos = 0;

static inline int gc_() {
    if (ipos == ilen) {
        ilen = (int)fread(ibuf, 1, IBUF, stdin);
        ipos = 0;
        if (ilen <= 0) return -1;
    }
    return (unsigned char)ibuf[ipos++];
}

static inline bool rd(long long &out) {
    int c = gc_();
    while (c != -1 && (c < '0' || c > '9') && c != '-') c = gc_();
    if (c == -1) return false;
    int sgn = 1;
    if (c == '-') { sgn = -1; c = gc_(); }
    long long v = 0;
    while (c >= '0' && c <= '9') { v = v * 10 + (c - '0'); c = gc_(); }
    out = v * sgn;
    return true;
}

static char obuf[1 << 20];
static int olen = 0;
static inline void flushOut() {
    if (olen) { fwrite(obuf, 1, olen, stdout); olen = 0; }
}
static inline void putNum(long long v) {
    char tmp[24];
    int t = 0;
    if (v == 0) tmp[t++] = '0';
    while (v > 0) { tmp[t++] = (char)('0' + v % 10); v /= 10; }
    while (t > 0) {
        if (olen == (int)sizeof(obuf)) flushOut();
        obuf[olen++] = tmp[--t];
    }
    if (olen == (int)sizeof(obuf)) flushOut();
    obuf[olen++] = '\n';
}

struct H64 {
    size_t operator()(long long x) const {
        unsigned long long z = (unsigned long long)x;
        z ^= z >> 33; z *= 0xff51afd7ed558ccdULL;
        z ^= z >> 33; z *= 0xc4ceb9fe1a85ec53ULL;
        z ^= z >> 33;
        return (size_t)z;
    }
};

int main() {
    long long nn, mm;
    if (!rd(nn)) { flushOut(); return 0; }
    rd(mm);
    int n = (int)nn, m = (int)mm;

    const long long BASE = (long long)n + 1;
    unordered_map<long long, long long, H64> emap;
    emap.reserve(1 << 18);
    emap.max_load_factor(0.7f);

    // 每条边 (u,v) 当前额度；key = min*BASE+max
    // 小根堆存 (-额度, 对方编号)：堆顶 = 额度最大、编号最小
    typedef priority_queue<pair<long long, int>,
                           vector<pair<long long, int> >,
                           greater<pair<long long, int> > > MinHeap;
    vector<MinHeap> heap(n + 1);

    vector<int> mt(n + 1, 0);       // 主要通信对象，0 表示孤岛
    long long islandCnt = n;        // mt[v]==0 的个数
    long long sumPair = 0;          // sum of pairFlag（通信对数是 sumPair/2）

    vector<int> appU, appV;         // 申请端点
    vector<long long> appX;         // 申请额度
    vector<int> appExpire;          // 失效日（= 起始日 + y，该日开始时减去）

    vector<vector<int> > startAt(m + 2), expireAt(m + 2);

    // 脏点集合（按天打标去重）
    vector<int> dirtyStamp(n + 1, 0);
    vector<int> dirtyList;

    // 唯一化三个编号后再加减 pairFlag，避免重复计数
    auto pairVal = [&](int x) -> int {
        return (mt[x] != 0 && mt[mt[x]] == x) ? 1 : 0;
    };

    for (int day = 1; day <= m; ++day) {
        // 1) 过期
        for (int id : expireAt[day]) {
            int u = appU[id], v = appV[id];
            long long x = appX[id];
            long long key = ((long long)u < (long long)v)
                                ? (long long)u * BASE + v
                                : (long long)v * BASE + u;
            auto it = emap.find(key);
            long long nw = it->second - x;
            if (nw > 0) {
                it->second = nw;
                heap[u].push(make_pair(-nw, v));
                heap[v].push(make_pair(-nw, u));
            } else {
                emap.erase(it);
            }
            if (dirtyStamp[u] != day) { dirtyStamp[u] = day; dirtyList.push_back(u); }
            if (dirtyStamp[v] != day) { dirtyStamp[v] = day; dirtyList.push_back(v); }
        }

        // 2) 读入当天申请并生效
        long long k;
        rd(k);
        for (long long i = 0; i < k; ++i) {
            long long u, v, x, y;
            rd(u); rd(v); rd(x); rd(y);
            int exp = day + (int)y;   // 第 day..day+y-1 天有效
            long long key = (u < v) ? u * BASE + v : v * BASE + u;
            long long nw;
            auto it = emap.find(key);
            if (it == emap.end()) { emap.emplace(key, x); nw = x; }
            else { it->second += x; nw = it->second; }
            heap[u].push(make_pair(-nw, (int)v));
            heap[v].push(make_pair(-nw, (int)u));
            if (dirtyStamp[u] != day) { dirtyStamp[u] = day; dirtyList.push_back((int)u); }
            if (dirtyStamp[v] != day) { dirtyStamp[v] = day; dirtyList.push_back((int)v); }
            if (exp <= m) {
                appU.push_back((int)u); appV.push_back((int)v);
                appX.push_back(x); appExpire.push_back(exp);
                expireAt[exp].push_back((int)appU.size() - 1);
            }
        }

        // 3) 重算所有脏点
        for (int v : dirtyList) {
            auto &h = heap[v];
            while (!h.empty()) {
                pair<long long, int> tp = h.top();
                int j = tp.second;
                long long key = ((long long)v < (long long)j)
                                    ? (long long)v * BASE + j
                                    : (long long)j * BASE + v;
                auto it = emap.find(key);
                long long cw = (it == emap.end()) ? 0 : it->second;
                if (cw == -tp.first) break;
                h.pop();
            }
            int nt = h.empty() ? 0 : h.top().second;
            int a = mt[v];
            if (nt != a) {
                // 先减掉 v, a, nt 的旧贡献
                sumPair -= pairVal(v);
                if (a != 0) sumPair -= pairVal(a);
                if (nt != 0 && nt != a) sumPair -= pairVal(nt);
                if (a == 0 && nt != 0) --islandCnt;
                else if (a != 0 && nt == 0) ++islandCnt;
                mt[v] = nt;
                sumPair += pairVal(v);
                if (a != 0) sumPair += pairVal(a);
                if (nt != 0 && nt != a) sumPair += pairVal(nt);
            }
        }
        dirtyList.clear();

        // 4) 查询主要通信对象
        long long l;
        rd(l);
        for (long long i = 0; i < l; ++i) {
            long long a;
            rd(a);
            putNum((long long)mt[(int)a]);
        }

        // 5) 孤岛 / 通信对
        long long p, q;
        rd(p); rd(q);
        if (p) putNum(islandCnt);
        if (q) putNum(sumPair / 2);
    }

    flushOut();
    return 0;
}
