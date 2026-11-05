#include <bits/stdc++.h>
using namespace std;
int main(){
    int n,N,q;cin >> n >> N >> q;using Entry=pair<int,bool>;
    vector<list<Entry>>cache(N);unordered_map<int,list<Entry>::iterator>where;
    while(q--){
        int op,a;cin >> op >> a;int g=(a/n)%N;auto &c=cache[g];auto it=where.find(a);
        if(it!=where.end()){
            it->second->second|=op;c.splice(c.begin(),c,it->second);
        }else{
            if((int)c.size()==n){auto [old,dirty]=c.back();if(dirty)cout << "1 " << old << '\n';where.erase(old);c.pop_back();}
            cout << "0 " << a << '\n';c.push_front({a,op!=0});where[a]=c.begin();
        }
    }
}
