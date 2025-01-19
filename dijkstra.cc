#include <bits/stdc++.h>
using namespace std;

const int MAXN = 1010;      // 根据题目需要调整
const int INF = 0x3f3f3f3f; // 代表无穷大

int G[MAXN][MAXN];  // 邻接矩阵
int dista[MAXN];    // dist数组，存储源点到各点的最短距离
bool used[MAXN];    // 标记该点的最短距离是否已经确定

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, m; 
    cin >> n >> m;
    
    // 1) 初始化邻接矩阵
    // G[i][j] = INF 表示节点i到节点j无法直达
    for(int i = 1; i <= n; i++){
        for(int j = 1; j <= n; j++){
            if(i == j) G[i][j] = 0; // 自己到自己距离0（有时无需设置，INF也行）
            else G[i][j] = INF;
        }
    }

    // 2) 读入边并填充邻接矩阵
    // 以下假设是有向图，如果题目是无向图，请同时设置 G[b][a] = w
    for(int i = 0; i < m; i++){
        int a, b, w;
        cin >> a >> b >> w;
        // 如果存在多条(a -> b)边，可选取更小权重
        if(w < G[a][b]) {
            G[a][b] = w;
        }
        // 无向图请在此添加: G[b][a] = w;
    }

    // 3) Dijkstra：以1号点为源点
    //    dista[i] 存储"从1号点到 i号点"的最短距离
    for(int i = 1; i <= n; i++){
        dista[i] = INF;
        used[i] = false;
    }
    dista[1] = 0; // 源点1号到自己距离为0
    
    // 4) 主循环：执行 n 轮
    for(int i = 0; i < n; i++){
        // (1) 找到未确定最短距离的节点中dist最小者 t
        int t = -1;
        for(int j = 1; j <= n; j++){
            if(!used[j] && (t == -1 || dista[j] < dista[t])){
                t = j;
            }
        }
        
        // 若找不到可更新的节点 或 dista[t] == INF，提前结束
        if(t == -1 || dista[t] == INF) break;
        
        // (2) 标记 t 的最短距离已确定
        used[t] = true;
        
        // (3) 用 t 来松弛其他节点
        for(int j = 1; j <= n; j++){
            if(!used[j] && G[t][j] < INF){
                // dista[t] + G[t][j] 是从1号到 j 的可能新距离
                int nd = dista[t] + G[t][j];
                if(nd < dista[j]){
                    dista[j] = nd;
                }
            }
        }
    }

    // 5) 判断 dista[n] 是否被更新过
    if(dista[n] >= INF) cout << -1 << "\n";
    else cout << dista[n] << "\n";

    return 0;
}
