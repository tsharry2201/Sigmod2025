// (C) 2018 University of NKU. Free for used

/*
 * DataParty.cpp
 *
 */
#include "Protocol/DataParty.h"
#include "Protocol/MurmurHash3.h"

#include "Networking/sockets.h"
#include "Networking/ServerSocket.h"
#include "Networking/data.h"

#include "FM/IFM.h"

#include "Tools/ezOptionParser.h"

#include "Math/Setup.h"
#include "Math/bigint.h"

#include <iostream>
#include <pthread.h>

#include <sys/time.h>
#include <math.h>
#include <stdlib.h>
#include <vector>



/*
* for multi threads
*/
template<class T>
struct DP_thread_communication_info {
   DP* obj;
   T*  data;
   octetStream* o;
   int player_no;
   int t_id;
} ;

template<class T>
struct DP_thread_bucket_communication_info {
   DP* obj;
   std::vector<T>*  data;
   octetStream* o;
   int player_no;
   int t_id;
} ;

template<class T>
using thread_commu_info = DP_thread_communication_info<T>;

template<class T>
using thread_bucket_commu_info = DP_thread_bucket_communication_info<T>;



/* Takes command line arguments of 
       - Number of machines connecting
       - Base PORTNUM address
*/

DP::DP(int argc,const char **argv)
{
    ez::ezOptionParser opt;

    opt.syntax = "./DP.x [OPTIONS]\n";
    opt.example = "./DP.x -ndp 1 -ncp 2 -p 1 -lgp 64\n";

    opt.add(
        "1", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "number of data parties (default: 1)", // Help description.
        "-ndp", // Flag token.
        "--number_data_parties" // Flag token.
    );
    opt.add(
        "2", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "number of computation parties (default: 2)", // Help description.
        "-ncp", // Flag token.
        "--number_computation_parties" // Flag token.
    );
    opt.add(
            "500", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "number of FlajoletMartin trails", // Help description.
            "-oM", // Flag token.
            "--number_OFS_trails" // Flag token.
    );
    opt.add(
            "20000", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "number of unique items", // Help description.
            "-uN", // Flag token.
            "--number_unique_itmes" // Flag token.
    );
    opt.add(
        "100", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "marking of the party itself (starting from 1)", // Help description.
        "-p", // Flag token.
        "--my_num" // Flag token.
    );
    opt.add(
          "-1", // Default.
          0, // Required?
          1, // Number of args expected.
          0, // Delimiter if expecting multiple args.
          "Port to listen on (default: port number base - player number)", // Help description.
          "-mp", // Flag token.
          "--my-port" // Flag token.
    );
    opt.add(
          "5000", // Default.
          0, // Required?
          1, // Number of args expected.
          0, // Delimiter if expecting multiple args.
          "Port number base to attempt to start connections from (default: 5000)", // Help description.
          "-pn", // Flag token.
          "--portnumbase" // Flag token.
    );
    opt.add(
          "localhost", // Default.
          0, // Required?
          1, // Number of args expected.
          0, // Delimiter if expecting multiple args.
          "Host where Server.x is running to coordinate startup (default: localhost). Ignored if --ip-file-name is used.", // Help description.
          "-h", // Flag token.
          "--hostname" // Flag token.
    );
    opt.add(
      "", // Default.
      0, // Required?
      1, // Number of args expected.
      0, // Delimiter if expecting multiple args.
      "Filename containing list of party ip addresses. Alternative to --hostname and running Server.x for startup coordination.", // Help description.
      "-ip", // Flag token.
      "--ip-file-name" // Flag token.
    );
    opt.add(
        "128", // Default.
        0, // Required?
        1, // Number of args expected.
        0, // Delimiter if expecting multiple args.
        "Bit length of GF(p) field (default: 128)", // Help description.
        "-lgp", // Flag token.
        "--lgp" // Flag token.
    );
    opt.add(
            "", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "Directory containing the data (default: " PREP_DIR "<nparties>-<lgp>-<lg2>", // Help description.
            "-d", // Flag token.
            "--dir" // Flag token.
    );
    opt.add(
            "1", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "number of the threads(default 1)", // Help description.
            "-nt", // Flag token.
            "--number_threads" // Flag token.
    );
    opt.add(
            "0.3", // Default.
            0, // Required?
            1, // Number of args expected.
            0, // Delimiter if expecting multiple args.
            "the differential privacy requirment", // Help description.
            "-ep", // Flag token.
            "--epsilon" // Flag token.
    );


    opt.parse(argc, argv);

    string usage;
    opt.get("--my_num")->getInt(mynum);
    opt.get("--number_computation_parties")->getInt(n_computation_machines);
    opt.get("--number_data_parties")->getInt(n_data_machines);
    opt.get("--portnumbase")->getInt(PortnumBase);
    opt.get("--hostname")->getString(hostname);
    opt.get("--my-port")->getInt(my_port);
    opt.get("--lgp")->getInt(lgp);
    opt.get("--number_OFS_trails")->getInt(oM);
    opt.get("--number_unique_itmes")->getInt(uN);
    opt.get("--number_threads")->getInt(nthreads);
    opt.get("--epsilon")->getDouble(epsi);

    if(mynum>10)
        throw runtime_error("the player number have not been set!");

    if (opt.isSet("--dir"))
    {
        opt.get("--dir")->getString(PREP_DATA_PREFIX);
        PREP_DATA_PREFIX += "/";
    }
    else{
        PREP_DATA_PREFIX = get_prep_dir(n_computation_machines, lgp, 40);
    }

    if(opt.isSet("--ip-file-name")){
        opt.get("--ip-file-name")->getString(ipFileName);
    }else{
      ipFileName.clear();
    }
        

    if(my_port == DP::DEFAULT_PORT){
      my_port = PortnumBase-mynum+DP::OFFSET_PORT;
    }

    // ******************************** Network Part ************************************
    // setup concurrent 
    mutex_go = PTHREAD_MUTEX_INITIALIZER;

    // setup math function
    read_setup(PREP_DATA_PREFIX);


}

