// 枚举首次聚拢中心的精确基础解：O(n^2 log n)，适用于 n <= 3000。
// 算法交叉参考：https://pcs2.roj.ac.cn/problems/shumeng/CSP202605E/
// 下面独立实现红点删除，使用 list::splice 与并查集跳过已删祖先。
#include <bits/stdc++.h>
using namespace std;
struct FixedRoot{
    int n,root,head;
    const vector<vector<int>>&g;
    vector<int> parent,order,small,nearest,skip,byValue;
    vector<bool> red,ready;
    vector<list<int>> children;
    vector<list<int>::iterator> position;
    set<pair<int,int>> available,visible;
    deque<int> word;
    FixedRoot(const vector<vector<int>>&g,int root):n(g.size()),root(root),g(g),
        parent(n,-1),small(n,n),nearest(n,n),skip(n+1),byValue(n,-1),
        red(n),ready(n),children(n+1),position(n){
        iota(skip.begin(),skip.end(),0);order.push_back(root);
        for(size_t at=0;at<order.size();at++){
            int u=order[at];for(int v:g[u])if(v!=parent[u]){
                parent[v]=u;order.push_back(v);small[u]=min(small[u],v);
            }
        }
        vector<int> first=g[root];first.push_back(root);sort(first.begin(),first.end());
        for(int v:first)word.push_back(v);head=word.front();
        for(int u:order)if(u!=root){
            int p=parent[u];nearest[u]=red[p]?p:nearest[p];
            if(small[u]<head){
                red[u]=true;byValue[small[u]]=u;
                children[nearest[u]].push_back(u);position[u]=prev(children[nearest[u]].end());
                if(nearest[u]==n)visible.insert({small[u],u});
            }
        }
        for(int v:g[root])expose(v);
    }
    int find(int u){return skip[u]==u?u:skip[u]=find(skip[u]);}
    void expose(int u){if(small[u]<n){ready[u]=true;available.insert({small[u],u});}}
    void eraseRed(int u){
        if(!red[u])return;
        int p=find(nearest[u]);
        if(p==n){
            visible.erase({small[u],u});
            for(int v:children[u])visible.insert({small[v],v});
        }
        // 整段接到存活祖先，不逐个修改孩子的父指针。
        children[p].splice(children[p].end(),children[u]);children[p].erase(position[u]);
        red[u]=false;skip[u]=p;
    }
    vector<int> solve(){
        while((int)word.size()<n){
            int r=visible.empty()?-1:visible.rbegin()->second;
            auto it=available.upper_bound({head,n});
            int ordinary=it==available.end()?-1:it->second;
            int after=n;
            if(r!=-1)for(int v:g[r])if(v!=parent[r]&&v>=head){after=v;break;}
            int chosen=r;
            if(ordinary!=-1&&(r==-1||!ready[r]||small[ordinary]<after))chosen=ordinary;
            if(chosen==-1)throw runtime_error("unreachable expansion state");
            available.erase({small[chosen],chosen});ready[chosen]=false;eraseRed(chosen);
            vector<int> add;
            for(int v:g[chosen])if(v!=parent[chosen])add.push_back(v);
            auto cut=lower_bound(add.begin(),add.end(),head);
            for(auto p=cut;p!=add.begin();){--p;word.push_front(*p);}
            for(auto p=cut;p!=add.end();++p)word.push_back(*p);
            for(int v:add)expose(v);
            int newHead=word.front();
            for(int x=head-1;x>=newHead;x--)if(byValue[x]!=-1)eraseRed(byValue[x]);
            head=newHead;
        }
        return vector<int>(word.begin(),word.end());
    }
};
int main(){
    ios::sync_with_stdio(false);cin.tie(nullptr);
    int n;cin>>n;vector<vector<int>>g(n);
    for(int i=1;i<n;i++){int u,v;cin>>u>>v;--u;--v;g[u].push_back(v);g[v].push_back(u);}
    for(auto &row:g)sort(row.begin(),row.end());
    vector<int> answer;
    for(int r=0;r<n;r++){
        auto current=FixedRoot(g,r).solve();
        if(answer.empty()||current<answer)answer=move(current);
    }
    for(int i=0;i<n;i++)cout<<answer[i]+1<<(i+1==n?'\n':' ');
}
