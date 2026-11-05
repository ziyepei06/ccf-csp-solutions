// CCF CSP 202406-5 哥德尔机
//
// 关键观察：
//   V(x,y) <- v + ReLU(V(x,y) - v) = max(V(x,y), v)
//   所以每个修改操作就是一次"矩形区间 chmax"。所有修改都在询问之前，
//   于是最终矩阵 G(x,y) = max{ v_A : 矩形 A 覆盖 (x,y) }。
//   一次询问 = 子矩形内的最大值
//            = max{ v_A : 矩形 A 与询问矩形有公共格子 }
//            = max{ v_A : A 与询问矩形相交 }。
//   所以问题变成：n 个带权矩形，m 次"与查询矩形相交的矩形权值最大值"。
//
// 一维区间相交的完整划分（[a1,a2] 与 [q1,q2] 相交）：
//   (a1 <= q1 <= a2)  或  (a1 > q1 且 a1 <= q2)
// 二维分别取"或"，得到 4 种情形（每种都是相交的充要部分，取 max 即答案）：
//   A: 矩形包含点 (qx1,qy1)                --- 扫 x + y 区间刺穿结构
//   B: 矩形 x 方向含 qx1，且 A.y1 ∈[qy1,qy2] --- 扫 x + 对 y1 的区间最大值
//   C: 矩形 y 方向含 qy1，且 A.x1 ∈[qx1,qx2] --- 扫 y + 对 x1 的区间最大值
//   D: A.x1∈[qx1,qx2] 且 A.y1∈[qy1,qy2]    --- 静态二维矩形最大值(CDQ 分治)
//
// 复杂度 O((n+m) log^2 n)：A/B/C 各 O((n+m) log n)，D 为 O((n+m) log^2 n)。

#include <bits/stdc++.h>
using namespace std;

// ------------------------- 快速读入 -------------------------
static char IBUF[1 << 16];
static int IPOS = 0, ILEN = 0;
static inline int gc() {
    if (IPOS == ILEN) {
        ILEN = (int)fread(IBUF, 1, sizeof(IBUF), stdin);
        IPOS = 0;
        if (ILEN <= 0) return -1;
    }
    return IBUF[IPOS++];
}
static inline int readInt() {
    int c = gc();
    while (c != -1 && (c < '0' || c > '9')) c = gc();
    int x = 0;
    while (c >= '0' && c <= '9') { x = x * 10 + (c - '0'); c = gc(); }
    return x;
}

int N, M;                             // N: 修改个数(=坐标上界), M: 询问个数
struct Rect { int x1, x2, y1, y2, v; };
struct Qry  { int x1, x2, y1, y2; };
vector<Rect> R;
vector<Qry>  Q;
vector<int>  ansv;

