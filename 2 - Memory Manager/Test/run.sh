# commands to compile and run custom memory allocator
if [ -f alloc.cpp ]
then
    cp test_alloc1.c test_alloc1.cpp
    cp test_alloc2.c test_alloc2.cpp

    echo "testing alloc.cpp"
    
    # for test case 1
    g++ -c test_alloc1.cpp
    g++ -c alloc.cpp
    g++ test_alloc1.o alloc.o -o alloc
    ./alloc
    
    # for test case 2
    g++ -c test_alloc2.cpp
    g++ -c alloc.cpp
    g++ test_alloc2.o alloc.o -o alloc
    ./alloc
else
    # for test case 1
    gcc -c test_alloc1.c
    gcc -c alloc.c
    gcc test_alloc1.o alloc.o -o alloc
    ./alloc
    
    # for test case 2
    gcc -c test_alloc2.c
    gcc -c alloc.c
    gcc test_alloc2.o alloc.o -o alloc
    ./alloc
fi

rm *.o alloc
