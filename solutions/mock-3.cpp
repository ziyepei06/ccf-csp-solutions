#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;
    vector<long long> h(n + 2, 0);
    for (int i = 1; i <= n; ++i) cin >> h[i];
    // h[0] = h[n+1] = 0 act as sentinels so every bar gets popped.

    long long ans = 0;
    vector<int> st;          // stack of indices with non-decreasing heights
    st.reserve(n + 2);
    for (int i = 0; i <= n + 1; ++i) {
        while (!st.empty() && h[st.back()] > h[i]) {
            long long height = h[st.back()];
            st.pop_back();
            int left = st.empty() ? 0 : st.back();
            long long width = i - left - 1;
            ans = max(ans, height * width);
        }
        st.push_back(i);
    }
    cout << ans << '\n';
    return 0;
}
