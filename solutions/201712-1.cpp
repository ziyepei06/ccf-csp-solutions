#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;
    vector<int> a(n);
    for (int i = 0; i < n; ++i) cin >> a[i];

    sort(a.begin(), a.end());
    int ans = INT_MAX;
    for (int i = 1; i < n; ++i) ans = min(ans, a[i] - a[i - 1]);

    cout << ans << '\n';
    return 0;
}
