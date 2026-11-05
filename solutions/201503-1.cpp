#include <bits/stdc++.h>
using namespace std;

static char buf[1 << 25];
int main() {
    // Read all input at once for speed (n*m up to 1e6 numbers).
    size_t len = fread(buf, 1, sizeof(buf) - 1, stdin);
    buf[len] = '\0';

    const char *p = buf;
    auto readInt = [&]() -> int {
        while (*p && (*p < '0' || *p > '9') && *p != '-') ++p;
        int sign = 1;
        if (*p == '-') { sign = -1; ++p; }
        int v = 0;
        while (*p >= '0' && *p <= '9') { v = v * 10 + (*p - '0'); ++p; }
        return v * sign;
    };

    int n = readInt();
    int m = readInt();
    if (n <= 0 || m <= 0) return 0;

    // Store row-wise: a[r][c]
    vector<int> a((size_t)n * m);
    for (int r = 0; r < n; ++r)
        for (int c = 0; c < m; ++c)
            a[(size_t)r * m + c] = readInt();

    // Counter-clockwise rotation by 90 degrees:
    // output is m rows x n cols, out[i][j] = a[j][m-1-i]
    string out;
    out.reserve((size_t)m * n * 4 + m);
    char tmp[8];
    int tl;
    auto writeInt = [&](int v) {
        tl = 0;
        if (v == 0) { out.push_back('0'); return; }
        if (v < 0) { out.push_back('-'); v = -v; }
        while (v > 0) { tmp[tl++] = char('0' + v % 10); v /= 10; }
        while (tl > 0) out.push_back(tmp[--tl]);
    };

    for (int i = 0; i < m; ++i) {
        int base = (m - 1 - i);
        for (int j = 0; j < n; ++j) {
            if (j) out.push_back(' ');
            writeInt(a[(size_t)j * m + base]);
        }
        out.push_back('\n');
    }
    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
