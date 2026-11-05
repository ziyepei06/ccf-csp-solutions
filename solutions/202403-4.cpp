#include <bits/stdc++.h>
using namespace std;
int main() {
    int c,m,n;cin >> c >> m >> n;map<int,int>a;
    for(int i=0,x,w;i<m;++i) {cin >> x >> w;a[x]=w;}
    priority_queue<int,vector<int>,greater<int>> q;
    while(n--) {
        int p;cin >> p;if(++a[p]>=5)q.push(p);
        while(!q.empty()) {
            int x=q.top();q.pop();auto it=a.find(x);
            if(it==a.end() || it->second<5)continue;
            auto right=next(it);int left=-1;
            if(it!=a.begin())left=prev(it)->first;
            a.erase(it);
            if(left!=-1 && ++a[left]>=5)q.push(left);
            if(right!=a.end() && ++right->second>=5)q.push(right->first);
        }
        cout << a.size() << '\n';
    }
}
