#include <bits/stdc++.h>
using namespace std;

// 201712-2 游戏
// n 个小朋友围成一圈报数，报到 k 的倍数或个位为 k 的人淘汰，
// 求最后剩下的人。用循环双链表把每个人挂在圈上，淘汰时 O(1) 摘除。
int main() {
    int n, k;
    if (scanf("%d %d", &n, &k) != 2) return 1;
    if (n <= 0) { printf("0\n"); return 0; }

    vector<int> nxt(n + 1), pre(n + 1);
    for (int i = 1; i <= n; i++) {
        nxt[i] = (i == n ? 1 : i + 1);
        pre[i] = (i == 1 ? n : i - 1);
    }

    int alive = n;
    int cur = 1;    // 本轮要报数的人
    long long num = 0;   // 已报到的数

    while (alive > 1) {
        num++;
        if (num % k == 0 || num % 10 == k) {
            // cur 淘汰
            nxt[pre[cur]] = nxt[cur];
            pre[nxt[cur]] = pre[cur];
            alive--;
            cur = nxt[cur];   // 下一个报数的人
        } else {
            cur = nxt[cur];
        }
    }

    printf("%d\n", cur);
    return 0;
}
