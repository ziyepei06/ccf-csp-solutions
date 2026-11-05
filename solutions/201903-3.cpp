// CCF CSP 201903-3 损坏的 RAID5
// RAID5: n 块盘, 条带 s 块, 现存活 l 块. 块大小 4 字节(8 个十六进制字符).
// 第 k 个条带组(k 从 0 开始)的校验盘为 (n-1) - (k mod n),
// 数据条带从校验盘的下一块盘开始顺时针依次存放.
// 逻辑块 bi 落点: k = bi / ((n-1)*s), r = (bi % ((n-1)*s)) / s, off = bi % s
//                盘 = (校验盘 + 1 + r) mod n, 盘内块号 = k*s + off
// 缺失一块盘时用其余盘同位置数据异或恢复.
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>

using namespace std;

static string inbuf;
static size_t pos_ = 0;

static inline int nextInt(long long &out) {
    const size_t L = inbuf.size();
    while (pos_ < L && (inbuf[pos_] < '0' || inbuf[pos_] > '9')) ++pos_;
    if (pos_ >= L) return 0;
    long long v = 0;
    while (pos_ < L && inbuf[pos_] >= '0' && inbuf[pos_] <= '9') {
        v = v * 10 + (inbuf[pos_] - '0');
        ++pos_;
    }
    out = v;
    return 1;
}

static inline void nextToken(string &tok) {
    const size_t L = inbuf.size();
    while (pos_ < L && (inbuf[pos_] == ' ' || inbuf[pos_] == '\n' ||
                        inbuf[pos_] == '\r' || inbuf[pos_] == '\t'))
        ++pos_;
    size_t st = pos_;
    while (pos_ < L && inbuf[pos_] != ' ' && inbuf[pos_] != '\n' &&
           inbuf[pos_] != '\r' && inbuf[pos_] != '\t')
        ++pos_;
    tok.assign(inbuf, st, pos_ - st);
}

static inline int hexv(char c) {
    return (c <= '9') ? (c - '0') : (c - 'A' + 10);
}

// 从十六进制串的第 idx 个块(每块 8 字符 = 4 字节)取 4 字节
static inline unsigned getBlock(const string &s, long long idx) {
    const char *p = s.data() + idx * 8;
    unsigned v = 0;
    for (int i = 0; i < 8; ++i) v = (v << 4) | (unsigned)hexv(p[i]);
    return v;
}

int main() {
    // 一次性读入全部输入(数据量可达数十 MB), 避免 cin 慢
    {
        size_t cap = 1 << 20, len = 0;
        inbuf.resize(cap);
        while (true) {
            if (len == inbuf.size()) inbuf.resize(inbuf.size() * 2);
            size_t r = fread(&inbuf[len], 1, inbuf.size() - len, stdin);
            if (r == 0) break;
            len += r;
        }
        inbuf.resize(len);
    }

    long long n, s, l;
    if (!nextInt(n)) return 0;
    nextInt(s);
    nextInt(l);

    vector<string> disk((size_t)n);
    vector<char> exist((size_t)n, 0);
    long long present = 0, missingIdx = -1;

    for (long long i = 0; i < l; ++i) {
        long long id;
        nextInt(id);
        string tok;
        nextToken(tok);
        disk[(size_t)id] = std::move(tok);
        exist[(size_t)id] = 1;
        ++present;
    }

    long long B = 0;  // 每块盘的块数
    for (long long i = 0; i < n; ++i)
        if (exist[(size_t)i]) { B = (long long)disk[(size_t)i].size() / 8; break; }

    long long missCnt = n - present;
    if (missCnt == 1)
        for (long long i = 0; i < n; ++i)
            if (!exist[(size_t)i]) { missingIdx = i; break; }

    // 缺失盘的块内容按需异或恢复并缓存
    vector<unsigned> cacheVal((size_t)B, 0);
    vector<char> cached((size_t)B, 0);

    const long long dataPerDisk = (n - 1) * B;  // 阵列总逻辑块数
    const long long groupSpan = (n - 1) * s;    // 每个条带组承载的逻辑块数

    long long m;
    nextInt(m);

    string out;
    out.reserve((size_t)m * 9 + 16);

    for (long long qi = 0; qi < m; ++qi) {
        long long bi;
        if (!nextInt(bi)) break;

        if (bi < 0 || bi >= dataPerDisk) { out += "-\n"; continue; }

        long long k = bi / groupSpan;
        long long rem = bi % groupSpan;
        long long r = rem / s;
        long long off = rem % s;

        long long parity = (n - 1) - (k % n);
        long long d = (parity + 1 + r) % n;
        long long blk = k * s + off;

        if (exist[(size_t)d]) {
            unsigned v = getBlock(disk[(size_t)d], blk);
            char tmp[9];
            snprintf(tmp, sizeof(tmp), "%08X", v);
            out.append(tmp, 8);
            out += '\n';
        } else if (missCnt == 1) {
            // 恰好缺一块盘: 其余全部盘异或
            unsigned v;
            if (cached[(size_t)blk]) {
                v = cacheVal[(size_t)blk];
            } else {
                v = 0;
                for (long long j = 0; j < n; ++j) {
                    if (j == missingIdx) continue;
                    v ^= getBlock(disk[(size_t)j], blk);
                }
                cacheVal[(size_t)blk] = v;
                cached[(size_t)blk] = 1;
            }
            char tmp[9];
            snprintf(tmp, sizeof(tmp), "%08X", v);
            out.append(tmp, 8);
            out += '\n';
        } else {
            // 缺盘过多, 无法推算
            out += "-\n";
        }

        // 超长输出时及时刷出, 避免内存膨胀
        if (out.size() > (1u << 20)) { fwrite(out.data(), 1, out.size(), stdout); out.clear(); }
    }

    if (!out.empty()) fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
