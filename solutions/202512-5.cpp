#include <bits/stdc++.h>
using namespace std;
struct Node{int child[2]={0,0},count=0,match=0,cost=0;};
vector<Node>tr(1);vector<int>recycle;int W;
int make(){if(recycle.empty()){tr.push_back({});return tr.size()-1;}int u=recycle.back();recycle.pop_back();tr[u]={};return u;}
int cnt(int u){return tr[u].count;}
void insert(int root,int x){
    int path[31],partner[31];path[30]=partner[30]=root;
    for(int r=30;r;--r){
        int bit=(x>>(r-1))&1,u=path[r];int v=tr[u].child[bit];
        if(!v){v=make();tr[u].child[bit]=v;}path[r-1]=v;
        int p=partner[r];partner[r-1]=p?tr[p].child[((x^W)>>(r-1))&1]:0;
    }
    for(int r=0;r<=30;++r)++tr[path[r]].count;
    for(int r=0;r<=30;++r){
        int u=path[r],v=partner[r],matching=0;
        if(v){
            if(!r)matching=min(cnt(u),cnt(v));
            else{
                int a=tr[u].child[0],b=tr[u].child[1],c=tr[v].child[0],d=tr[v].child[1];
                if((W>>(r-1))&1)matching=tr[a].match+tr[b].match;
                else matching=min({cnt(u),cnt(v),cnt(a)+cnt(c)+tr[b].match,cnt(b)+cnt(d)+tr[a].match});
            }
        }
        tr[u].match=matching;if(v)tr[v].match=matching;
        if(!r)tr[u].cost=cnt(u);
        else{
            int a=tr[u].child[0],b=tr[u].child[1];
            tr[u].cost=((W>>(r-1))&1)?cnt(u)-tr[a].match:max(tr[a].cost,tr[b].cost);
        }
    }
}
void moveValues(int u,int bit,int prefix,int target){
    if(!u)return;int l=tr[u].child[0],r=tr[u].child[1],c=tr[u].count;
    if(bit<0){while(c--)insert(target,prefix);}else{moveValues(l,bit-1,prefix,target);moveValues(r,bit-1,prefix|(1<<bit),target);}
    recycle.push_back(u);
}
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);int n;cin >> n >> W;vector<int>roots(n+1),size(n+1);long long total=0;
    for(int i=1;i<=n;++i){roots[i]=make();cin >> size[i];for(int j=0,x;j<size[i];++j){cin >> x;insert(roots[i],x);}total+=tr[roots[i]].cost;}
    int q;cin >> q;while(q--){
        int op;cin >> op;
        if(op==1){int u,x;cin >> u >> x;total-=tr[roots[u]].cost;insert(roots[u],x);++size[u];total+=tr[roots[u]].cost;}
        else if(op==2){int u,v;cin >> u >> v;total-=tr[roots[u]].cost+tr[roots[v]].cost;
            if(size[u]<size[v]){swap(size[u],size[v]);swap(roots[u],roots[v]);}
            moveValues(roots[v],29,0,roots[u]);size[u]+=size[v];size[v]=0;roots[v]=0;total+=tr[roots[u]].cost;
        }else cout << total << '\n';
    }
}