// ============================================================
// 情形 A：矩形包含点 (qx1, qy1)
//   按 x 扫描，把矩形的 y 区间插入"区间刺穿结构"：
//   线段树每个结点维护一个以 (v, x2) 为关键字的最大堆（惰性删除）,
//   点查询 y 时走 y 的叶子到根的路径，取各结点堆顶的最大值。
// ============================================================
void solveA() {
    int sz = 1;
    while (sz < N) sz <<= 1;
    int tot = 2 * sz;
    vector<vector<pair<int, int>>> H(tot);   // (v, x2)

    // 先统计每个结点会被插入多少个元素，精确 reserve，避免扩容浪费内存
    {
        vector<int> cnt(tot, 0);
        for (const Rect& r : R) {
            int l = r.y1 - 1 + sz, rr = r.y2 + sz;   // 叶子下标 = y-1
            while (l < rr) {
                if (l & 1) cnt[l++]++;
                if (rr & 1) cnt[--rr]++;
                l >>= 1; rr >>= 1;
            }
        }
        for (int i = 1; i < tot; i++)
            if (cnt[i]) H[i].reserve(cnt[i]);
    }

    vector<int> ri(N);
    for (int i = 0; i < N; i++) ri[i] = i;
    sort(ri.begin(), ri.end(), [](int a, int b) { return R[a].x1 < R[b].x1; });
    vector<int> qi(M);
    for (int i = 0; i < M; i++) qi[i] = i;
    sort(qi.begin(), qi.end(), [](int a, int b) { return Q[a].x1 < Q[b].x1; });

    int ptr = 0;
    for (int k = 0; k < M; k++) {
        const int qid = qi[k];
        const int cx = Q[qid].x1;
        while (ptr < N && R[ri[ptr]].x1 <= cx) {
            const Rect& r = R[ri[ptr]];
            pair<int, int> val = {r.v, r.x2};
            int l = r.y1 - 1 + sz, rr = r.y2 + sz;
            while (l < rr) {
                if (l & 1) {
                    auto& V = H[l];
                    V.push_back(val);
                    push_heap(V.begin(), V.end());
                    l++;
                }
                if (rr & 1) {
                    rr--;
                    auto& V = H[rr];
                    V.push_back(val);
                    push_heap(V.begin(), V.end());
                }
                l >>= 1; rr >>= 1;
            }
            ptr++;
        }
        int p = Q[qid].y1 - 1 + sz, best = 0;
        while (p >= 1) {
            auto& V = H[p];
            while (!V.empty() && V.front().second < cx) {   // 惰性删除：x2 < cx 已失效
                pop_heap(V.begin(), V.end());
                V.pop_back();
            }
            if (!V.empty() && V.front().first > best) best = V.front().first;
            p >>= 1;
        }
        if (best > ansv[qid]) ansv[qid] = best;
    }
}

// ============================================================
// 情形 B / C（同一份代码，通过 swapdims 切换维度）：
//   扫描维度上要求 矩形区间含查询坐标（扫描线 + 进入/删除事件），
//   另一维度上要求 矩形在该维的左端点 KEY 落在 [LO, HI] 内 -> 线段树区间最大值。
//   叶子用双堆做"可删除的 multiset 最大值"。
// ============================================================
void solveSweep(bool swapdims) {
    vector<int> S1(N), E1(N), KEY(N), VV(N);
    vector<int> QS(M), LO(M), HI(M);
    for (int i = 0; i < N; i++) {
        S1[i] = swapdims ? R[i].y1 : R[i].x1;
        E1[i] = swapdims ? R[i].y2 : R[i].x2;
        KEY[i] = swapdims ? R[i].x1 : R[i].y1;
        VV[i] = R[i].v;
    }
    for (int j = 0; j < M; j++) {
        QS[j] = swapdims ? Q[j].y1 : Q[j].x1;
        LO[j] = swapdims ? Q[j].x1 : Q[j].y1;
        HI[j] = swapdims ? Q[j].x2 : Q[j].y2;
    }
    vector<int> ks(KEY.begin(), KEY.end());
    sort(ks.begin(), ks.end());
    ks.erase(unique(ks.begin(), ks.end()), ks.end());
    const int K = (int)ks.size();
    int sz = 1;
    while (sz < K) sz <<= 1;
    vector<priority_queue<int>> add(sz), del(sz);
    vector<int> tr(2 * sz, 0);

    auto refresh = [&](int rk) {
        auto& A = add[rk];
        auto& D = del[rk];
        while (!D.empty() && !A.empty() && A.top() == D.top()) { A.pop(); D.pop(); }
        int val = A.empty() ? 0 : A.top();
        int p = rk + sz;
        tr[p] = val;
        for (p >>= 1; p; p >>= 1) tr[p] = max(tr[2 * p], tr[2 * p + 1]);
    };
    auto rangeMax = [&](int lo, int hi) {   // 闭区间 [lo,hi]
        int l = lo + sz, r = hi + 1 + sz, res = 0;
        while (l < r) {
            if (l & 1) res = max(res, tr[l++]);
            if (r & 1) res = max(res, tr[--r]);
            l >>= 1; r >>= 1;
        }
        return res;
    };

    struct Ev { int x, t, id; };   // t: 0=进入, 1=询问, 2=删除
    vector<Ev> ev;
    ev.reserve((size_t)2 * N + M);
    for (int i = 0; i < N; i++) {
        ev.push_back({S1[i], 0, i});
        ev.push_back({E1[i], 2, i});
    }
    for (int j = 0; j < M; j++) ev.push_back({QS[j], 1, j});
    sort(ev.begin(), ev.end(), [](const Ev& a, const Ev& b) {
        return a.x != b.x ? a.x < b.x : a.t < b.t;
    });

    for (const Ev& e : ev) {
        if (e.t == 0) {
            int rk = (int)(lower_bound(ks.begin(), ks.end(), KEY[e.id]) - ks.begin());
            add[rk].push(VV[e.id]);
            refresh(rk);
        } else if (e.t == 2) {
            int rk = (int)(lower_bound(ks.begin(), ks.end(), KEY[e.id]) - ks.begin());
            del[rk].push(VV[e.id]);
            refresh(rk);
        } else {
            int lo = (int)(lower_bound(ks.begin(), ks.end(), LO[e.id]) - ks.begin());
            int hi = (int)(upper_bound(ks.begin(), ks.end(), HI[e.id]) - ks.begin()) - 1;
            if (lo <= hi) {
                int got = rangeMax(lo, hi);
                if (got > ansv[e.id]) ansv[e.id] = got;
            }
        }
    }
}

