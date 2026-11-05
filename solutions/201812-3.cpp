// 201812-3 CIDR 合并
// 1) 解析三种简写形式，统一成 <ip, len> 并规范化（低位清零）；
// 2) 按 (ip, len) 升序排序；
// 3) 第二步：相邻 a,b，若 b 的匹配集包含于 a 的匹配集则删除 b；
// 4) 第三步：相邻同长 a,b，若 b == a + 2^(32-len) 且 a'=(a.ip,len-1) 合法则合并，
//    合并后回退到 a' 的前一个元素继续。
// 两次扫描用双向链表（数组实现）保证整体 O(n log n)。
#include <bits/stdc++.h>
using namespace std;
typedef unsigned int u32;
typedef unsigned long long u64;

// ---------- 快速输入 ----------
static char ibuf[1 << 16];
static int ipos = 0, ilen = 0;
static inline int gc() {
    if (ipos == ilen) {
        ilen = (int)fread(ibuf, 1, sizeof(ibuf), stdin);
        ipos = 0;
        if (ilen <= 0) return -1;
    }
    return (unsigned char)ibuf[ipos++];
}

// ---------- 快速输出 ----------
static char obuf[1 << 16];
static int opos = 0;
static inline void flushOut() {
    if (opos) { fwrite(obuf, 1, opos, stdout); opos = 0; }
}
static inline void putc_(char c) {
    if (opos == (int)sizeof(obuf)) flushOut();
    obuf[opos++] = c;
}
static inline void putNum(u32 x) {
    char t[12];
    int k = 0;
    if (x == 0) t[k++] = '0';
    while (x) { t[k++] = (char)('0' + x % 10); x /= 10; }
    while (k) putc_(t[--k]);
}

int main() {
    // 读 n
    int c = gc();
    while (c != -1 && (c < '0' || c > '9')) c = gc();
    if (c == -1) return 0;
    int n = 0;
    while (c >= '0' && c <= '9') { n = n * 10 + (c - '0'); c = gc(); }

    vector<u64> key;                 // (ip << 8) | len，同时作为排序键
    key.reserve(n > 0 ? n : 0);

    char line[64];
    for (int t = 0; t < n; ++t) {
        int L = 0;
        c = gc();
        while (c != -1 && c != '\n') {
            if (c != '\r' && L < 63) line[L++] = (char)c;
            c = gc();
        }
        if (L == 0) {
            if (c == -1) break;          // 输入提前结束，防止死循环
            --t;                         // 跳过空行
            continue;
        }

        // 解析点分十进制（至少一段），遇到 '/' 结束地址部分
        u32 ip = 0;
        u32 cur = 0;
        int nseg = 0;
        int i = 0;
        while (i < L && line[i] != '/') {
            char ch = line[i];
            if (ch == '.') {
                ip = (ip << 8) | cur;
                cur = 0;
                ++nseg;
                ++i;
            } else if (ch >= '0' && ch <= '9') {
                cur = cur * 10 + (u32)(ch - '0');
                ++i;
            } else {
                break;                   // 行内空白等杂字符，视作地址部分结束
            }
        }
        ip = (ip << 8) | cur;
        ++nseg;
        // 只写了前 nseg 段，剩余低位段视为 0，故左移到最高 nseg 段
        ip <<= (4 - nseg) * 8;

        int len;
        if (i < L) {                     // 有斜线：显式给出前缀长度
            ++i;
            len = 0;
            while (i < L && line[i] >= '0' && line[i] <= '9') {
                len = len * 10 + (line[i] - '0');
                ++i;
            }
        } else {                         // 省略长度型：len = 8 * 段数
            len = nseg * 8;
        }
        // 规范化：低 (32-len) 位清零（合法输入本就为 0，这里兜底）
        if (len < 32) ip &= (u32)(0xFFFFFFFFull << (32 - len));
        key.push_back(((u64)ip << 8) | (u64)len);
    }

    sort(key.begin(), key.end());
    size_t m = key.size();
    if (m == 0) return 0;

    vector<u32> ip(m);
    vector<int> plen(m);
    for (size_t i = 0; i < m; ++i) {
        ip[i] = (u32)(key[i] >> 8);
        plen[i] = (int)(key[i] & 0xFF);
    }
    key.clear();
    key.shrink_to_fit();

    // 双向链表
    vector<int> nx(m), pv(m);
    for (size_t i = 0; i < m; ++i) { nx[i] = (i + 1 < m) ? (int)i + 1 : -1; pv[i] = (int)i - 1; }

    // 第二步：删除被前驱覆盖的元素
    int head = 0;
    int i = head;
    while (nx[i] != -1) {
        int j = nx[i];
        int L = plen[i];
        bool sub;
        if (L == 0) {
            sub = true;                                  // 0.0.0.0/0 覆盖一切
        } else if (plen[j] < L) {
            sub = false;
        } else {
            sub = ((ip[j] >> (32 - L)) == (ip[i] >> (32 - L)));
        }
        if (sub) {                                       // 删除 j，i 不动
            int k = nx[j];
            nx[i] = k;
            if (k != -1) pv[k] = i;
        } else {
            i = j;
        }
    }

    // 第三步：同级合并
    i = head;
    while (nx[i] != -1) {
        int j = nx[i];
        int L = plen[i];
        bool canMerge = false;
        if (L == plen[j] && L >= 1) {
            u32 step = 1u << (32 - L);
            // a' 合法 <=> a.ip 的第 (32-L) 位为 0；两段恰好拼成 a'
            if ((ip[i] & step) == 0 && ip[j] == ip[i] + step) canMerge = true;
        }
        if (canMerge) {
            int k = nx[j];
            nx[i] = k;
            if (k != -1) pv[k] = i;
            plen[i] = L - 1;                             // a' 取代 a，b 被移除
            if (pv[i] != -1) i = pv[i];                  // 回到 a' 的前一个元素
        } else {
            i = j;
        }
    }

    // 输出（链表顺序即 (ip,len) 升序）
    for (int p = head; p != -1; p = nx[p]) {
        u32 x = ip[p];
        putNum((x >> 24) & 255); putc_('.');
        putNum((x >> 16) & 255); putc_('.');
        putNum((x >> 8) & 255);  putc_('.');
        putNum(x & 255);         putc_('/');
        putNum((u32)plen[p]);    putc_('\n');
    }
    flushOut();
    return 0;
}