void DP::share_zero_array(){
    std::vector<std::vector<gfp>> tmp_gfp_array(1);
    tmp_gfp_array[0].resize(oM);
    for(int m=0; m<oM; m++){
      tmp_gfp_array[0][m].assign_zero();
      //tmp_gfp_array[0][m].assign(m);
    }

    bucket_share_value(tmp_gfp_array);

}

double DP::cal_var_epsilon_d(int d, double sigma, double sensitivity)
{
  double tao_d = 0, epsilon_d_0 = 0, epsilon_d_1 = 0;
  int k = 0;
  for (k = 1; k < d; ++k)
  {
    tao_d += exp((-2) * k * M_PI * M_PI * sigma * sigma / (k+1));
  }
  tao_d *= 10;
  epsilon_d_0 = sqrt(sensitivity * sensitivity / (d * sigma * sigma) + tao_d / 2);
  epsilon_d_1 = sensitivity / (sqrt((double)d) + tao_d);
  if (epsilon_d_0 > epsilon_d_1)
  {
    epsilon_d_0 = epsilon_d_1;
  }
  return epsilon_d_0;
}

double DP::cal_sigma(int d, double epsilon_d, double sensitivity)
{
  double left = 0.5, right = pow(10, 10);
  while (fabs(right - left) > 0.01)
  {
    if ((cal_var_epsilon_d(d, (left + right) / 2, sensitivity) - epsilon_d) < 0)
    {
      right = (right + left) / 2.0;
    }
    else
    {
      left = (right + left) / 2.0;
    }
  }
  return (right + left) / 2.0;
}

double DP::converse_epsilon_d(double epsilon, double delta)
{
  double epsilon_d = (-1) * sqrt((-2) * log(delta)) + sqrt((-2) * log(delta) + 2 * epsilon);
  return epsilon_d;
}


