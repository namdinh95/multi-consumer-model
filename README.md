# Multi-consumer model

A multi-threaded C++ Program that implements a producer to a multi-consumer model (25 consumers) using Boost thread library. A work in my Linux programming class. 

The program displays a matrix on the screen. Each box in the matrix represents a bin, which is initially empty. A producer will then randomly placing items in the bin to consume each second. 

Technically, each bin represents a consumer thread and the producer is another thread. If no items are available for processing, the consumer thread blocks and waits for new items. 

Visualization uses non-thread safe CDK library. 

## Compile and run 

Download the CDK library from: http://invisible-island.net/datafiles/release/cdk.tar.gz

Extract the compressed file and go to its directory. Then run:

```bash
./configure --prefix=<full-path-to-your-home-folder>
make
make install
```
After that, `make` the C++ file then run `./program` to see result. 

To quit the program, type `Ctrl-C`. The producer will stop working. Each bin will finish its remaining job. The program will exit only when all bins finish their jobs. 