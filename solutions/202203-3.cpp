// 202203-3 计算资源调度器
// 按启动顺序逐个分配任务：
//   过滤：节点亲和性 na（可用区）、任务亲和性 pa（该应用已出现在某些可用区）、
//         任务反亲和性 paa（该应用的已有任务不得在本节点）；若过 soft 的 paa 后为空，则去掉 paa 再过滤一次。
//   排序：任务数最少，其次编号最小。
// 每个任务的候选集合按 可用区/全部节点 枚举，平均 O(n + m/64)，n,m<=1000。
#include <cstdio>
#include <cstring>
#include <climits>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

using namespace std;

static const int BUF = 1 << 20;
static char ibuf[BUF];
static int ipos = 0, ilen = 0;
static inline int gc() {
    if (ipos == ilen) {
        ilen = (int)fread(ibuf, 1, BUF, stdin);
        ipos = 0;
        if (ilen <= 0) return -1;
    }
    return ibuf[ipos++];
}
static inline int rdInt() {
    int c = gc();
    while (c != -1 && (c < '0' || c > '9') && c != '-') c = gc();
    int sgn = 1;
    if (c == '-') { sgn = -1; c = gc(); }
    long long v = 0;
    while (c >= '0' && c <= '9') { v = v * 10 + (c - '0'); c = gc(); }
    return (int)(v * sgn);
}

int main() {
    int n = rdInt(), m = rdInt();
    vector<int> zoneOf(n + 1);
    vector<vector<int> > zoneNodes(m + 1);
    for (int i = 1; i <= n; ++i) {
        zoneOf[i] = rdInt();
        zoneNodes[zoneOf[i]].push_back(i);
    }

    // 位图：每个应用占 Wn 个字（节点集合）与 Wm 个字（可用区集合）
    const int Wn = (n + 64) >> 6;  // 至少 1 个字，避免 n=0 时出现 0 长数组
    const int Wm = (m + 64) >> 6;
    unordered_map<int, int> slotOf;
    slotOf.reserve(1024);
    vector<uint64_t> appNode;  // 应用 -> 已部署任务的节点
    vector<uint64_t> appZone;  // 应用 -> 已部署任务的可用区
    appNode.reserve(Wn * 16);
    appZone.reserve(Wm * 16);

    vector<int> cntNode(n + 1, 0);

    int g = rdInt();

    string out;
    out.reserve(1 << 20);
    char tmp[16];

    int curSlot = -1;
    for (int gi = 0; gi < g; ++gi) {
        int f = rdInt(), a = rdInt(), na = rdInt(), pa = rdInt(), paa = rdInt(), paar = rdInt();

        // 应用 a 的槽位
        {
            unordered_map<int, int>::iterator it = slotOf.find(a);
            if (it == slotOf.end()) {
                curSlot = (int)slotOf.size();
                slotOf[a] = curSlot;
                appNode.resize((size_t)(curSlot + 1) * Wn, 0);
                appZone.resize((size_t)(curSlot + 1) * Wm, 0);
            } else {
                curSlot = it->second;
            }
        }
        int paSlot = -1, antiSlot = -1;
        if (pa) {
            unordered_map<int, int>::iterator it = slotOf.find(pa);
            if (it == slotOf.end()) {
                int s = (int)slotOf.size();
                slotOf[pa] = s;
                appNode.resize((size_t)(s + 1) * Wn, 0);
                appZone.resize((size_t)(s + 1) * Wm, 0);
                paSlot = s;
            } else {
                paSlot = it->second;
            }
        }
        if (paa) {
            unordered_map<int, int>::iterator it = slotOf.find(paa);
            if (it == slotOf.end()) {
                int s = (int)slotOf.size();
                slotOf[paa] = s;
                appNode.resize((size_t)(s + 1) * Wn, 0);
                appZone.resize((size_t)(s + 1) * Wm, 0);
                antiSlot = s;
            } else {
                antiSlot = it->second;
            }
        }

        for (int t = 0; t < f; ++t) {
            int bestIdx = -1;
            int bestCnt = INT_MAX;

            // 是否允许某个候选节点（只看反亲和性）
            const uint64_t *antiBits = (antiSlot >= 0) ? &appNode[(size_t)antiSlot * Wn] : NULL;

            for (int pass = 0; pass < 2 && bestIdx < 0; ++pass) {
                // pass 0：完整过滤；pass 1：仅当 paa 为“尽量满足”时去掉它
                if (pass == 1 && !(antiSlot >= 0 && paar == 0)) break;
                const uint64_t *bits = (pass == 0) ? antiBits : NULL;
                bestIdx = -1;
                bestCnt = INT_MAX;

                if (na) {  // 节点亲和性：只看一个可用区
                    if (paSlot >= 0 && !((appZone[(size_t)paSlot * Wm + (na >> 6)] >> (na & 63)) & 1ULL)) {
                        // 该可用区没有应用 pa 的任务
                    } else {
                        const vector<int> &vn = zoneNodes[na];
                        for (size_t k = 0; k < vn.size(); ++k) {
                            int nd = vn[k];
                            if (bits && ((bits[nd >> 6] >> (nd & 63)) & 1ULL)) continue;
                            int c = cntNode[nd];
                            if (c < bestCnt || (c == bestCnt && nd < bestIdx)) {
                                bestCnt = c; bestIdx = nd;
                            }
                        }
                    }
                } else if (paSlot >= 0) {  // 任务亲和性：遍历应用 pa 出现过的可用区
                    const uint64_t *zb = &appZone[(size_t)paSlot * Wm];
                    for (int w = 0; w < Wm; ++w) {
                        uint64_t x = zb[w];
                        while (x) {
                            int b = __builtin_ctzll(x);
                            x &= x - 1;
                            int z = (w << 6) + b;
                            const vector<int> &vn = zoneNodes[z];
                            for (size_t k = 0; k < vn.size(); ++k) {
                                int nd = vn[k];
                                if (bits && ((bits[nd >> 6] >> (nd & 63)) & 1ULL)) continue;
                                int c = cntNode[nd];
                                if (c < bestCnt || (c == bestCnt && nd < bestIdx)) {
                                    bestCnt = c; bestIdx = nd;
                                }
                            }
                        }
                    }
                } else {  // 全部节点
                    for (int nd = 1; nd <= n; ++nd) {
                        if (bits && ((bits[nd >> 6] >> (nd & 63)) & 1ULL)) continue;
                        int c = cntNode[nd];
                        if (c < bestCnt || (c == bestCnt && nd < bestIdx)) {
                            bestCnt = c; bestIdx = nd;
                        }
                    }
                }
            }

            if (bestIdx < 0) {
                out += '0';
                out += (t + 1 == f) ? '\n' : ' ';
                continue;
            }
            // 落位
            ++cntNode[bestIdx];
            uint64_t *nb = &appNode[(size_t)curSlot * Wn];
            nb[bestIdx >> 6] |= (1ULL << (bestIdx & 63));
            int z = zoneOf[bestIdx];
            uint64_t *zb2 = &appZone[(size_t)curSlot * Wm];
            zb2[z >> 6] |= (1ULL << (z & 63));

            int len = snprintf(tmp, sizeof(tmp), "%d", bestIdx);
            out.append(tmp, len);
            out += (t + 1 == f) ? '\n' : ' ';
        }
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