// ============================================================
// 情形 D：A.x1 ∈ [qx1,qx2] 且 A.y1 ∈ [qy1,qy2]
//   把每个矩形看成点 (x1, y1)，权值 v；询问是静态二维矩形最大值。
//   CDQ 分治：点按 x1 排序后按下标分治，询问落在 LCA 结点处处理，
//   左右两半各做一次"按下标扫描 + 可回滚线段树（对 y1 取区间最大值）"。
// ============================================================
namespace CD {
    int SZD;
    vector<int> tr;
    vector<pair<int, int>> hist;   // (位置, 旧值)

    inline void upd(int leaf, int val) {
        int p = leaf + SZD;
        while (p >= 1) {
            if (tr[p] >= val) break;          // 祖先都 >= val，无需继续
            hist.push_back({p, tr[p]});
            tr[p] = val;
            p >>= 1;
        }
    }
    inline int qry(int a, int b) {            // 闭区间 [a,b]
        if (a > b) return 0;
        int l = a + SZD, r = b + 1 + SZD, res = 0;
        while (l < r) {
            if (l & 1) res = max(res, tr[l++]);
            if (r & 1) res = max(res, tr[--r]);
            l >>= 1; r >>= 1;
        }
        return res;
    }
    inline void rollback(size_t mark) {
        while (hist.size() > mark) {
            tr[hist.back().first] = hist.back().second;
            hist.pop_back();
        }
    }
}

