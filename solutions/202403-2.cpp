#include <bits/stdc++.h>
using namespace std;
int main() {
    int n,m;cin >> n >> m;unordered_set<string>A,B;
    for(int i=0;i<n+m;++i) {
        string s;cin >> s;for(char &c:s)c=tolower((unsigned char)c);
        (i<n?A:B).insert(s);
    }
    int both=0;for(auto &s:A)both+=B.count(s);
    cout << both << '\n' << A.size()+B.size()-both << '\n';
}
