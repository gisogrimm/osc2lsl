#include <lo/lo.h>
#include <lo/lo_cpp.h>
#include <lsl_cpp.h>
#include <iostream>
#include <atomic>
#include <unistd.h>
#include <map>

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
	std::cout << "allocating "  << name << std::endl;
      }
    }
  }
  return 0;
}

int main(int argc, char**argv)
{
  if( argc < 2 ){
    std::cerr << "Usage: osc2lsl <portnumber>" << std::endl;
    return 1;
  }
  int port(atoi(argv[1]));
  lo::ServerThread st(port);
  if (!st.is_valid()) {
    std::cout << "Nope." << std::endl;
    return 1;
  }
  std::atomic<bool> b_quit(false);
  stream_map_t streams;
  st.add_method("/osc2lsl/quit", "",[&b_quit](lo_arg **,int){b_quit = true;});
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
