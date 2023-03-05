// clion
// Created by 86195 on 2023/3/4.
#include <iostream>
#include <vector>
#include <iomanip>
#include "./include/cppjieba/Jieba.hpp"
#include "./include/simhash/Simhasher.hpp"

using namespace std;

// 读写函数
bool Loaded_context(string Path, string& context);
bool Output_result(string Path,double res);

//重新实现海明距离
unsigned hamdist( string str1,  string str2);
//基于海明距离的相似度计算
bool cal_HamSim(double & res, int dis);

// 字典、模型路径
const char* const DICT_PATH = "include/dict/jieba.dict.utf8";
const char* const HMM_PATH = "./include/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "./include/dict/user.dict.utf8";
const char* const IDF_PATH = "./include/dict/idf.utf8";
const char* const STOP_WORD_PATH = "./include/dict/stop_words.utf8";
const char* const HMM_MODEL_PATH = "./include/dict/hmm_model.utf8";
const char* const KEYWORDS_1_PATH = "./keyword1.txt";
const char* const KEYWORDS_2_PATH = "./keyword2.txt";

//文本数据
string context_1;
string context_2;
vector<pair<string ,double>> result_1;
vector<pair<string ,double>> result_2;

//simHash_key
uint64_t simHashKey_contxt_1 = 0;
uint64_t simHashKey_contxt_2 = 0;
//海明距离


//关键词top1000
size_t topN = 1000;

/*
 * main参数说明
 * argc的含义是argument count：它是一个int行变量，表示传递给main函数的参数数量
 * argv的含义是argument value：它是一个指向字符串的指针数组，每个指针元素指向各个具体的参数
 * argv[0] : main.exe 的首地址
 * argv[1] : 测试文本 1 的地址
 * argv[2] : 测试文本 2 的地址
 * argv[3] : 结果文本的地址
 * */
int main(int argc, char * argv[]){

    //读取 argv[] 中的文本路径信
    /* const char* const SRC_1_PATH = argv[1];
     * const char* const SRC_2_PATH = argv[2];
     * const char* const RES_PATH = argv[3];*/

    const char* const SRC_1_PATH = "src1.txt";
    const char* const SRC_2_PATH = "src2.txt";
    const char* const RES_PATH = "res.txt";

    //加载文本数据
    Loaded_context(SRC_1_PATH,context_1);
    Loaded_context(SRC_2_PATH,context_2);

    //初始化simhash对象
    simhash::Simhasher simhasher(DICT_PATH, HMM_MODEL_PATH, IDF_PATH, STOP_WORD_PATH);

    //关键词序列
    simhasher.extract(context_1, result_1, topN);
    simhasher.extract(context_2, result_2, topN);

    // 计算simhash
    simhasher.make(context_1, topN, simHashKey_contxt_1);
    simhasher.make(context_2, topN, simHashKey_contxt_2);

    //unit_64（ll int）转 01 串
    string s1;
    string s2;
    simhash::Simhasher::toBinaryString(simHashKey_contxt_1,s1);
    cout <<"Simhash_context_1: " << s1 << endl;
    simhash::Simhasher::toBinaryString(simHashKey_contxt_2,s2);
    cout <<"Simhash_context_2: "<< s2 << endl;

    //重新实现hamdist（） 重新计算海明距离
    //cout << hamdist(s1,s2) << endl;

    //计算相似度和海明距离
    int dis = simhash::Simhasher::isEqual(simHashKey_contxt_1, simHashKey_contxt_2);
    cout <<"hamming: "<< dis << endl;
    double res;
    cal_HamSim(res,dis);
    cout<<fixed<<setprecision(2);
    cout <<"similarity: " << res << endl;

    Output_result(RES_PATH, res);

    //输出结果
    //Output_result(RES_PATH);
    //0.01 * (100 - distance * 100 / 128);
    return 0;

}

bool Loaded_context(string Path, string& Context){
    ifstream context_ifs(Path);
    if(context_ifs.fail()){
        cout << "error: SRC_1_PATH null!" << endl;
        context_ifs.close();
        return false;
    }else{
        while(!context_ifs.eof()){
            string temp;
            context_ifs >> temp;
            Context.append(temp);
        }
        context_ifs.close();
        cout << "File path: " << Path << " loaded " << endl;
        return 1;
    };


}

bool Output_result(string Path, double res){
    ofstream of(Path,ios::app);
    ofstream of_res_1(KEYWORDS_1_PATH);
    ofstream of_res_2(KEYWORDS_2_PATH);

    if(of.fail() || of_res_1.fail() || of_res_2.fail()){
        cout << "error: RES_PATH null!" << endl;
        of.close();
        of_res_1.close();
        of_res_2.close();
        return false;
    }else{

        of << "本次结果: " << res << endl;
        of_res_1 << "前1000个关键词: " <<result_1 << endl;
        of_res_2 << "前1000个关键词: " <<result_2 << endl;
        of.close();
        of_res_1.close();
        of_res_2.close();
        cout << "File path: " << Path << " output " << endl;
        return 1;
    }

}

unsigned hamdist(string str1, string str2)
{
    if (str1.empty() || str2.empty()) {
        return 0;
    }
    if (str1.length() != str2.length()){
        return 0;
    }
    unsigned dist = 0;
    unsigned i = 0;
    while(i < str1.length()){
        dist += (str1[i] != str2[i]) ? 1 : 0;
        i++;
    }
    return dist;
}

//基于海明距离的相似度计算
bool cal_HamSim(double & res, int dis){
    res = 0.01 * (100 - dis * 100 / 128.0);
    return 1;
}

//余弦相似度计算
double getMold(const vector<double>& vec)
{
    int n = vec.size();
    double sum = 0.0;
    for (int i = 0; i < n; ++i)
        sum += vec[i] * vec[i];
    return sqrt(sum);
}
double cos_distance(const vector<double>& base, const vector<double>& target)
{
    int n = base.size();
    assert(n == target.size());
    double tmp = 0.0;
    for (int i = 0; i < n; ++i)
        tmp += base[i] * target[i];
    double simility =  tmp / (getMold(base)*getMold(target));
    return simility;
}