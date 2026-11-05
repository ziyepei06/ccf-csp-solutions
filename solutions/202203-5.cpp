#include <cstdio>
#include <vector>
using namespace std;
typedef long long ll;

int main(){
    int n, k;
    if(scanf("%d %d", &n, &k) != 2) return 0;
    vector<ll> a(n);
    for (int i = 0; i < n; i++) scanf("%lld", &a[i]);

    int m = n / 2 + 1;                    // 最终保留的元素个数(未被扔掉的个数)
    // 最后一个操作者:走第 n-1 步的人。k=0 时小 c 走奇数步,k=1 时小 z 走奇数步
    bool cLast = (n % 2 == 0 && k == 0) || (n % 2 == 1 && k == 1);

    if (!cLast) {
        // 小 z 最后一步(合并),答案 = 长度为 m 的连续段和的最小值(滑动窗口)
        ll cur = 0;
        for (int i = 0; i < m; i++) cur += a[i];
        ll best = cur;
        for (int i = m; i < n; i++) {
            cur += a[i] - a[i - m];
            if (cur < best) best = cur;
        }
        printf("%lld\n", best);
    } else {
        // 小 c 最后一步(扔),答案 = 把数组切成长度为 m 份时"最小份和"的最大值
        // 二分答案 X,贪心判断能否切出 >= m 段且每段和 >= X
        ll lo = 0, hi = 0;
        for (int i = 0; i < n; i++) hi += a[i];
        hi += 1;                           // hi 一定不可行
        while (lo + 1 < hi) {
            ll mid = lo + (hi - lo) / 2;
            int cnt = 0;
            ll s = 0;
            for (int i = 0; i < n; i++) {
                s += a[i];
                if (s >= mid) { cnt++; s = 0; if (cnt >= m) break; }
            }
            if (cnt >= m) lo = mid; else hi = mid;
        }
        printf("%lld\n", lo);
    }
    return 0;
}
