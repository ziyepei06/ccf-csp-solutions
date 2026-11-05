#include <bits/stdc++.h>
using namespace std;

// DHCP 服务器模拟
// 地址状态：0 未分配、1 待分配、2 占用、3 过期
// 只有 0（未分配）没有占用者；3（过期）保留原占用者，只是过期时刻清零。
int main() {
    int N;
    long long Tdef, Tmax, Tmin;
    char Hbuf[64];
    if (scanf("%d %lld %lld %lld %63s", &N, &Tdef, &Tmax, &Tmin, Hbuf) != 5) return 0;
    string H(Hbuf);
    int n;
    if (scanf("%d", &n) != 1) return 0;

    vector<int> st(N + 1, 0);              // 状态
    vector<string> own(N + 1);             // 占用者
    vector<long long> expT(N + 1, 0);      // 过期时刻（待分配/占用时 > 0）

    set<int> freeS;                        // 未分配，begin() 即最小 IP
    set<int> expS;                         // 过期，begin() 即最小 IP
    map<long long, set<int>> expMap;       // 过期时刻 -> 处于待分配/占用且该时刻过期的 IP
    unordered_map<string, set<int>> byOwner; // 占用者 -> 其拥有的 IP

    // 初始化地址池：全部未分配
    for (int i = 1; i <= N; ++i) freeS.insert(i);

    auto dropOwner = [&](int ip) {
        if (own[ip].empty()) return;
        auto oit = byOwner.find(own[ip]);
        if (oit != byOwner.end()) {
            oit->second.erase(ip);
            if (oit->second.empty()) byOwner.erase(oit);
        }
    };

    // 把 ip 从其所在状态集合 / 过期时间表 / 占用者表中摘下（不改 st/own/expT 本身）
    auto detach = [&](int ip) {
        if (st[ip] == 0) {
            freeS.erase(ip);
        } else if (st[ip] == 3) {
            expS.erase(ip);
        } else { // 1 或 2：在 expMap 中
            auto it = expMap.find(expT[ip]);
            if (it != expMap.end()) {
                it->second.erase(ip);
                if (it->second.empty()) expMap.erase(it);
            }
        }
        dropOwner(ip);
    };

    // 把 ip 挂到新状态上（先摘下旧状态）
    // ns==0 时 host 传空串；ns==3 时过期时刻强制为 0
    auto attach = [&](int ip, int ns, const string& host, long long e) {
        detach(ip);
        st[ip] = ns;
        own[ip] = host;
        expT[ip] = e;
        if (ns == 0) {
            own[ip].clear();
            expT[ip] = 0;
            freeS.insert(ip);
        } else if (ns == 3) {
            expT[ip] = 0;
            expS.insert(ip);
            byOwner[own[ip]].insert(ip); // 过期状态仍保留占用者
        } else {
            expMap[e].insert(ip);
            byOwner[own[ip]].insert(ip);
        }
    };

    // 时间推进到 t：所有过期时刻 <= t 的地址自动转移
    auto advance = [&](long long t) {
        while (!expMap.empty() && expMap.begin()->first <= t) {
            auto it = expMap.begin();
            vector<int> ips(it->second.begin(), it->second.end());
            expMap.erase(it);
            for (int ip : ips) {
                if (st[ip] == 1) {        // 待分配 -> 未分配：清空占用者与过期时刻
                    dropOwner(ip);
                    st[ip] = 0;
                    own[ip].clear();
                    expT[ip] = 0;
                    freeS.insert(ip);
                } else if (st[ip] == 2) { // 占用 -> 过期：只清零过期时刻，占用者保留
                    st[ip] = 3;
                    expT[ip] = 0;
                    expS.insert(ip);
                }
            }
        }
    };

    // 按报文里的过期时刻与收到时刻 t 计算最终过期时刻
    auto calcExp = [&](long long t, long long msgExp) -> long long {
        if (msgExp == 0) return t + Tdef;
        long long e = msgExp;
        if (e > t + Tmax) e = t + Tmax;
        if (e < t + Tmin) e = t + Tmin;
        return e;
    };

    for (int i = 0; i < n; ++i) {
        long long t, ip, mexp;
        char sbuf[64], rbuf[64], tbuf[16];
        if (scanf("%lld %63s %63s %15s %lld %lld", &t, sbuf, rbuf, tbuf, &ip, &mexp) != 6) break;
        string sender(sbuf), recv(rbuf), type(tbuf);

        advance(t);

        bool toH = (recv == H), toStar = (recv == "*");

        if (!toH && !toStar && type != "REQ") continue; // 规则 1
        if (type != "DIS" && type != "REQ") continue;    // 规则 2
        if (toStar && type != "DIS") continue;           // 规则 3
        if (toH && type == "DIS") continue;              // 规则 3

        if (type == "DIS") {
            int chosen = -1;
            auto oit = byOwner.find(sender);
            if (oit != byOwner.end() && !oit->second.empty()) {
                chosen = *oit->second.begin();   // 该客户端已有地址（含待分配/占用/过期），直接复用
            } else if (!freeS.empty()) {
                chosen = *freeS.begin();         // 最小未分配
            } else if (!expS.empty()) {
                chosen = *expS.begin();          // 最小过期
            }
            if (chosen < 0) continue;            // 地址池耗尽，忽略该报文

            long long e = calcExp(t, mexp);
            attach(chosen, 1, sender, e);
            printf("%s %s OFR %d %lld\n", H.c_str(), sender.c_str(), chosen, e);
        } else { // REQ
            if (!toH) {
                // 发给其它服务器：解除该客户端的所有待分配地址
                auto oit = byOwner.find(sender);
                if (oit != byOwner.end()) {
                    vector<int> ips(oit->second.begin(), oit->second.end());
                    for (int ipx : ips)
                        if (st[ipx] == 1) attach(ipx, 0, "", 0);
                }
                continue;
            }
            if (ip < 1 || ip > N || own[ip] != sender) {
                printf("%s %s NAK %lld 0\n", H.c_str(), sender.c_str(), ip);
                continue;
            }
            long long e = calcExp(t, mexp);
            attach((int)ip, 2, sender, e);
            printf("%s %s ACK %lld %lld\n", H.c_str(), sender.c_str(), ip, e);
        }
    }
    return 0;
}
