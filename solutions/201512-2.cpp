#include <cstdio>

// 201512-2 消除类游戏
// 思路：先完整读入 n*m 棋盘，再用两个标记数组分别记录
// 由“行内连续 >=3 同色”与“列内连续 >=3 同色”引起的消除，
// 最后一并输出（同一格子可能同时被行、列标记，只输出一次 0）。
// 时间 O(n*m)，空间 O(n*m)。

int main() {
    int n = 0, m = 0;
    if (scanf("%d %d", &n, &m) != 2) return 0;
    const int MAXN = 35;
    int a[MAXN][MAXN];
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (scanf("%d", &a[i][j]) != 1) a[i][j] = 0;
        }
    }

    bool gone[MAXN][MAXN] = {{false}};

    // 行方向扫描：对每行用双指针找出所有长度 >= 3 的同色连续段
    for (int i = 0; i < n; ++i) {
        int j = 0;
        while (j < m) {
            int k = j;
            while (k + 1 < m && a[i][k + 1] == a[i][j]) ++k;
            if (k - j + 1 >= 3) {
                for (int t = j; t <= k; ++t) gone[i][t] = true;
            }
            j = k + 1;
        }
    }

    // 列方向扫描：同理
    for (int j = 0; j < m; ++j) {
        int i = 0;
        while (i < n) {
            int k = i;
            while (k + 1 < n && a[k + 1][j] == a[i][j]) ++k;
            if (k - i + 1 >= 3) {
                for (int t = i; t <= k; ++t) gone[t][j] = true;
            }
            i = k + 1;
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (j) putchar(' ');
            printf("%d", gone[i][j] ? 0 : a[i][j]);
        }
        putchar('\n');
    }
    return 0;
}
