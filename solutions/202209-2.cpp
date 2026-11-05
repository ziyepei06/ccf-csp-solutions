#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,x; cin >> n >> x; vector<int>a(n);int S=0;
    for (int &v:a) {cin >> v;S+=v;}
    vector<char> dp(S+1);dp[0]=1;
    for (int v:a) for (int s=S;s>=v;--s) dp[s]|=dp[s-v];
    while (!dp[x]) ++x;cout << x << '\n';
}
