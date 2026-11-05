#include <bits/stdc++.h>
using namespace std;

int main() {
    int n;
    if (!(cin >> n)) return 0;
    vector<long long> B(n);
    for (int i = 0; i < n; ++i) cin >> B[i];

    // 最大和：每个 A_i 都取 B_i（B 单调不降，前缀最大值恰为 B）
    // 最小和：A_1 = B_1；i > 1 时若 B_i > B_{i-1} 则 A_i 必须等于 B_i，
    //         否则取 0（A_i 为非负整数，不会超过当前前缀最大值 B_i）
    long long maxSum = 0, minSum = 0;
    for (int i = 0; i < n; ++i) maxSum += B[i];
    minSum = B[0];
    for (int i = 1; i < n; ++i)
        if (B[i] > B[i - 1]) minSum += B[i];

    cout << maxSum << "\n" << minSum << "\n";
    return 0;
}
