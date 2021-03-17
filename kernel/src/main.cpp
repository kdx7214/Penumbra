
typedef unsigned long long uint64_t ;


//
// b_param:	Pass parameters needed from UEFI to the kernel
//

	typedef struct {
		void		*map ;					// Memory map
		uint64_t	msize ;					// Map size
		uint64_t	dsize ;					// Descriptor size
	} b_param ;



extern "C" int _main(b_param *bp) {
	return 42 ;
}

