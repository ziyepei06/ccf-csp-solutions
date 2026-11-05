#include <bits/stdc++.h>
using namespace std;
const long long MOD=1000000007;
struct Var{long long constant=0;vector<int>deps;};
int main(){
    int n;cin >> n;string line;getline(cin,line);unordered_map<string,int>id;vector<Var>a;
    auto getid=[&](const string&s){auto it=id.find(s);if(it!=id.end())return it->second;int v=a.size();id[s]=v;a.push_back({});return v;};
    auto evaluate=[&](int root,vector<long long>&memo){
        vector<pair<int,bool>>st{{root,false}};
        while(!st.empty()){
            auto [u,exit]=st.back();st.pop_back();if(memo[u]>=0)continue;
            if(exit){long long x=a[u].constant;for(int v:a[u].deps)x=(x+memo[v])%MOD;memo[u]=x;}
            else{st.push_back({u,true});for(int v:a[u].deps)if(memo[v]<0)st.push_back({v,false});}
        }return memo[root];
    };
    while(n--){
        getline(cin,line);istringstream ss(line);int op;string name;ss >> op >> name;int u=getid(name);
        if(op==3){vector<long long>memo(a.size(),-1);cout << evaluate(u,memo) << '\n';continue;}
        Var expr;string token;
        while(ss >> token)if(token[0]=='$')expr.deps.push_back(getid(token.substr(1)));else expr.constant=(expr.constant+token.size())%MOD;
        if(op==1){vector<long long>memo(a.size(),-1);for(int v:expr.deps)expr.constant=(expr.constant+evaluate(v,memo))%MOD;expr.deps.clear();}
        a[u]=move(expr);
    }
}
