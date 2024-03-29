#include <iostream>
using namespace std;

#include <rtac_base/cuda/DeviceVector.h>
#include <rtac_base/cuda/HostVector.h>
using namespace rtac::cuda;

#include "vector_view.h"

DeviceVector<float> generate_data()
{
    std::vector<float> data(10);
    for(int i = 0; i < data.size(); i++) data[i] = i;
    return data;
}

int main()
{
    DeviceVector<float> input(generate_data());
    DeviceVector<float> output(input.size());

    copy(input,output);

    cout <<  input << endl;
    cout << output << endl;
    
    // does not compile (intended). DeviceVector.operator[] not defined on CPU)
    // cout << VectorView(output).front() << endl;

    HostVector<float> tmp(output);
    cout << make_view(tmp).back() << endl;

    return 0;
}
