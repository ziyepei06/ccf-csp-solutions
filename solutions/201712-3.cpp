// 201712-3 Crontab
// 解析 5 个时间域（支持 * , - 与英文缩写），按天枚举区间内的日期，
// 日期（月/日/星期，按“同时满足”处理）匹配后展开该规则的小时*分钟，
// 落在 [s,t) 内的即产生一次调度，最后按时间稳定排序输出。
#include <bits/stdc++.h>
using namespace std;

static const char *MNAMES[12] = {"jan", "feb", "mar", "apr", "may", "jun",
                                 "jul", "aug", "sep", "oct", "nov", "dec"};
static const char *DNAMES[7] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};

inline bool isLeap(int y) { return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0; }

inline int monthLen(int m, int y) {
    static const int len[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && isLeap(y)) return 29;
    return len[m];
}

// 单个取值：数字或英文缩写（不区分大小写）
int atom(const string &s, bool isMon) {
    if (!s.empty() && isdigit((unsigned char)s[0])) return atoi(s.c_str());
    string t;
    t.reserve(s.size());
    for (char c : s) t += (char)tolower((unsigned char)c);
    if (isMon) {
        for (int i = 0; i < 12; ++i)
            if (t == MNAMES[i]) return i + 1;
    } else {
        for (int i = 0; i < 7; ++i)
            if (t == DNAMES[i]) return i;  // sun=0 ... sat=6
    }
    return -1;
}

// 解析一个域（* / 逗号列表 / 减号区间 / 混合），结果写入 out[lo..hi]
void parseField(const string &f, int lo, int hi, bool isMon, unsigned char *out) {
    for (int i = lo; i <= hi; ++i) out[i] = 0;
    if (f == "*") {
        for (int i = lo; i <= hi; ++i) out[i] = 1;
        return;
    }
    size_t pos = 0;
    while (true) {
        size_t e = f.find(',', pos);
        string part = f.substr(pos, (e == string::npos ? f.size() : e) - pos);
        if (part == "*") {
            for (int i = lo; i <= hi; ++i) out[i] = 1;
        } else {
            size_t dash = part.find('-');
            if (dash == string::npos) {
                int v = atom(part, isMon);
                if (v >= lo && v <= hi) out[v] = 1;
            } else {
                int a = atom(part.substr(0, dash), isMon);
                int b = atom(part.substr(dash + 1), isMon);
                for (int v = a; v <= b; ++v)
                    if (v >= lo && v <= hi) out[v] = 1;
            }
        }
        if (e == string::npos) break;
        pos = e + 1;
    }
}

// Howard Hinnant civil_from_days 的逆：返回相对 1970-01-01 的天数
long long daysFromCivil(int y, int m, int d) {
    long long yy = (long long)y - (m <= 2 ? 1 : 0);
    long long era = (yy >= 0 ? yy : yy - 399) / 400;
    long long yoe = yy - era * 400;                              // [0,399]
    long long doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;  // [0,365]
    long long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;       // [0,146096]
    return era * 146097 + doe - 719468;
}

struct Rule {
    unsigned char mn[60];   // 分钟 0..59
    unsigned char hr[24];   // 小时 0..23
    unsigned char dom[32];  // 日 1..31
    unsigned char mon[13];  // 月 1..12
    unsigned char dow[7];   // 星期 0..6 (Sun=0)
    string cmd;
    vector<int> hlist, mlist;
};

struct Event {
    long long ts;
    int idx;
    string cmd;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    string sStr, tStr;
    if (!(cin >> n >> sStr >> tStr)) return 0;

    auto comp = [](const string &x, int &yy, int &mm, int &dd, int &hh, int &mi) {
        yy = atoi(x.substr(0, 4).c_str());
        mm = atoi(x.substr(4, 2).c_str());
        dd = atoi(x.substr(6, 2).c_str());
        hh = atoi(x.substr(8, 2).c_str());
        mi = atoi(x.substr(10, 2).c_str());
    };
    int sy, sm, sd, sh, smi, ty, tm, td, th, tmi;
    comp(sStr, sy, sm, sd, sh, smi);
    comp(tStr, ty, tm, td, th, tmi);
    long long sTs = atoll(sStr.c_str());
    long long tTs = atoll(tStr.c_str());

    vector<Rule> rules((size_t)n);
    for (int i = 0; i < n; ++i) {
        string a, b, c, d, e;
        cin >> a >> b >> c >> d >> e >> rules[i].cmd;
        Rule &r = rules[i];
        parseField(a, 0, 59, false, r.mn);
        parseField(b, 0, 23, false, r.hr);
        parseField(c, 1, 31, false, r.dom);
        parseField(d, 1, 12, true, r.mon);
        parseField(e, 0, 6, false, r.dow);
        for (int h = 0; h < 24; ++h)
            if (r.hr[h]) r.hlist.push_back(h);
        for (int m = 0; m < 60; ++m)
            if (r.mn[m]) r.mlist.push_back(m);
    }

    vector<Event> ev;
    int y = sy, mo = sm, d = sd;
    while (true) {
        long long base = ((((long long)y * 100 + mo) * 100 + d)) * 10000;
        if (base > tTs) break;
        bool lastDay = (y == ty && mo == tm && d == td);
        // 当天与 [s,t) 有交集才需要展开
        if (base + 2359 >= sTs) {
            long long days = daysFromCivil(y, mo, d);
            int dow = (int)((days % 7 + 7 + 4) % 7);  // 1970-01-01 是周四(4)
            for (int i = 0; i < n; ++i) {
                Rule &r = rules[i];
                if (!r.mon[mo] || !r.dom[d] || !r.dow[dow]) continue;
                for (int h : r.hlist) {
                    for (int mi : r.mlist) {
                        long long ts = base + (long long)h * 100 + mi;
                        if (ts >= sTs && ts < tTs)
                            ev.push_back(Event{ts, i, r.cmd});
                    }
                }
            }
        }
        if (lastDay) break;
        ++d;
        if (d > monthLen(mo, y)) {
            d = 1;
            ++mo;
            if (mo > 12) {
                mo = 1;
                ++y;
            }
        }
    }

    stable_sort(ev.begin(), ev.end(),
                [](const Event &a, const Event &b) { return a.ts < b.ts; });

    string out;
    out.reserve(ev.size() * 24);
    for (const Event &e : ev) {
        char buf[13];
        snprintf(buf, sizeof(buf), "%012lld", e.ts);
        out += buf;
        out += ' ';
        out += e.cmd;
        out += '\n';
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
