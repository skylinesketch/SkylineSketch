GCC = g++
CFLAGS = -std=c++17 -O2 -g 

all: performance bucket_size_expr global_size_expr optimize_expr

performance: performance.cc bucket.h slot.h globalbucket.h skylinesketch.h item.h strawman.h utils.h utils.cc
	$(GCC) $(CFLAGS) performance.cc utils.cc -o performance

bucket_size_expr: bucket_size_expr.cc bucket.h slot.h globalbucket.h skylinesketch.h item.h strawman.h utils.h utils.cc
	$(GCC) $(CFLAGS) bucket_size_expr.cc utils.cc -o bucket_size_expr

global_size_expr: global_size_expr.cc bucket.h slot.h globalbucket.h skylinesketch.h item.h strawman.h utils.h utils.cc
	$(GCC) $(CFLAGS) global_size_expr.cc utils.cc -o global_size_expr

optimize_expr: optimize_expr.cc bucket.h slot.h globalbucket.h skylinesketch.h item.h strawman.h utils.h utils.cc
	$(GCC) $(CFLAGS) optimize_expr.cc utils.cc -o optimize_expr


clean:
	rm global_size_expr performance bucket_size_expr

