#include <lo/lo.h>
#include <lo/lo_cpp.h>
#include <lsl_cpp.h>
#include <iostream>
#include <atomic>
#include <unistd.h>
#include <map>
#include <getopt.h>
#include <cstdint>

#define DEBUG(x) std::cerr << __FILE__ << ":" << __LINE__ << " " << #x << "=" << x << std::endl

typedef std::map<std::string,lsl::stream_outlet*> stream_map_t;

static int send_something(const char *path, const char *types,
			  lo_arg **argv, int argc, lo_message msg,
			  void *user_data)
{
  if( argc > 0 ){
    bool all_same(true);
    for( uint32_t k=1;k<argc;++k)
      if( types[k] != types[0] )
	all_same = false;
    if( all_same ){
      std::string name(path);
      name+=types;
      stream_map_t* smap((stream_map_t*)user_data);
      lsl::stream_outlet* sop(NULL);
      if( smap->find(name) == smap->end() ){
	lsl::channel_format_t lslfmt(lsl::cf_float32);
	switch( types[0] ){
	case 'd' : lslfmt = lsl::cf_double64; break;
	case 'i' : lslfmt = lsl::cf_int32; break;
	case 's' : lslfmt = lsl::cf_string; break;
	}
	lsl::stream_info info(path, "osc2lsl", argc, lsl::IRREGULAR_RATE, lslfmt, name.c_str() );
	sop = new lsl::stream_outlet(info);
	(*smap)[name] = sop;
	std::cout << "allocating "  << name << std::endl;
      }else{
	sop = (*smap)[name];
      }
      if( sop ){
	switch( types[0] ){
	case 'f' : {
	  std::vector<float> data;
          for( std::uint32_t k=0;k<argc;++k)
	    data.push_back(argv[k]->f);
	  sop->push_sample( data );
	  break;
	}
	case 'd' : {
	  std::vector<double> data;
          for( std::uint32_t k=0;k<argc;++k)
	    data.push_back(argv[k]->d);
	  sop->push_sample( data );
	  break;
	}
	case 'i' : {
	  std::vector<int32_t> data;
          for( std::uint32_t k=0;k<argc;++k)
	    data.push_back(argv[k]->i);
	  sop->push_sample( data );
	  break;
	}
	case 's' : {
	  sop->push_sample( &(argv[0]->s) );
	  break;
	}
	}
      }
    }
  }
  return 0;
}

void app_usage(const std::string& app_name,struct option * opt,const std::string& app_arg)
{
  std::cout << "Usage:\n\n" << app_name << " [options] " << app_arg << "\n\nOptions:\n\n";
  while( opt->name ){
    std::cout << "  -" << (char)(opt->val) << " " << (opt->has_arg?"#":"") <<
      "\n  --" << opt->name << (opt->has_arg?"=#":"") << "\n\n";
    opt++;
  }
  std::cout << std::endl;
}

void add_stream( stream_map_t& streams, const std::string& path_and_format )
{
  int32_t p(path_and_format.find(":"));
  if( p != std::string::npos ){
    std::string types(path_and_format.substr(p+1));
    std::string path(path_and_format.substr(0,p));
    bool all_same(true);
    for( uint32_t k=1;k<types.size();++k)
      if( types[k] != types[0] )
	all_same = false;
    if( all_same ){
      int32_t argc(types.size());
      std::string name(path);
      name+=types;
      lsl::stream_outlet* sop(NULL);
      if( streams.find(name) == streams.end() ){
	lsl::channel_format_t lslfmt(lsl::cf_float32);
	switch( types[0] ){
	case 'd' : lslfmt = lsl::cf_double64; break;
	case 'i' : lslfmt = lsl::cf_int32; break;
	case 's' : lslfmt = lsl::cf_string; break;
	}
	lsl::stream_info info(path, "osc2lsl", argc, lsl::IRREGULAR_RATE, lslfmt, name.c_str() );
	sop = new lsl::stream_outlet(info);
	streams[name] = sop;
	std::cout << "allocating "  << name << std::endl;
      }
    }
  }
}

int main(int argc, char**argv)
{
  stream_map_t streams;
  int32_t port(-1);
  const char *options = "ha:np:";
  struct option long_options[] = { 
    { "help",     0, 0, 'h' },
    { "add",      1, 0, 'a' },
    { "noauto",   0, 0, 'n' },
    { "port",     1, 0, 'p' },
    { 0, 0, 0, 0 }
  };
  int opt(0);
  int option_index(0);
  bool noauto(false);
  while( (opt = getopt_long(argc, argv, options,
			    long_options, &option_index)) != -1){
    switch(opt){
    case 'h':
      app_usage("osc2lsl",long_options,"");
      return -1;
    case 'a':
      add_stream( streams, optarg );
      break;
    case 'n':
      noauto = true;
      break;
    case 'p':
      port = atoi(optarg);
      break;
    }
  }
  if( port < 0 ){
      app_usage("osc2lsl",long_options,"");
      return -1;
  }
  lo::ServerThread st(port);
  if (!st.is_valid()) {
    std::cout << "Nope." << std::endl;
    return 1;
  }
  std::atomic<bool> b_quit(false);
  st.add_method("/osc2lsl/quit", "",[&b_quit](lo_arg **,int){b_quit = true;});
  for( auto & t : streams ){
    //DEBUG(t.second->info().name());
    //DEBUG(t.first.substr(t.second->info().name().size()));
    st.add_method( t.second->info().name(), t.first.substr(t.second->info().name().size()), &send_something, &streams );
  }
  if( !noauto )
    st.add_method( NULL, NULL, &send_something, &streams );
  st.start();
  while( !b_quit )
    usleep( 10000 );
  st.stop();
  for( stream_map_t::iterator it=streams.begin();it!=streams.end();++it)
    delete it->second;
}


/*
 * Local Variables:
 * compile-command: "make -C .."
 * End:
 */