void solveD() {
    struct QD { int L, H, ylo, yhi, id; };
    vector<int> ord(N);
    for (int i = 0; i < N; i++) ord[i] = i;
    sort(ord.begin(), ord.end(), [](int a, int b) {
        if (R[a].x1 != R[b].x1) return R[a].x1 < R[b].x1;
        return R[a].y1 < R[b].y1;
    });
    vector<int> px(N), py(N), pv(N);
    for (int i = 0; i < N; i++) {
        px[i] = R[ord[i]].x1;
        py[i] = R[ord[i]].y1;
        pv[i] = R[ord[i]].v;
    }
    vector<int> Yd(py.begin(), py.end());
    sort(Yd.begin(), Yd.end());
    Yd.erase(unique(Yd.begin(), Yd.end()), Yd.end());
    vector<int> yrank(N);
    for (int i = 0; i < N; i++)
        yrank[i] = (int)(lower_bound(Yd.begin(), Yd.end(), py[i]) - Yd.begin());

    CD::SZD = 1;
    while (CD::SZD < (int)Yd.size()) CD::SZD <<= 1;
    CD::tr.assign(2 * CD::SZD, 0);
    CD::hist.reserve(1 << 20);

    vector<QD> list;
    list.reserve(M);
    for (int j = 0; j < M; j++) {
        int L = (int)(lower_bound(px.begin(), px.end(), Q[j].x1) - px.begin());
        int H = (int)(upper_bound(px.begin(), px.end(), Q[j].x2) - px.begin()) - 1;
        if (L > H) continue;
        int ylo = (int)(lower_bound(Yd.begin(), Yd.end(), Q[j].y1) - Yd.begin());
        int yhi = (int)(upper_bound(Yd.begin(), Yd.end(), Q[j].y2) - Yd.begin()) - 1;
        if (ylo > yhi) continue;
        list.push_back({L, H, ylo, yhi, j});
    }
    if (list.empty()) return;

    // 显式栈递归，避免函数递归开销/爆栈
    struct Frame { int l, r; vector<QD> ql; };
    vector<Frame> stk;
    stk.push_back({0, N - 1, std::move(list)});
    while (!stk.empty()) {
        Frame fr = std::move(stk.back());
        stk.pop_back();
        if (fr.ql.empty()) continue;
        const int l = fr.l, r = fr.r;
        if (l == r) {
            const int y = yrank[l];
            for (const QD& q : fr.ql)
                if (q.ylo <= y && y <= q.yhi && pv[l] > ansv[q.id]) ansv[q.id] = pv[l];
            continue;
        }
        const int mid = (l + r) >> 1;
        vector<QD> left, right, cross;
        left.reserve(fr.ql.size() / 2 + 1);
        right.reserve(fr.ql.size() / 2 + 1);
        for (const QD& q : fr.ql) {
            if (q.L <= mid && q.H > mid) cross.push_back(q);
            else if (q.H <= mid) left.push_back(q);
            else right.push_back(q);
        }
        if (!cross.empty()) {
            // 左半部分 [L, mid]：按 L 降序扫描，边扫边插入
            sort(cross.begin(), cross.end(), [](const QD& a, const QD& b) { return a.L > b.L; });
            size_t mark = CD::hist.size();
            int i = mid + 1;
            for (const QD& q : cross) {
                while (i - 1 >= q.L) {
                    --i;
                    CD::upd(yrank[i], pv[i]);
                }
                int got = CD::qry(q.ylo, q.yhi);
                if (got > ansv[q.id]) ansv[q.id] = got;
            }
            CD::rollback(mark);
            // 右半部分 [mid+1, H]：按 H 升序扫描
            sort(cross.begin(), cross.end(), [](const QD& a, const QD& b) { return a.H < b.H; });
            mark = CD::hist.size();
            int j = mid;
            for (const QD& q : cross) {
                while (j + 1 <= q.H) {
                    ++j;
                    CD::upd(yrank[j], pv[j]);
                }
                int got = CD::qry(q.ylo, q.yhi);
                if (got > ansv[q.id]) ansv[q.id] = got;
            }
            CD::rollback(mark);
        }
        if (!left.empty()) stk.push_back({l, mid, std::move(left)});
        if (!right.empty()) stk.push_back({mid + 1, r, std::move(right)});
    }
}

int main() {
    N = readInt();
    M = readInt();
    if (N < 1) N = 1;
    R.resize(N);
    for (int i = 0; i < N; i++) {
        R[i].x1 = readInt(); R[i].x2 = readInt();
        R[i].y1 = readInt(); R[i].y2 = readInt();
        R[i].v = readInt();
    }
    Q.resize(M);
    for (int j = 0; j < M; j++) {
        Q[j].x1 = readInt(); Q[j].x2 = readInt();
        Q[j].y1 = readInt(); Q[j].y2 = readInt();
    }
    ansv.assign(M, 0);

    solveA();
    solveSweep(false);   // 情形 B：扫 x，区间看 y1
    solveSweep(true);    // 情形 C：扫 y，区间看 x1
    solveD();

    // 输出
    string out;
    out.reserve((size_t)M * 8);
    char tmp[16];
    for (int j = 0; j < M; j++) {
        int len = 0;
        int x = ansv[j];
        if (x == 0) tmp[len++] = '0';
        while (x > 0) { tmp[len++] = char('0' + x % 10); x /= 10; }
        while (len > 0) out.push_back(tmp[--len]);
        out.push_back('\n');
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