int DP::gene_discrete_gaussian(double sigma)
{
  int scope = 1000, rand_num;
  double prob[2 * scope] = {0}, rand_double;
  int i = 0;
  double sum_pro = 0, single_prob;
  for (i = 0; i < 2 * scope; ++i)
  {
    single_prob = exp((-1) * (i - scope) * (i - scope) / (2 * sigma * sigma));
    prob[i] = single_prob;
    sum_pro += single_prob;
  }
  for (i = 0; i < 2 * scope; ++i)
  {
    prob[i] = prob[i] / sum_pro;
  }
  srand(time(NULL));
  rand_num = rand();
  rand_double = (double)rand_num / RAND_MAX;
  sum_pro = 0;
  for (i = 0; i < scope * 2; ++i)
  {
    sum_pro += prob[i];
    if (rand_double < sum_pro)
    {
      rand_num = i - scope;
      break;
    }
  }
  return rand_num;
}

int DP::get_BucketNum_and_Rank(__uint128_t out, int * bucket_index, int w, int bits)
{
  int zeros = 0;
  __uint128_t musk = 1, bench = 0, musk_bucket = pow(2, bits) - 1;
  while(zeros < w-1)
  {
    if ((out & musk) == bench)
    {
      ++zeros;;
    }
    else
    {
      break;
    }
    musk *= 2;
  }
  out = out >> w;
  *bucket_index = musk_bucket & out;
  return zeros;
}


bool DP::data_collection(){
    vector<vector<int>> fms_sketch(oM);
    ofs_w = log2((float)uN / oM) + 4;
    for (int i = 0; i < oM; ++i)
    {
      fms_sketch[i].resize(ofs_w);
    }
    unsigned seed = rand();
    string file_name{"twitter-5.txt"};
    ifstream input_file;
    input_file.open(file_name);
    int i = 0, scale = 100000, rank, bits = log2(oM), bucket_index;
    __uint128_t hash_output;
    const char *key;
    string sentence;
    double sigma;
    for (i = 0; i < scale; ++i)
    {
      getline(input_file, sentence);
      key = sentence.c_str();
      MurmurHash3_x64_128(key, sentence.size(), seed, &hash_output);
      rank = get_BucketNum_and_Rank(hash_output, &bucket_index, ofs_w, bits);
      fms_sketch[bucket_index][rank] = 1;
    }
    input_file.close();
    bool ret = true;
    double epsi_d = 0;
    int noise;
    epsi_d = converse_epsilon_d(epsi, pow(10, -12));
    sigma = cal_sigma(n_data_machines, epsi_d, 1.0);
    noise = gene_discrete_gaussian(sigma);

    std::vector<std::vector<gfp>> tmp_gfp_array;
    tmp_gfp_array.resize(oM + 1);
    for(int m=0; m<oM; m++){
      tmp_gfp_array[m].resize(ofs_w);
      for(int i=0; i<ofs_w; i++){
        tmp_gfp_array[m][i].assign(fms_sketch[m][i]);
      }
    }
    tmp_gfp_array[oM].resize(1);
    tmp_gfp_array[oM][0].assign(noise);

    if(nthreads>1){
      ret = multi_thread_bucket_share_value(tmp_gfp_array);
    }else{
      ret = bucket_share_value(tmp_gfp_array);
    }
    return ret;
}

