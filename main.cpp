#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>   //for using setw() and setprecision()
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <iterator> 

#define TESTING
//#define ISCLASS_ANS  //If the given dataset have CLASS answer
//#define DEBUG
#define element float
#define PEAK_NUM 3
using namespace std;

ifstream dataset;


class queue_node
{
    public:
        element data = 0;
        queue_node* next = NULL;
};
queue_node* head = NULL;
queue_node* tail = NULL;

void enqueue(element data)
{
    if(head)
    {
        tail->next = new queue_node;
        tail = tail->next;
        tail->data = data;
    }
    else
    {
        head = new queue_node;
        tail = head;
        head->data = data;
    }
    return;
}

element dequeue(void)
{
    element data;
    queue_node* temp;
    if(head)
    {
        data = head->data;
        temp = head;
        head = head->next;
        delete temp;
        return data;
    }
    else
    {
        return -1;
    }
}

void print_1d_vector(vector<int> vector)
{
    for(int i=0;i<vector.size();i++)
    {
        cout<<"For point "<<setw(5)<<left<<i<<"the attributes is: ";
        cout<<fixed<<setprecision(4)<<vector[i]<<"\t";
    }
    cout<<endl;
    return;
}

void print_2d_vector(vector<vector<float>> vector)
{
    for(int i = 0;i<vector.size();i++)
    {
        cout<<"For point "<<setw(5)<<left<<i<<"the attributes is: ";    
        for(int j = 0;j<vector[0].size();j++)
        {
            cout<<fixed<<setprecision(4)<<vector[i][j]<<" ";
        }
        cout<<endl;
    }
    return;
}

int get_dim(string input_filename)
{
    string temp = "";
    int i = 0,dim = 0;
    getline(dataset,temp);
    //######用來數dim, 會扣掉class的那個dim######
    while(temp[i]!='\0')
    {
        if(temp[i]==',')
        {
            dim++;
        }
        i++;
    }
    //#########################################
    dataset.close();
    dataset.open(input_filename);   //Read the dataset once more since the first element is used for count dim
    return dim;
}

float dist(vector<float> point1, vector<float> point2)
{
    float sum = 0;
    for(int i = 0;i<point1.size();i++)
    {
        sum+= pow(point1[i]-point2[i],2);
    }
    return sqrt(sum);
}

bool all_visited(vector<bool> visited)
{
    for(int i = 0;i<visited.size();i++)
    {
        if(visited[i]==0)
        {
            return false;
        }
    }
    return true;
}

int findPosVector(vector <float> input ,float number)
{
    vector<float>::iterator iter=find(input.begin(),input.end(),number);//返回的是一个迭代器指针
    if(iter == input.end())
    {
        return -1;
    } 
    else
    {
        return distance(input.begin(),iter);
    }
}


void DPC(vector<vector<float>> distance, float dc, int* cluster_result)
{
    vector<float> dep_dist;     //Distance of a point to it's nearest point with higher density 
    vector<int> dep_neighbor;   //As above, what point is that nearest point
    vector<int> rho;            //Density of each point, i.e. number of points within dc range
    vector<float> gamma;        //Equals to dep_dist * rho for every point
    int temp;
    int num_of_points = distance[0].size();
    //Calculating rho for each point
    for(int i = 0;i<num_of_points;i++)
    {
        temp = 0;
        for(int j = 0;j<num_of_points;j++)
        {
            if(distance[i][j]<=dc)
            {
                temp++;
            }
            
        }
        rho.push_back(temp);
    }

    #ifdef DEBUG
        cout<<"rho:"<<endl;
        for(int i=0;i<rho.size();i++)
        {
            cout<<i<<": ("<<rho[i]<<")"<<" ";
        }
    #endif

    vector<int> denser_num;
    vector<float> dep_dist_cand;
    
    //print_2d_vector(distance);
    print_1d_vector(rho);

    //Calculating the dependent distance and dependent neighbor
    for(int i = 0;i<num_of_points;i++)
    {
        for(int j = 0;j<num_of_points;j++)
        {
            if(rho[i]<rho[j])
            {
                denser_num.push_back(j);        //the candidates that have higher rho than point i
                dep_dist_cand.push_back(distance[i][j]);        //And their corresponding distance to point i
            }
        }
        if(denser_num.empty())      //empty mean that point i is the most dense point
        {
            dep_dist.push_back(*max_element(distance[i].begin(),distance[i].end()));
            dep_neighbor.push_back(-1);
        }
        else
        {
            dep_dist.push_back(*min_element(dep_dist_cand.begin(),dep_dist_cand.end()));
            dep_neighbor.push_back(denser_num[findPosVector(dep_dist_cand,dep_dist[i])]);
        }
        denser_num.clear();
        dep_dist_cand.clear();
    }

    //Get the clustering result
    for(int i=0;i<num_of_points;i++)
    {
        gamma.push_back(dep_dist[i]*(float)rho[i]);
    }
    vector<float> sort_gamma = gamma;
    sort(sort_gamma.begin(),sort_gamma.end());
    
    vector <int> density_peaks[PEAK_NUM];
    density_peaks[0].push_back(5);
    //density_peaks[1].push_back(5);
    int hi = 0;
    
}

int main()
{   

    vector<vector<float>> datapoints;
    vector<float> each_datapoint;
    vector<string> class_ans;
    int i = 0,dim = 0;
    string input_filename  = "iris.data";
    string temp = "temp"; 
    #ifndef TESTING
                cout<<"Insert the dataset name:"<<endl;
                getline(cin,input_filename);
    #endif
    
    dataset.open(input_filename);
    if(dataset.fail())
    {
        cout<<"File "<<input_filename<<" doesn't exist."<<endl;
    }
    else
    {
        dim = get_dim(input_filename);

        while(!dataset.eof())
        {
            for(int k = 0;k<dim;k++)
            {
                getline(dataset,temp,',');      //抓直到下一個逗號或是換行之前的文字
                each_datapoint.push_back(atof(temp.c_str()));
            }
            getline(dataset,temp,'\n');  //最後的這個是class

            #ifdef ISCLASS_ANS
                class_ans.push_back(temp);
            #endif

            datapoints.push_back(each_datapoint);
            each_datapoint.clear();
            
        }
        int number_of_points = datapoints.size();       //The number is 3 if from 0~2
        vector<vector<float>> distance_between_all_points;
        vector<float> distance_for_a_point;
        int cluster_result[number_of_points] = {-1};    //-1 refers to noise
        for(int row = 0;row<number_of_points;row++)
        {
            for(int col = 0;col<number_of_points;col++)
            {
                distance_for_a_point.push_back(dist(datapoints[row],datapoints[col]));
            }
            distance_between_all_points.push_back(distance_for_a_point);
            distance_for_a_point.clear();
        }
        print_2d_vector(distance_between_all_points);
        
        DPC(distance_between_all_points, 0.5, cluster_result);

        /*for(int i =0 ;i<number_of_points;i++)
        {
            cout<<"For point "<<i<<" is belong to cluster "<<cluster_result[i]<<endl;
        }*/

        #ifdef DEBUG
            #ifdef ISCLASS_ANS
                for(int k = 0;k<class_ans.size();k++)
                {
                    cout<<"Point"<<k<<"'s class is:"<< class_ans[k]<<endl;
                }
            #endif
            print_2d_vector(datapoints);
        #endif
            
            
        }


    system("pause");
}