#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;

    long long sum = 0;  // |w|<=10, score<=100, n<=1e5 => |sum|<=1e8
    for (int i = 0; i < n; ++i) {
        long long w, score;
        cin >> w >> score;
        sum += w * score;
    }

    // y = ReLU(sum) = max(0, sum)
    cout << (sum > 0 ? sum : 0LL) << '\n';
    return 0;
}