void DP::start()
{
  //time measure tools 
  struct timeval t1;
  struct timeval t2;
  double cpu_time_used;

  int i;


  /* Set up the sockets */
  socket_num.resize(n_computation_machines);
  for (i=0; i<n_computation_machines; i++) { socket_num[i]=-1; }

  if(mynum ==0){
    std::cout<<"invalid data party number, must be positive!\n";
    exit(-1);
  }


  std::vector<std::string> hn(n_computation_machines); 
  if(ipFileName.size()>0){
      ifstream file(ipFileName.c_str());
      if(!file.good()){
        throw file_missing(ipFileName);
      }

      string tmp;
      for(int i=0; i<n_computation_machines; i++){
        tmp.clear();
        std::getline(file,tmp);
        hn[i] = string(tmp.c_str(), tmp.size()-1);
      }

      file.close();
  }else{
    for(int i=0; i<n_computation_machines; i++){
      hn[i] = hostname;
    }
  }
        

  int pn = PortnumBase+ DP::OFFSET_PORT;
  // set up connections
  for (i=0; i<n_computation_machines; i++)
  {
    cerr << "Sent " << mynum << " to " << hn[i].c_str() << ":" << pn+i << endl;
    set_up_client_socket(socket_num[i], hn[i].c_str(), pn+i);
    send(socket_num[i], (octet*)&mynum, sizeof(mynum));
    cerr << "Sent Complete " << mynum << " to " << hn[i].c_str() << ":" << pn+i << endl;
  }
  connection_active = true;


  cerr << "************** Start data Collection phase **************" << endl;
  // wait until instruction to start from all computation parties
  int inst = -1;
  for (i=0; i<n_computation_machines; i++){
      while (inst != GO) 
        { 
          receive(socket_num[i], inst); 
        }
      inst = -1;
  }
  //cerr << "************** Start Test Online Protocol **************" << endl;
  // setup test
  bool ret;
  cout << "test Share(x) protocol \t\t\t";
  int int_x = 0;
  gfp test_x(int_x); 

  ret = share_value(test_x);

  if(ret){
      cout<<"pass test\n";
  }else{
      cout<<"failed test\n";
  }

  cout << "test data collection phase \n";

  gettimeofday(&t1, NULL);
  ret = data_collection();
  gettimeofday(&t2, NULL);
  cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
  printf("data collection phase: %d unique items with %d fs trails need  %f (ms) \n",uN,oM,cpu_time_used);


  if(ret){
      cout<<"pass test\n";
  }else{
      cout<<"failed test\n";
  }

  //cerr << "************** End Test Online Protocol **************" << endl;

  cerr << "************** End data Collection phase **************" << endl;


  for (int i = 0; i < n_computation_machines; i++){
    close(socket_num[i]);
  }
  connection_active = false;
}

template <class T>
bool DP::bucket_share_value(std::vector<std::vector<T>>& des_vec){
  if(!connection_active){
        throw runtime_error("The socket has been expired");
  }

  int ofs_M = des_vec.size()-1;
  int ofs_w = des_vec[0].size();
  int elements_size = ofs_M*ofs_w + 1;

  cout<<ofs_M<<"  "<<ofs_w<<endl;

  std::vector<T> a_value_Array(elements_size);
  T tmp;
  std::vector<octetStream>  share_stream(n_computation_machines);

  // receive value
  for (int i = 0; i < n_computation_machines; i++){
    receive_player(i,share_stream[i]);
  }
  // initialize
  for(int i=0; i<elements_size; i++){
    a_value_Array[i].assign_zero(); 
  }
  // reveal value
  for(int j=0; j<n_computation_machines; j++){
    for(int i=elements_size-1; i>=0; i--){
      tmp.unpack(share_stream[j]);
      a_value_Array[i].add(tmp);
    }
  }
  // reset the sokect stream
  share_stream[0].reset_write_head();
  // calculate and pack
  for(int m=ofs_M-1; m>=0; m--){
    for(int i=ofs_w-1; i>=0; i--){
        des_vec[m][i].sub(a_value_Array[m*ofs_w+i]);
        des_vec[m][i].pack(share_stream[0]);
    }
  }
  des_vec[ofs_M][0].sub(a_value_Array[ofs_M*ofs_w]);
  des_vec[ofs_M][0].pack(share_stream[0]);

  for(int i=0; i<n_computation_machines; i++){
    send_to(i,share_stream[0]);
  }

  return true;
}


template <class T>
bool DP::share_value(T& x){
  if(!connection_active){
        throw runtime_error("The socket has been expired");
  }

  T a_value,tmp;
  octetStream share_stream;
  // get the reveal value from the CPs
  a_value.assign_zero();
  for (int i = 0; i < n_computation_machines; i++){
    receive_player(i,share_stream);
    tmp.unpack(share_stream);
    a_value.add(tmp);
  }

  //calculation x-a and broadcasts it to all CPs
  //cout<<"revealed value a is " <<a_value <<endl;
  x.sub(a_value);
  //cout<<"x-a is " <<x <<endl;

  share_stream.reset_write_head();
  x.pack(share_stream);
  for (int i = 0; i < n_computation_machines; i++){
    send_to(i,share_stream);
  }

  //
  return true;
}

