x86:
	g++ -g -O0 -D __I386__ -D _GNU_SOURCE $(CFLAGS) -w -static -fpermissive -std=gnu++11 -o cachepnp \
	main.c \
	-pthread -lrt -lpthread

arm:
	arm-linux-gnueabi-g++ -D __ARM__ $(CFLAGS) -D _GNU_SOURCE -w -std=gnu++11 -fpermissive -static -o cachepnp main.c -lrt

aarch64:
	aarch64-linux-gnu-g++ -D __ARM__ $(CFLAGS) -D _GNU_SOURCE -w -std=gnu++11 -fpermissive -static -o cachepnp main.c -lrt
	
clean:
	rm -fr cachepnp

