#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace std;

// 一条指令：t='S' 表示发送，t='R' 表示接收；to 为目标进程号。
struct Cmd { char t; int to; };

static string readAll() {              // 一次性读入全部输入，避免逐行 IO 开销
    string s;
    char tmp[1 << 16];
    size_t r;
    while ((r = fread(tmp, 1, sizeof tmp, stdin)) > 0) s.append(tmp, r);
    return s;
}

int main() {
    string in = readAll();
    const char *buf = in.data();
    size_t len = in.size(), p = 0;

    auto readInt = [&]() {             // 跳过非数字，读一个整数
        while (p < len && (buf[p] < '0' || buf[p] > '9')) ++p;
        int v = 0;
        while (p < len && buf[p] >= '0' && buf[p] <= '9') {
            v = v * 10 + (buf[p] - '0');
            ++p;
        }
        return v;
    };

    int T = readInt(), n = readInt();
    while (p < len && buf[p] != '\n') ++p;   // 跳过表头行剩余部分
    if (p < len) ++p;

    vector<vector<Cmd>> g(n);
    vector<int> ptr(n);
    vector<pair<int, int>> qu;               // 待检查的进程对；vector + 头指针模拟队列
    qu.reserve((size_t)n * 9 + 16);

    for (int tc = 0; tc < T; ++tc) {
        // ---------- 读入 n 个进程的指令表（每行一条进程，行内 1-8 条指令） ----------
        for (int i = 0; i < n; ++i) {
            g[i].clear();
            for (;;) {
                while (p < len && buf[p] != '\n' && (unsigned char)buf[p] <= ' ') ++p;
                if (p >= len) break;
                if (buf[p] == '\n') { ++p; break; }    // 本行结束
                char t = buf[p++];                     // 'S' 或 'R'
                int v = 0;
                while (p < len && buf[p] >= '0' && buf[p] <= '9') {
                    v = v * 10 + (buf[p] - '0');
                    ++p;
                }
                g[i].push_back({t, v});
            }
        }

        // ---------- 队列式模拟 ----------
        // 每个进程只能执行自己的“队首指令”，而它只能与目标进程的“队首指令”配对，
        // 因此匹配是强制的、没有选择余地：两队首互相指向且一 S 一 R 时必然配对执行。
        fill(ptr.begin(), ptr.end(), 0);
        qu.clear();
        size_t qh = 0;

        for (int i = 0; i < n; ++i)
            if (!g[i].empty()) qu.push_back({i, g[i][0].to});

        while (qh < qu.size()) {
            int a = qu[qh].first, b = qu[qh].second;
            ++qh;
            if (ptr[a] >= (int)g[a].size() || ptr[b] >= (int)g[b].size()) continue;
            const Cmd &ca = g[a][ptr[a]];
            const Cmd &cb = g[b][ptr[b]];
            if (ca.to != b || cb.to != a) continue;    // 没有互相指向
            if (ca.t == cb.t) continue;                // 同为 S 或同为 R，无法配对
            ++ptr[a];                                  // 配对成功：两指令同时执行完毕
            ++ptr[b];
            if (ptr[a] < (int)g[a].size()) qu.push_back({a, g[a][ptr[a]].to});
            if (ptr[b] < (int)g[b].size()) qu.push_back({b, g[b][ptr[b]].to});
        }

        bool deadlock = false;                         // 仍有指令未执行 ⇒ 死锁
        for (int i = 0; i < n; ++i)
            if (ptr[i] != (int)g[i].size()) { deadlock = true; break; }

        putchar(deadlock ? '1' : '0');
        putchar('\n');
    }
    return 0;
}
