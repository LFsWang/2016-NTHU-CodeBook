#include<experimental/filesystem>
#include<algorithm>
#include<iostream>
#include<iomanip>
#include<string>
#include<cstdlib>
#include<vector>

namespace fs = std::experimental::filesystem::v1;
using namespace std::string_literals;

#define TESTDIR "./test"

#if defined CPP11
    #define CPPV "c++11"
#elif defined CPP14
    #define CPPV "c++14"
#else
    #define CPPV "c++98"
#endif

#define GCCCMD "g++ %s -O2 -std=" CPPV " -o opt.o -I%s"

#define C_RED "\033[1;31m"
#define C_END "\033[0m"
std::vector<const char *> ignore_list {"test",".git","test.cpp"};
struct test_status{
    int total;
    int succ;
    int fail;
    int untest;
};
// make ' ' to '\ '
std::string deal_space(const std::string &s)
{
    std::string opt;
    for(char c:s)
    {
        if(c==' ')opt.push_back('\\');
        opt.push_back(c);
    }
    return opt;
}

void ForeachFile(std::string path,test_status &status,int deep = 0)
{
    for(fs::directory_entry data : fs::directory_iterator(path))
    {
        auto p = data.path();
        std::string fname = data.path().filename().string();
        
        if( !fname.empty() && fname[0] == '.')
            continue;
        if( std::find(ignore_list.begin(),ignore_list.end(),fname)!=ignore_list.end() )
            continue;
        if( is_directory(data.path()) )
        {
            std::cout<<std::setw(deep*4)<<""<<fname<<std::endl;
            ForeachFile(path + "/" + fname,status,deep+1);
            continue;
        }
        if( p.extension().string()!=".cpp" )
            continue;
        std::cout<<std::setw(deep*4)<<""<<fname<<std::endl;
        status.total++;
        
        //find TESTDIR/ path / "test" + fname 
        std::string include_base = path;
        std::string testCpp = TESTDIR + "/"s + path + "/test" + fname;
        
        if( fs::exists(testCpp) )
        {
            testCpp = deal_space(testCpp);
            include_base = deal_space(include_base);
            
            std::remove("opt.o");
            int res = system(("g++-6 " + testCpp + " -O2 -std=" CPPV " -o opt.o -I"+include_base).c_str());
            if( res != 0 ){
                status.fail++;
                return;
            }
            
            res = system("./opt.o");
            
            if( res != 0 ){
                status.fail++;
                return;
            }
            status.succ++;
        }
        else
        {
            status.untest++;
            std::cout<<std::setw(deep*4+1)<<""<< C_RED "test not found" C_END <<std::endl;
        }
    }
}

int main()
{
    std::cout<<"GCCCMD:"<<GCCCMD<<std::endl;
    test_status st {0};
    ForeachFile(".",st);
    std::cout<<"============================="<<std::endl
             <<"total file:\t"<<st.total<<std::endl
             <<"succ :\t"<<st.succ<<std::endl
             <<C_RED"fail :\t"<<st.fail<<C_END<<std::endl
             <<"untest:\t"<<st.untest<<std::endl
             <<"============================="<<std::endl;
    return st.fail!=0;
}
