#include <rtac_base/files.h>

#include <cstdlib>
#include <regex>

#include <experimental/filesystem>

namespace rtac { namespace files {

namespace fs = std::experimental::filesystem;

std::string rtac_data_path()
{
    return std::string(std::getenv(DATA_PATH_ENV_VARIABLE));
}

PathList rtac_data_paths(const std::string& delimiter)
{
    PathList paths;
    auto env = rtac_data_path();
    size_t idx = env.find(delimiter);
    while(idx != std::string::npos) {
        paths.push_back(env.substr(0, idx));
        idx = env.find(delimiter);
        env.erase(0, paths.back().length() + delimiter.length());
    }
    if(env.length() > 0) {
        paths.push_back(env);
    }
    return paths;
}

PathList find(const std::string& reString, bool followSimlink)
{
    auto paths = rtac_data_paths();
    return find(reString, paths, followSimlink);
}

PathList find(const std::string& reString, const std::string& path, bool followSimlink)
{
    return find(reString, {path}, followSimlink);
}

PathList find(const std::string& reString, const PathList& searchPaths, bool followSimlink)
{
    PathList paths;
    
    fs::directory_options doptions = fs::directory_options::none;
    if (followSimlink)
        doptions = fs::directory_options::follow_directory_symlink;
    
    std::regex re(reString);
    for(auto& sPath : searchPaths) {
        for(auto& path : fs::recursive_directory_iterator(sPath, doptions)) {
            if(std::regex_match(path.path().string(), re))
                paths.push_back(path.path());
        }
    }
    paths.sort();

    return paths;
}

std::string find_one(const std::string& reString, bool followSimlink)
{
    auto paths = rtac_data_paths();
    return find_one(reString, paths, followSimlink);
}

std::string find_one(const std::string& reString,
                     const std::string& path, bool followSimlink)
{
    return find_one(reString, {path}, followSimlink);
}

std::string find_one(const std::string& reString,
                     const PathList& searchPaths, bool followSimlink)
{
    fs::directory_options doptions = fs::directory_options::none;
    if (followSimlink)
        doptions = fs::directory_options::follow_directory_symlink;
    
    std::regex re(reString);
    for(auto& sPath : searchPaths) {
        for(auto& path : fs::recursive_directory_iterator(sPath, doptions)) {
            if(std::regex_match(path.path().string(), re))
                return path.path().string();
        }
    }
    return NotFound;
}

std::string append_extension(const std::string& path, const std::string& ext)
{
    fs::path res(path);
    res.replace_extension(ext);
    return res.string();
}

void write_pgm(const std::string& path, size_t width, size_t height, const char* data,
               const std::string& comment)
{
    std::ofstream f;
    f.open(path, std::ios::out | std::ios::binary);
    if(!f.good())
        throw std::runtime_error("Cloud not open file for pgm export : " + path);
    f << "P5\n";
    if(comment.size() > 0) {
        std::istringstream iss(comment);
        for(std::string line; std::getline(iss, line);) {
            f << "# " << line << "\n";
        }
    }
    f << width << " " << height << "\n" << 255 << "\n";
    f.write(data, width*height); 

    f.close();
}

void write_ppm(const std::string& path, size_t width, size_t height, const char* data,
               const std::string& comment)
{
    std::ofstream f;
    f.open(path, std::ios::out | std::ios::binary);
    if(!f.good())
        throw std::runtime_error("Cloud not open file for pgm export : " + path);
    f << "P6\n"; // PPM file format magic number
    if(comment.size() > 0) {
        std::istringstream iss(comment);
        for(std::string line; std::getline(iss, line);) {
            f << "# " << line << "\n";
        }
    }
    f << width << " " << height << "\n" << 255 << "\n";
    f.write(data, width*height*3);

    f.close();
}

}; //namespace files
}; //namespace rtac

