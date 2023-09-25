# Lets: Test Suit for Tests in C

Lets is a Test Suit to test things in C.
This library in only the pair of files ***./core/unitytest.h*** and ***./core/unitytest.c***.

#### 1) Compilation
To compile the lib **core** and all **test suits**, just type:
```$ make```

Or, from the **libIFT root dir**, you can run:
```$ make tests```


#### 2) Running
To run all test suits:
```$ make run```

Or, from the **libIFT root dir**, you can run:
```$ make run_tests```

#### 3) Creating a new Test Suit
All test suits **must** be in folder ***./test_suit***, and must have the preffix ***test***
Exs: *./test_suit/testImage.c*, *./test_suit/testMatrixOperations.c*, ...

See an example of suit case in ****./test_suit/testExamples.c***

#### 4) Creating Tests which uses external data
All external data (image, dataset, etc) which your tests will use must be stored on ***./data***
**IMPORTANT: ALWAYS USE SMALL FILES**