template <class T>
bool DP::multi_thread_bucket_share_value(std::vector<std::vector<T>>& des_vec){
  if(!connection_active){
        throw runtime_error("The socket has been expired");
  }

  int ofs_M = des_vec.size()-1;
  int ofs_w = des_vec[0].size();
  int elements_size = ofs_M*ofs_w + 1;

  std::vector<std::vector<T>> tmp(n_computation_machines);
  for(int i=0; i<n_computation_machines; i++){
    tmp[i].resize(elements_size);
  }
  std::vector<octetStream> share_stream(n_computation_machines);

  // multi-thread version receive value 
  std::vector<thread_bucket_commu_info<T>> thread_commu_data(n_computation_machines);
  Go = 0;
  pthread_t* t = new pthread_t[n_computation_machines];
  for(int i=0; i<n_computation_machines; i++){
    thread_commu_data[i].obj = this;
    thread_commu_data[i].data = &tmp[i];
    thread_commu_data[i].o = &share_stream[i];
    thread_commu_data[i].player_no = i;
    thread_commu_data[i].t_id = i;
    pthread_create(&t[i], NULL,thread_bucket_receive_player, (void*) &thread_commu_data[i]);
  }
  while(Go < n_computation_machines){
      usleep(10);
  }

  // reveal value
  for(int j=1; j<n_computation_machines; j++){
    for(int i=elements_size-1; i>=0; i--){
      tmp[0][i].add(tmp[j][i]);
    }
  }

  // reset the sokect stream
  share_stream[0].reset_write_head();
  // calculate and pack
  for(int m=ofs_M-1; m>=0; m--){
    for(int i=ofs_w-1; i>=0; i--){
        des_vec[m][i].sub(tmp[0][m*ofs_w+i]);
        des_vec[m][i].pack(share_stream[0]);
    }
  }
  des_vec[ofs_M][0].sub(tmp[0][ofs_M*ofs_w]);
  des_vec[ofs_M][0].pack(share_stream[0]);

  Go = 0;
  for(int i=0; i<n_computation_machines; i++){
    thread_commu_data[i].o = &share_stream[0];
    pthread_create(&t[i], NULL,thread_bucket_send_to, (void*) &thread_commu_data[i]);
  }
  while(Go < n_computation_machines){
      usleep(10);
  }

  delete t;
  return true;
}


template <class T>
bool DP::multi_thread_share_value(T& x){
  throw runtime_error("horrible slowest, do not use this , just for test");
  if(!connection_active){
        throw runtime_error("The socket has been expired");
  }

  T a_value;
  std::vector<T> tmp(n_computation_machines);
  std::vector<octetStream> share_stream(n_computation_machines);
  // get the reveal value from the CPs

  // multi-thread version receive value 
  std::vector<thread_commu_info<T>> thread_commu_data(n_computation_machines);
  Go = 0;
  pthread_t* t = new pthread_t[n_computation_machines];
  for(int i=0; i<n_computation_machines; i++){
    thread_commu_data[i].obj = this;
    thread_commu_data[i].data = &tmp[i];
    thread_commu_data[i].o = &share_stream[i];
    thread_commu_data[i].player_no = i;
    thread_commu_data[i].t_id = i;
    pthread_create(&t[i], NULL,thread_receive_player, (void*) &thread_commu_data[i]);
  }
  while(Go < n_computation_machines){
      usleep(10);
  }

  delete t;
  t = new pthread_t[n_computation_machines];

  //calculation x-a and broadcasts it to all CPs
  a_value.assign_zero();
  for(int i = 0; i < n_computation_machines; i++){
    a_value.add(tmp[i]);
  } 
  x.sub(a_value);

  Go = 0;
  for (int i = 0; i < n_computation_machines; i++){  
    thread_commu_data[i].data = &x;
    pthread_create(&t[i], NULL,thread_send_to, (void*) &thread_commu_data[i]);
  }
  while(Go < n_computation_machines){
      usleep(10);
  }

  delete t;

  return true;
}

