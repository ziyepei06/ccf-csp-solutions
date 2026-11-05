#include <bits/stdc++.h>
using namespace std;

// 201412-3 集合竞价
// 价格按"分"存成整数（题目保证恰好两位小数），避免浮点误差。
// 对每个候选价 p：成交量 = min(出价 >= p 的买单总量, 出价 <= p 的卖单总量)。
// 成交量关于 p 是分段常数，最大值一定能取在某个订单报价上，故只枚举订单报价。
// 相同最大成交量取最高价：候选价升序扫描，用 >= 更新即可。

struct Rec {
    int type;        // 0 = buy, 1 = sell, 2 = cancel
    int cents;       // 价格（分），cancel 记录无意义
    long long qty;   // 股数
    int target;      // cancel 记录撤销的行号（1-based）
};

static int parseCents(const string &s) {
    long long ip = 0, fp = 0;
    int fd = 0;
    size_t i = 0;
    while (i < s.size() && isdigit((unsigned char)s[i])) ip = ip * 10 + (s[i++] - '0');
    if (i < s.size() && s[i] == '.') {
        ++i;
        while (i < s.size() && isdigit((unsigned char)s[i]) && fd < 2) {
            fp = fp * 10 + (s[i++] - '0');
            ++fd;
        }
    }
    while (fd < 2) { fp *= 10; ++fd; }
    return (int)(ip * 100 + fp);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<Rec> recs;
    string line;
    while (getline(cin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        stringstream ss(line);
        string op;
        if (!(ss >> op)) continue;
        Rec r{};
        if (op == "buy" || op == "sell") {
            string ps;
            long long q;
            ss >> ps >> q;
            r.type = (op == "buy") ? 0 : 1;
            r.cents = parseCents(ps);
            r.qty = q;
        } else if (op == "cancel") {
            int idx;
            ss >> idx;
            r.type = 2;
            r.target = idx;
        } else {
            continue;
        }
        recs.push_back(r);
    }

    int n = (int)recs.size();
    vector<char> dead(n, 0);
    for (int i = 0; i < n; ++i)
        if (recs[i].type == 2) {
            int t = recs[i].target - 1;   // 行号 1-based -> 下标
            if (t >= 0 && t < n) dead[t] = 1;
        }

    vector<long long> buyP, buyQ, sellP, sellQ;
    vector<int> cand;
    for (int i = 0; i < n; ++i) {
        if (dead[i]) continue;
        if (recs[i].type == 0)      { buyP.push_back(recs[i].cents); buyQ.push_back(recs[i].qty); }
        else if (recs[i].type == 1) { sellP.push_back(recs[i].cents); sellQ.push_back(recs[i].qty); }
        if (recs[i].type != 2) cand.push_back(recs[i].cents);
    }

    if (cand.empty()) {           // 全部被撤销：无成交
        printf("0.00 0\n");
        return 0;
    }

    int nb = (int)buyP.size(), ns = (int)sellP.size();
    vector<int> bidx(nb), sidx(ns);
    iota(bidx.begin(), bidx.end(), 0);
    iota(sidx.begin(), sidx.end(), 0);
    sort(bidx.begin(), bidx.end(), [&](int a, int b) { return buyP[a] < buyP[b]; });
    sort(sidx.begin(), sidx.end(), [&](int a, int b) { return sellP[a] < sellP[b]; });

    vector<int> bp(nb), sp(ns);
    vector<long long> buyPref(nb + 1, 0), sellPref(ns + 1, 0);
    for (int i = 0; i < nb; ++i) { bp[i] = buyP[bidx[i]];  buyPref[i + 1]  = buyPref[i]  + buyQ[bidx[i]]; }
    for (int i = 0; i < ns; ++i) { sp[i] = sellP[sidx[i]]; sellPref[i + 1] = sellPref[i] + sellQ[sidx[i]]; }
    long long totalBuy = buyPref[nb];

    sort(cand.begin(), cand.end());
    cand.erase(unique(cand.begin(), cand.end()), cand.end());

    long long bestVol = -1;
    int bestCents = 0;
    for (int p : cand) {
        // 出价 >= p 的买单总量
        int lo = (int)(lower_bound(bp.begin(), bp.end(), p) - bp.begin());
        long long bsum = totalBuy - buyPref[lo];
        // 出价 <= p 的卖单总量
        int hi = (int)(upper_bound(sp.begin(), sp.end(), p) - sp.begin());
        long long ssum = sellPref[hi];
        long long vol = min(bsum, ssum);
        if (vol >= bestVol) {      // 升序扫描，等值时取更高价
            bestVol = vol;
            bestCents = p;
        }
    }

    printf("%lld.%02lld %lld\n", (long long)(bestCents / 100), (long long)(bestCents % 100), bestVol);
    return 0;
}
