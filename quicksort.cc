#include <iostream>
#include <vector>
using namespace std;
int counter = 0;
int Partition(vector<int>&vec, int low, int high)
{
    int pivotVal = vec[low];   
    while (low < high)
    {
        while (low < high && vec[high] >= pivotVal) high--;   
        swap(vec[low], vec[high]);
        while (low < high && vec[low] <= pivotVal) low++;    
        swap(vec[low], vec[high]);
    }
    vec[low] = pivotVal;

    cout << "NO." << ++counter << "cishu: ";
    for (int i = 0; i < 11; i++)
        cout << vec[i] << "   ";
    cout << endl << endl;
    return low;
}
void quick_sort(vector<int>& vec, int low, int high)
{
    if (low < high)
    {
        int pivot = Partition(vec, low, high);  
        quick_sort(vec, low, pivot - 1);        
        quick_sort(vec, pivot + 1, high);       
    }
}
int main(int argc, const char* argv[])
{
    vector<int> vec = { 100, 1, 53, 5, 36, 7, 8, 109,  10, 11, 15 };
    cout << "begin array is: ";
    for (int i = 0; i < 11; i++)
        cout << vec[i] << " ";
    cout << endl << endl;
    quick_sort(vec, 0, vec.size() - 1);

    cout << "final answer is: ";
    for (int i = 0; i < 11; i++)
        cout << vec[i] << " ";
    cout << endl << endl;
    return 0;
}