void DP::receive_player(int i,octetStream& o,bool donthash) const{
  //TimeScope ts(timer);
  o.reset_write_head();
  o.Receive(socket_num[i]);
  if (!donthash)
    { blk_SHA1_Update(&ctx,o.get_data(),o.get_length()); }
}

void DP::send_to(int i,const octetStream& o,bool donthash) const{
  //TimeScope ts(timer);
  o.Send(socket_num[i]);
  if (!donthash)
      { blk_SHA1_Update(&ctx,o.get_data(),o.get_length()); }
  sent += o.get_length();
}

/*
*   serve for multi-thread
*/
void* DP::thread_receive_player(void* arg){
  thread_commu_info<gfp>* data = static_cast<thread_commu_info<gfp>*>(arg);
  DP* obj = data->obj;
  //obj->run_thread_receive_player(data->player_no,*(data->data),*(data->o),data->t_id);
  obj->run_thread_receive_player(data->player_no,*(data->data),*(data->o));

  pthread_exit(NULL);
}

void* DP::thread_bucket_receive_player(void* arg){
  thread_bucket_commu_info<gfp>* data = static_cast<thread_bucket_commu_info<gfp>*>(arg);
  DP* obj = data->obj;
  obj->run_thread_bucket_receive_player(data->player_no,*(data->data),*(data->o));

  pthread_exit(NULL);
}

void* DP::thread_bucket_send_to(void* arg){
  thread_bucket_commu_info<gfp>* data = static_cast<thread_bucket_commu_info<gfp>*>(arg);
  DP* obj = data->obj;
  obj->run_thread_bucket_send_to(data->player_no,*(data->o));
  pthread_exit(NULL);
}

void* DP::thread_send_to(void* arg){
  thread_commu_info<gfp>* data = static_cast<thread_commu_info<gfp>*>(arg);
  DP* obj = data->obj;
  //obj->run_thread_send_to(data->player_no,*(data->data),*(data->o),data->t_id);
  obj->run_thread_send_to(data->player_no,*(data->data),*(data->o));

  pthread_exit(NULL);
}
  
template <class T>
//void DP::run_thread_receive_player(int& player_no, T& data, octetStream& o,const int thread_id)
void DP::run_thread_receive_player(int& player_no, T& data, octetStream& o){
  // functionality
  receive_player(player_no,o);
  data.unpack(o);   

  // concurrent
  pthread_mutex_lock(&mutex_go); 
  Go++;
  //std::cout << "thread "<< thread_id << "enter to receive data from computation party "<<player_no<<std::endl;
  pthread_mutex_unlock(&mutex_go);
}

template <class T>
void DP::run_thread_bucket_receive_player(int& player_no, std::vector<T>& data, octetStream& o){
  // functionality
  int elements_size = data.size();
  receive_player(player_no,o);
  for(int i=elements_size-1; i>=0; i--){
    data[i].unpack(o);
  }

  // concurrent
  pthread_mutex_lock(&mutex_go); 
  Go++;
  //std::cout << "thread "<< thread_id << "enter to receive data from computation party "<<player_no<<std::endl;
  pthread_mutex_unlock(&mutex_go);
}

void DP::run_thread_bucket_send_to(int& player_no, octetStream& o){
  send_to(player_no,o);  

  // concurrent
  pthread_mutex_lock(&mutex_go); 
  Go++;
  pthread_mutex_unlock(&mutex_go);
}

template <class T>
//void DP::run_thread_send_to(int& player_no, T& data, octetStream& o,const int thread_id)
void DP::run_thread_send_to(int& player_no, T& data, octetStream& o){
  // functionality
  o.reset_write_head();
  data.pack(o); 
  send_to(player_no,o);  

  // concurrent
  pthread_mutex_lock(&mutex_go); 
  Go++;
  //std::cout << "thread "<< thread_id << "enter to send data to computation party "<<player_no<<std::endl;
  pthread_mutex_unlock(&mutex_go);
}