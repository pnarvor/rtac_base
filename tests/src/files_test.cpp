#include <iostream>

#include <rtac_base/files.h>
#include <rtac_base/time.h>
using namespace rtac::time;

using namespace std;
namespace nf = rtac::files;

int main()
{
    Clock clock;
    cout << "RTAC_DATA : " << nf::rtac_data_path() << endl;
    
    auto paths = nf::rtac_data_paths();
    for(auto p : paths) {
        cout << "  " << p << endl;
    }

    for(auto& path : nf::find()) {
        cout << path << endl;
    }
    cout << "Ellapsed : " << clock.interval() << "s" << endl;

    for(auto& path : nf::find(".*\\.tif")) {
        cout << path << endl;
    }
    cout << "Ellapsed : " << clock.interval() << "s" << endl;

    cout << nf::find_one(".*dsm.*\\.tif") << endl;
    cout << "Ellapsed : " << clock.interval() << "s" << endl;

    return 0;
}


