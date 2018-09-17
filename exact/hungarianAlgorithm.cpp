#define INF 100000000 
#include <iostream>

int n, mark, dist[128][128], newdist[128][128];
int m[128], h[128], vh[128], vm[128];
 
int augment(int cur){
    int i; 
    vm[cur] = mark;
    
    for(i=0;i<n;i++)
    {
        if(!newdist[cur][i] && m[i]==-1){
            m[i]=cur;
            h[cur]=i; 
            return 1;
        }
    }
    

    for (i=0; i<n; i++)
    {
        if(!newdist[cur][i] && vh[i]!=mark){
            vh[i] = mark;
            if(augment(m[i])){
                h[cur]=i;
                m[i]=cur;
                return 1;
            }
        }
    }

    return 0;
}
 
int hungarian(){

    for (int i=0; i<n; i++ )
    {
        for (int j=0; j<n; j++)
            newdist[i][j]=dist[i][j];
    }
    
    mark=0;
    
    for (int i=0; i<n; i++)
    {
        vm[i]=0;
        h[i]=-1;
        
        vh[i]=0;
        m[i]=-1;
    }

    for (int k=0; k<n; k++)
    {
        mark++;
        while(!augment(k))
        {
            int minc = INF;
            for (int i=0; i<n; i++)
            {
                if (vm[i]==mark) 
                {
                    for (int j=0; j<n; j++)
                    {
                        if(vh[j]!=mark && newdist[i][j] && newdist[i][j]<minc)
                            minc=newdist[i][j];
                    }
                }
            }

            for (int i=0; i<n; i++)
            {
                if(vm[i]==mark)
                {
                    for (int j=0; j<n; j++)
                        newdist[i][j] -= minc;
                }               
            }

            for (int j=0; j<n;j++)
            {
                if(vh[j]==mark)
                {
                    for (int i=0; i<n; i++)
                        newdist[i][j] += minc;
                }
            }

            mark++;
        }
    }

    int ans = 0;
    for (int i=0; i<n; i++)
        ans += dist[i][h[i]];
    return ans;
}

int main ()
{
    /**
     * Define here a cost matrix
     */


    std::cout << hungarian() << std::endl;
    return 0;
}